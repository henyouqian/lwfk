#ifndef __LW_BUTTON_H__
#define __LW_BUTTON_H__

#include "lwWidget.h"

namespace lw{
    class Widget;
    class Touch;
    class Button;
    class Sprite;
    
    //==================================
    class ButtonCallback{
    public:
        virtual ~ButtonCallback(){}
        virtual void vDown(lw::Button* pButton){}
        virtual void vClick(lw::Button* pButton){}
    };
    
    //==================================
    struct SpriteButtonDef{
        ButtonCallback* callback;
        Widget* parent;
        const char *sptNormal, *sptDown, *sptDisable;
        float x, y;
        float overrideW, overrideH;
        float extTop, extBottom, extLeft, extRight;
        
        SpriteButtonDef()
        :callback(NULL), parent(NULL), sptNormal(NULL), sptDown(NULL), sptDisable(NULL)
        ,x(0), y(0), overrideW(0), overrideH(0), extTop(0), extBottom(0), extLeft(0), extRight(0){
            
        }
    };
    
    //==================================
    class Button : public Widget {
    public:
        static Button* create(SpriteButtonDef &def);
        virtual ~Button();
        
    protected:
        Button(Widget* parent, ButtonCallback* callback,
               float x, float y, float w, float h,
               float extTop, float extBottom, float extLeft, float extRight);
        
        virtual bool vTouchBegan(const Touch &touch);
        virtual bool vTouchEnded(const Touch &touch);
        virtual bool vTouchCanceled(const Touch &touch);
        
        float _w, _h;
        const Touch *_pTouch;
        ButtonCallback *_pCallback;
        float _extTop, _extBottom, _extLeft, _extRight;
    };
    
    //==================================
    class SpriteButton : public Button {
    public:
        SpriteButton(SpriteButtonDef &def);
        ~SpriteButton();
        
    private:
        virtual void vMoved();
        virtual void vDraw();
        Sprite *_pSptNormal, *_pSptDown, *_pSptDisable;
    };
    
    //

} //namespace lw


#endif //__LW_WIDGET_H__