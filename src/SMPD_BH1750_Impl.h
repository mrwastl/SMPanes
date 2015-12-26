/*
 *************************************************************************
 *
 * Implementation part for SMPD_BH1750.h
 * Class for BH1750 light sensor
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

SMPD_BH1750::SMPD_BH1750 (uint8_t address, uint32_t updateInterval)
: SMPD_LightSensor(updateInterval)
{
  this->sensor = NULL;
  this->address = address;
  this->mode = RESOLUTION_AUTO_HIGH;
  this->autoPowerDown = true;
  this->sensor = new AS_BH1750(address);
  if (this->sensor) {
    this->status = (this->sensor->begin(this->mode, this->autoPowerDown) ) ? OK : INITERR;
  } else {
    this->status = INITERR;
  }
}

SMPD_BH1750::~SMPD_BH1750 () {
  if (this->sensor) {
    free( this->sensor );
    this->sensor = NULL;
  }
}

bool SMPD_BH1750::update(uint32_t currMS) {
  if (currMS == 0) {
    currMS = millis();
  }

  //if ( !this->lastCheck ||  (currMS - this->lastCheck) > this->updateInterval) {
  if ( this->doUpdate(currMS) ) {
    this->lastCheck = currMS;
    if (! this->sensor) {
#if DATAPROVIDER_SERIAL_DEBUG == 1 && DATAPROVIDER_BH1750_DEBUG == 1
      Serial.println("BH1750: ERROR, sensor not initialised");
#endif
      return false;
    }

    if (sensor->isPresent()) {
      float newLux = sensor->readLightLevel();
      if (this->lux != newLux) {
        this->lux = newLux;
#if DATAPROVIDER_SERIAL_DEBUG == 1 && DATAPROVIDER_BH1750_DEBUG == 1
        Serial.print("BH1750: Status: OK, Lux: "); Serial.print(this->lux); Serial.print(", Normalised: "); Serial.println(this->getNormalised());
#endif
        this->status = OK;
        this->lastSuccessfulCheck = currMS;
        return true;
      }
    } else {
#if DATAPROVIDER_SERIAL_DEBUG == 1 && DATAPROVIDER_BH1750_DEBUG == 1
      Serial.println("BH1750: Status: ERROR, isPresent() = false!");
#endif
    }
    this->status = ERROR;
  }
  return false;
}

float SMPD_BH1750::getNormalised() {
  if (this->lux >= 10000.0) return 1.0;  // clip at 10000
  if (this->lux < 0.0) return 0.0; // clip negative lux values
  return log(this->lux+1.0)/log(10000.0); // +1 to avoid negative results (-> lux [0 .. 9999] -> [1 .. 10000])
}
