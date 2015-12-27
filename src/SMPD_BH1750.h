/*
 *************************************************************************
 *
 * SMPD_BH1750.h
 * Class for BH1750 light sensor
 *
 * Required non standard libraries:
 *   AS_BH1750:         https://github.com/hexenmeister/AS_BH1750
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

#ifndef SMPD_BH1750_h
#define SMPD_DH1750_h

#include "SMPD_LightSensor.h"
#include <Wire.h>
#include <AS_BH1750.h>

class SMPD_BH1750 : public SMPD_LightSensor {
  public:
                                 SMPD_BH1750(
                                   uint8_t address = BH1750_DEFAULT_I2CADDR,
                                   uint32_t updateInterval = 1000   // default update every second
                                 );
                                ~SMPD_BH1750();

    virtual bool                 update(uint32_t currMS = 0);
            float                getNormalised();
  protected:
            AS_BH1750          * sensor;
            uint8_t              address;
            sensors_resolution_t mode;
            bool                 autoPowerDown;
  private:
};

#include "SMPD_BH1750_Impl.h"

#endif // SMPD_BH1750_h