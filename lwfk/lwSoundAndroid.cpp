#include "lwPrefix.h"
#include "lwSound.h"
#include "lwLog.h"
#include "lwRes.h"

namespace lw {

    void soundInit() {
        
    }
    
    void soundQuit() {
        
    }
    
    
    //======================================
    class SoundImpl : public Sound{
    public:
        SoundImpl(const char *file);
        virtual ~SoundImpl();
        virtual void play();
        virtual void stop();
    
    private:
        
    };
    
    Sound* Sound::create(const char *file){
        Sound *p = new SoundImpl(file);
        return p;
    }
    
    SoundImpl::SoundImpl(const char *file) {
        
    }
    
    SoundImpl::~SoundImpl() {
       
    }
    
    void SoundImpl::play() {
        
    }
    
    void SoundImpl::stop() {
        
    }
}
