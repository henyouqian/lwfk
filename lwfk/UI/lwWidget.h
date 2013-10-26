#ifndef __LW_WIDGET_H__
#define __LW_WIDGET_H__


namespace lw{

    class Touch;
	class Widget{
	public:
		Widget(Widget *pParent);
        virtual ~Widget();
        
        void update();
        void draw();
        bool touchBegan(const Touch &touch);
        bool touchMoved(const Touch &touch);
        bool touchEnded(const Touch &touch);
        bool touchCanceled(const Touch &touch);
        
        void setPos(float x, float y);
        void getPos(float &x, float &y);
        void show(bool b);
        void enable(bool b);

    protected:
		virtual void vUpdate() {}
        virtual void vDraw() {}
        virtual bool vTouchBegan(const Touch &touch) {return false;}
        virtual bool vTouchMoved(const Touch &touch) {return false;}
        virtual bool vTouchEnded(const Touch &touch) {return false;}
        virtual bool vTouchCanceled(const Touch &touch) {return false;}
        virtual void vMoved() {}
        virtual void vShow(bool b) {}
        virtual void vEnable(bool b) {}
        
        void setDirty();
        
        bool _isDirty;
        std::list<Widget*> _chidren;
        Widget* _pParent;
        
        float _x, _y;
        float _finalX, _finalY;
        bool _show;
        bool _enable;
	};

} //namespace lw


#endif //__LW_WIDGET_H__