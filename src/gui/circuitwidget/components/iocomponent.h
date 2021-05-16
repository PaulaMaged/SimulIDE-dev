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

#ifndef IOCOMPONENT_H
#define IOCOMPONENT_H

#include "component.h"
//#include "e-element.h"
#include "iopin.h"

class MAINMODULE_EXPORT IoComponent : public Component//, public eElement
{
        Q_OBJECT
        Q_PROPERTY( quint64 Tpd_ps READ propDelay WRITE setPropDelay DESIGNABLE true USER true )
        Q_PROPERTY( quint64  Tr_ps READ riseTime WRITE setRiseTime DESIGNABLE true USER true )
        Q_PROPERTY( quint64  Tf_ps READ fallTime WRITE setFallTime DESIGNABLE true USER true )

        Q_PROPERTY( double Input_High_V READ inputHighV WRITE setInputHighV DESIGNABLE true USER true )
        Q_PROPERTY( double Input_Low_V  READ inputLowV  WRITE setInputLowV  DESIGNABLE true USER true )
        Q_PROPERTY( double Input_Imped  READ inputImp   WRITE setInputImp   DESIGNABLE true USER true )
        Q_PROPERTY( double Out_High_V   READ outHighV   WRITE setOutHighV   DESIGNABLE true USER true )
        Q_PROPERTY( double Out_Low_V    READ outLowV    WRITE setOutLowV    DESIGNABLE true USER true )
        Q_PROPERTY( double Out_Imped    READ outImp     WRITE setOutImp     DESIGNABLE true USER true )

    public:
        IoComponent( QObject* parent, QString type, QString id );
        ~IoComponent();

        virtual QList<propGroup_t> propGroups() override;

        void initState();
        void runOutputs();
        void sheduleOutPuts( eElement* el );

        double inputHighV() const          { return m_inHighV; }
        void  setInputHighV( double volt );

        double inputLowV() const          { return m_inLowV; }
        void  setInputLowV( double volt );

        double outHighV() const           { return m_ouHighV; }
        void  setOutHighV( double volt );

        double outLowV() const            { return m_ouLowV; }
        void  setOutLowV( double volt );

        double inputImp() const           { return m_inImp; }
        void  setInputImp( double imp );

        double outImp() const            { return m_ouImp; }
        void  setOutImp( double imp );

        bool inverted() { return m_inverted; }
        void setInverted( bool inverted );

        bool invertInps() { return m_invInputs; }
        void setInvertInps( bool invert );

        virtual uint64_t propDelay() { return m_propDelay; }
        virtual void     setPropDelay( uint64_t pd ) { m_propDelay = pd; }


        uint64_t riseTime() { return m_timeLH; }
        void setRiseTime( uint64_t time ) {m_timeLH = time; }

        uint64_t fallTime() { return m_timeHL; }
        void setFallTime( uint64_t time ) {m_timeHL = time; }

        int  numInps() const { return m_numInputs; }
        virtual void setNumInps( int inputs );

        int  numOuts() const { return m_numOutputs; }
        void setNumOuts( int outputs );


        void init( QStringList pins );
        void initPin( IoPin* pin );

    protected:
        //bool getInputState( int input );
        IoPin* createPin( QString data , QString id , QString label, pinMode_t mode );
        void deleteInputs( int inputs );
        void deleteOutputs( int inputs );

        int m_outValue;
        int m_nextOutVal;
        int m_outStep;

        uint64_t m_propDelay; // Propagation delay
        uint64_t m_timeLH;    // Time for Output voltage to switch from 10% to 90%
        uint64_t m_timeHL;    // Time for Output voltage to switch from 90% to 10%
        bool m_rndPD;         // Randomize Propagation Delay


        double m_inHighV;  // currently in eClockedDevice
        double m_inLowV;  // currently in eClockedDevice
        double m_ouHighV;
        double m_ouLowV;

        double m_inImp;  // currently in eClockedDevice
        double m_ouImp;

        bool m_inverted;
        bool m_invInputs;

        int m_width;
        int m_height;

        int m_numInputs;
        int m_numOutputs;

        std::vector<bool> m_inputState;
        std::vector<IoPin*> m_inPin;
        std::vector<IoPin*> m_outPin;
};

#endif