#include "lwPrefix.h"
#include "lwWidget.h"

namespace lw {

    Widget::Widget(Widget *pParent)
    :_isDirty(true), _pParent(pParent), _show(true), _enable(true)
    ,_x(0), _y(0), _finalX(0), _finalY(0){
        if (pParent) {
            pParent->_chidren.push_back(this);
        }
    }
    
    Widget::~Widget() {
        
    }
    
    void Widget::setPos(float x, float y) {
        if (x != _x || y != _y){
            _x = x;
            _y = y;
            setDirty();
        }
    }
    
    void Widget::getPos(float &x, float &y) {
        x = _x;
        y = _y;
    }
    
    void Widget::setDirty() {
        _isDirty = true;
        std::list<Widget*>::iterator it = _chidren.begin();
        std::list<Widget*>::iterator itend = _chidren.end();
        for (; it != itend; ++it) {
            (*it)->setDirty();
        }
    }
    
    void Widget::show(bool b) {
        _show = b;
        vShow(b);
    }
    
    void Widget::enable(bool b) {
        _enable = b;
        vEnable(b);
    }
    
    void Widget::update() {
        if (_isDirty) {
            _isDirty = false;
            if (_pParent) {
                _finalX = _pParent->_finalX + _x;
                _finalY = _pParent->_finalY + _y;
            } else {
                _finalX = _x;
                _finalY = _y;
            }
            vMoved();
        }
        vUpdate();
        std::list<Widget*>::iterator it = _chidren.begin();
        std::list<Widget*>::iterator itend = _chidren.end();
        for (; it != itend; ++it) {
            (*it)->update();
        }
    }
    
    void Widget::draw() {
        if (!_show) {
            return;
        }
        
        vDraw();
        
        std::list<Widget*>::iterator it = _chidren.begin();
        std::list<Widget*>::iterator itend = _chidren.end();
        for (; it != itend; ++it) {
            (*it)->draw();
        }
    }
    
    bool Widget::touchBegan(const Touch &touch) {
        if (!_enable) {
            return false;
        }
        
        std::list<Widget*>::reverse_iterator it = _chidren.rbegin();
        std::list<Widget*>::reverse_iterator itend = _chidren.rend();
        for (; it != itend; ++it) {
            if ((*it)->touchBegan(touch)) {
                return true;
            }
        }
        
        return vTouchBegan(touch);
    }
    
    bool Widget::touchMoved(const Touch &touch) {
        if (!_enable) {
            return false;
        }
        
        std::list<Widget*>::reverse_iterator it = _chidren.rbegin();
        std::list<Widget*>::reverse_iterator itend = _chidren.rend();
        for (; it != itend; ++it) {
            if ((*it)->touchMoved(touch)) {
                return true;
            }
        }
        
        return vTouchMoved(touch);
    }
    
    bool Widget::touchEnded(const Touch &touch) {
        if (!_enable) {
            return false;
        }
        
        std::list<Widget*>::reverse_iterator it = _chidren.rbegin();
        std::list<Widget*>::reverse_iterator itend = _chidren.rend();
        for (; it != itend; ++it) {
            if ((*it)->touchEnded(touch)) {
                return true;
            }
        }
        
        return vTouchEnded(touch);
    }
    
    bool Widget::touchCanceled(const Touch &touch) {
        if (!_enable) {
            return false;
        }
        
        std::list<Widget*>::reverse_iterator it = _chidren.rbegin();
        std::list<Widget*>::reverse_iterator itend = _chidren.rend();
        for (; it != itend; ++it) {
            if ((*it)->touchCanceled(touch)) {
                return true;
            }
        }
        
        return vTouchCanceled(touch);
    }

} //namespace lw