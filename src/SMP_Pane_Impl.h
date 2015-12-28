/*
 *************************************************************************
 *
 * Implementation part for SMP_Pane.h
 * Abstract base classes SMP_Base and SMP_Pane for all derived Pane-classes
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

#include "SMP_Pane.h"

SMP_Base * SMP_Base::firstPane = NULL;
SMP_Base * SMP_Base::lastPane = NULL;
uint16_t SMP_Base::matrixWidth = 0;
uint16_t SMP_Base::matrixHeight = 0;


SMP_Base::SMP_Base() {
  this->nextPane = NULL;
}

void SMP_Base::chainPane(SMP_Base & nextPane) {
  if (this->nextPane == NULL) {
    this->nextPane = &nextPane;
  } else {
    SMP_Base * curr = this->nextPane; // save next pane in chain
    SMP_Base * next = &nextPane;
    this->nextPane = &nextPane;
    while(next->getNextPane()) {
      next = next->getNextPane();
    };
    next->chainPane(*curr); // set saved pane at end of added chain
  }
}



void SMP_Base::chainInit(uint16_t matrixWidth, uint16_t matrixHeight) {
  SMP_Base::matrixWidth = matrixWidth;
  SMP_Base::matrixHeight = matrixHeight;
}

#if SM_SUPPORT_ADDITIONAL_COLOURSPACES == 1
void SMP_Base::chainAddPane(SMP_Base & pane, SMLayerBackground<rgb8,0>& parent) {
  internalChainAdd(pane);
  pane.setParent(parent);
}

void SMP_Base::chainAddPane(SMP_Base & pane, SMLayerBackground<rgb16,0>& parent) {
  internalChainAdd(pane);
  pane.setParent(parent);
}
#endif

void SMP_Base::chainAddPane(SMP_Base & pane, SMLayerBackground<rgb24,0>& parent) {
  internalChainAdd(pane);
  pane.setParent(parent);
}

void SMP_Base::chainAddPane(SMP_Base & pane, SMLayerBackground<rgb48,0>& parent) {
  internalChainAdd(pane);
  pane.setParent(parent);
}

bool SMP_Base::chainNeedsUpdate(uint32_t currMS) {
  bool rv = false;
  SMP_Base * next = SMP_Base::firstPane;
  do {
    rv |= next->update(currMS);
  } while ( (next = next->getNextPane()) );
  return rv;
}

void SMP_Base::chainDraw(rgb24 clearCol) {
  SMP_Layer::clearAll(clearCol);

  SMP_Base * next = SMP_Base::firstPane;
  do {
    next->draw();
  } while ( (next = next->getNextPane()) );
}

void SMP_Base::internalChainAdd(SMP_Base & pane) {
  if (SMP_Base::firstPane) {
    SMP_Base::lastPane->chainPane(pane);
    SMP_Base::lastPane = &pane;
  } else {
    SMP_Base::firstPane = &pane;
    SMP_Base::lastPane = &pane;
  }
}


bool SMP_Base::sendMessage(uint8_t id, String message) {
  if ( ! id)  // ignore id == 0 
    return false;
  
  bool rc = false;

  SMP_Base * next = SMP_Base::firstPane;
  do {
    if (id == next->getID()) {
      next->setMessage(message);
      rc = true;
    }
  } while ( (next = next->getNextPane()) );
  return rc;
}



SMP_Layer * SMP_Layer::firstLayer = NULL;
SMP_Layer * SMP_Layer::lastLayer = NULL;


SMP_Layer::SMP_Layer() {
  this->currLayer = NULL;
  this->nextLayer = NULL;
}

bool SMP_Layer::internalAdd (SM_Layer& layer, LayerType layerType, uint8_t layerDepth) {
  bool found = false;
  SMP_Layer * l = SMP_Layer::firstLayer;
  while (!found && l) {
    if (l->currLayer == &layer) {
      found = true;  // don't add layer twice
    } else {
      l = l->getNextLayer();
    }
  }
  if (!found) {
    if ( ! SMP_Layer::firstLayer ) {
      SMP_Layer::firstLayer = new SMP_Layer();
      SMP_Layer::lastLayer = SMP_Layer::firstLayer;
      SMP_Layer::firstLayer->currLayer = &layer;
      SMP_Layer::firstLayer->currType = layerType;
      SMP_Layer::firstLayer->currDepth = layerDepth;
    } else {
      SMP_Layer::lastLayer->nextLayer = new SMP_Layer();
      SMP_Layer::lastLayer->nextLayer->currLayer = &layer;
      SMP_Layer::lastLayer->nextLayer->currType = layerType;
      SMP_Layer::lastLayer->nextLayer->currDepth = layerDepth;
      SMP_Layer::lastLayer = SMP_Layer::lastLayer->nextLayer;
    }
  }
  return found;
}

void SMP_Layer::clear(rgb24 clearCol) {
  switch(this->currType) {
    case background:
      switch(this->currDepth) {
#if SM_SUPPORT_ADDITIONAL_COLOURSPACES == 1
        case 8:  ((SMLayerBackground<rgb8,0>*)this->currLayer)->fillScreen( clearCol ); break;
        case 16: ((SMLayerBackground<rgb16,0>*)this->currLayer)->fillScreen( clearCol ); break;
#endif
        case 24: ((SMLayerBackground<rgb24,0>*)this->currLayer)->fillScreen( clearCol ); break;
        case 48: ((SMLayerBackground<rgb48,0>*)this->currLayer)->fillScreen( clearCol ); break;
        default: ; // ignore
      }
      break;
    case indexed:
      switch(this->currDepth) {
#if SM_SUPPORT_ADDITIONAL_COLOURSPACES == 1
        case 8:  ((SMLayerIndexed<rgb8,0>*)this->currLayer)->fillScreen( 0 ); break;
        case 16: ((SMLayerIndexed<rgb16,0>*)this->currLayer)->fillScreen( 0 ); break;
#endif
        case 24: ((SMLayerIndexed<rgb24,0>*)this->currLayer)->fillScreen( 0 ); break;
        case 48: ((SMLayerIndexed<rgb48,0>*)this->currLayer)->fillScreen( 0 ); break;
        default: ; // ignore
      }
      break;
    default:
      ; // do nothing
  }
}


void SMP_Layer::clearAll(rgb24 clearCol) {
  SMP_Layer * l = SMP_Layer::firstLayer;
  while (l) {
    l->clear(clearCol);
    l = l->getNextLayer();
  }
}




template <typename smpRGB>
SMP_Pane<smpRGB>::SMP_Pane() : SMP_Base() {
  this->x = 0;
  this->y = 0;
  this->transparent = true;
  this->active = true;
  this->fgCol = rgb24(0xFF, 0xFF, 0xFF);
  this->bgCol = rgb24(0x00, 0x00, 0x00);
  this->fgWhite = true;
  this->borderCol = rgb24(0x00, 0x00, 0x00);
  this->contentChanged = true;
  this->contentLayer = NULL;
  this->contentType = none;
  this->parentLayer = NULL;
  this->parentType = undefined;
  this->drawBorder = false;
}

template <typename smpRGB>
void SMP_Pane<smpRGB>::setPosition (uint16_t x, uint16_t y, Align posAlignHor, Align posAlignVert) {
  uint16_t oldX = this->x, oldY = this->y;
  uint16_t newX = x, newY = y;

  if (SMP_Base::matrixWidth > this->w) {
    switch(posAlignHor) {
      case center:
        newX = (SMP_Base::matrixWidth - this->w) / 2;
        break;
      case right:
          newX = SMP_Base::matrixWidth - this->w;
        break;
      default:
        ; // do nothing
    }
  } else {
    newX = 0;
  }

  if (SMP_Base::matrixHeight > this->h) {
    switch(posAlignVert) {
      case middle:
          newY = (SMP_Base::matrixHeight - this->h) / 2;
        break;
      case bottom:
          newY = SMP_Base::matrixHeight - this->h;
        break;
      default:
        ; // do nothing
    }
  } else {
    newY = 0;
  }

  if ( !(oldX == newX && oldY == newY) ) {
    this->x = newX;
    this->y = newY;
    this->contentChanged = true;
  }
}


template <typename smpRGB>
void SMP_Pane<smpRGB>::setBorder(bool drawBorder) {
  this->drawBorder = drawBorder;
  if (this->w < 5 || this->h < 5) { // no border if pane too small
    this->drawBorder = false;
  }
}


template <typename smpRGB>
void SMP_Pane<smpRGB>::setColours (smpRGB fgCol, smpRGB bgCol, smpRGB borderCol) {
  this->fgCol = fgCol;
  this->bgCol = bgCol;
  this->borderCol = borderCol;

  if (this->parentType == indexed) {
    this->fgWhite = ( this->fgCol.red || this->fgCol.green || this->fgCol.blue );
  }
};

template <typename smpRGB>
bool SMP_Pane<smpRGB>::update(uint32_t currMS) {
  this->updateContent(currMS);
  return contentChanged;
}

template <typename smpRGB>
void SMP_Pane<smpRGB>::run() {
  this->active = true;
}

template <typename smpRGB>
void SMP_Pane<smpRGB>::setRotation(rotationDegrees newrotation) {
  if (this->contentLayer) {
    this->contentLayer->setRotation(newrotation);
  }
}

template <typename smpRGB>
bool SMP_Pane<smpRGB>::internalSetParent (SM_Layer& parent, LayerType parentType, uint8_t parentDepth) {
  this->parentLayer = &parent;
  this->parentType = parentType;
  this->parentDepth = parentDepth;
  return true;
}


