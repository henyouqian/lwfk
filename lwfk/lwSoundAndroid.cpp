#include "lwPrefix.h"
#include "lwSound.h"
#include "lwLog.h"
#include "lwRes.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>
#include <assert.h>

namespace lw {

    SLObjectItf mEngineObj; SLEngineItf mEngine;
    SLObjectItf mOutputMixObj;

    void soundInit() {
        SLresult lRes;
        const SLuint32      lEngineMixIIDCount = 1;
        const SLInterfaceID lEngineMixIIDs[]={SL_IID_ENGINE};
        const SLboolean lEngineMixReqs[]={SL_BOOLEAN_TRUE};
        const SLuint32 lOutputMixIIDCount=0;
        const SLInterfaceID lOutputMixIIDs[]={};
        const SLboolean lOutputMixReqs[]={};
        lRes = slCreateEngine(&mEngineObj, 0, NULL, lEngineMixIIDCount, lEngineMixIIDs, lEngineMixReqs);
        assert(lRes == SL_RESULT_SUCCESS);
        lRes=(*mEngineObj)->Realize(mEngineObj, SL_BOOLEAN_FALSE);
        assert(lRes == SL_RESULT_SUCCESS);
        lRes=(*mEngineObj)->GetInterface(mEngineObj, SL_IID_ENGINE, &mEngine);
        assert(lRes == SL_RESULT_SUCCESS);
        lRes=(*mEngine)->CreateOutputMix(mEngine, &mOutputMixObj, lOutputMixIIDCount, lOutputMixIIDs, lOutputMixReqs);
        assert(lRes == SL_RESULT_SUCCESS);
        lRes=(*mOutputMixObj)->Realize(mOutputMixObj, SL_BOOLEAN_FALSE);
        assert(lRes == SL_RESULT_SUCCESS);
}
    
    void soundQuit() {
        if (mOutputMixObj != NULL) {
            (*mOutputMixObj)->Destroy(mOutputMixObj);
            mOutputMixObj = NULL;
        }
        if (mEngineObj != NULL) {
            (*mEngineObj)->Destroy(mEngineObj);
            mEngineObj = NULL; mEngine = NULL;
        }
    }

    //======================================
    namespace { KeyResMgr _soundBufferResMgr; }

    struct WavInfo{
        short format;
        short numChannels;
        int sampleRate;
        int byteRate; 
        short blockAlign;
        short bitsPerSample;
    };
    
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
        
    private:
        SoundBufferRes(const char *filename, bool &ok) {
            assert(filename);
            ok = false;
            
            _pFile = new lw::File(filename);
            if (!_pFile->IsOpen()) {
                lwerror("sound file is not exist: %s", filename);
                return;
            }

            SLresult lRes;

            //
            char *p = (char*)_pFile->DataPtr();
            pWaveInfo = (WavInfo*)(p + 20);
            p += 16;
            p += *((int*)p);
            p += 8;

            int* pLen = (int*)p;
            mBuffer = (int16_t*) (p+4);
            mBufLen = *pLen;
            
            //
            ok = true;
            _filename = filename;
            _soundBufferResMgr.add(filename, this);
        };
    
        ~SoundBufferRes() {
            if (!_filename.empty())
                _soundBufferResMgr.del(_filename.c_str());

            if (_pFile)
                delete _pFile;
        }
        
    private:
        std::string _filename;
        lw::File *_pFile;

