#include "lwPrefix.h"
#include "lwApp.h"
#include "lwTask.h"
#include "lwSprite.h"
#include "lwRenderState.h"
#include "lwSound.h"
#include "lwLog.h"
#include "PVRShell.h"


namespace {
    class App : public PVRShell
    {
    public:
        App(){}
        virtual bool InitApplication() {
            lw::File::SetReadPath((char*)PVRShellGet(prefReadPath));
            lw::File::SetLoadReleaseFunctions(PVRShellGet(prefLoadFileFunc), PVRShellGet(prefReleaseFileFunc));

            lw::rsInit();
            lw::Sprite::init();
            lw::soundInit();
            lw::Task::init();
            lwapp_init();
            return true;
        }
        virtual bool InitView() {
            lwinfo("InitView()");
            lw::Sprite::onInitView();
            return true;
        }
        virtual bool ReleaseView() {
            lwinfo("ReleaseView()");
            return true;
        }
        virtual bool QuitApplication() {
            lwapp_quit();
            lw::Task::quit();
            lw::soundQuit();
            lw::Sprite::quit();
            return true;
        }
        virtual bool RenderScene() {
            lw::Task::update();
            glDepthMask(GL_TRUE); //bugfix for some gpu driver
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            lw::Task::draw();
            lw::Sprite::flush();
            return true;
        }
    };
    
    App* _app = NULL;
}


PVRShell* NewDemo()
{
    _app = new App();
	return _app;
}

namespace lw {
    PVRTVec2 screenSize() {
        float width = (float)_app->PVRShellGet(prefWidth);
        float height = (float)_app->PVRShellGet(prefHeight);
        return PVRTVec2(width, height);
    }
    
    const char* readPath() {
        return (const char*)_app->PVRShellGet(prefReadPath);
    }
}

