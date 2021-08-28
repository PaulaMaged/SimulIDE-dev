/***************************************************************************
 *   Copyright (C) 2012 by santiago González                               *
 *   santigoro@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
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

#ifndef EDIODE_H
#define EDIODE_H

#include "e-resistor.h"

class MAINMODULE_EXPORT eDiode : public eResistor
{
    public:
        eDiode( QString id );
        ~eDiode();

        virtual double threshold() { return m_threshold; }
        virtual void  setThreshold( double threshold ) { m_threshold = threshold; }
        
        virtual double zenerV(){ return m_zenerV; }
        virtual void  setZenerV( double zenerV );

        virtual void stamp() override;
        virtual void initialize() override;

        virtual void voltChanged() override;

        virtual void   setRes( double resist );
        virtual double res() { return m_imped; }

    protected:
        void updateVI();

        double m_voltPN;
        double m_deltaV;
        double m_threshold;
        double m_imped;
        double m_zenerV;
        double m_lastCurrent;

        static constexpr double off_imp  = 1e14;
};
#endif


