#include "lwPrefix.h"
#include "lwSpriteNode.h"
#include "lwTexture.h"
#include "lwEffects.h"
#include "lwMaterial.h"
#include "lwMesh.h"
#include "lwRenderState.h"
#include "lwApp.h"
#include "lwLog.h"
#include "tinyxml2/tinyxml2.h"
#include "lwfk/lwCamera.h"
#include <dirent.h>
#include <string.h>


namespace lw {
    
    namespace {
        struct AtlasInfo {
            std::string file;
            int u, v, w, h;
        };
        std::map<std::string, AtlasInfo> _atlasMap;
        KeyResMgr _materialResMgr;
    }
    
    Camera _camera;
    
    class SpriteVertexBuffer: public Mesh {
    public:
        SpriteVertexBuffer();
        ~SpriteVertexBuffer();
        
        void collectVetices(SptVertex *vertices, int numVertices, Material *pMaterial);
        void flush();
        
        enum {
            VERTICIS_NUM_RESERVE = 8192,
            VERTICIS_NUM_LIMIT = 65536,
        };
        
    private:
        std::vector<SptVertex> _vertices;
        Material *_pCurrMaterial;
        int _posLocation;
        int _uvLocation;
        int _mvpMatLocation;
        int _colorLocation;
        int _samplerLocation;
    };
    
    SpriteVertexBuffer::SpriteVertexBuffer() {
        _vertices.reserve(VERTICIS_NUM_RESERVE);
        _pCurrMaterial = NULL;
    }
    
    SpriteVertexBuffer::~SpriteVertexBuffer() {
        
    }
    
    
    void SpriteVertexBuffer::collectVetices(SptVertex *vertices, int numVertices, Material *pMaterial) {
        if (pMaterial != _pCurrMaterial || _vertices.size() >= VERTICIS_NUM_LIMIT) {
            flush();
        }
        _pCurrMaterial = pMaterial;
        
        for ( int i = 0; i < numVertices; ++i ) {
            _vertices.push_back(vertices[i]);
        }
    }
    
    void SpriteVertexBuffer::flush() {
        if (_vertices.empty()) {
            return;
        }
        
        //setup mesh
        char* p0 = (char*)&_vertices[0];
        this->position.set(sizeof(SptVertex), p0);
        this->uv[0].set(sizeof(SptVertex), p0+sizeof(float)*3);
        this->color.set(sizeof(SptVertex), p0+sizeof(float)*5);
        this->verticesCount = _vertices.size();
        
        _pCurrMaterial->draw(*this, PVRTMat4::Identity(), _camera, false);
        
        _vertices.clear();
    }
    
    namespace {
        SpriteVertexBuffer* _pvb = NULL;
    }
    
    
    //====================================================
    SpriteNode* SpriteNode::create(Node* pParent, const char *key, const char *fxName) {
        std::map<std::string, AtlasInfo>::iterator it = _atlasMap.find(key);
        if (it != _atlasMap.end()) {
            return createFromAtlas(pParent, key, fxName);
        } else {
            return createFromFile(pParent, key, fxName);
        }
    }
    
	SpriteNode* SpriteNode::createFromFile(Node* pParent, const char* textureFile, const char* fxName) {
        assert(textureFile && fxName);
        bool ok = false;
        SpriteNode* p = new SpriteNode(pParent, textureFile, fxName, false, ok);
        if (p) {
            if (ok) {
                return p;
            } else {
                delete p;
                return NULL;
            }
        }
        return NULL;
    }
    
    SpriteNode* SpriteNode::createFromAtlas(Node* pParent, const char* key, const char* fxName) {
        bool ok = false;
        SpriteNode* p = new SpriteNode(pParent, key, fxName, true, ok);
        if (p){
            if (ok) {
                return p;
            } else {
                delete p;
                return NULL;
            }
        }
        return NULL;
    }
    
