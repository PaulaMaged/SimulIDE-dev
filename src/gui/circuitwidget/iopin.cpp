/***************************************************************************
 *   Copyright (C) 2021 by santiago González                               *
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

#include "iopin.h"
#include "simulator.h"

IoPin::IoPin(int angle, const QPoint pos, QString id, int index, Component* parent, pinMode_t mode )
       : Pin( angle, pos, id, index, parent )
       , eElement( id )
{
    m_scrEnode = new eNode( id+"scr" );
    m_scrEnode->setNodeNumber(0);
    Simulator::self()->remFromEnodeList( m_scrEnode, /*delete=*/ false );

    m_outState    = false;
    m_stateZ   = false;
    m_inverted = false;

    m_outHighV = cero_doub;
    m_outLowV  = cero_doub;
    m_outVolt  = cero_doub;

    m_vddAdmit = 0;
    m_gndAdmit = cero_doub;
    m_vddAdmEx = 0;
    m_gndAdmEx = 0;

    m_inputImp = high_imp;
    m_openImp  = 1e28;
    m_outputImp = 40;
    m_imp = cero_doub;
    m_admit = 1/ m_imp;

    m_pinMode = undef_mode;
    setPinMode( mode );
}
IoPin::~IoPin(){ delete m_scrEnode; }

void IoPin::initialize()
{
    ePin::setEnodeComp( m_scrEnode );
    m_inpState = false;
}

void IoPin::stamp()
{
    stampAll();
}

void IoPin::stampAll()
{
    ePin::stampAdmitance( m_admit );
    stampOutput();
}

void IoPin::stampOutput()
{
    m_scrEnode->setVolt( m_outVolt );
    ePin::stampCurrent( m_outVolt/m_imp );
}

void IoPin::setPinMode( pinMode_t mode )
{
    if( m_pinMode == mode ) return;
    m_pinMode = mode;

    if( mode == source )
    {
        m_vddAdmit = 1/cero_doub;
        m_gndAdmit = cero_doub;
        setPinState( out_high );
    }
    else if( mode == input )
    {
        m_vddAdmit = 0;
        m_gndAdmit = 1/m_inputImp;

        setPinState( input_low );
    }
    else if( mode == output )
    {
        m_vddAdmit = 1/m_outputImp;
        m_gndAdmit = cero_doub;

        if( m_inverted ) m_outState = !m_outState;
        setOutState( m_outState );
    }
    else if( mode == open_col )
    {
        m_vddAdmit = cero_doub;

        if( m_inverted ) m_outState = !m_outState;
        setOutState( m_outState );
    }
    update();
}

void IoPin::update()
{
    double vddAdmit = m_vddAdmit+m_vddAdmEx;
    double gndAdmit = m_gndAdmit+m_gndAdmEx;
    double Rth  = 1/(vddAdmit+gndAdmit);

    m_outVolt = m_outHighV*vddAdmit*Rth;

    IoPin::setImp( Rth );
}

bool IoPin::getInpState()
{
    double volt = getVolt();

    if     ( volt > m_inpHighV ) m_inpState = true;
    else if( volt < m_inpLowV )  m_inpState = false;

    m_pinState = m_inpState? input_high:input_low; // High : Low colors

    return m_inverted ? !m_inpState : m_inpState;
}

void IoPin::setOutState( bool out, bool st ) // Set Output to Hight or Low
{
    if( m_inverted ) m_outState = !out;
    else             m_outState =  out;

    if( m_stateZ ) return;

    if( m_pinMode == open_col )
    {
        if( m_outState ) m_gndAdmit = 1/m_openImp;
        else          m_gndAdmit = 1/m_outputImp;

        if( st ) update();
        setPinState( m_outState? out_open:out_low ); // Z-Low colors
    }
    else
    {
        if( m_outState ) m_outVolt = m_outHighV;
        else          m_outVolt = m_outLowV;

        if( st ) stampOutput();
        setPinState( m_outState? out_high:out_low ); // High-Low colors
    }
}

void IoPin::setStateZ( bool z )
{
    m_stateZ = z;
    if( z )
    {
        m_outVolt = m_outLowV;
        setImp( m_openImp );
        setPinState( out_open );
    }
    else
    {
        pinMode_t pm = m_pinMode; // Force pinMode
        m_pinMode = undef_mode;
        setPinMode( pm );
    }
}

void IoPin::setImp( double imp )
{
    m_imp = imp;
    m_admit = 1/m_imp;
    stampAll();
}

void IoPin::setInputImp( double imp )
{
    m_inputImp = imp;
    if( m_pinMode == input ) m_gndAdmit = 1/m_inputImp;
}

void IoPin::setOutputImp( double imp )
{
    m_outputImp = imp;
    if( m_pinMode == output ) m_vddAdmit = 1/m_outputImp;
}

void IoPin::setInverted( bool inverted )
{
    if( inverted == m_inverted ) return;

    if( inverted ) setOutState( !m_outState );
    else           setOutState( m_outState );

    m_inverted = inverted;
    ePin::setInverted( inverted );
}

void IoPin::controlPin( bool ctrl )
{
    if( ctrl == m_extCtrl ) return;

    if( ctrl && !m_extCtrl ) // Someone is getting control
    {
        m_oldPinMode = m_pinMode; // Save old Pin Mode to restore later
    }
    else                     // External control is being released
    {
        setPinMode( m_oldPinMode ); // Set Previous Pin MOde
    }
    m_extCtrl = ctrl;
}