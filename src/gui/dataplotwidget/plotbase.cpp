/***************************************************************************
 *   Copyright (C) 2020 by santiago González                               *
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

#include "plotbase.h"
#include "plotdisplay.h"
#include "datachannel.h"
#include "simulator.h"
#include "circuit.h"
#include "circuitwidget.h"


PlotBase::PlotBase( QObject* parent, QString type, QString id )
        : Component( parent, type, id )
        , eElement( id )
{
    m_color[0] = QColor( 240, 240, 100 );
    m_color[1] = QColor( 220, 220, 255 );
    m_color[2] = QColor( 255, 210, 90  );
    m_color[3] = QColor( 000, 245, 160 );
    m_color[4] = QColor( 255, 255, 255 );

    m_baSizeX = 135;
    m_baSizeY = 135;

    m_expand = false;

    Simulator::self()->addToUpdateList( this );
}
PlotBase::~PlotBase()
{
    Simulator::self()->remFromUpdateList( this );
}

void PlotBase::initialize()
{
    ///m_dataPlotW->m_refCondFlag = false;
}

void PlotBase::setBaSizeX( int size )
{
    if( size < 135 ) size = 135;
    m_baSizeX = size;
    expand( m_expand );
}

void PlotBase::setBaSizeY( int size )
{
    if( size < 135 ) size = 135;
    m_baSizeY = size;
    expand( m_expand );
}

void PlotBase::paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    Component::paint( p, option, widget );

    p->setBrush(QColor( 230, 230, 230 ));
    p->drawRoundedRect( m_area, 4, 4 );
    
    p->setBrush( Qt::white );
    QPen pen = p->pen();
    pen.setWidth( 0 );
    pen.setColor( Qt::white );
    p->setPen(pen);
}

#include "moc_plotbase.cpp"


