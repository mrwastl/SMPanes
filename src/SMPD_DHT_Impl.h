/*
 *************************************************************************
 *
 * Implementation part for SMPD_DHT.h
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


#include "SMPD_Base.h"

SMPD_DHT::SMPD_DHT (uint32_t updateInterval) : SMPD_Base(updateInterval) {
  this->setTemperatureUnit('C');
  this->temperature = 0.0;
  this->humidity = 0.0;
  this->humPrecision = 0;
}

void SMPD_DHT::setTemperatureUnit(char unit) {
  if (unit == 'F' || unit == 'K') {
    this->tempPrecision = 1;
    this->unit = unit;
  } else {
    this->tempPrecision = 0;
    this->unit = 'C';
  }
}

float SMPD_DHT::getTemperature() {
  switch (this->unit) {
    case 'K':
      return this->temperature + 273.15;
      break;
    case 'F':
      return this->temperature * 1.8 + 32.0;
      break;
    default:
      return this->temperature;
  }
}


float SMPD_DHT::getHumidity() {
  return this->humidity;
}
