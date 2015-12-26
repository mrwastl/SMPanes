/*
 *************************************************************************
 *
 * Implementation part for SMPD_idDHT.h
 * Class for DHT11 / DHT22 sensors (interrupt driven sensor read)
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


//#include "SMPD_Base.h"

static idDHTLib * dht_workaround = NULL;

void dht11_wrapper() {
  if (dht_workaround) {
    dht_workaround->dht11Callback();
  }
}

void dht22_wrapper() {
  if (dht_workaround) {
    dht_workaround->dht22Callback();
  }
}


SMPD_idDHT::SMPD_idDHT (uint8_t dataPin, uint8_t interruptNr, bool dht22, uint32_t updateInterval) : SMPD_DHT(updateInterval) {
  this->dht = new idDHTLib(dataPin, interruptNr, (dht22) ? dht22_wrapper : dht11_wrapper);
  this->status = (this->dht) ? OK : INITERR;
  if (dht22) {
    this->tempPrecision = 1;
    this->humPrecision = 1;
  }
}

SMPD_idDHT::~SMPD_idDHT () {
  if (this->dht) {
    free( this->dht );
    this->dht = NULL;
  }
}

bool SMPD_idDHT::update(uint32_t currMS) {
  if (currMS == 0) {
    currMS = millis();
  }

  if (dht->acquiring()) {
#if DATAPROVIDER_SERIAL_DEBUG == 1 && DATAPROVIDER_idDHT_DEBUG == 1
      Serial.println("idDHT: ERROR, already aquiring");
#endif
    return false;
  }

  //if ( !this->lastCheck ||  (currMS - this->lastCheck) > this->updateInterval) {
  if ( this->doUpdate(currMS) ) {
    this->lastCheck = currMS;

    if (! this->dht) {
#if DATAPROVIDER_SERIAL_DEBUG == 1 && DATAPROVIDER_idDHT_DEBUG == 1
      Serial.println("idDHT: ERROR, sensor not initialised");
#endif
      return false;
    }


#if DATAPROVIDER_SERIAL_DEBUG == 1 && DATAPROVIDER_idDHT_DEBUG == 1
      Serial.print("idDHT: ACQUIRING ... ");
#endif
    dht_workaround = dht;
    int result = dht->acquireAndWait();
#if DATAPROVIDER_SERIAL_DEBUG == 1 && DATAPROVIDER_idDHT_DEBUG == 1
      Serial.println("done");
#endif

    switch(result) {
      case IDDHTLIB_OK:
        this->temperature = dht->getCelsius();
        this->humidity = dht->getHumidity();
        this->status = OK;
        this->lastSuccessfulCheck = currMS;
#if DATAPROVIDER_SERIAL_DEBUG == 1 && DATAPROVIDER_idDHT_DEBUG == 1
        Serial.print("idDHT: Status: OK, T/H: "); Serial.print(this->temperature); Serial.print("/"); Serial.println(this->humidity);
#endif
        break;
      case IDDHTLIB_ERROR_TIMEOUT:
        this->status = TIMEOUT;
#if DATAPROVIDER_SERIAL_DEBUG == 1 && DATAPROVIDER_idDHT_DEBUG == 1
        Serial.println("idDHT: Status: TIMEOUT");
#endif
        break;
      case IDDHTLIB_ERROR_CHECKSUM:
      case IDDHTLIB_ERROR_ACQUIRING:
      case IDDHTLIB_ERROR_DELTA:
      case IDDHTLIB_ERROR_NOTSTARTED:
        this->status = ERROR;
#if DATAPROVIDER_SERIAL_DEBUG == 1 && DATAPROVIDER_idDHT_DEBUG == 1
        Serial.println("idDHT: Status: ERROR");
#endif
        break;
      default:
        this->status = UNKNOWN;
#if DATAPROVIDER_SERIAL_DEBUG == 1 && DATAPROVIDER_idDHT_DEBUG == 1
        Serial.println("idDHT: Status: UNKNOWN");
#endif
    }
    return true;
  }
  return false;
}
