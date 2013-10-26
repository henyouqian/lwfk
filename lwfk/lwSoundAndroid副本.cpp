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
    SLObjectItf mPlayerObj; SLPlayItf mPlayer;
    SLBufferQueueItf mPlayerQueue;

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

        //
        // Set-up sound audio source.
        SLDataLocator_AndroidSimpleBufferQueue lDataLocatorIn;
        lDataLocatorIn.locatorType =
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
        // At most one buffer in the queue.
        lDataLocatorIn.numBuffers = 1;
        SLDataFormat_PCM lDataFormat;
        lDataFormat.formatType = SL_DATAFORMAT_PCM;
        lDataFormat.numChannels = 2;
        lDataFormat.samplesPerSec = SL_SAMPLINGRATE_44_1;
        lDataFormat.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
        lDataFormat.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
        lDataFormat.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
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
        lRes = (*mEngine)->CreateAudioPlayer(mEngine, &mPlayerObj, &lDataSource, &lDataSink, lSoundPlayerIIDCount, lSoundPlayerIIDs, lSoundPlayerReqs);
        assert(lRes == SL_RESULT_SUCCESS);
        lRes = (*mPlayerObj)->Realize(mPlayerObj, SL_BOOLEAN_FALSE);
        assert(lRes == SL_RESULT_SUCCESS);
        lRes = (*mPlayerObj)->GetInterface(mPlayerObj, SL_IID_PLAY, &mPlayer);
        assert(lRes == SL_RESULT_SUCCESS);
        lRes = (*mPlayerObj)->GetInterface(mPlayerObj, SL_IID_BUFFERQUEUE, &mPlayerQueue);
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
        if (mPlayerObj != NULL) {
            (*mPlayerObj)->Destroy(mPlayerObj);
            mPlayerObj = NULL; mPlayer = NULL; mPlayerQueue = NULL;
        }
    }

    
    //======================================
    class SoundImpl : public Sound{
    public:
        SoundImpl(const char *file);
        virtual ~SoundImpl();
        virtual void play();
        virtual void stop();
    
    private:
        lw::File *_pFile;
    };
    
    Sound* Sound::create(const char *file){
        Sound *p = new SoundImpl(file);
        return p;
    }
    
    SoundImpl::SoundImpl(const char *file) {
        _pFile = new lw::File(file);
        if (!_pFile->IsOpen()) {
            lwerror("sound file is not exist: %s", file);
            return;
        }
    }
    
    SoundImpl::~SoundImpl() {
        if (_pFile)
            delete _pFile;
    }
    
    void SoundImpl::play() {
        SLresult lRes;
        SLuint32 lPlayerState;
        (*mPlayerObj)->GetState(mPlayerObj, &lPlayerState);
        if (lPlayerState == SL_OBJECT_STATE_REALIZED) {
            char *p = (char*)_pFile->DataPtr();
            p += 16;
            p += *((int*)p);
            p += 8;

            int* pLen = (int*)p;
            int16_t* lBuffer = (int16_t*) (p+4);

            // Removes any sound from the queue.
            lRes = (*mPlayerQueue)->Clear(mPlayerQueue);
            assert(lRes == SL_RESULT_SUCCESS);
            // Plays the new sound.
            lRes = (*mPlayerQueue)->Enqueue(mPlayerQueue, lBuffer, *pLen);
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
