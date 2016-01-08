/*
 *************************************************************************
 *
 * Implementation part for SMP_DateTimePane.h
 * Pane for displaying date / time with 7 segment-like digits
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

#include "SMP_DateTimePane.h"

template <typename smpRGB>
SMP_DateTimePane<smpRGB>::SMP_DateTimePane(uint16_t width, uint16_t height, smpRGB* rgbBuffer) :
    SMP_TextPane<smpRGB>::SMP_TextPane(width, height, rgbBuffer) {
  this->dataProvider = NULL;

  this->format = HHmm;
  this->timeSeparator = tsBlinking;
  this->dateSeparator = dsDash;

  this->segThick = 1;
  this->segWidth = 3;
  this->segHeight = this->segWidth * 2 - this->segThick;
  this->gapCount = 0;
  this->drawFX = false;
  this->fgColFX = rgb24(0xFF, 0xFF, 0xFF);

  this->oldMin = 255;
  this->oldSec = 255;
  this->oldDay = 255;
  this->drawWidth = 0;
  this->calculateSizes();
}


template <typename smpRGB>
void SMP_DateTimePane<smpRGB>::setFormat(DateTimeFormat format, TimeSeparator timeSeparator, DateSeparator dateSeparator) {
  this->format = format;
  this->timeSeparator = timeSeparator;
  this->dateSeparator = dateSeparator;
  this->calculateSizes();
}


template <typename smpRGB>
void SMP_DateTimePane<smpRGB>::setFX(uint8_t percent) {
  if (percent > 0 && percent < 100) {
    this->drawFX = true;
    this->fgColFX.red = (this->fgCol.red * percent) / 100;
    this->fgColFX.green = (this->fgCol.green * percent) / 100;
    this->fgColFX.blue = (this->fgCol.blue * percent) / 100;
  } else {
    this->drawFX = false;
  }
}


template <typename smpRGB>
void SMP_DateTimePane<smpRGB>::setFX(smpRGB fxCol) {
  if ( ! (this->fgCol.red == fxCol.red && this->fgCol.green == fxCol.green && this->fgCol.blue == fxCol.blue)) {
    this->drawFX = true;
    this->fgColFX = fxCol;
  } else {
    this->drawFX = false;
  }
}


template <typename smpRGB>
uint8_t SMP_DateTimePane<smpRGB>::calculateSizes() {
  uint8_t maxW = this->w - ((this->drawBorder) ? 4 : 0);
  uint8_t maxH = this->h - ((this->drawBorder) ? 4 : 0);

  uint8_t digits = 4;
  uint8_t separators = 1;
  uint8_t gapCount;
  if (this->format < DDMM) {
    if (this->format >= HHmmss && this->format <= hhmmss) {
      digits += 2;
      separators += 1;
    }
  } else {
    if (this->format >= YYYYMMDD) {
      digits += 4;
      separators += 1;
    }
  }

  gapCount = digits - 1 + 2 * separators;

  // estimate best fitting thickness of segments
  uint8_t initEst = maxW / (digits + separators / 2);
  if (maxH < initEst) {
    initEst = maxH;
  }
  this->segThick = initEst / 5; // 5 horizontal bars = total segment height

  // calculate width and height of segments
  //this->segWidth = ( maxW - ( (this->secondsMode == 1) ? 9 : 5) * this->segThick ) / ((this->secondsMode == 1) ? 6 : 4);
  this->segWidth = (maxW -  this->segThick * gapCount ) / digits;

  if (this->segWidth * 2 - this->segThick > maxH) {
    this->segWidth = (maxH + this->segThick ) / 2;
  }

  this->gapCount = gapCount;

  this->drawWidth = this->segWidth * digits + this->gapCount * this->segThick;
  this->segHeight = this->segWidth * 2 - this->segThick;

#if PANE_SERIAL_DEBUG == 1 && PANE_DATETIME_DEBUG > 0
  if (Serial) {
    Serial.println("calculateSizes():");
    Serial.print("  Digits/Separators/GapCount/initEst/DrawWidth: ");
      Serial.print(digits); Serial.print(" / "); Serial.print(separators);
      Serial.print(" / "); Serial.print(gapCount);
      Serial.print(" / "); Serial.print(initEst);
      Serial.print(" / "); Serial.println(this->drawWidth);
    Serial.print("  SegWidth/SegHeight/SegThick: "); Serial.print(this->segWidth); Serial.print(" / "); 
      Serial.print(this->segHeight); Serial.print(" / ");  Serial.println(this->segThick); 
  }
#endif
  SMP_TextPane::calculateSizes(); // call parent method (because of calc. font-size if autoFont)
  return digits;
}


template <typename smpRGB>
void SMP_DateTimePane<smpRGB>::forceSegmentSettings(uint8_t segWidth, uint8_t segThick) {
  uint8_t digits = calculateSizes();

  this->segThick = segThick;
  this->segWidth = segWidth;

  this->drawWidth = this->segWidth * digits + this->gapCount * this->segThick;
  this->segHeight = this->segWidth * 2 - this->segThick;
#if PANE_SERIAL_DEBUG == 1 && PANE_DATETIME_DEBUG > 0
  if (Serial) {
    Serial.println("forceSegmentSettings():");
    Serial.print("  Digits/GapCount/DrawWidth: ");
      Serial.print(digits);
      Serial.print(" / "); Serial.print(this->gapCount);
      Serial.print(" / "); Serial.println(this->drawWidth);
    Serial.print("  SegWidth/SegHeight/SegThick: "); Serial.print(this->segWidth); Serial.print(" / "); 
      Serial.print(this->segHeight); Serial.print(" / ");  Serial.println(this->segThick); 
  }
#endif
}


template <typename smpRGB>
void SMP_DateTimePane<smpRGB>::updateContent(uint32_t currMS) {
  if ( ! (this->active && this->contentLayer) )
    return;

  uint32_t currTime = this->dataProvider->getEpochTime();
  uint8_t colonPos = 0;
  uint16_t tx = 0;
  uint16_t ty = 0;
  uint16_t tw = this->w;
  uint16_t th = this->h;
  bool border = false;

  if (this->format < DDMM) {
    uint8_t  m = minute(currTime);
    uint8_t  s = second(currTime);
    uint8_t  h = hour(currTime);
    bool showSeconds = (this->format >= HHmmss && this->format <= hhmmss);
    bool changeSecs = showSeconds || (this->timeSeparator == tsBlinking);

    this->contentChanged = ((changeSecs) && (oldSec != s)) || (oldMin != m);
    if ( this->contentChanged ) {
      oldSec = s;
      oldMin = m;


      switch (this->timeSeparator) {
        case  tsColon:
        case tsBlinking:
          colonPos = (this->segHeight - 2 * this->segThick ) / 3;
          break;
        default:
          colonPos = this->segHeight - this->segThick;
      }

      border = this->getTextAlignTrans(&tx, &ty, &tw, &th);

      if (border) {
        ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->fillRectangle(0, 0, this->w - 1, this->h - 1, this->borderCol, this->bgCol);
      } else {
        ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->fillScreen(this->bgCol);
      }

      tx += this->drawNumber(tx, ty, h, 2);
      tx += this->segThick;

      if ( showSeconds || ( (this->timeSeparator == tsBlinking) && (s % 2) ) ) {
        this->drawBar(tx, ty + colonPos, this->segThick, this->segThick, false, this->fgCol);
        this->drawBar(tx, ty + this->segHeight - this->segThick - colonPos, this->segThick, this->segThick, false, this->fgCol);
      } else {
//        this->drawBar(tx, ty + colonPos, this->segThick, this->segThick, false, this->fgColFX);
//        this->drawBar(tx, ty + this->segHeight - this->segThick - colonPos, this->segThick, this->segThick, false, this->fgColFX);
        this->drawBar(tx, ty + colonPos, this->segThick, this->segThick, false, this->bgCol);
        this->drawBar(tx, ty + this->segHeight - this->segThick - colonPos, this->segThick, this->segThick, false, this->bgCol);
      }
      tx += this->segThick + this->segThick;
      tx += this->drawNumber(tx, ty, m, 2);
      if (showSeconds == 1) {
        tx += this->segThick;
        this->drawBar(tx, ty + colonPos, this->segThick, this->segThick, false, this->fgCol);
        this->drawBar(tx, ty + this->segHeight - this->segThick - colonPos, this->segThick, this->segThick, false, this->fgCol);
        tx += this->segThick + this->segThick;
        tx += this->drawNumber(tx, ty, s, 2);
      }
    }
  } else {
    uint8_t  d = day(currTime);
    uint8_t  m = month(currTime);
    uint16_t  y = year(currTime);

    this->contentChanged = (oldDay != d);
    if ( this->contentChanged ) {
      oldDay = d;

      switch (this->dateSeparator) {
        case dsDash:
          colonPos = this->segHeight / 2;
          break;
        default:
          colonPos = this->segHeight - this->segThick;
      }

      border = this->getTextAlignTrans(&tx, &ty, &tw, &th);

      if (border) {
        ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->fillRectangle(0, 0, this->w - 1, this->h - 1, this->borderCol, this->bgCol);
      } else {
        ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->fillScreen(this->bgCol);
      }

      tx += this->drawNumber(tx, ty, d, 2);
      tx += this->segThick;
      this->drawBar(tx, ty + colonPos, this->segThick, this->segThick, false, this->fgCol);
      tx += this->segThick + this->segThick;
      tx += this->drawNumber(tx, ty, m, 2);
      if (this->format >= YYYYMMDD) {
        tx += this->segThick;
        this->drawBar(tx, ty + colonPos, this->segThick, this->segThick, false, this->fgCol);
        tx += this->segThick + this->segThick;
        tx += this->drawNumber(tx, ty, y, 4);
      }
    }
  }
}


template <typename smpRGB>
bool SMP_DateTimePane<smpRGB>::getTextAlignTrans(uint16_t* tX, uint16_t* tY, uint16_t* tW, uint16_t* tH, uint8_t dummy) {
  uint16_t drawW = 0;
  uint16_t drawH = 0;

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

  drawW = this->drawWidth;
  drawH = this->segHeight;

  switch(this->textAlignHor) {
    case center:
      pX = ( (*tW) - drawW) / 2.0;
      break;
    case right:
      pX = ( (*tW) - drawW );
      break;
    default: // left
      ; // do nothing
  }

  switch(this->textAlignVert) {
    case middle:
      pY = ( (*tH) - drawH) / 2.0;
      break;
    case bottom:
      pY = ( (*tH) - drawH);
      break;
    default: // left
      ; // do nothing
  }

  if (pX > 0) *tX += pX;
  if (pY > 0) *tY += pY;

  return this->drawBorder;
}

/* taken from my library 'serdisplib', testserdisp,c */