    public:
        void *mBuffer;
        int mBufLen;
        WavInfo *pWaveInfo;
    };

    
    //======================================
    class SoundImpl : public Sound{
    public:
        SoundImpl(const char *file);
        virtual ~SoundImpl();
        virtual void play();
        virtual void stop();
    
    private:
        SLObjectItf mPlayerObj;
        SLPlayItf mPlayer;
        SLBufferQueueItf mPlayerQueue;

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

        WavInfo *pInfo = _pBufRes->pWaveInfo;
        int channelMask = pInfo->numChannels == 1 ? SL_SPEAKER_FRONT_CENTER : SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;

        //
        // Set-up sound audio source.
        SLDataLocator_AndroidSimpleBufferQueue lDataLocatorIn;
        lDataLocatorIn.locatorType =
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
        // At most one buffer in the queue.
        lDataLocatorIn.numBuffers = 1;
        SLDataFormat_PCM lDataFormat;
        lDataFormat.formatType = SL_DATAFORMAT_PCM;
        lDataFormat.numChannels = pInfo->numChannels;
        lDataFormat.samplesPerSec = pInfo->sampleRate*1000;
        lDataFormat.bitsPerSample = pInfo->bitsPerSample;
        lDataFormat.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
        lDataFormat.channelMask = channelMask;
        lDataFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;
        SLDataSource lDataSource;
        lDataSource.pLocator = &lDataLocatorIn;
        lDataSource.pFormat = &lDataFormat;

        SLDataLocator_OutputMix lDataLocatorOut;
        lDataLocatorOut.locatorType = SL_DATALOCATOR_OUTPUTMIX;
        lDataLocatorOut.outputMix = mOutputMixObj;
        SLDataSink lDataSink;
        lDataSink.pLocator = &lDataLocatorOut;
        lDataSink.pFormat = NULL;

        const SLuint32 lSoundPlayerIIDCount = 2;
        const SLInterfaceID lSoundPlayerIIDs[] = { SL_IID_PLAY, SL_IID_BUFFERQUEUE };
        const SLboolean lSoundPlayerReqs[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
        SLresult lRes;
        lRes = (*mEngine)->CreateAudioPlayer(mEngine, &mPlayerObj, &lDataSource, &lDataSink, lSoundPlayerIIDCount, lSoundPlayerIIDs, lSoundPlayerReqs);
        assert(lRes == SL_RESULT_SUCCESS);
        lRes = (*mPlayerObj)->Realize(mPlayerObj, SL_BOOLEAN_FALSE);
        assert(lRes == SL_RESULT_SUCCESS);
        lRes = (*mPlayerObj)->GetInterface(mPlayerObj, SL_IID_PLAY, &mPlayer);
        assert(lRes == SL_RESULT_SUCCESS);
        lRes = (*mPlayerObj)->GetInterface(mPlayerObj, SL_IID_BUFFERQUEUE, &mPlayerQueue);
        assert(lRes == SL_RESULT_SUCCESS);
    }
    
    SoundImpl::~SoundImpl() {
        if (mPlayerObj != NULL) {
            (*mPlayerObj)->Destroy(mPlayerObj);
            mPlayerObj = NULL; mPlayer = NULL; mPlayerQueue = NULL;
        }
        if (_pBufRes) {
            _pBufRes->release();
        }
    }
    
    void SoundImpl::play() {
        SLresult lRes;
        SLuint32 lPlayerState;
        (*mPlayerObj)->GetState(mPlayerObj, &lPlayerState);
        if (lPlayerState == SL_OBJECT_STATE_REALIZED) {
            // Removes any sound from the queue.
            lRes = (*mPlayerQueue)->Clear(mPlayerQueue);
            assert(lRes == SL_RESULT_SUCCESS);
            // Plays the new sound.
            lRes = (*mPlayerQueue)->Enqueue(mPlayerQueue, _pBufRes->mBuffer, _pBufRes->mBufLen);
            assert(lRes == SL_RESULT_SUCCESS);

            (*mPlayer)->SetPlayState(mPlayer, SL_PLAYSTATE_PLAYING);
        }
    }
    
    void SoundImpl::stop() {
        SLresult lRes = (*mPlayerQueue)->Clear(mPlayerQueue);
        assert(lRes == SL_RESULT_SUCCESS);
        (*mPlayer)->SetPlayState(mPlayer, SL_PLAYSTATE_STOPPED);
    }
}
