#ifndef __LW_SOUND_H__
#define __LW_SOUND_H__

namespace lw {
    
    void soundInit();
    void soundQuit();
    
    class Sound{
    public:
        static Sound* create(const char *file);
        virtual ~Sound(){};
        virtual void play(){};
        virtual void stop(){};
    };
    
}

#endif //__LW_THREAD_H__