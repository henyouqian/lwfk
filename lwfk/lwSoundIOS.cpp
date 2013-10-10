#include "lwPrefix.h"
#include "lwSound.h"
#include "lwLog.h"
#include "lwRes.h"
#import "ALUT/alut.h"

namespace lw {

    void soundInit() {
        alutInit (NULL, NULL);
    }
    
    void soundQuit() {
        alutExit();
    }
    
    namespace { KeyResMgr _soundBufferResMgr; }
    
    class SoundBufferRes : public Res {
    public:
        static SoundBufferRes* create(const char *filename) {
            assert(filename);
            
            SoundBufferRes *pRes = (SoundBufferRes*)_soundBufferResMgr.get(filename);
            if (pRes) {
                return pRes;
            } else {
                bool ok = false;
                SoundBufferRes* p = new SoundBufferRes(filename, ok);
                if ( p && ok  ){
                    return p;
                }else if (p){
                    delete p;
                    return NULL;
                }
            }
            return NULL;
        }
        
        ALuint getBuffer() {
            return _buffer;
        }
        
    private:
        SoundBufferRes(const char *filename, bool &ok)
        :_buffer(0){
            assert(filename);
            ok = false;
            
            lw::File resFile(filename);
            if (!resFile.IsOpen()) {
                lwerror("sound file is not exist: %s", filename);
                return;
            }
            
            _buffer = alutCreateBufferFromFileImage(resFile.DataPtr(), resFile.Size());
            if (_buffer == AL_NONE) {
                lwerror("sound buffer create failed: %s, error=%s", filename, alutGetErrorString(alutGetError()));
                return;
            }
            
            ok = true;
            _filename = filename;
            _soundBufferResMgr.add(filename, this);
        };
    
        ~SoundBufferRes() {
            if (_buffer)
                alDeleteBuffers(1, &_buffer);
            if (!_filename.empty())
                _soundBufferResMgr.del(_filename.c_str());
        }
        
    private:
        std::string _filename;
        ALuint _buffer;
    };
    
    //======================================
    class SoundImpl : public Sound{
    public:
        SoundImpl(const char *file);
        virtual ~SoundImpl();
        virtual void play();
        virtual void stop();
    
    private:
        ALuint _source;
        SoundBufferRes *_pBufRes;
    };
    
    Sound* Sound::create(const char *file){
        Sound *p = new SoundImpl(file);
        return p;
    }
    
    SoundImpl::SoundImpl(const char *file) {
        _pBufRes = SoundBufferRes::create(file);
        if (_pBufRes == NULL) {
            _pBufRes = SoundBufferRes::create("default.wav");
            assert(_pBufRes);
        }
        
        alGenSources(1, &_source);
        ALenum e = alGetError();
        if (e != AL_NO_ERROR) {
            lwerror("alGenSources failed: error=%d", e);
            return;
        }
        alSourcei(_source, AL_BUFFER, _pBufRes->getBuffer());
    }
    
    SoundImpl::~SoundImpl() {
        alDeleteSources(1, &_source);
        _pBufRes->release();
    }
    
    void SoundImpl::play() {
        alSourcePlay(_source);
    }
    
    void SoundImpl::stop() {
        alSourceStop(_source);
    }
}
