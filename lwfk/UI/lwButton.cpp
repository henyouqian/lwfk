#include "lwPrefix.h"
#include "lwButton.h"
#include "lwTouch.h"
#include "lwSprite.h"

namespace lw {

    Button::Button(Widget* parent, ButtonCallback* callback,
                   float x, float y, float w, float h,
                   float extTop, float extBottom, float extLeft, float extRight)
    :Widget(parent), _w(w), _h(h), _pCallback(callback), _pTouch(NULL)
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
            float xMin = _finalX - _extLeft;
            float yMin = _finalY - _extTop;
            float xMax = _finalX + _w + _extRight;
            float yMax = _finalY + _h + _extBottom;
            if (touch.x >= xMin && touch.x < xMax
              && touch.y >= yMin && touch.y < yMax) {
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
            float xMin = _finalX - _extLeft;
            float yMin = _finalY - _extTop;
            float xMax = _finalX + _w + _extRight;
            float yMax = _finalY + _h + _extBottom;
            if (touch.x >= xMin && touch.x < xMax
                && touch.y >= yMin && touch.y < yMax) {
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
            _pSptNormal = Sprite::createFromAtlas(def.sptNormal, "normal");
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
            _pSptDown = Sprite::create(def.sptDown, "normal");
            if (needResize)
                _pSptDown->setSize(_w, _h);
        }
        
        if (def.sptDisable) {
            _pSptDisable = Sprite::create(def.sptDisable, "normal");
            if (needResize)
                _pSptDisable->setSize(_w, _h);
        }
        
        if (_pSptNormal && needResize) {
            _pSptNormal->setSize(_w, _h);
        }
    }
    
    SpriteButton::~SpriteButton() {
        if (_pSptNormal)
            delete _pSptNormal;
        if (_pSptDown)
            delete _pSptDown;
        if (_pSptDisable)
            delete _pSptDisable;
    }
    
    void SpriteButton::vMoved() {
        if (_pSptNormal) {
            _pSptNormal->setPos(_finalX, _finalY);
        }
        if (_pSptDown) {
            _pSptDown->setPos(_finalX, _finalY);
        }
        if (_pSptDisable) {
            _pSptDisable->setPos(_finalX, _finalY);
        }
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