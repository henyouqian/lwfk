#include "lwPrefix.h"
#include "lwSceneNode.h"
#include "lwLog.h"

namespace lw {
    
    namespace {
        SceneNode *_currScene = NULL;
        SceneNode *_switchToScene = NULL;
        bool _unloadCurrentScene = true;
    }
    
    void SceneNode::switchTo(SceneNode* pScene, bool unloadCurrent) {
        if (_currScene != pScene) {
            _switchToScene = pScene;
            _unloadCurrentScene = unloadCurrent;
        }
    }
    
    void SceneNode::sInit() {
        
    }
    
    void SceneNode::sQuit() {
        
    }
    
    void SceneNode::sUpdate() {
        if (_switchToScene) {
            if (_currScene && _unloadCurrentScene) {
                _currScene->unload();
                _currScene->vQuit();
            }
            _switchToScene->load();
            _switchToScene->vEnter();
            _currScene = _switchToScene;
            _switchToScene = NULL;
        }
        
        if (_currScene) {
            _currScene->update();
        }
    }
    
    void SceneNode::sDraw() {
        if (_currScene) {
            _currScene->draw();
        }
    }
    
    void SceneNode::sTouchBegan(const Touch &touch) {
        if (_currScene) {
            _currScene->touchBegan(touch);
        }
    }
    
    void SceneNode::sTouchMoved(const Touch &touch) {
        if (_currScene) {
            _currScene->touchMoved(touch);
        }
    }
    
    void SceneNode::sTouchEnded(const Touch &touch) {
        if (_currScene) {
            _currScene->touchEnded(touch);
        }
    }
    
    void SceneNode::sTouchCanceled(const Touch &touch) {
        if (_currScene) {
            _currScene->touchCanceled(touch);
        }
    }
    
    SceneNode::SceneNode()
    :Node(NULL), _loaded(false){
        
    }
    
    SceneNode::~SceneNode() {
        
    }
    
    void SceneNode::load() {
        if (!_loaded) {
            vLoad();
            _loaded = true;
        }
    }
    
    void SceneNode::unload() {
        if (_loaded) {
            vUnload();
            _loaded = false;
        }
    }

} //namespace lw