    SpriteNode::SpriteNode(Node* pParent, const char *textureFile, const char *fxName, bool fromAtlas, bool &ok)
    :Node(pParent){
        ok = false;
        
        _color.set(1.0f, 1.0f, 1.0f, 1.0f);
        _pMaterial = NULL;
        
        int err = 0;
        if (fromAtlas) {
            err = loadFromAtlas(textureFile, fxName);
        }else{
            err = loadFromFile(textureFile, fxName);
        }
        
        ok = !err;
    }
    
    SpriteNode::~SpriteNode() {
        lwinfo("SpriteNode::~SpriteNode: refcount=%d", _pMaterial->refCount());
        if (_pMaterial) {
            if (_pMaterial->refCount() == 1) {
                _materialResMgr.del(_materialKey.c_str());
                lwinfo("del material");
            }
            _pMaterial->release();
        }
    }
    
    int SpriteNode::loadFromFile(const char *textureFile, const char *fxName) {
        assert(textureFile && fxName);
        
        _materialKey = fxName;
        _materialKey.append("/");
        _materialKey.append(textureFile);
        
        _pMaterial = (Material*)_materialResMgr.get(_materialKey.c_str());
        if (_pMaterial == NULL) {
            _pMaterial = Material::create("sprite.lwfx", fxName);
            if (_pMaterial == NULL) {
                lwerror("Material::create failed: sprite.lwfx, %s", fxName);
                return -1;
            }
            _materialResMgr.add(_materialKey.c_str(), _pMaterial);
        }
        _pTexture = _pMaterial->setTexture("input_texture", textureFile, 0);
        if (_pTexture == NULL) {
            lwerror("_pMaterial->setTexture failed: %s", textureFile);
            return -1;
        }
        uvInit();
        
        return 0;
    }
    
    int SpriteNode::loadFromAtlas(const char *key, const char *fxName) {
        assert(key && fxName);
        
        std::map<std::string, AtlasInfo>::iterator it = _atlasMap.find(key);
        if (it != _atlasMap.end()) {
            AtlasInfo& atlas = it->second;
            int err = loadFromFile(atlas.file.c_str(), fxName);
            if (err) {
                lwerror("loadFromFile error");
                return -1;
            }
            setUV(atlas.u, atlas.v, atlas.w, atlas.h);
            return 0;
        }
        return -1;
    }
    
    void SpriteNode::setUV(float u, float v, float w, float h) {
        float texW = (float)_pTexture->w;
        float texH = (float)_pTexture->h;
        
        _u = u;
        _v = v;
        _w = w;
        _h = h;
        
        _u1 = u/texW;
		_v1 = v/texH;
        _u2 = (u+_w)/texW;
		_v2 = (v+_h)/texH;
    }
    
    void SpriteNode::uvInit() {
        _u = _v = 0.f;
        _w = (float)_pTexture->w;
        _h = (float)_pTexture->h;
        _u1 = _v1 = 0.f;
        _u2 = _v2 = 1.f;
    }
    
    void SpriteNode::vFinalMatrixDidUpdate() {
        const PVRTMat4 &mat = getFinalMatrix();
        
        float posX1 = 0;
		float posY1 = 0;
		float posX2 = _w;
		float posY2 = _h;
        
        PVRTVec3 pt[4];
		pt[0].x = posX1;
        pt[0].y = posY1;
        pt[0].z = 0;
        pt[1].x = posX1;
        pt[1].y = posY2;
        pt[1].z = 0;
        pt[2].x = posX2;
        pt[2].y = posY1;
        pt[2].z = 0;
		pt[3].x = posX2;
        pt[3].y = posY2;
        pt[3].z = 0;
        
        PVRTVec3 ptt[4];
		PVRTTransformArray(ptt, pt, 4, &mat, f2vt(1.0));
        
        _vertexPos[0].x = ptt[0].x;
		_vertexPos[0].y = -ptt[0].y;
        
        _vertexPos[1].x = ptt[1].x;
		_vertexPos[1].y = -ptt[1].y;
        
        _vertexPos[2].x = ptt[2].x;
		_vertexPos[2].y = -ptt[2].y;
        
        _vertexPos[3].x = ptt[3].x;
		_vertexPos[3].y = -ptt[3].y;
    }
    
