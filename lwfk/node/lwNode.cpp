#include "lwPrefix.h"
#include "lwNode.h"

namespace lw {

    Node::Node(Node *pParent)
    :_isDirty(true), _pParent(pParent), _show(true), _enable(true)
    ,_x(0), _y(0), _z(0), _rotateZ(0), _scaleX(1.f), _scaleY(1.f)
    ,_pivotX(0), _pivotY(0), _pCustomMat(NULL){
        if (pParent) {
            pParent->_chidren.push_back(this);
        }
    }
    
    Node::~Node() {
        
    }
    
    void Node::setPos(float x, float y) {
        if (x != _x || y != _y){
            setDirty();
            _x = x;
            _y = y;
        }
    }
    
    void Node::getPos(float &x, float &y) {
        x = _x;
        y = _y;
    }
    
    void Node::setZ(float z) {
        if (_z != z){
            setDirty();
            _z = z;
        }
    }
    
    float Node::getZ() {
        return _z;
    }
    
    void Node::setRotateZ(float f) {
        if (_rotateZ != f) {
            setDirty();
            _rotateZ = f;
        }
    }
    
    float Node::getRotateZ() {
        return _rotateZ;
    }
    
    void Node::setScale(float x, float y) {
        if (_scaleX != x || _scaleY != y){
            setDirty();
            _scaleX = x;
            _scaleY = y;
        }
    }
    
    void Node::getScale(float &x, float &y) {
        x = _scaleX;
        y = _scaleY;
    }
    
    void Node::setPivot(float x, float y) {
        if (_pivotX != x || _pivotY != y) {
            setDirty();
            _pivotX = x;
            _pivotY = y;
        }
    }
    
    void Node::getPivot(float &x, float &y) {
        x = _pivotX;
        y = _pivotY;
    }
    
    void Node::setMatrix(PVRTMat4 &mat) {
        setDirty();
        if (_pCustomMat == NULL){
            _pCustomMat = new PVRTMat4(mat);
        } else {
            *_pCustomMat = mat;
        }
    }
    
    PVRTMat4* Node::getMatrix() {
        return _pCustomMat;
    }
    
    const PVRTMat4& Node::getFinalMatrix() {
        if (_isDirty) {
            _isDirty = false;
            
            if (_pCustomMat) {
                if (_pParent) {
                    _finalMat = _pParent->getFinalMatrix() * *_pCustomMat;
                }
            } else {
                PVRTMat4 matPivots = PVRTMat4::Translation(-_pivotX, -_pivotY, 0);
                PVRTMat4 matScale = PVRTMat4::Scale(_scaleX, _scaleY, 1.f);
                PVRTMat4 matRotate = PVRTMat4::RotationZ(_rotateZ);
                PVRTMat4 matTrans = PVRTMat4::Translation(_x, _y, _z);
                if (_pParent != NULL) {
                    _finalMat = _pParent->getFinalMatrix() * matTrans * matRotate * matScale * matPivots;
                } else {
                    _finalMat = matTrans * matRotate * matScale * matPivots;
                }
            }
            vFinalMatrixDidUpdate();
        }
        
        return _finalMat;
    }
    
    void Node::setDirty() {
        _isDirty = true;
        std::list<Node*>::iterator it = _chidren.begin();
        std::list<Node*>::iterator itend = _chidren.end();
        for (; it != itend; ++it) {
            (*it)->setDirty();
        }
    }
    
    void Node::show(bool b) {
        _show = b;
        vShow(b);
    }
    
    void Node::enable(bool b) {
        _enable = b;
        vEnable(b);
    }
    
    void Node::update() {
        if (_isDirty) {
            getFinalMatrix();
        }
        vUpdate();
        std::list<Node*>::iterator it = _chidren.begin();
        std::list<Node*>::iterator itend = _chidren.end();
        for (; it != itend; ++it) {
            (*it)->update();
        }
    }
    
    void Node::draw() {
        if (!_show) {
            return;
        }
        
        vDraw();
        
        std::list<Node*>::iterator it = _chidren.begin();
        std::list<Node*>::iterator itend = _chidren.end();
        for (; it != itend; ++it) {
            (*it)->draw();
        }
    }
    
    bool Node::touchBegan(const Touch &touch) {
        if (!_enable) {
            return false;
        }
        
        std::list<Node*>::reverse_iterator it = _chidren.rbegin();
        std::list<Node*>::reverse_iterator itend = _chidren.rend();
        for (; it != itend; ++it) {
            if ((*it)->touchBegan(touch)) {
                return true;
            }
        }
        
        return vTouchBegan(touch);
    }
    
    bool Node::touchMoved(const Touch &touch) {
        if (!_enable) {
            return false;
        }
        
        std::list<Node*>::reverse_iterator it = _chidren.rbegin();
        std::list<Node*>::reverse_iterator itend = _chidren.rend();
        for (; it != itend; ++it) {
            if ((*it)->touchMoved(touch)) {
                return true;
            }
        }
        
        return vTouchMoved(touch);
    }
    
    bool Node::touchEnded(const Touch &touch) {
        if (!_enable) {
            return false;
        }
        
        std::list<Node*>::reverse_iterator it = _chidren.rbegin();
        std::list<Node*>::reverse_iterator itend = _chidren.rend();
        for (; it != itend; ++it) {
            if ((*it)->touchEnded(touch)) {
                return true;
            }
        }
        
        return vTouchEnded(touch);
    }
    
    bool Node::touchCanceled(const Touch &touch) {
        if (!_enable) {
            return false;
        }
        
        std::list<Node*>::reverse_iterator it = _chidren.rbegin();
        std::list<Node*>::reverse_iterator itend = _chidren.rend();
        for (; it != itend; ++it) {
            if ((*it)->touchCanceled(touch)) {
                return true;
            }
        }
        
        return vTouchCanceled(touch);
    }

} //namespace lw