/*
 *************************************************************************
 *
 * Implementation part for SMP_MonitorPane.h
 * Pane for displaying monitoring information (e.g. coming from nagios)
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

#include "SMP_MonitorPane.h"

template <typename smpRGB>
SMP_MonitorPane<smpRGB>::SMP_MonitorPane(uint16_t width, uint16_t height, smpRGB* rgbBuffer) :
    SMP_TextPane<smpRGB>::SMP_TextPane(width, height, rgbBuffer) {
  this->drawWidth = 0;
  this->expireTime = 60 * 15;  // no data after 15 minutes: error
  this->lastUpdate = 0;
  this->saneData = true;
  this->showChangedDuration = 60 * 2;  // show changed data flag for at least 2 minutes
  this->expiredData = false;
  this->lastMS = 0;

  this->okCol = rgb24(0x00, 0xFF, 0x00);
  this->errCol = rgb24(0xFF, 0x00, 0x00);
  this->chgCol = rgb24(0x77, 0x77, 0x77);

  setColumnParams(0, 'C', rgb24(0xFF, 0x00, 0x00) );
  setColumnParams(1, 'W', rgb24(0xFF, 0xFF, 0x00) );
  setColumnParams(2, 'U', rgb24(0xFF, 0x00, 0xFF) );
  setColumnParams(3, 'D', rgb24(0x99, 0x99, 0x99), rgb24(0x77, 0x00, 0x00) );
}


template <typename smpRGB>
bool SMP_MonitorPane<smpRGB>::setColumnParams(uint8_t column, char id, smpRGB fg, smpRGB bg) {
  if (column < MONITORING_COLUMNS) {
    this->monValue[column] = 0;

    this->monID[column] = id;
    this->monFGCol[column] = fg;
    this->monBGCol[column] = bg;
    this->monLastChanged[column] = UINT32_MAX - (this->showChangedDuration * 1001);
    return true;
  }
  return false;
}


template <typename smpRGB>
void SMP_MonitorPane<smpRGB>::updateContent(uint32_t currMS) {
  uint8_t i;

  this->expiredData = (this->lastUpdate) ? false : true;

  if ( ! (this->active && this->contentLayer) )
    return;

  if (!currMS) {
    currMS = millis();
  }

  this->lastMS = currMS; // save timestamp for drawContent()

  // data expired: force redraw (erase all content) + invalidate data
  if ( this->lastUpdate && (this->lastUpdate + (this->expireTime * 1000) < this->lastMS)) {
    this->expiredData = true;
    this->lastUpdate = 0;
    this->contentChanged = true;
    for (i = 0; i < MONITORING_COLUMNS; i++) {
      this->monValue[i] = 0; // reset
    }
  }

}


template <typename smpRGB>
void SMP_MonitorPane<smpRGB>::drawContent() {
  uint8_t i;
  uint16_t tx = 0;
  uint16_t ty = 0;
  uint16_t tw = this->w;
  uint16_t th = this->h;
  bool border = false;

  border = this->getTextAlignTrans(&tx, &ty, &tw, &th);

  if (this->contentChanged && this->contentLayer) {
    uint16_t pos = 0;

    if (border) {
      ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->fillRectangle(0, 0, this->w - 1, this->h - 1, this->borderCol, this->bgCol);
    } else {
      ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->fillScreen(this->bgCol);
    }
    /* ARRGGHH: font is static in background layer class: thus set font before writing ... */
    ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->setFont( this->fontChoice );
    pos = tx;
    if (this->saneData) {
      if ( ! this->expiredData ) {
        bool isFirst = true;
        for(i = 0; i < MONITORING_COLUMNS; i++) {
          if (this->monValue[i]) {
            if (! isFirst) {
              pos += this->font->Width; // space
            }
            isFirst = false;
            ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->fillRectangle(pos-1, ty-1, pos+this->font->Width-1, ty+this->font->Height-1, this->monBGCol[i]);
            if (this->monValue[i] < 100) {
              ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->drawString(pos, ty, this->monFGCol[i], String(this->monValue[i]).c_str());
            } else {
              ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->drawString(pos, ty, this->monFGCol[i], "##");
            }
            pos += this->font->Width * ( (this->monValue[i] > 9) ? 2 : 1);
            if ( (this->monLastChanged[i] + this->showChangedDuration * 1000) > this->lastMS ) {
              ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->drawString(pos-1, ty - (this->font->Height / 2) + 1, this->chgCol, ".");
              //pos += this->font->Width;
            }
          }
        }
        if (pos == tx) { // all values == 0 -> print OK
          ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->drawString(pos, ty, this->okCol, "OK");
        }
      }
    } else {
      ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->drawString(pos, ty, this->errCol, "ERR");
    }
  }
}




