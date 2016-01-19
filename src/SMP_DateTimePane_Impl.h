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

  this->message = "%H::%M";   // default: hour, blinking ':', minute
  this->resolution = 0;       // 0: blinking separator every second: 1: seconds, 2: minutes, 3: hours
  this->showSec = false;
  this->showMin = true;

  this->segThick = 1;
  this->segWidth = 3;
  this->segHeight = this->segWidth * 2 - this->segThick;
  this->segTCount = 0;
  this->digitCount = 0;
  this->drawFX = false;
  this->fgColFX = rgb24(0xFF, 0xFF, 0xFF);

  this->forceSegWidth = 0;
  this->forceSegThick = 0;

  this->oldMin = 60;
  this->oldSec = 61;
  this->oldDay = 32;
  this->oldMon = 13;
  this->oldYear = 0;

  this->drawWidth = 0;
  this->processFormat();
  this->calculateSizes();
}


template <typename smpRGB>
bool SMP_DateTimePane<smpRGB>::setFormat(String format) {
  this->message = format;
  this->processFormat();
  return (this->calculateSizes() > 0);
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
bool SMP_DateTimePane<smpRGB>::processFormat() {
  int idx, pos;
  char c;
  bool valid = true;
  bool isPercent = false;

  this->dateTimeElems = 0;
  this->resolution = 3;
  this->showSec = false;
  this->showMin = false;

  for (idx = 0; idx < MAX_DATETIME_ELEMS - 1; idx++) {
    this->separatorCode[idx] = 'x';
    this->separatorWidth[idx] = 0;
  }

  pos = 0;
  while (valid && (pos < this->message.length())) {
    c = this->message[pos];
    pos++;

    if (isPercent) {
      if (this->dateTimeElems >= MAX_DATETIME_ELEMS) {
        valid = false;
        break;
      }

      switch(c) {
        case 'S':
          if (this->resolution > 1) this->resolution = 1; // refresh at least every second
          this->showSec = true;
        case 'M':
          if (this->resolution > 2) this->resolution = 2; // refresh at least every minute
          this->showMin = true;
        case 'I':
        case 'H':
        case 'd':
        case 'm':
        case 'y':
        case 'Y':
          this->formatCode[this->dateTimeElems] = c;
          this->formatSize[this->dateTimeElems] = (c == 'Y') ? 4 : 2;
          this->dateTimeElems ++;
          break;
        default:
          valid = false;
      }
      isPercent = false;
    } else {
      switch(c) {
        case '%':
          isPercent = true;
          break;
        case '-':
        case '.':
        case '/':
          if ( ( this->dateTimeElems > 0 && (this->dateTimeElems - 1) < (MAX_DATETIME_ELEMS - 1)) && 
               (this->separatorCode[this->dateTimeElems - 1] == 'x')
             ) {
            this->separatorCode[this->dateTimeElems - 1] = c;
            this->separatorWidth[this->dateTimeElems - 1] = (c == '-') ? 3 : 1;
          } else {
              valid = false;
          }
          break;
        case ' ':
        case ':':
          if ( this->dateTimeElems > 0 && (this->dateTimeElems - 1) < (MAX_DATETIME_ELEMS - 1)) {
            if (this->separatorCode[this->dateTimeElems - 1] == 'x' || this->separatorCode[this->dateTimeElems - 1] == c) {
              if (c == ' ') {
                this->separatorCode[this->dateTimeElems - 1] = c;
                this->separatorWidth[this->dateTimeElems - 1] += 3;
              } else {  // c == ':'
                if (this->separatorCode[this->dateTimeElems - 1] == ':') {
                  this->resolution = 0; // refresh every second and blink separator
                } else {
                  this->separatorCode[this->dateTimeElems - 1] = c;
                  this->separatorWidth[this->dateTimeElems - 1] = 1;
                }
              }
            } else {
              valid = false;
            }
          } else {
            valid = false;
          }
          break;
        default:
          valid = false;
      }
    }
  }

  return valid;
}


template <typename smpRGB>
uint8_t SMP_DateTimePane<smpRGB>::calculateSizes() {
  int idx;

  uint8_t separatorT = 0;

  /*
   * calculating width using segment thickness ( = 1 T)
   *
   * TTTT
   * ||||
   * vvvv
   * ***
   * * *
   * ***
   * * *
   * ***
   *    ^
   *    |
   *    gap to next char / separator
   */

  this->digitCount = 0;
  this->segTCount = 0;

  for (idx = 0; idx < this->dateTimeElems; idx++) {
    this->segTCount += this->formatSize[idx] * 3;   // one digit = 3 T
    this->digitCount += this->formatSize[idx];
  }
  this->segTCount += (this->digitCount - 1);  // add gap (1 T) to next digit / separator


  for (idx = 0; idx < this->dateTimeElems - 1; idx++) {
    if (this->separatorCode[idx] != 'x') {
      separatorT += this->separatorWidth[idx] + 1; // separator width (between 1 and 3 T) + gap (1 T) to next digit
    }
  }

  this->segTCount += separatorT; // add separator width + gap to next digit


  if (this->forceSegThick && this->forceSegWidth) {
    this->segThick = this->forceSegThick;
    this->segWidth = this->forceSegWidth;

    this->segHeight = this->segWidth * 2 - this->segThick;
  } else{
    uint8_t maxW = this->w - ((this->drawBorder) ? 4 : 0);
    uint8_t maxH = this->h - ((this->drawBorder) ? 4 : 0);


    // estimate best fitting thickness of segments
    this->segThick = maxW / this->segTCount;

    if (this->segThick * 5 > maxH) {
      this->segThick = maxH / 5;
    }

    this->segWidth = this->segThick * 3; // + ( (maxW - this->segTCount * this->segThick) / this->digitCount);
    // temporary drawWidth, will be recalced below
    this->drawWidth = 
      this->segWidth * this->digitCount +  // width of all digits
      this->segThick * (this->digitCount - 1) + // gaps to next digit (ignoring separators)
      separatorT * this->segThick; // adding separator widths + gaps
    // split remaining space to segwidth of digits
    this->segWidth += (maxW - this->drawWidth) / this->digitCount;

    this->segHeight = this->segWidth * 2 - this->segThick;
    if (this->segHeight > maxH) {
      this->segWidth = maxH / 2 + this->segThick;
      this->segHeight = this->segWidth * 2 - this->segThick;
    }

  }
  this->drawWidth = 
    this->segWidth * this->digitCount +  // width of all digits
    this->segThick * (this->digitCount - 1) + // gaps to next digit (ignoring separators)
    separatorT * this->segThick; // adding separator widths + gaps

#if PANE_SERIAL_DEBUG == 1 && PANE_DATETIME_DEBUG > 0
  if (Serial) {
    Serial.print("calculateSizes("); Serial.println( (this->forceSegWidth) ? "forced): " : "): ");
    Serial.println("  format codes / chars");
    Serial.print("     | ");
    for (idx = 0; idx < this->dateTimeElems; idx++) {
      Serial.print(idx); Serial.print(" ");
    }
    Serial.println();
    Serial.print("  ---|-");
    for (idx = 0; idx < this->dateTimeElems; idx++) {
      Serial.print("--");
    }
    Serial.println();
    Serial.print("  fc | ");
    for (idx = 0; idx < this->dateTimeElems; idx++) {
      Serial.print(this->formatCode[idx]); Serial.print(" ");
    }
    Serial.println();
    Serial.print("  fd | ");
    for (idx = 0; idx < this->dateTimeElems; idx++) {
      Serial.print(this->formatSize[idx]); Serial.print(" ");
    }
    Serial.println();
    Serial.print("  sc | ");
    for (idx = 0; idx < this->dateTimeElems - 1; idx++) {
      Serial.print(this->separatorCode[idx]); Serial.print(" ");
    }
    Serial.println();
    Serial.print("  sw | ");
    for (idx = 0; idx < this->dateTimeElems - 1; idx++) {
      Serial.print(this->separatorWidth[idx]); Serial.print(" ");
    }
    Serial.println();
    Serial.println();

    Serial.print("  digitCount/segTCount/resolution/dateTimeElems/drawWidth: ");
      Serial.print(this->digitCount); Serial.print(" / "); Serial.print(this->segTCount);
      Serial.print(" / "); Serial.print(this->resolution);
      Serial.print(" / "); Serial.print(this->dateTimeElems);
      Serial.print(" / "); Serial.println(this->drawWidth);
    Serial.print("  SegWidth/SegHeight/SegThick: "); Serial.print(this->segWidth); Serial.print(" / "); 
      Serial.print(this->segHeight); Serial.print(" / ");  Serial.println(this->segThick); 
  }
#endif
  SMP_TextPane<smpRGB>::calculateSizes(); // call parent method (because of calc. font-size if autoFont)
  return this->digitCount;
}


template <typename smpRGB>
void SMP_DateTimePane<smpRGB>::forceSegmentSettings(uint8_t segWidth, uint8_t segThick) {
  this->forceSegThick = segThick;
  this->forceSegWidth = segWidth;
  calculateSizes();
}


template <typename smpRGB>
void SMP_DateTimePane<smpRGB>::updateContent(uint32_t currMS) {
  if ( ! this->active )
    return;

  uint32_t currTime = this->dataProvider->getEpochTime();

  uint8_t currSec = second(currTime);
  uint8_t currMin = minute(currTime);
  uint8_t currHour = hour(currTime);
  uint8_t currDay = day(currTime);
  uint8_t currMon = month(currTime);
  uint16_t currYear = year(currTime);

  this->contentChanged = false;

  switch (this->resolution) {
    case 0:
      if (currSec != this->oldSec)
        this->contentChanged = true;
      break;
    case 1:
      if (this->showSec && currSec != this->oldSec)
        this->contentChanged = true;
      break;
    case 2:
      if (this->showMin && currMin != this->oldMin)
        this->contentChanged = true;
      break;
    default:
      if (currHour != this->oldHour || currDay != this->oldDay || currMon != this->oldMon || currYear != this->oldYear)
        this->contentChanged = true;
  }

  if (this->contentChanged) {
    this->oldSec = currSec;
    this->oldMin = currMin;
    this->oldHour = currHour;
    this->oldDay = currDay;
    this->oldMon = currMon;
    this->oldYear = currYear;
  }
}


template <typename smpRGB>
void SMP_DateTimePane<smpRGB>::drawContent() {
  uint16_t tx = 0;
  uint16_t ty = 0;
  uint16_t tw = this->w;
  uint16_t th = this->h;
  bool border = false;

  if ( ! (this->active && this->contentLayer) )
    return;

  border = this->getTextAlignTrans(&tx, &ty, &tw, &th);

  if (border) {
    ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->fillRectangle(0, 0, this->w - 1, this->h - 1, this->borderCol, this->bgCol);
  } else {
    ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->fillScreen(this->bgCol);
  }

  for (int i = 0; i < this->dateTimeElems; i++) {
    switch(this->formatCode[i]) {
      case 'S':
        tx += this->drawNumber(tx, ty, this->oldSec, 2);
        break;
      case 'M':
        tx += this->drawNumber(tx, ty, this->oldMin, 2);
        break;
      case 'I': // stupid am/pm format with [12, 1, 2, ..., 11]
        tx += this->drawNumber(tx, ty, ((this->oldHour % 12) ? (this->oldHour % 12) : 12), 2);
        break;
      case 'H':
        tx += this->drawNumber(tx, ty, this->oldHour, 2);
        break;
      case 'd':
        tx += this->drawNumber(tx, ty, this->oldDay, 2);
        break;
      case 'm':
        tx += this->drawNumber(tx, ty, this->oldMon, 2);
        break;
      case 'y':
        tx += this->drawNumber(tx, ty, (this->oldYear % 100), 2);
        break;
      case 'Y':
        tx += this->drawNumber(tx, ty, this->oldYear, 4);
        break;
    }
    tx += this->segThick;
    if (i < MAX_DATETIME_ELEMS -1 && separatorCode[i] != 'x') {
      switch(this->separatorCode[i]) {
        case '-':
          this->drawBar(tx, ty + this->segWidth - this->segThick, this->segThick, 3 * this->segThick, false, this->fgCol);
          tx += (3 + 1) * this->segThick;
          break;
        case '.':
          this->drawBar(tx, ty + 2 * (this->segWidth - this->segThick), this->segThick, this->segThick, false, this->fgCol);
          tx += (1 + 1) * this->segThick;
          break;
        case '/':
          this->drawBar(tx, ty + this->segWidth - this->segThick, this->segThick, this->segThick, false, this->fgCol);
          tx += (1 + 1) * this->segThick;
          break;
        case ' ':
          tx += (3 + 1) * this->segThick;
          break;
        case ':':
          if (this->resolution > 0 || (this->oldSec % 2)) {
            this->drawBar(tx, ty + this->segWidth - 2 * this->segThick, this->segThick, this->segThick, false, this->fgCol);
            this->drawBar(tx, ty + this->segWidth, this->segThick, this->segThick, false, this->fgCol);
          }
          tx += (1 + 1) * this->segThick;
          break;
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

