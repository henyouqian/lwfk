#ifndef __LW_SCENENODE_H__
#define __LW_SCENENODE_H__

#include "../lwTouch.h"
#include "../lwNode.h"

namespace lw {
    
    class SceneNode : public Node {
    public:
        static void switchTo(SceneNode* pScene, bool unloadCurrent);
        static void sInit();
        static void sQuit();
        static void sUpdate();
        static void sDraw();
        static void sTouchBegan(const Touch &touch);
        static void sTouchMoved(const Touch &touch);
        static void sTouchEnded(const Touch &touch);
        static void sTouchCanceled(const Touch &touch);
        
    protected:
        SceneNode();
        virtual ~SceneNode();
        
        void load();
        void unload();
        
        virtual void vLoad() {}
        virtual void vUnload() {}
        virtual void vEnter() {}
        virtual void vQuit() {}
        
        bool _loaded;
    };
    
} //namespace lw


#endif //__LW_SCENENODE_H__