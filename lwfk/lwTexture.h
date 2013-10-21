#ifndef __LW_TEXTURE_H__
#define __LW_TEXTURE_H__

#include "lwRes.h"

namespace lw{

	class TextureRes : public Res{
	public:
		static TextureRes* create(const char* fileName);
        
        GLuint glId;
		std::string fileName;
		int numChannels;
		int w, h;

	private:
		~TextureRes();
		TextureRes(const char* filename, bool& ok);
		int loadAndCreateOgl(const unsigned char* buf, int buflen);
	};

} //namespace lw


#endif //__LW_TEXTURE_H__