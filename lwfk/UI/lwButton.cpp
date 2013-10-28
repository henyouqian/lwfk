#include "lwPrefix.h"
#include "lwButton.h"
#include "lwTouch.h"
#include "lwSpriteNode.h"

namespace lw {

    Button::Button(Node* parent, ButtonCallback* callback,
                   float x, float y, float w, float h,
                   float extTop, float extBottom, float extLeft, float extRight)
    :Node(parent), _w(w), _h(h), _pCallback(callback), _pTouch(NULL)
    ,_extTop(extTop), _extBottom(extBottom), _extLeft(extLeft), _extRight(extRight) {
        _x = x;
        _y = y;
    }
    
    Button* Button::create(SpriteButtonDef &def) {
        return new SpriteButton(def);
    }
    
    Button::~Button() {
        
    }
    
    bool Button::vTouchBegan(const Touch &touch) {
        if (!_pTouch) {
            if (isHit(-_extLeft, -_extTop, _w+_extRight, _h+_extBottom, touch.x, touch.y)) {
                _pTouch = &touch;
                if (_pCallback) {
                    _pCallback->vDown(this);
                }
                return true;
            }
        }
        return false;
    }
    
    bool Button::vTouchEnded(const Touch &touch) {
        if (_pTouch == &touch) {
            _pTouch = NULL;
            if (isHit(-_extLeft, -_extTop, _w+_extRight, _h+_extBottom, touch.x, touch.y)) {
                if (_pCallback) {
                    _pCallback->vClick(this);
                }
                return true;
            }
        }
        return false;
    }
    
    bool Button::vTouchCanceled(const Touch &touch) {
        return vTouchEnded(touch);
    }
    
    //======================================
    SpriteButton::SpriteButton(SpriteButtonDef &def)
    :Button(def.parent, def.callback, def.x, def.y, 0, 0
            ,def.extTop, def.extBottom, def.extLeft, def.extRight) {
        if (def.sptNormal) {
            _pSptNormal = SpriteNode::create(this, def.sptNormal, "normal");
            _pSptNormal->getSize(_w, _h);
        }
        
        bool needResize = false;
        if (def.overrideW != 0) {
            _w = def.overrideW;
            needResize = true;
        }
        if (def.overrideH != 0) {
            _h = def.overrideH;
            needResize = true;
        }
        
        if (def.sptDown) {
            _pSptDown = SpriteNode::create(this, def.sptDown, "normal");
            if (needResize)
                _pSptDown->setSize(_w, _h);
        }
        
        if (def.sptDisable) {
            _pSptDisable = SpriteNode::create(this, def.sptDisable, "normal");
            if (needResize)
                _pSptDisable->setSize(_w, _h);
        }
        
        if (_pSptNormal && needResize) {
            _pSptNormal->setSize(_w, _h);
        }
        
        _autoDrawChild = false;
    }
    
    SpriteButton::~SpriteButton() {
        
    }
    
    void SpriteButton::vDraw() {
        if (_enable == false) {
            if (_pSptDisable) {
                _pSptDisable->draw();
            } else if (_pSptNormal) {
                _pSptNormal->draw();
            }
        } else if (_pTouch) {
            if (_pSptDown) {
                _pSptDown->draw();
            } else if (_pSptNormal) {
                _pSptNormal->draw();
            }
        } else if (_pSptNormal) {
            _pSptNormal->draw();
        }
    }

} //namespace lw