template <typename smpRGB>
void SMP_DateTimePane<smpRGB>::drawBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool hor, smpRGB col) {
  int i,j;
  for (j = y; j < y + ((hor)? h : w); j ++)
    for (i = x; i < x + ((hor)? w : h); i ++)
      ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->drawPixel(i, j, col);
}


template <typename smpRGB>
void SMP_DateTimePane<smpRGB>::drawDigit(uint16_t x, uint16_t y, int8_t digit) {
  uint8_t segwidth = this->segWidth;
  uint8_t thick = this->segThick;

  if (digit < 0 || digit > 9) return;

  // draw 'passive' 8
  if ( this->drawFX ) {
    this->drawBar(x,                    y,                        segwidth, thick, true, this->fgColFX);
    this->drawBar(x,                    y + segwidth - thick,     segwidth, thick, true, this->fgColFX);
    this->drawBar(x,                    y + 2*(segwidth - thick), segwidth, thick, true, this->fgColFX);

    this->drawBar(x,                    y,                        segwidth, thick, false, this->fgColFX);
    this->drawBar(x + segwidth - thick, y,                        segwidth, thick, false, this->fgColFX);
    this->drawBar(x,                    y + segwidth - thick,     segwidth, thick, false, this->fgColFX);
    this->drawBar(x + segwidth - thick, y + segwidth - thick,     segwidth, thick, false, this->fgColFX);
  }

  // draw active digit
  if ( digit != 1 && digit != 4 )
    this->drawBar(x,                    y,                        segwidth, thick, true, this->fgCol);
  if ( digit != 1 && digit != 7 && digit != 0 )
    this->drawBar(x,                    y + segwidth - thick,     segwidth, thick, true, this->fgCol);
  if ( digit != 1 && digit != 4 && digit != 7 )
    this->drawBar(x,                    y + 2*(segwidth - thick), segwidth, thick, true, this->fgCol);

  if ( digit != 1 && digit != 2 && digit != 3 && digit != 7 )
    this->drawBar(x,                    y,                        segwidth, thick, false, this->fgCol);
  if ( digit != 5 && digit != 6 )
    this->drawBar(x + segwidth - thick, y,                        segwidth, thick, false, this->fgCol);
  if ( digit == 2 || digit == 6 || digit == 8 || digit == 0 )
    this->drawBar(x,                    y + segwidth - thick,     segwidth, thick, false, this->fgCol);
  if ( digit != 2 )
    this->drawBar(x + segwidth - thick, y + segwidth - thick,     segwidth, thick, false, this->fgCol);
}


template <typename smpRGB>
uint16_t SMP_DateTimePane<smpRGB>::drawNumber(uint16_t x, uint16_t y, int16_t number, uint8_t digits) {
  uint8_t segwidth = this->segWidth;
  uint8_t thick = this->segThick;
  int i, posx = x + (segwidth + thick ) * (digits - 1);

  int16_t num = number;

  if (num < 0 || num > 9999) return 0;

  for (i = digits-1; i >= 0; i--) {
    this->drawDigit(posx, y, num % 10);
    posx -= segwidth + thick;
    num /= 10;
  }
  return ( segwidth * digits + thick * (digits - 1) );
}

