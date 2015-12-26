/*
 *************************************************************************
 *
 * SMPD_DHT.h
 * Abstract base class for DHT sensor classes (temperature / humidity)
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

#ifndef SMPD_DHT_h
#define SMPD_DHT_h

#include "SMPD_Base.h"

class SMPD_DHT : public SMPD_Base {
  public:
    virtual bool    update(uint32_t currMS = 0)  = 0;

            void    setTemperatureUnit(char unit = 'C');

            float   getTemperature();
            float   getHumidity();
            
            uint8_t getTemperaturePrecision() { return this->tempPrecision; }
            uint8_t getHumidityPrecision() { return this->humPrecision; }

  protected:
                    SMPD_DHT(uint32_t updateInterval = (30 * 1000) );  // default update every 30 seconds
            char    unit;
            uint8_t tempPrecision;
            uint8_t humPrecision;
            float   temperature;
            float   humidity;
  private:
};

#include "SMPD_DHT_Impl.h"

#endif // SMPD_DHT_h