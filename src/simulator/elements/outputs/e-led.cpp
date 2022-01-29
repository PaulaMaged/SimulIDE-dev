/***************************************************************************
 *   Copyright (C) 2012 by santiago González                               *
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

#include <math.h>

#include "e-led.h"
#include "e-pin.h"
#include "e-node.h"
#include "simulator.h"

eLed::eLed( QString id ) 
    : eResistor( id )
{
    m_imped = 0.6;
    m_threshold  = 2.4;
    m_maxCurrent = 0.03;
    initialize();
}
eLed::~eLed() {}

void eLed::initialize()
{
    m_prevStep    = 0;
    m_avgCurrent  = 0.;
    m_intensity   = 25;
    m_brightness  = 0;
    m_totalCurrent = 0;
    m_lastPeriod   = 0;

    m_admit = 1e-9;
    m_voltPN  = 0;
    m_current = 0;
    m_lastCurrent = 0;
    m_lastThCurrent = 0;
}

void eLed::stamp()
{
    eNode* node = m_ePin[0]->getEnode();
    if( node ) node->addToNoLinList( this );

    node = m_ePin[1]->getEnode();
    if( node ) node->addToNoLinList( this );

    eResistor::stamp();
}

void eLed::voltChanged()
{
    m_voltPN = m_ePin[0]->getVolt()-m_ePin[1]->getVolt();

    double ThCurrent = m_current = 0;
    double admit = 1e-9;

    double deltaV = m_voltPN-m_threshold;
    if( deltaV > -1e-12 )   // Conducing
    {
        admit = 1/m_imped;
        ThCurrent = m_threshold*admit;
        if( deltaV > 0 ) m_current = deltaV*admit;
    }
    if( admit != m_admit ) eResistor::setAdmit( admit );

    if( ThCurrent == m_lastThCurrent ) { updateVI(); return; }
    m_lastThCurrent = ThCurrent;

    Simulator::self()->notCorverged();

    m_ePin[0]->stampCurrent( ThCurrent );
    m_ePin[1]->stampCurrent(-ThCurrent );
}

void eLed::updateVI()
{
    const uint64_t step = Simulator::self()->circTime();
    uint64_t period = (step-m_prevStep);
    m_prevStep = step;
    m_lastPeriod += period;

    if( m_lastCurrent > 0 ) m_totalCurrent += m_lastCurrent*period;
    m_lastCurrent = m_current;
}

void eLed::updateBright()
{
    if( !Simulator::self()->isRunning() )
    {
        m_totalCurrent = 0;
        m_lastPeriod = 0;
        m_intensity = 25;
        return;
    }
    updateVI();

    uint64_t sPF = Simulator::self()->realSPF();//stepsPerFrame();
    //uint64_t sPS = Simulator::self()->stepSize();

    if( m_lastPeriod > sPF/2 ) // Update 2 times per frame
    {
        m_avgCurrent = m_totalCurrent/m_lastPeriod;
        m_brightness = pow( m_avgCurrent/m_maxCurrent, 1.0/2.0 );

        m_totalCurrent  = 0;
        m_lastPeriod = 0;
        m_intensity  = uint32_t(m_brightness*255)+25;
}   }

void eLed::setRes( double resist )
{
    Simulator::self()->pauseSim();

    if( resist == 0 ) resist = 0.1;
    m_imped = resist;
    voltChanged();

    Simulator::self()->resumeSim();
}
