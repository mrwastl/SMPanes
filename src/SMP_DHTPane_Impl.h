/*
 *************************************************************************
 *
 * Implementation part for SMP_DHTPane.h
 * Pane for displaying temperature / humidity information provided by a DHT-sensor
 *
 *************************************************************************
 *
 * copyright (C)  2015-2016  wolfgang astleitner
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
#include "SMP_DHTPane.h"

template <typename smpRGB>
SMP_DHTPane<smpRGB>::SMP_DHTPane(uint16_t width, uint16_t height, smpRGB* rgbBuffer) :
    SMP_TextPane<smpRGB>::SMP_TextPane(width, height, rgbBuffer) {
  this->feature = Celsius;
  this->dataProvider = NULL;
  this->oldValue = 999.0;
  this->status = OK;
}


template <typename smpRGB>
void SMP_DHTPane<smpRGB>::setFeature(DHTFeature feature, bool showUnit) {
  this->feature = feature;
  this->showUnit = showUnit;
  switch(this->feature) {
    case Humidity:
      unitText = "%";
      break;
    case Fahrenheit:
      unitText = "\260F"; // \260 == degree sign (would be interpreted as unicode if put directly in here)
      break;
    case Kelvin:
      unitText = "K";
      break;
    default: // Celsius
      unitText = "\260C";
  }
}


template <typename smpRGB>
void SMP_DHTPane<smpRGB>::updateContent(uint32_t currMS) {
  float value = 0;
  uint8_t precision = 0;
  if (this->active && this->dataProvider) {

    this->contentChanged = this->dataProvider->update(/*currMS*/);
    this->status = this->dataProvider->getStatus();

    // only change value if status is OK
    if (this->status == OK) {
      switch(this->feature) {
        case Humidity:
          precision = this->dataProvider->getHumidityPrecision();
          value = this->dataProvider->getHumidity();
          break;
        default: // all temperatures
          precision = this->dataProvider->getTemperaturePrecision();
          value = this->dataProvider->getTemperature();
      }
    }

    this->contentChanged = abs(value - this->oldValue) > 0.05;
    if (this->contentChanged) {
      oldValue = value;
    }
  }
  this->message = ( (this->status == OK) ? String(value, precision) : "-" ) + ((this->showUnit) ? this->unitText : "");
}


template <typename smpRGB>
void SMP_DHTPane<smpRGB>::drawContent() {
  uint16_t tx = 0;
  uint16_t ty = 0;
  uint16_t tw = this->w;
  uint16_t th = this->h;
  bool     border = false;

  border = this->getTextAlignTrans(&tx, &ty, &tw, &th, this->message.length());

  /* prepare drawing pane */
  if (border) {
    ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->fillRectangle(0, 0, this->w - 1, this->h - 1, this->borderCol, this->bgCol);
  } else {
    ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->fillScreen(this->bgCol);
  }

  /* write dataProvider value */
  /* ARRGGHH: font is static in background layer class: thus set font before writing ... */
  ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->setFont( this->fontChoice );
  ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->drawString(tx, ty, this->fgCol, this->message.c_str());
}
