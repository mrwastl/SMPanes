/*
 *************************************************************************
 *
 * SMP_Pane.h
 * Abstract base classes SMP_Base and SMP_Pane for all derived Pane-classes
 *
 * SMP_Layers is an internal helper class for managing SmartMatrix layers
 *
 * SMP_Base is the template-free base class of SMP_Pane and also defines
 * static methods for chaining panes and layers and process these in one go.
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

#ifndef SMP_PANE_h
#define SMP_PANE_h

#include "Layer.h"
#include "MatrixCommon.h"

#define SM_SUPPORT_ADDITIONAL_COLOURSPACES 1


typedef enum LayerType {
  none = -2,
  undefined = -1,
  background = 0,
  scrolling,
  indexed
} LayerType;

typedef enum Align {
  left = 0,
  top = 0,
  center = 1,
  middle = 1,
  right = 2,
  bottom = 2
} Align;


// internal class for managing SmartMatrix layers
class SMP_Layer {
  friend class SMP_Base;
  protected:
                              SMP_Layer();
    static    bool            internalAdd (SM_Layer& layer, LayerType layerType, uint8_t layerDepth);

              void            clear(rgb24 clearCol = rgb24(0, 0, 0));
    static    void            clearAll(rgb24 clearCol = rgb24(0, 0, 0));

              SMP_Layer     * getNextLayer() { return this->nextLayer; };
  private:
              SM_Layer      * currLayer;
              LayerType       currType;
              uint8_t         currDepth;

              SMP_Layer     * nextLayer;
    static    SMP_Layer     * firstLayer;
    static    SMP_Layer     * lastLayer;
};


// small helper macro that unfortunately cannot be defined as normal static class method
// (because of the lack of a common non-template SmartMatrix base class)
#define SMP_BASE__CHAINADDLAYER(_matrix, _layer)  (_matrix).addLayer(& (_layer)); SMP_Base::chainAddLayer(_layer)

class SMP_Base {
  public:
    virtual   bool            update(uint32_t currMS = 0) = 0;
    virtual   void            draw() = 0;
    virtual   void            setMessage(String message, uint32_t currMS = 0) {};

              void            chainPane(SMP_Base & nextPane);
              SMP_Base      * getNextPane() { return this->nextPane; };

 /* poor man's reflection. very ugly, better solution would be nice */
 /*           bool            setParent(SMLayerXXXXX & parent);  */
#if SM_SUPPORT_ADDITIONAL_COLOURSPACES == 1
              bool            setParent(SMLayerBackground<rgb8,0>& parent) { return internalSetParent(parent, background, 8);};
              bool            setParent(SMLayerBackground<rgb16,0>& parent) { return internalSetParent(parent, background, 16);};
#endif
              bool            setParent(SMLayerBackground<rgb24,0>& parent) { return internalSetParent(parent, background, 24);};
              bool            setParent(SMLayerBackground<rgb48,0>& parent) { return internalSetParent(parent, background, 48);};
#if SM_SUPPORT_ADDITIONAL_COLOURSPACES == 1
              bool            setParent(SMLayerScrolling<rgb8,0>& parent) { return internalSetParent(parent, indexed, 8);};
              bool            setParent(SMLayerScrolling<rgb16,0>& parent) { return internalSetParent(parent, indexed, 16);};
#endif
              bool            setParent(SMLayerScrolling<rgb24,0>& parent) { return internalSetParent(parent, indexed, 24);};
              bool            setParent(SMLayerScrolling<rgb48,0>& parent) { return internalSetParent(parent, indexed, 48);};
#if SM_SUPPORT_ADDITIONAL_COLOURSPACES == 1
              bool            setParent(SMLayerIndexed<rgb8,0>& parent) { return internalSetParent(parent, scrolling, 8);};
              bool            setParent(SMLayerIndexed<rgb16,0>& parent) { return internalSetParent(parent, scrolling, 16);};
#endif
              bool            setParent(SMLayerIndexed<rgb24,0>& parent) { return internalSetParent(parent, scrolling, 24);};
              bool            setParent(SMLayerIndexed<rgb48,0>& parent) { return internalSetParent(parent, scrolling, 48);};

    virtual   void            setID(uint8_t id) { this->id = id; };
              uint8_t         getID()           { return this->id; };

    virtual   void            run() = 0;
    virtual   void            stop(bool clear = true) = 0;

    /* static public methods, single destination */
    static    bool            sendMessage(uint8_t id, String message);
    static    bool            sendActivate(uint8_t id, bool active, bool clear = true);

    /* static public methods, multiple destinations */
    static    void            chainInit(uint16_t matrixWidth, uint16_t matrixHeight);

 /* poor man's reflection. very ugly, better solution would be nice */
 /* static    bool            chainAddLayer(SMLayerXXXXX & layer); */
#if SM_SUPPORT_ADDITIONAL_COLOURSPACES == 1
    static    bool            chainAddLayer(SMLayerBackground<rgb8,0>& layer)  { return SMP_Layer::internalAdd(layer, background, 8);};
    static    bool            chainAddLayer(SMLayerBackground<rgb16,0>& layer) { return SMP_Layer::internalAdd(layer, background, 16);};
