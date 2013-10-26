#ifndef __LW_SPRITE_NODE_H__
#define __LW_SPRITE_NODE_H__

#include "lwfk/lwColor.h"
#include "lwfk/node/lwNode.h"

namespace lw{
    
    class Material;
    class TextureRes;
    
    struct SptVertex{
        float x, y, z;
        float u, v;
        float r, g, b, a;
    };
    
	class SpriteNode : public Node{
    public:
        static SpriteNode* create(const char *key, const char *fxName, Node* pParent);
        static SpriteNode* createFromFile(const char *textureFile, const char *fxName, Node* pParent);
        static SpriteNode* createFromAtlas(const char *key, const char *fxName, Node* pParent);
        ~SpriteNode();
        void setUV(float u, float v, float w, float h);
        void getUV(float &u, float &v, float &w, float &h);
		void setSize(float w, float h);
		void getSize(float &w, float &h);
        void setColor(const Color &color);
        TextureRes* getTexture();
        
        static void addAtlas(const char *file);
        
    private:
        void uvInit();
        virtual void vFinalMatrixDidUpdate();
        virtual void vDraw();
        
        Material *_pMaterial;
        TextureRes *_pTexture;
        std::string _materialKey;
        Color _color;
        float _u, _v, _w, _h;
        float _u1, _v1, _u2, _v2;
        PVRTVec2 _vertexPos[4];
        SptVertex _vertices[6];
        
        SpriteNode(Node* pParent, const char *textureFile, const char *fxName, bool fromAtlas, bool &ok);
        int loadFromFile(const char* textureFile, const char* fxName);
        int loadFromAtlas(const char *key, const char *fxName);
        
    public:
        static void init();
        static void quit();
        static void flush();
        static void onInitView();
    };
    
	
} //namespace lw

#endif //__LW_SPRITE_H__