template <typename smpRGB>
void SMP_MonitorPane<smpRGB>::setMessage(String message, uint32_t currMS) {
  if (1==1 || this->message != message) {
    if (!currMS) {
      currMS = millis();
    }

    size_t len = message.length();
    size_t strPos = 0;
    int idx = 0;
    char currColumn = ' ';
    int currVal = 0;
    bool err = false;
    bool found = false;

    bool colChanged[MONITORING_COLUMNS];

    uint8_t totalChars = 0;

    for (idx = 0; idx < MONITORING_COLUMNS; idx ++) {
      colChanged[idx] = false;
    }

#if PANE_SERIAL_DEBUG == 1 && PANE_MONITOR_DEBUG > 0
    Serial.print("SMP_MonitorPane::setMessage(): monitor matrix changed to: "); Serial.println(message);
#endif

    while ( (!err) && (strPos < len) ) {
      idx = message.indexOf(':', strPos);

      if (idx < 0) {
        currColumn = message.charAt(strPos);
        strPos ++;
        if (strPos >= len ) {
          err = true;
        } else {
          currVal = message.substring(strPos).toInt();
          strPos = len;
        }
      } else {
        currColumn = message.charAt(strPos);
        strPos ++;
        if (strPos - idx == 0) {
          err = true;
        } else {
          currVal = message.substring(strPos, idx).toInt();
          strPos = idx + 1;
        }
      }

      found = false;
      idx = 0;
      while ( ! found && idx < MONITORING_COLUMNS ) {
        if( this->monID[idx] == currColumn ) {
          if ( this->monValue[idx] != (uint8_t)currVal ) {
            this->monLastChanged[idx] = currMS;
          }
          this->monValue[idx] = (uint8_t)currVal;
          colChanged[idx] = true;
          found = true;
        }
        idx ++;
      }
    }


    if (!err) {
      int lastIdx = -1;

      for (idx = 0; idx < MONITORING_COLUMNS; idx++) {
        if ( !colChanged[idx] ) { // implicit 0
          this->monLastChanged[idx] = 0;
          this->monValue[idx] = 0;
        }
#if PANE_SERIAL_DEBUG == 1 && PANE_MONITOR_DEBUG > 0
        Serial.print("  "); Serial.print(this->monID[idx]); Serial.print(": "); Serial.print(this->monValue[idx]);
        if ( (this->monLastChanged[idx] + this->showChangedDuration * 1000) > currMS ) {
          Serial.println(" *");
        } else {
          Serial.println();
        }
#endif
        if (this->monValue[idx]) {
          totalChars += (this->monValue[idx] > 9) ? 2 : 1;
          if ( (this->monLastChanged[idx] + this->showChangedDuration * 1000) > currMS ) {
            lastIdx = idx;  // print * instead of space (exception: if last column value > 0 contains changed value)
          } else {
            lastIdx = -1;
          }
          totalChars ++;  // space
        }
      }
      if (lastIdx >= 0 && ( (this->monLastChanged[idx] + this->showChangedDuration * 1000) > currMS )) {
        totalChars++; // exception: append * at the end if last column value > 0 contains changed value
      }
    }

    this->saneData = !err;

    if (!err) {
      if (totalChars) {
        totalChars--; // cut trailing space
        this->drawWidth = this->font->Width * totalChars;
      } else {
        this->drawWidth = this->font->Width * 2;  // OK
      }
    } else {
      this->drawWidth = this->font->Width * 3; // ERR
    }

    this->contentChanged = true;
    this->message = message;
  }
  this->lastUpdate = currMS;
}



template <typename smpRGB>
bool SMP_MonitorPane<smpRGB>::getTextAlignTrans(uint16_t* tX, uint16_t* tY, uint16_t* tW, uint16_t* tH, uint8_t dummy) {
  uint16_t strW = 0;
  uint16_t strH = 0;

  int16_t pX = 0;
  int16_t pY = 0;

  *tX = 0;
  *tY = 0;
  *tW = this->w;
  *tH = this->h;

  if (this->drawBorder) {
    *tX += 2;
    *tY += 2;
    *tW -= 4;
    *tH -= 4;
  }

  strW = this->drawWidth;
  strH = this->font->Height;

  switch(this->textAlignHor) {
    case center:
      pX = ( (*tW) - strW) / 2.0;
      break;
    case right:
      pX = ( (*tW) - strW );
      break;
    default: // left
      ; // do nothing
  }

  switch(this->textAlignVert) {
    case middle:
      pY = ( (*tH) - (strH - 1)) / 2.0;  // ignore descender  (g, p, j, ...)
      break;
    case bottom:
      pY = ( (*tH) - strH);
      break;
    default: // left
      ; // do nothing
  }

  if (pX > 0) *tX += pX;
  if (pY > 0) *tY += pY;

  return this->drawBorder;
}
