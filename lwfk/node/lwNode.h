#ifndef __LW_NODE_H__
#define __LW_NODE_H__


namespace lw{

    class Touch;
	class Node{
	public:
		Node(Node *pParent);
        virtual ~Node();
        
        void update();
        void draw();
        bool touchBegan(const Touch &touch);
        bool touchMoved(const Touch &touch);
        bool touchEnded(const Touch &touch);
        bool touchCanceled(const Touch &touch);
        
        void show(bool b);
        void enable(bool b);
        
        void setPos(float x, float y);
        void getPos(float &x, float &y);
        void setZ(float z);
        float getZ();
        void setRotateZ(float f);
        float getRotateZ();
        void setScale(float x, float y);
        void getScale(float &x, float &y);
        void setPivot(float x, float y);
        void getPivot(float &x, float &y);
        
        void setMatrix(PVRTMat4 &mat);
        PVRTMat4* getMatrix();
        const PVRTMat4& getFinalMatrix();

    protected:
		virtual void vUpdate() {}
        virtual void vDraw() {}
        virtual bool vTouchBegan(const Touch &touch) {return false;}
        virtual bool vTouchMoved(const Touch &touch) {return false;}
        virtual bool vTouchEnded(const Touch &touch) {return false;}
        virtual bool vTouchCanceled(const Touch &touch) {return false;}
        virtual void vShow(bool b) {}
        virtual void vEnable(bool b) {}
        virtual void vFinalMatrixDidUpdate() {}
        
        void setDirty();
        
        bool _isDirty;
        std::list<Node*> _chidren;
        Node* _pParent;
        
        float _x, _y, _z;
        float _rotateZ;
        float _scaleX, _scaleY;
        float _pivotX, _pivotY;
        
        PVRTMat4 *_pCustomMat;
        PVRTMat4 _finalMat;
        
        bool _show;
        bool _enable;
	};

} //namespace lw


#endif //__LW_WIDGET_H__