    void SpriteNode::getUV(float &u, float &v, float &w, float &h) {
        u = _u; v = _v; w = _w; h = _h;
    }
    
    void SpriteNode::setSize(float w, float h) {
        setScale(w/_w, h/_h);
    }
    
    void SpriteNode::getSize(float &w, float &h) {
        w = _w*_scaleX;
        h = _h*_scaleY;
    }
    
    void SpriteNode::setColor(const Color& color) {
        _color = color;
    }
    
    static void setSV(SptVertex& sv, float x, float y, float z, float u, float v, Color& color) {
        sv.x = x;
		sv.y = y;
        sv.z = z;
		sv.u = u;
		sv.v = v;
        sv.r = color.r;
        sv.g = color.g;
        sv.b = color.b;
        sv.a = color.a;
    }
    
    void SpriteNode::vDraw() {
        //1
        setSV(_vertices[0], _vertexPos[0].x, _vertexPos[0].y, _z, _u1, _v1, _color);

		//2
        setSV(_vertices[1], _vertexPos[1].x, _vertexPos[1].y, _z, _u1, _v2, _color);
        
		//3
        setSV(_vertices[2], _vertexPos[2].x, _vertexPos[2].y, _z, _u2, _v1, _color);
		
        //3
        setSV(_vertices[3], _vertexPos[2].x, _vertexPos[2].y, _z, _u2, _v1, _color);
		
		//2
        setSV(_vertices[4], _vertexPos[1].x, _vertexPos[1].y, _z, _u1, _v2, _color);

		//4
        setSV(_vertices[5], _vertexPos[3].x, _vertexPos[3].y, _z, _u2, _v2, _color);
        
        _pvb->collectVetices(_vertices, 6, _pMaterial);
    }
    
    TextureRes* SpriteNode:: getTexture(){
        return _pTexture;
    }

    
    //===============================================
    void SpriteNode::init() {
        _pvb = new SpriteVertexBuffer();
    }
    
    void SpriteNode::onInitView() {
        PVRTVec2 ss = screenSize();
        _camera.lookat(PVRTVec3(0.f, 0.f, 100.f), PVRTVec3(0.f, 0.f, 0.f), PVRTVec3(0.f, 1.f, 0.f));
        _camera.ortho(ss.x, ss.y, 0, 200.f);
    }
    
    void SpriteNode::quit() {
        if (_pvb) {
            delete _pvb;
            _pvb = NULL;
        }
    }
    
    void SpriteNode::flush() {
        _pvb->flush();
    }
    
    
    void SpriteNode::addAtlas(const char *file) {
        assert(file);
        tinyxml2::XMLDocument doc;
        lw::File resFile(file);
        if (resFile.IsOpen()) {
            tinyxml2::XMLError err = doc.Parse((const char*)resFile.DataPtr(), resFile.Size());
            assert(err == tinyxml2::XML_SUCCESS);
            const tinyxml2::XMLElement *pElem = doc.RootElement();
            pElem = pElem->FirstChildElement("image");
            while ( pElem ){
                const char *name = pElem->Attribute("name");
                AtlasInfo atlas;
                atlas.file = pElem->Attribute("file");
                pElem->QueryIntAttribute("x", &atlas.u);
                pElem->QueryIntAttribute("y", &atlas.v);
                pElem->QueryIntAttribute("w", &atlas.w);
                pElem->QueryIntAttribute("h", &atlas.h);
                _atlasMap[name] = atlas;
                
                pElem = pElem->NextSiblingElement();
            }
        } else {
            lwerror("file open error: %s", file);
        }
    }
    
    
} //namespace lw
