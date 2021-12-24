/***************************************************************************
 *   Copyright (C) 2018 by santiago González                               *
 *   santigoro@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/

#include <QPainter>
#include <QPushButton>
#include <QGraphicsProxyWidget>

#include "dht22.h"
#include "iopin.h"
#include "simulator.h"
#include "circuit.h"
#include "itemlibrary.h"

#include "doubleprop.h"
#include "stringprop.h"

Component* Dht22::construct( QObject* parent, QString type, QString id )
{ return new Dht22( parent, type, id ); }

LibraryItem* Dht22::libraryItem()
{
    return new LibraryItem(
        "Dht22/11",
        tr( "Sensors" ),
        "dht22.png",
        "Dht22",
        Dht22::construct );
}

Dht22::Dht22( QObject* parent, QString type, QString id )
     : Component( parent, type, id )
     , eElement( id )
{
    m_area = QRect(-28,-20, 56, 40 );

    m_tempInc = 0.5;
    m_humiInc = 5;
    m_set = true;

    m_pin.resize(4);
    m_pin[0] = m_inpin = new IoPin( 270, QPoint(-4, 28), id+"-inPin", 0, this );
    m_inpin->setOutHighV( 5 );
    m_pin[1] = new Pin( 270, QPoint(-12, 28), id+"-vccPin", 0, this );
    m_pin[1]->setUnused( true );
    m_pin[2] = new Pin( 270, QPoint( 12, 28), id+"-gdnPin", 0, this );
    m_pin[2]->setUnused( true );
    m_pin[3] = new Pin( 270, QPoint( 4, 28), id+"-ncPin", 0, this );
    m_pin[3]->setUnused( true );

    m_font.setFamily("Ubuntu");
    m_font.setBold( true );
    setLabelPos(-40,-40, 0);
    setModel( "DHT22" );

    m_button = new QPushButton( );
    m_button->setMaximumSize( 6, 23);
    m_button->setGeometry( 0, 0, 6, 23);
    m_button->setCheckable( true );
    m_button->setIconSize( QSize( 6, 23) );
    m_button->setIcon(QIcon(":/up.png"));

    QPushButton* u_button = new QPushButton( );
    u_button->setMaximumSize( 9, 9 );
    u_button->setGeometry(-5,-5, 9, 9);
    u_button->setCheckable( false );
    u_button->setIcon(QIcon(":/su.png"));

    QPushButton* d_button = new QPushButton( );
    d_button->setMaximumSize( 9, 9 );
    d_button->setGeometry(-5,-5, 9, 9);
    d_button->setCheckable( false );
    d_button->setIcon(QIcon(":/giu.png"));

    m_proxy = Circuit::self()->addWidget( m_button );
    m_proxy->setParentItem( this );
    m_proxy->setPos( QPoint(-23,-8) );

    m_proxy = Circuit::self()->addWidget( u_button );
    m_proxy->setParentItem( this );
    m_proxy->setPos( QPoint( 17,-7) );

    m_proxy = Circuit::self()->addWidget( d_button );
    m_proxy->setParentItem( this );
    m_proxy->setPos( QPoint( 17, 6) );

    connect( m_button, SIGNAL( clicked()),
             this,     SLOT  ( onbuttonclicked()) );

    connect( u_button, SIGNAL( pressed()),
             this,     SLOT  ( upbuttonclicked()) );

    connect( d_button, SIGNAL( pressed()),
             this,     SLOT  ( downbuttonclicked()) );

    addPropGroup( { tr("Main"), {
new StringProp<Dht22>( "DHT22", tr("Model"),"", this, &Dht22::model, &Dht22::setModel, "enum"  ),
new DoubProp  <Dht22>( "TempInc", tr("Temp. increment"),"C°", this, &Dht22::tempInc,  &Dht22::setTempInc ),
new DoubProp  <Dht22>( "HumiInc", tr("Humid. increment"),"_%", this, &Dht22::humidInc, &Dht22::setHumidInc ),
    }} );
}
Dht22::~Dht22(){}

void Dht22::stamp()   // Called at Simulation Start
{
    m_outStep = 0;
    m_bitStep = 0;
    m_lastIn = false;
    m_inpin->setPinMode( input );
    m_inpin->changeCallBack( this, true );
}

void Dht22::voltChanged()                              // Called when Input Pin changes
{
    bool inState = m_inpin->getVolt() > 2.5;
    if( m_lastIn && !inState ) m_lastTime = Simulator::self()->circTime(); // Falling edge
    else if( !m_lastIn && inState )                                        // Rising edge
    {
        uint64_t time = Simulator::self()->circTime()-m_lastTime; // in picosecondss
        if( time > m_start )                              // Minimum input pulse
        {
            m_inpin->setPinMode( openCo );
            m_inpin->setOutState( true );
            m_inpin->changeCallBack( this, false ); // Stop receiving voltChange() CallBacks
            Simulator::self()->addEvent( 30*1e6, this ); // Send ack after 30 us
    }   }
    m_lastIn = inState;
}

void Dht22::runEvent()
{
    if( m_outStep == 0 )       // Send ACK
    {
        m_bit = 1UL<<39;
        if( m_bitStep == 0 )   // Start ACK
        {
            m_bitStep = 1;
            m_inpin->setOutState( false );
            Simulator::self()->addEvent( 80*1e6, this );
        }
        else{                  // End ACK
            m_bitStep = 0;
            m_outStep++;
            m_inpin->setOutState( true );
            Simulator::self()->addEvent( 80*1e6, this );
    }   }
    else{                     // Send data
        if( m_bitStep == 0 )  // Start bit (low side)
        {
            m_bitStep = 1;
            m_inpin->setOutState( false );
            Simulator::self()->addEvent( 50*1e6, this );
        }
        else{                 // End bit (high side)
            m_bitStep = 0;
            m_inpin->setOutState( true );
            if( m_bit == 0 )             // Transmission finished
            {
                m_outStep = 0;
                m_inpin->setPinMode( input );
                m_inpin->changeCallBack( this, true );
                return;
            }
            uint64_t time = ( (m_data & m_bit) > 0 ) ? 70*1e6 : 27*1e6;
            Simulator::self()->addEvent( time, this );
            m_bit >>= 1;
}   }   }

void Dht22::calcData()
{
    uint64_t tempI, tempD, humiI, humiD;
    if( m_DHT22 )
    {
        uint64_t temp = abs( m_temp*10 );
        if( m_temp < 0 ) temp |= 1<<15;
        tempI = temp >> 8;
        tempD = temp & 0xFF;

        uint64_t humi = m_humi*10;
        humiI = humi >> 8;
        humiD = humi & 0xFF;
    }else{
        tempI = m_temp;
        tempD = 0;//(m_temp-tempI)*10;
        humiI = m_humi;
        humiD = 0;//(m_humi-humiI)*10;
    }
    uint8_t checksum = tempI+tempD+humiI+humiD;
    m_data = (humiI<<32)+(humiD<<24)+(tempI<<16)+(tempD<<8)+checksum;
    update();
}

void Dht22::onbuttonclicked()
{
    m_set = m_button->isChecked();
    if( m_set ) m_button->setIcon( QIcon(":/up.png") );
    else        m_button->setIcon( QIcon(":/down.png") );
}

void Dht22::upbuttonclicked()
{
    if( m_set ) m_temp += m_tempInc;
    else        m_humi += m_humiInc;

    if( m_DHT22) {
       if( m_temp > 80 )  m_temp = 80;
       if( m_humi > 100 ) m_humi = 100;
    }else{
        if( m_temp > 50 ) m_temp = 50;
        if( m_humi > 90 ) m_humi = 90;
    }
    calcData();
}

void Dht22::downbuttonclicked()
{
    if( m_set ) m_temp -= m_tempInc;
    else        m_humi -= m_humiInc;

    if( m_DHT22) {
        if( m_temp < -40 ) m_temp = -40;
        if( m_humi < 0  )  m_humi = 0;
    }else{
        if( m_temp < 0 )  m_temp = 0;
        if( m_humi < 20 ) m_humi = 20;
    }
    calcData();
}

void Dht22::setModel( QString model )
{
    m_DHT22 = (model == "DHT22");
    if( m_DHT22 ) m_start = 1e9;
    else          m_start = 18e9;

    m_temp = 22;
    m_humi = 68;
    calcData();
}

QString Dht22::model() { return m_DHT22 ? "DHT22" : "DHT11"; }

double Dht22::trim( double data ) { return (double)(((int)(data*10)))/10; }

QStringList Dht22::getEnums( QString e ) { return { "DHT11", "DHT22" }; }

void Dht22::paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    Component::paint( p, option, widget );
    QPen pen( Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    p->setPen( pen );
    p->setBrush( QColor(50, 70, 100) );
    p->drawRoundedRect( m_area, 2, 2 );

    p->setBrush( QColor(200, 220, 180) );
    p->drawRoundedRect( QRect(-25,-10, 40, 26 ),2,2 );

    m_font.setPixelSize(6);
    p->setFont( m_font );
    if( m_DHT22 ){
        p->setPen( QColor(250, 250, 250) );
        p->drawText(-10,-13, "DHT22" );
    }else{
        p->setPen( QColor(200, 200, 255) );
        p->drawText(-10,-13, "DHT11" );
    }
    m_font.setPixelSize(9);
    p->setFont( m_font );
    p->setPen( QColor(0, 0, 0) );
    p->drawText( -16, 1, "C° "+QString::number( m_temp ) );
    p->drawText( -16,12, "%  "+QString::number( m_humi ) );
}

#include "moc_dht22.cpp"