#endif
    static    bool            chainAddLayer(SMLayerBackground<rgb24,0>& layer) { return SMP_Layer::internalAdd(layer, background, 24);};
    static    bool            chainAddLayer(SMLayerBackground<rgb48,0>& layer) { return SMP_Layer::internalAdd(layer, background, 48);};
#if SM_SUPPORT_ADDITIONAL_COLOURSPACES == 1
    static    bool            chainAddLayer(SMLayerScrolling<rgb8,0>& layer)   { return SMP_Layer::internalAdd(layer, indexed, 8);};
    static    bool            chainAddLayer(SMLayerScrolling<rgb16,0>& layer)  { return SMP_Layer::internalAdd(layer, indexed, 16);};
#endif
    static    bool            chainAddLayer(SMLayerScrolling<rgb24,0>& layer)  { return SMP_Layer::internalAdd(layer, indexed, 24);};
    static    bool            chainAddLayer(SMLayerScrolling<rgb48,0>& layer)  { return SMP_Layer::internalAdd(layer, indexed, 48);};
#if SM_SUPPORT_ADDITIONAL_COLOURSPACES == 1
    static    bool            chainAddLayer(SMLayerIndexed<rgb8,0>& layer)     { return SMP_Layer::internalAdd(layer, scrolling, 8);};
    static    bool            chainAddLayer(SMLayerIndexed<rgb16,0>& layer)    { return SMP_Layer::internalAdd(layer, scrolling, 16);};
#endif
    static    bool            chainAddLayer(SMLayerIndexed<rgb24,0>& layer)    { return SMP_Layer::internalAdd(layer, scrolling, 24);};
    static    bool            chainAddLayer(SMLayerIndexed<rgb48,0>& layer)    { return SMP_Layer::internalAdd(layer, scrolling, 48);};

 /* static    void            chainAddPane(SMP_Base & pane, SMLayerXXXXX & parent); */
#if SM_SUPPORT_ADDITIONAL_COLOURSPACES == 1
    static    void            chainAddPane(SMP_Base & pane, SMLayerBackground<rgb8,0>& parent);
    static    void            chainAddPane(SMP_Base & pane, SMLayerBackground<rgb16,0>& parent);
#endif
    static    void            chainAddPane(SMP_Base & pane, SMLayerBackground<rgb24,0>& parent);
    static    void            chainAddPane(SMP_Base & pane, SMLayerBackground<rgb48,0>& parent);

    static    bool            chainNeedsUpdate(uint32_t currMS = 0);
    static    void            chainDraw(rgb24 clearCol = rgb24(0,0,0));

    static    void            chainActivate();

  protected:
                              SMP_Base();
    virtual   bool            internalSetParent (SM_Layer& parent, LayerType parentType, uint8_t parentDepth) = 0;
                              // default: support BackgroundLayer only
    virtual   bool            supportsLayerType (LayerType type) { return (type == background); };

              uint8_t         id;

              bool            active;

              SMP_Base      * nextPane; // for chaining

              SM_Layer      * parentLayer;
              LayerType       parentType;
              uint8_t         parentDepth;

    /* static protected methods and fields */
    static    void            internalChainAdd(SMP_Base & pane);

    static    SMP_Base      * firstPane;
    static    SMP_Base      * lastPane;
    static    uint16_t        matrixWidth;
    static    uint16_t        matrixHeight;

};


template <typename smpRGB>
class SMP_Pane : public SMP_Base {
  public:
              void            setPosition (uint16_t x = 0, uint16_t y = 0, Align posAlignHor = left, Align posAlignVert = top);
              void            setTransparent(bool transparent) { this->transparent = transparent; };
              void            setBorder(bool drawBorder = true);
              bool            isTransparent() { return this->transparent; }
              bool            isBorder() { return this->drawBorder; }
              void            setRotation(rotationDegrees newrotation);
              uint16_t        getX() { return this->x; };
              uint16_t        getY() { return this->y; };
              uint16_t        getWidth() { return this->w; };
              uint16_t        getHeight() { return this->h; };
              rotationDegrees getRotation() { return this->rotation; };
              void            setColours(smpRGB fgCol, smpRGB bgCol = rgb24(0, 0, 0), smpRGB borderCol = rgb24(0, 0, 0));

    virtual   void            run();
    virtual   void            stop(bool clear = true);

              bool            update(uint32_t currMS = 0);
    virtual   void            draw() = 0;

  protected:
                              SMP_Pane();
    virtual   uint8_t         calculateSizes() = 0;
    virtual   bool            internalSetParent (SM_Layer& parent, LayerType parentType, uint8_t parentDepth);

    virtual   void            updateContent(uint32_t currMS = 0) = 0;
    virtual   void            drawContent() = 0;

              bool            contentChanged;

              bool            transparent;
              rotationDegrees rotation;
              uint16_t        w;
              uint16_t        h;
              uint16_t        x;
              uint16_t        y;

              SM_Layer      * contentLayer;
              LayerType       contentType;

              smpRGB          bgCol;
              smpRGB          fgCol;
              smpRGB          borderCol;
              bool            drawBorder;
              bool            fgWhite; // for indexed parent layer
  private:
};


#include "SMP_Pane_Impl.h"

#endif // SM_PANE_h