#ifndef _renderer_H_
#define _renderer_H_

#include "egRendererBase.h"

namespace Horde3DOpenGLRenderer {

	using namespace Horde3D;

	class OpenGLRenderDevice;

	class OpenGLRenderer : public IRenderer
	{
	public:
		OpenGLRenderer();
		~OpenGLRenderer();

		static IRenderer* factoryFunc();
		IRenderDevice* renderDevice();
		void registerRenderFunc( int nodeType, RenderFunc rf );

		unsigned char *useScratchBuf( uint32 minSize );

		bool init();
		void initStates();
		void initInternalRenderFuncs( int nodeType );

		void drawAABB( const Vec3f &bbMin, const Vec3f &bbMax );
		void drawSphere( const Vec3f &pos, float radius );
		void drawCone( float height, float fov, const Matrix4f &transMat );

		bool createShaderComb( const char *vertexShader, const char *fragmentShader, ShaderCombination &sc );
		void releaseShaderComb( ShaderCombination &sc );
		void setShaderComb( ShaderCombination *sc );
		void commitGeneralUniforms();
		bool setMaterial( MaterialResource *materialRes, const std::string &shaderContext );

		bool createShadowRB( uint32 width, uint32 height );
		void releaseShadowRB();

		int registerOccSet();
		void unregisterOccSet( int occSet );
		void drawOccProxies( uint32 list );
		void pushOccProxy( uint32 list, const Vec3f &bbMin, const Vec3f &bbMax, uint32 queryObj )
		{ _occProxies[list].push_back( OccProxy( bbMin, bbMax, queryObj ) ); }

		void showOverlays( const float *verts, uint32 vertCount, float *colRGBA,
			MaterialResource *matRes, int flags );
		void clearOverlays();

		void static drawMeshes( uint32 firstItem, uint32 lastItem, const std::string &shaderContext, const std::string &theClass,
			bool debugView, const Frustum *frust1, const Frustum *frust2, RenderingOrder::List order, int occSet );
		void static drawParticles( uint32 firstItem, uint32 lastItem, const std::string &shaderContext, const std::string &theClass,
			bool debugView, const Frustum *frust1, const Frustum *frust2, RenderingOrder::List order, int occSet );

		void render( CameraNode *camNode );
		void finalizeFrame();

		uint32 getFrameID() { return _frameID; }
		ShaderCombination *getCurShader() { return _curShader; }
		CameraNode *getCurCamera() { return _curCamera; }
		uint32 getQuadIdxBuf() { return _quadIdxBuf; }
		uint32 getParticleVBO() { return _particleVBO; }

	protected:
		void setupViewMatrices( const Matrix4f &viewMat, const Matrix4f &projMat );

		void createPrimitives();

		bool setMaterialRec( MaterialResource *materialRes, const std::string &shaderContext, ShaderResource *shaderRes );

		void setupShadowMap( bool noShadows );
		Matrix4f calcCropMatrix( const Frustum &frustSlice, const Vec3f lightPos, const Matrix4f &lightViewProjMat );
		void updateShadowMap();

		void drawOverlays( const std::string &shaderContext );

		void bindPipeBuffer( uint32 rbObj, const std::string &sampler, uint32 bufIndex );
		void clear( bool depth, bool buf0, bool buf1, bool buf2, bool buf3, float r, float g, float b, float a );
		void drawFSQuad( Resource *matRes, const std::string &shaderContext );
		void drawGeometry( const std::string &shaderContext, const std::string &theClass,
			RenderingOrder::List order, int occSet );
		void drawLightGeometry( const std::string &shaderContext, const std::string &theClass,
			bool noShadows, RenderingOrder::List order, int occSet );
		void drawLightShapes( const std::string &shaderContext, bool noShadows, int occSet );

		void drawRenderables( const std::string &shaderContext, const std::string &theClass, bool debugView,
			const Frustum *frust1, const Frustum *frust2, RenderingOrder::List order, int occSet );

		void renderDebugView();
		void finishRendering();

	protected:
		OpenGLRenderDevice				   *_rd;
		std::vector< RenderFuncListItem >  _renderFuncRegistry;

		unsigned char                      *_scratchBuf;
		uint32                             _scratchBufSize;

		Matrix4f                           _viewMat, _viewMatInv, _projMat, _viewProjMat, _viewProjMatInv;

		std::vector< PipeSamplerBinding >  _pipeSamplerBindings;
		std::vector< char >                _occSets;  // Actually bool
		std::vector< OccProxy >            _occProxies[2];  // 0: renderables, 1: lights

		std::vector< OverlayBatch >        _overlayBatches;
		OverlayVert                        *_overlayVerts;
		uint32                             _overlayVB;

		uint32                             _shadowRB;
		uint32                             _frameID;
		uint32                             _defShadowMap;
		uint32                             _quadIdxBuf;
		uint32                             _particleVBO;
		MaterialResource                   *_curStageMatLink;
		CameraNode                         *_curCamera;
		LightNode                          *_curLight;
		ShaderCombination                  *_curShader;
		RenderTarget                       *_curRenderTarget;
		uint32                             _curShaderUpdateStamp;

		uint32                             _maxAnisoMask;
		float                              _smSize;
		float                              _splitPlanes[5];
		Matrix4f                           _lightMats[4];

		uint32                             _vlPosOnly, _vlOverlay, _vlModel, _vlParticle;
		ShaderCombination                  _defColorShader;
		int                                _defColShader_color;  // Uniform location

		uint32                             _vbCube, _ibCube, _vbSphere, _ibSphere;
		uint32                             _vbCone, _ibCone, _vbFSPoly;
	};

}
#endif // _renderer_H_