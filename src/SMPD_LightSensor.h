/*
 *************************************************************************
 *
 * SMPD_LightSensor.h
 * Abstract base class for light sensor classes
 *
 *************************************************************************
 *
 * copyright (C)  2015  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *
 *************************************************************************
 * This program is free software; you can redistribute it and/or modify   
 * it under the terms of the GNU General Public License as published by   
 * the Free Software Foundation; either version 2 of the License, or (at  
 * your option) any later version.                                        
 *                                                                        
 * This program is distributed in the hope that it will be useful, but    
 * WITHOUT ANY WARRANTY; without even the implied warranty of             
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      
 * General Public License for more details.                               
 *                                                                        
 * You should have received a copy of the GNU General Public License      
 * along with this program; if not, write to the Free Software            
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA              
 * 02111-1307, USA.  Or, point your browser to                            
 * http://www.gnu.org/copyleft/gpl.html                                   
 *************************************************************************
 */

#ifndef SMPD_LIGHTSENSOR_h
#define SMPD_LIGHTSENSOR_h

#include "SMPD_Base.h"

class SMPD_LightSensor : public SMPD_Base {
  public:
    virtual bool    update(uint32_t currMS = 0)  = 0;

    virtual float   getNormalised() = 0;
            float   getValue() { return this->lux; };

            uint8_t getPrecision() { return this->precision; }

  protected:
                    SMPD_LightSensor(uint32_t updateInterval = (30 * 1000) );  // default update every 30 seconds
            uint8_t precision;
            float   lux;
  private:
};

#include "SMPD_LightSensor_Impl.h"

#endif // SMPD_LIGHTSENSOR_h