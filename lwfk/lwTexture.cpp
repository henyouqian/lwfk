#include "lwPrefix.h"
#include "lwTexture.h"
#include "lwLog.h"
#include "soil/SOIL.h"
#include <map>

namespace lw {

    namespace {
        KeyResMgr _resMgr;
    }
	

	TextureRes::TextureRes(const char* fileName, bool& ok)
    :glId(-1) {
        ok = false;
		assert(fileName);
		fileName = fileName;

		size_t len = strlen(fileName);
		if ( len < 4 ) {
			lwerror("texture file name too short: %s", fileName);
			glId = -1;
			return;
		}
    
        lw::File resFile(fileName);
        if (resFile.IsOpen()) {
            if (loadAndCreateOgl((const unsigned char*)resFile.DataPtr(), resFile.Size())) {
                lwerror("Failed to load texture: %s", fileName);
            }
        } else {
            lwerror("texture file is not exist: %s", fileName);
            return;
        }
        ok = true;
	}

	int TextureRes::loadAndCreateOgl(const unsigned char* buf, int buflen) {
		unsigned char* pImgData = SOIL_load_image_from_memory(buf, buflen, &w, &h, &numChannels, SOIL_LOAD_AUTO);
        assert(pImgData);
		glId = SOIL_internal_create_OGL_texture(pImgData, w, h, numChannels,
			SOIL_CREATE_NEW_ID, 0,
			GL_TEXTURE_2D, GL_TEXTURE_2D,
			GL_MAX_TEXTURE_SIZE);
		
		SOIL_free_image_data(pImgData);
		
		if ( glId == 0 ){
            glId = -1;
            return -1;
		}
        return 0;
        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	}

	TextureRes::~TextureRes() {
		if ( glId != -1 ){
			glDeleteTextures(1, &glId);
		}
        _resMgr.del(fileName.c_str());
	}
	TextureRes* TextureRes::create(const char* fileName){
		assert(fileName);
        
        TextureRes *pRes = (TextureRes*)_resMgr.get(fileName);
        if (pRes) {
            return pRes;
        } else {
            bool ok = false;
            TextureRes* p = new TextureRes(fileName, ok);
			if ( p && ok  ){
                _resMgr.add(fileName, p);
				return p;
			}else if (p){
				delete p;
				return NULL;
			}
        }
        return NULL;
	}

} //namespace lw