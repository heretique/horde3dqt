#include "utOpenGL.h"
#include "renderer.h"
#include "renderdevice.h"
#include "egParticle.h"
#include "egLight.h"
#include "egCamera.h"
#include "egModules.h"
#include "egCom.h"
#include <cstring>

#include "utDebug.h"

namespace Horde3DOpenGLRenderer {

	using namespace std;

	const char *vsDefColor =
		"uniform mat4 viewProjMat;\n"
		"uniform mat4 worldMat;\n"
		"attribute vec3 vertPos;\n"
		"void main() {\n"
		"	gl_Position = viewProjMat * worldMat * vec4( vertPos, 1.0 );\n"
		"}\n";

	const char *fsDefColor =
		"uniform vec4 color;\n"
		"void main() {\n"
		"	gl_FragColor = color;\n"
		"}\n";


	OpenGLRenderer::OpenGLRenderer() : _rd(0x0)
	{
		_scratchBuf = 0x0;
		_overlayVerts = 0x0;
		_scratchBufSize = 0;
		_frameID = 1;
		_defShadowMap = 0;
		_quadIdxBuf = 0;
		_particleVBO = 0;
		_curCamera = 0x0;
		_curLight = 0x0;
		_curShader = 0x0;
		_curRenderTarget = 0x0;
		_curShaderUpdateStamp = 1;
		_maxAnisoMask = 0;
		_smSize = 0;
		_shadowRB = 0;
		_vlPosOnly = 0;
		_vlOverlay = 0;
		_vlModel = 0;
		_vlParticle = 0;
	}


	OpenGLRenderer::~OpenGLRenderer()
	{
		releaseShadowRB();
		_rd->destroyTexture( _defShadowMap );
		_rd->destroyBuffer( _particleVBO );
		releaseShaderComb( _defColorShader );

		delete[] _scratchBuf;
		delete[] _overlayVerts;
		delete _rd;
	}


	// =================================================================================================
	// Basic Initialization and Setup
	// =================================================================================================


	IRenderer* OpenGLRenderer::factoryFunc()
	{
		return new OpenGLRenderer();
	}

	IRenderDevice* OpenGLRenderer::renderDevice()
	{
		return _rd;
	}

	void OpenGLRenderer::registerRenderFunc( int nodeType, RenderFunc rf )
	{
		RenderFuncListItem item;
		item.nodeType = nodeType;
		item.renderFunc = rf;
		_renderFuncRegistry.push_back( item );
	}


	unsigned char *OpenGLRenderer::useScratchBuf( uint32 minSize )
	{
		if( _scratchBufSize < minSize )
		{
			delete[] _scratchBuf;
			_scratchBuf = new unsigned char[minSize + 15];
			_scratchBufSize = minSize;
		}

		return _scratchBuf + (size_t)_scratchBuf % 16;  // 16 byte aligned
	}


	bool OpenGLRenderer::init()
	{
		_rd = new OpenGLRenderDevice();
		// Init Render Device Interface
		if( !_rd->init() ) return false;

		// Check capabilities
        if( !_rd->getCaps().texFloat )
            Modules::log().writeWarning( "Renderer: No floating point texture support available" );
        if( !_rd->getCaps().texNPOT )
            Modules::log().writeWarning( "Renderer: No non-Power-of-two texture support available" );
        if( !_rd->getCaps().rtMultisampling )
            Modules::log().writeWarning( "Renderer: No multisampling for render targets available" );

		// Create vertex layouts
		VertexLayoutAttrib attribsPosOnly[1] = {
			{"vertPos", 0, 3, 0}
		};
		_vlPosOnly = _rd->registerVertexLayout( 1, attribsPosOnly );

		VertexLayoutAttrib attribsOverlay[2] = {
			{"vertPos", 0, 2, 0},
			{"texCoords0", 0, 2, 8}
		};
		_vlOverlay = _rd->registerVertexLayout( 2, attribsOverlay );

		VertexLayoutAttrib attribsModel[7] = {
			{"vertPos", 0, 3, 0},
			{"normal", 1, 3, 0},
			{"tangent", 2, 4, 0},
			{"joints", 3, 4, 8},
			{"weights", 3, 4, 24},
			{"texCoords0", 3, 2, 0},
			{"texCoords1", 3, 2, 40}
		};
		_vlModel = _rd->registerVertexLayout( 7, attribsModel );

		VertexLayoutAttrib attribsParticle[2] = {
			{"texCoords0", 0, 2, 0},
			{"parIdx", 0, 1, 8}
		};
		_vlParticle = _rd->registerVertexLayout( 2, attribsParticle );

		// Upload default shaders
		if( !createShaderComb( vsDefColor, fsDefColor, _defColorShader ) )
		{
            Modules::log().writeError( "Failed to compile default shaders" );
			return false;
		}

		// Cache common uniforms
		_defColShader_color = _rd->getShaderConstLoc( _defColorShader.shaderObj, "color" );

		// Create shadow map render target
		if( !createShadowRB( Modules::config().shadowMapSize, Modules::config().shadowMapSize ) )
		{
            Modules::log().writeError( "Failed to create shadow map" );
			return false;
		}

		// Create default shadow map
		float shadowTex[16] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
		_defShadowMap = _rd->createTexture( TextureTypes::Tex2D, 4, 4, 1, TextureFormats::DEPTH, false, false, false, false );
		_rd->uploadTextureData( _defShadowMap, 0, 0, shadowTex );

		// Create index buffer used for drawing quads
		uint16 *quadIndices = new uint16[QuadIndexBufCount];
		for( uint32 i = 0; i < QuadIndexBufCount / 6; ++i )
		{
			quadIndices[i*6+0] = i * 4 + 0; quadIndices[i*6+1] = i * 4 + 1; quadIndices[i*6+2] = i * 4 + 2;
			quadIndices[i*6+3] = i * 4 + 2; quadIndices[i*6+4] = i * 4 + 3; quadIndices[i*6+5] = i * 4 + 0;
		}
		_quadIdxBuf = _rd->createIndexBuffer( QuadIndexBufCount * sizeof( uint16 ), quadIndices );
		delete[] quadIndices; quadIndices = 0x0;

		// Create particle geometry array
		ParticleVert v0( 0, 0 );
		ParticleVert v1( 1, 0 );
		ParticleVert v2( 1, 1 );
		ParticleVert v3( 0, 1 );

		ParticleVert *parVerts = new ParticleVert[ParticlesPerBatch * 4];
		for( uint32 i = 0; i < ParticlesPerBatch; ++i )
		{
			parVerts[i * 4 + 0] = v0; parVerts[i * 4 + 0].index = (float)i;
			parVerts[i * 4 + 1] = v1; parVerts[i * 4 + 1].index = (float)i;
			parVerts[i * 4 + 2] = v2; parVerts[i * 4 + 2].index = (float)i;
			parVerts[i * 4 + 3] = v3; parVerts[i * 4 + 3].index = (float)i;
		}
		_particleVBO = _rd->createVertexBuffer( ParticlesPerBatch * 4 * sizeof( ParticleVert ), (float *)parVerts );
		delete[] parVerts; parVerts = 0x0;

		_overlayBatches.reserve( 64 );
		_overlayVerts = new OverlayVert[MaxNumOverlayVerts];
		_overlayVB = _rd->createVertexBuffer( MaxNumOverlayVerts * sizeof( OverlayVert ), 0x0 );

		// Create unit primitives
		createPrimitives();

		// Init scratch buffer with some default size
		useScratchBuf( 4 * 1024*1024 );

		// Reset states
		finishRendering();

		return true;
	}


	void OpenGLRenderer::initStates()
	{
		_rd->initStates();
	}

	void OpenGLRenderer::initInternalRenderFuncs( int nodeType )
	{
		switch (nodeType)
		{
		case SceneNodeTypes::Mesh:
			registerRenderFunc( SceneNodeTypes::Mesh, drawMeshes );
			break;
		case SceneNodeTypes::Emitter:
			registerRenderFunc( SceneNodeTypes::Emitter, drawParticles );
			break;
		}
	}


	// =================================================================================================
	// Misc Helper Functions
	// =================================================================================================

	void OpenGLRenderer::setupViewMatrices( const Matrix4f &viewMat, const Matrix4f &projMat )
	{
		// Note: The viewing matrices should be set before a material is set, otherwise the general
		//       uniforms need to be commited manually

		_viewMat = viewMat;
		_viewMatInv = viewMat.inverted();
		_projMat = projMat;
		_viewProjMat = projMat * viewMat;
		_viewProjMatInv = _viewProjMat.inverted();

		++_curShaderUpdateStamp;
	}


	// =================================================================================================
	// Rendering Helper Functions
	// =================================================================================================

	void OpenGLRenderer::createPrimitives()
	{
		// Unit cube
		float cubeVerts[8 * 3] = {  // x, y, z
			0.f, 0.f, 1.f,   1.f, 0.f, 1.f,   1.f, 1.f, 1.f,   0.f, 1.f, 1.f,
			0.f, 0.f, 0.f,   1.f, 0.f, 0.f,   1.f, 1.f, 0.f,   0.f, 1.f, 0.f
		};
		uint16 cubeInds[36] = {
			0, 1, 2, 2, 3, 0,   1, 5, 6, 6, 2, 1,   5, 4, 7, 7, 6, 5,
			4, 0, 3, 3, 7, 4,   3, 2, 6, 6, 7, 3,   4, 5, 1, 1, 0, 4
		};
		_vbCube = _rd->createVertexBuffer( 8 * 3 * sizeof( float ), cubeVerts );
		_ibCube = _rd->createIndexBuffer( 36 * sizeof( uint16 ), cubeInds );

		// Unit (geodesic) sphere (created by recursively subdividing a base octahedron)
		Vec3f spVerts[126] = {  // x, y, z
			Vec3f( 0.f, 1.f, 0.f ),   Vec3f( 0.f, -1.f, 0.f ),
			Vec3f( -0.707f, 0.f, 0.707f ),   Vec3f( 0.707f, 0.f, 0.707f ),
			Vec3f( 0.707f, 0.f, -0.707f ),   Vec3f( -0.707f, 0.f, -0.707f )
		};
		uint16 spInds[128 * 3] = {  // Number of faces: (4 ^ iterations) * 8
			2, 3, 0,   3, 4, 0,   4, 5, 0,   5, 2, 0,   2, 1, 3,   3, 1, 4,   4, 1, 5,   5, 1, 2
		};
		for( uint32 i = 0, nv = 6, ni = 24; i < 2; ++i )  // Two iterations
		{
			// Subdivide each face into 4 tris by bisecting each edge and push vertices onto unit sphere
			for( uint32 j = 0, prevNumInds = ni; j < prevNumInds; j += 3 )
			{
				spVerts[nv++] = ((spVerts[spInds[j + 0]] + spVerts[spInds[j + 1]]) * 0.5f).normalized();
				spVerts[nv++] = ((spVerts[spInds[j + 1]] + spVerts[spInds[j + 2]]) * 0.5f).normalized();
				spVerts[nv++] = ((spVerts[spInds[j + 2]] + spVerts[spInds[j + 0]]) * 0.5f).normalized();

				spInds[ni++] = spInds[j + 0]; spInds[ni++] = nv - 3; spInds[ni++] = nv - 1;
				spInds[ni++] = nv - 3; spInds[ni++] = spInds[j + 1]; spInds[ni++] = nv - 2;
				spInds[ni++] = nv - 2; spInds[ni++] = spInds[j + 2]; spInds[ni++] = nv - 1;
				spInds[j + 0] = nv - 3; spInds[j + 1] = nv - 2; spInds[j + 2] = nv - 1;
			}
		}
		_vbSphere = _rd->createVertexBuffer( 126 * sizeof( Vec3f ), spVerts );
		_ibSphere = _rd->createIndexBuffer( 128 * 3 * sizeof( uint16 ), spInds );

		// Unit cone
		float coneVerts[13 * 3] = {  // x, y, z
			0.f, 0.f, 0.f,
			0.f, 1.f, -1.f,   -0.5f, 0.866f, -1.f,   -0.866f, 0.5f, -1.f,
			-1.f, 0.f, -1.f,   -0.866f, -0.5f, -1.f,   -0.5f, -0.866f, -1.f,
			0.f, -1.f, -1.f,   0.5f, -0.866f, -1.f,   0.866f, -0.5f, -1.f,
			1.f, 0.f, -1.f,   0.866f, 0.5f, -1.f,   0.5f, 0.866f, -1.f,
		};
		uint16 coneInds[22 * 3] = {
			0, 1, 2,   0, 2, 3,   0, 3, 4,   0, 4, 5,   0, 5, 6,   0, 6, 7,
			0, 7, 8,   0, 8, 9,   0, 9, 10,   0, 10, 11,   0, 11, 12,   0, 12, 1,
			10, 6, 2,   10, 8, 6,   10, 9, 8,   8, 7, 6,   6, 4, 2,   6, 5, 4,   4, 3, 2,
			2, 12, 10,   2, 1, 12,   12, 11, 10
		};
		_vbCone = _rd->createVertexBuffer( 13 * 3 * sizeof( float ), coneVerts );
		_ibCone = _rd->createIndexBuffer( 22 * 3 * sizeof( uint16 ), coneInds );

		// Fullscreen polygon
		float fsVerts[3 * 3] = {  // x, y, z
			0.f, 0.f, 1.f,   2.f, 0.f, 1.f,   0.f, 2.f, 1.f
		};
		_vbFSPoly = _rd->createVertexBuffer( 3 * 3 * sizeof( float ), fsVerts );
	}


	void OpenGLRenderer::drawAABB( const Vec3f &bbMin, const Vec3f &bbMax )
	{
		ASSERT( _curShader != 0x0 );

		Matrix4f mat = Matrix4f::TransMat( bbMin.x, bbMin.y, bbMin.z ) *
			Matrix4f::ScaleMat( bbMax.x - bbMin.x, bbMax.y - bbMin.y, bbMax.z - bbMin.z );
		_rd->setShaderConst( _curShader->uni_worldMat, CONST_FLOAT44, &mat.x[0] );

		_rd->setVertexBuffer( 0, _vbCube, 0, 12 );
		_rd->setIndexBuffer( _ibCube, IDXFMT_16 );
		_rd->setVertexLayout( _vlPosOnly );

		_rd->drawIndexed( PRIM_TRILIST, 0, 36, 0, 8 );
	}


	void OpenGLRenderer::drawSphere( const Vec3f &pos, float radius )
	{
		ASSERT( _curShader != 0x0 );

		Matrix4f mat = Matrix4f::TransMat( pos.x, pos.y, pos.z ) *
			Matrix4f::ScaleMat( radius, radius, radius );
		_rd->setShaderConst( _curShader->uni_worldMat, CONST_FLOAT44, &mat.x[0] );

		_rd->setVertexBuffer( 0, _vbSphere, 0, 12 );
		_rd->setIndexBuffer( _ibSphere, IDXFMT_16 );
		_rd->setVertexLayout( _vlPosOnly );

		_rd->drawIndexed( PRIM_TRILIST, 0, 128 * 3, 0, 126 );
	}


	void OpenGLRenderer::drawCone( float height, float radius, const Matrix4f &transMat )
	{
		ASSERT( _curShader != 0x0 );

		Matrix4f mat = transMat * Matrix4f::ScaleMat( radius, radius, height );
		_rd->setShaderConst( _curShader->uni_worldMat, CONST_FLOAT44, &mat.x[0] );

		_rd->setVertexBuffer( 0, _vbCone, 0, 12 );
		_rd->setIndexBuffer( _ibCone, IDXFMT_16 );
		_rd->setVertexLayout( _vlPosOnly );

		_rd->drawIndexed( PRIM_TRILIST, 0, 22 * 3, 0, 13 );
	}


	// =================================================================================================
	// Material System
	// =================================================================================================

	bool OpenGLRenderer::createShaderComb( const char *vertexShader, const char *fragmentShader, ShaderCombination &sc )
	{
		// Create shader program
		uint32 shdObj = _rd->createShader( vertexShader, fragmentShader );
		if( shdObj == 0 ) return false;

		sc.shaderObj = shdObj;
		_rd->bindShader( shdObj );

		// Set standard uniforms
		int loc =_rd-> getShaderSamplerLoc( shdObj, "shadowMap" );
		if( loc >= 0 ) _rd->setShaderSampler( loc, 12 );

		// Misc general uniforms
		sc.uni_frameBufSize = _rd->getShaderConstLoc( shdObj, "frameBufSize" );

		// View/projection uniforms
		sc.uni_viewMat = _rd->getShaderConstLoc( shdObj, "viewMat" );
		sc.uni_viewMatInv = _rd->getShaderConstLoc( shdObj, "viewMatInv" );
		sc.uni_projMat = _rd->getShaderConstLoc( shdObj, "projMat" );
		sc.uni_viewProjMat = _rd->getShaderConstLoc( shdObj, "viewProjMat" );
		sc.uni_viewProjMatInv = _rd->getShaderConstLoc( shdObj, "viewProjMatInv" );
		sc.uni_viewerPos = _rd->getShaderConstLoc( shdObj, "viewerPos" );

		// Per-instance uniforms
		sc.uni_worldMat = _rd->getShaderConstLoc( shdObj, "worldMat" );
		sc.uni_worldNormalMat = _rd->getShaderConstLoc( shdObj, "worldNormalMat" );
		sc.uni_nodeId = _rd->getShaderConstLoc( shdObj, "nodeId" );
		sc.uni_customInstData = _rd->getShaderConstLoc( shdObj, "customInstData[0]" );
		sc.uni_skinMatRows = _rd->getShaderConstLoc( shdObj, "skinMatRows[0]" );

		// Lighting uniforms
		sc.uni_lightPos = _rd->getShaderConstLoc( shdObj, "lightPos" );
		sc.uni_lightDir = _rd->getShaderConstLoc( shdObj, "lightDir" );
		sc.uni_lightColor = _rd->getShaderConstLoc( shdObj, "lightColor" );
		sc.uni_shadowSplitDists = _rd->getShaderConstLoc( shdObj, "shadowSplitDists" );
		sc.uni_shadowMats = _rd->getShaderConstLoc( shdObj, "shadowMats" );
		sc.uni_shadowMapSize = _rd->getShaderConstLoc( shdObj, "shadowMapSize" );
		sc.uni_shadowBias = _rd->getShaderConstLoc( shdObj, "shadowBias" );

		// Particle-specific uniforms
		sc.uni_parPosArray = _rd->getShaderConstLoc( shdObj, "parPosArray" );
		sc.uni_parSizeAndRotArray = _rd->getShaderConstLoc( shdObj, "parSizeAndRotArray" );
		sc.uni_parColorArray = _rd->getShaderConstLoc( shdObj, "parColorArray" );

		// Overlay-specific uniforms
		sc.uni_olayColor = _rd->getShaderConstLoc( shdObj, "olayColor" );

		return true;
	}


	void OpenGLRenderer::releaseShaderComb( ShaderCombination &sc )
	{
		_rd->destroyShader( sc.shaderObj );
	}


	void OpenGLRenderer::setShaderComb( ShaderCombination *sc )
	{
		if( _curShader != sc )
		{
			if( sc == 0x0 ) _rd->bindShader( 0 );
			else _rd->bindShader( sc->shaderObj );

			_curShader = sc;
		}
	}


	void OpenGLRenderer::commitGeneralUniforms()
	{
		ASSERT( _curShader != 0x0 );

		// Note: Make sure that all functions which modify one of the following params increase the stamp
		if( _curShader->lastUpdateStamp != _curShaderUpdateStamp )
		{
			if( _curShader->uni_frameBufSize >= 0 )
			{
				float dimensions[2] = { (float)_rd->frameBufferWidth(), (float)_rd->frameBufferHeight() };
				_rd->setShaderConst( _curShader->uni_frameBufSize, CONST_FLOAT2, dimensions );
			}

			// Viewer params
			if( _curShader->uni_viewMat >= 0 )
				_rd->setShaderConst( _curShader->uni_viewMat, CONST_FLOAT44, _viewMat.x );

			if( _curShader->uni_viewMatInv >= 0 )
				_rd->setShaderConst( _curShader->uni_viewMatInv, CONST_FLOAT44, _viewMatInv.x );

			if( _curShader->uni_projMat >= 0 )
				_rd->setShaderConst( _curShader->uni_projMat, CONST_FLOAT44, _projMat.x );

			if( _curShader->uni_viewProjMat >= 0 )
				_rd->setShaderConst( _curShader->uni_viewProjMat, CONST_FLOAT44, _viewProjMat.x );

			if( _curShader->uni_viewProjMatInv >= 0 )
				_rd->setShaderConst( _curShader->uni_viewProjMatInv, CONST_FLOAT44, _viewProjMatInv.x );

			if( _curShader->uni_viewerPos >= 0 )
				_rd->setShaderConst( _curShader->uni_viewerPos, CONST_FLOAT3, &_viewMatInv.x[12] );

			// Light params
			if( _curLight != 0x0 )
			{
				if( _curShader->uni_lightPos >= 0 )
				{
					float data[4] = { _curLight->absolutePos().x, _curLight->absolutePos().y,
						_curLight->absolutePos().z, _curLight->radius() };
					_rd->setShaderConst( _curShader->uni_lightPos, CONST_FLOAT4, data );
				}

				if( _curShader->uni_lightDir >= 0 )
				{
					float data[4] = { _curLight->spotDirection().x, _curLight->spotDirection().y,
						_curLight->spotDirection().z, cosf( degToRad( _curLight->fov() / 2.0f ) ) };
					_rd->setShaderConst( _curShader->uni_lightDir, CONST_FLOAT4, data );
				}

				if( _curShader->uni_lightColor >= 0 )
				{
					Vec3f col = _curLight->diffuseColor() * _curLight->diffuseColorMult();
					_rd->setShaderConst( _curShader->uni_lightColor, CONST_FLOAT3, &col.x );
				}

				if( _curShader->uni_shadowSplitDists >= 0 )
					_rd->setShaderConst( _curShader->uni_shadowSplitDists, CONST_FLOAT4, &_splitPlanes[1] );

				if( _curShader->uni_shadowMats >= 0 )
					_rd->setShaderConst( _curShader->uni_shadowMats, CONST_FLOAT44, &_lightMats[0].x[0], 4 );

				if( _curShader->uni_shadowMapSize >= 0 )
					_rd->setShaderConst( _curShader->uni_shadowMapSize, CONST_FLOAT, &_smSize );

				if( _curShader->uni_shadowBias >= 0 )
				{
					float shadowMapBias = _curLight->shadowMapBias();
					_rd->setShaderConst( _curShader->uni_shadowBias, CONST_FLOAT, &shadowMapBias );
				}
			}

			_curShader->lastUpdateStamp = _curShaderUpdateStamp;
		}
	}


	bool OpenGLRenderer::setMaterialRec( MaterialResource *materialRes, const string &shaderContext,
		ShaderResource *shaderRes )
	{
		if( materialRes == 0x0 ) return false;

		bool firstRec = (shaderRes == 0x0);
		bool result = true;

		// Set shader in first recursion step
		if( firstRec )
		{	
			shaderRes = materialRes->shaderRes();
			if( shaderRes == 0x0 ) return false;	

			// Find context
			ShaderContext *context = shaderRes->findContext( shaderContext );
			if( context == 0x0 ) return false;

			// Set shader combination
			ShaderCombination *sc = shaderRes->getCombination( *context, materialRes->combinationMask() );
			if( sc != _curShader ) setShaderComb( sc );
			if( _curShader == 0x0 || _rd->currentShaderHandle() == 0 ) return false;

			// Setup standard shader uniforms
			commitGeneralUniforms();

			// Configure depth mask
			if( context->writeDepth ) glDepthMask( GL_TRUE );
			else glDepthMask( GL_FALSE );

			// Configure cull mode
			if( !Modules::config().wireframeMode )
			{
				switch( context->cullMode )
				{
				case CullModes::Back:
					glEnable( GL_CULL_FACE );
					glCullFace( GL_BACK );
					break;
				case CullModes::Front:
					glEnable( GL_CULL_FACE );
					glCullFace( GL_FRONT );
					break;
				case CullModes::None:
					glDisable( GL_CULL_FACE );
					break;
				}
			}

			// Configure blending
			switch( context->blendMode )
			{
			case BlendModes::Replace:
				glDisable( GL_BLEND );
				break;
			case BlendModes::Blend:
				glEnable( GL_BLEND );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				break;
			case BlendModes::Add:
				glEnable( GL_BLEND );
				glBlendFunc( GL_ONE, GL_ONE );
				break;
			case BlendModes::AddBlended:
				glEnable( GL_BLEND );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE );
				break;
			case BlendModes::Mult:
				glEnable( GL_BLEND );
				glBlendFunc( GL_DST_COLOR, GL_ZERO );
				break;
			}

			// Configure depth test
			if( context->depthTest )
			{
				glEnable( GL_DEPTH_TEST );

				switch( context->depthFunc )
				{
				case TestModes::LessEqual:
					glDepthFunc( GL_LEQUAL );
					break;
				case TestModes::Equal:
					glDepthFunc( GL_EQUAL );
					break;
				case TestModes::Always:
					glDepthFunc( GL_ALWAYS );
					break;
				case TestModes::Less:
					glDepthFunc( GL_LESS );
					break;
				case TestModes::Greater:
					glDepthFunc( GL_GREATER );
					break;
				case TestModes::GreaterEqual:
					glDepthFunc( GL_GEQUAL );
					break;
				}
			}
			else
			{
				glDisable( GL_DEPTH_TEST );
			}

			// Configure alpha-to-coverage
			if( context->alphaToCoverage && Modules::config().sampleCount > 0 )
				glEnable( GL_SAMPLE_ALPHA_TO_COVERAGE );
			else
				glDisable( GL_SAMPLE_ALPHA_TO_COVERAGE );
		}

		// Setup texture samplers
		const std::vector< ShaderSampler > &shaderSamplers = shaderRes->samplers();
		for( size_t i = 0, si = shaderSamplers.size(); i < si; ++i )
		{
			if( _curShader->customSamplers[i] < 0 ) continue;

			const ShaderSampler &sampler = shaderSamplers[i];
			TextureResource *texRes = 0x0;

			// Use default texture
			if( firstRec) texRes = sampler.defTex;

			// Find sampler in material
			const std::vector< MatSampler > &matSamplers = materialRes->samplers();
			for( size_t j = 0, sj = matSamplers.size(); j < sj; ++j )
			{
				if( matSamplers[j].name == sampler.id )
				{
					if( matSamplers[j].texRes->isLoaded() )
						texRes = matSamplers[j].texRes;
					break;
				}
			}

			uint32 sampState = shaderSamplers[i].sampState;
			if( (sampState & SS_FILTER_TRILINEAR) && !Modules::config().trilinearFiltering )
				sampState = (sampState & ~SS_FILTER_TRILINEAR) | SS_FILTER_BILINEAR;
			if( (sampState & SS_ANISO_MASK) > _maxAnisoMask )
				sampState = (sampState & ~SS_ANISO_MASK) | _maxAnisoMask;

			// Bind texture
			if( texRes != 0x0 )
			{
				if( texRes->getTexType() != sampler.type ) break;  // Wrong type

				if( texRes->getTexType() == TextureTypes::Tex2D )
				{
					if( texRes->getRBObject() == 0 )
					{
						_rd->setTexture( shaderSamplers[i].texUnit, texRes->getTexObject(), sampState );
					}
					else if( texRes->getRBObject() != _rd->currentRenderBufferHandle() )
					{
						_rd->setTexture( shaderSamplers[i].texUnit,
							_rd->getRenderBufferTex( texRes->getRBObject(), 0 ), sampState );
					}
					else  // Trying to bind active render buffer as texture
					{
						_rd->setTexture( shaderSamplers[i].texUnit, TextureResource::defTex2DObject, 0 );
					}
				}
				else
				{
					_rd->setTexture( shaderSamplers[i].texUnit, texRes->getTexObject(), sampState );
				}
			}

			// Find sampler in pipeline
			if( firstRec )
			{
				for( size_t j = 0, sj = _pipeSamplerBindings.size(); j < sj; ++j )
				{
					if( strcmp( _pipeSamplerBindings[j].sampler, sampler.id.c_str() ) == 0 )
					{
						_rd->setTexture( shaderSamplers[i].texUnit, _rd->getRenderBufferTex(
							_pipeSamplerBindings[j].rbObj, _pipeSamplerBindings[j].bufIndex ), sampState );

						break;
					}
				}
			}
		}

		// Set custom uniforms
		std::vector< ShaderUniform > &shaderUniforms = shaderRes->uniforms();
		for( size_t i = 0, si = shaderUniforms.size(); i < si; ++i )
		{
			if( _curShader->customUniforms[i] < 0 ) continue;

			float *unifData = 0x0;

			// Find uniform in material
			std::vector< MatUniform > &matUniforms = materialRes->uniforms();
			for( size_t j = 0, sj = matUniforms.size(); j < sj; ++j )
			{
				MatUniform &matUniform = matUniforms[j];

				if( matUniform.name == shaderUniforms[i].id )
				{
					unifData = matUniform.values;
					break;
				}
			}

			// Use default values if not found
			if( unifData == 0x0 && firstRec )
				unifData = shaderUniforms[i].defValues;

			if( unifData )
			{
				switch( shaderUniforms[i].size )
				{
				case 1:
					_rd->setShaderConst( _curShader->customUniforms[i], CONST_FLOAT, unifData );
					break;
				case 4:
					_rd->setShaderConst( _curShader->customUniforms[i], CONST_FLOAT4, unifData );
					break;
				}
			}
		}

		if( firstRec )
		{
			// Handle link of stage
			if( _curStageMatLink != 0x0 && _curStageMatLink != materialRes )
				result &= setMaterialRec( _curStageMatLink, shaderContext, shaderRes );

			// Handle material of light source
			if( _curLight != 0x0 && _curLight->materialRes() != 0x0 && _curLight->materialRes() != materialRes )
				result &= setMaterialRec( _curLight->materialRes(), shaderContext, shaderRes );
		}

		// Handle link of material resource
		if( materialRes->materialLink() != 0x0 )
			result &= setMaterialRec( materialRes->materialLink(), shaderContext, shaderRes );

		return result;
	}


	bool OpenGLRenderer::setMaterial( MaterialResource *materialRes, const string &shaderContext )
	{
		if( materialRes == 0x0 )
		{	
			setShaderComb( 0x0 );
			glDisable( GL_BLEND );
			glDisable( GL_SAMPLE_ALPHA_TO_COVERAGE );
			glEnable( GL_DEPTH_TEST );
			glDepthFunc( GL_LEQUAL );
			glDepthMask( GL_TRUE );
			return false;
		}

		if( !setMaterialRec( materialRes, shaderContext, 0x0 ) )
		{
			_curShader = 0x0;
			return false;
		}

		return true;
	}


	// =================================================================================================
	// Shadowing
	// =================================================================================================

	bool OpenGLRenderer::createShadowRB( uint32 width, uint32 height )
	{
		_shadowRB = _rd->createRenderBuffer( width, height, TextureFormats::BGRA8, true, 0, 0 );

		return _shadowRB != 0;
	}


	void OpenGLRenderer::releaseShadowRB()
	{
		if( _shadowRB ) _rd->destroyRenderBuffer( _shadowRB );
	}


	void OpenGLRenderer::setupShadowMap( bool noShadows )
	{
		uint32 sampState = SS_FILTER_BILINEAR | SS_ANISO1 | SS_ADDR_CLAMPCOL | SS_COMP_LEQUAL;

		// Bind shadow map
		if( !noShadows && _curLight->shadowMapCount() > 0 )
		{
			_rd->setTexture( 12, _rd->getRenderBufferTex( _shadowRB, 32 ), sampState );
			_smSize = (float)Modules::config().shadowMapSize;
		}
		else
		{
			_rd->setTexture( 12, _defShadowMap, sampState );
			_smSize = 4;
		}
	}


	Matrix4f OpenGLRenderer::calcCropMatrix( const Frustum &frustSlice, const Vec3f lightPos, const Matrix4f &lightViewProjMat )
	{
		float frustMinX =  Math::MaxFloat, bbMinX =  Math::MaxFloat;
		float frustMinY =  Math::MaxFloat, bbMinY =  Math::MaxFloat;
		float frustMinZ =  Math::MaxFloat, bbMinZ =  Math::MaxFloat;
		float frustMaxX = -Math::MaxFloat, bbMaxX = -Math::MaxFloat;
		float frustMaxY = -Math::MaxFloat, bbMaxY = -Math::MaxFloat;
		float frustMaxZ = -Math::MaxFloat, bbMaxZ = -Math::MaxFloat;

		// Find post-projective space AABB of all objects in frustum
		Modules::sceneMan().updateQueues( frustSlice, 0x0, RenderingOrder::None,
			SceneNodeFlags::NoDraw | SceneNodeFlags::NoCastShadow, false, true );
		RenderQueue &renderQueue = Modules::sceneMan().getRenderQueue();

		for( size_t i = 0, s = renderQueue.size(); i < s; ++i )
		{
			const BoundingBox &aabb = renderQueue[i].node->getBBox();

			// Check if light is inside AABB
			if( lightPos.x >= aabb.min.x && lightPos.y >= aabb.min.y && lightPos.z >= aabb.min.z &&
				lightPos.x <= aabb.max.x && lightPos.y <= aabb.max.y && lightPos.z <= aabb.max.z )
			{
				bbMinX = bbMinY = bbMinZ = -1;
				bbMaxX = bbMaxY = bbMaxZ = 1;
				break;
			}

			for( uint32 j = 0; j < 8; ++j )
			{
				Vec4f v1 = lightViewProjMat * Vec4f( aabb.getCorner( j ) );
				v1.w = 1.f / fabsf( v1.w );
				v1.x *= v1.w; v1.y *= v1.w; v1.z *= v1.w;

				if( v1.x < bbMinX ) bbMinX = v1.x;
				if( v1.y < bbMinY ) bbMinY = v1.y;
				if( v1.z < bbMinZ ) bbMinZ = v1.z;
				if( v1.x > bbMaxX ) bbMaxX = v1.x;
				if( v1.y > bbMaxY ) bbMaxY = v1.y;
				if( v1.z > bbMaxZ ) bbMaxZ = v1.z;
			}
		}

		// Find post-projective space AABB of frustum slice if light is not inside
		if( frustSlice.cullSphere( _curLight->absolutePos(), 0 ) )
		{
			// Get frustum in post-projective space
			for( uint32 i = 0; i < 8; ++i )
			{
				// Frustum slice
				Vec4f v1 = lightViewProjMat * Vec4f( frustSlice.getCorner( i ) );
				v1.w = 1.f / fabsf( v1.w );  // Use absolute value to reduce problems with back projection when v1.w < 0
				v1.x *= v1.w; v1.y *= v1.w; v1.z *= v1.w;

				if( v1.x < frustMinX ) frustMinX = v1.x;
				if( v1.y < frustMinY ) frustMinY = v1.y;
				if( v1.z < frustMinZ ) frustMinZ = v1.z;
				if( v1.x > frustMaxX ) frustMaxX = v1.x;
				if( v1.y > frustMaxY ) frustMaxY = v1.y;
				if( v1.z > frustMaxZ ) frustMaxZ = v1.z;
			}
		}
		else
		{
			frustMinX = frustMinY = frustMinZ = -1;
			frustMaxX = frustMaxY = frustMaxZ = 1;
		}

		// Merge frustum and AABB bounds and clamp to post-projective range [-1, 1]
		float minX = clamp( maxf( frustMinX, bbMinX ), -1, 1 );
		float minY = clamp( maxf( frustMinY, bbMinY ), -1, 1 );
		float minZ = clamp( minf( frustMinZ, bbMinZ ), -1, 1 );
		float maxX = clamp( minf( frustMaxX, bbMaxX ), -1, 1 );
		float maxY = clamp( minf( frustMaxY, bbMaxY ), -1, 1 );
		float maxZ = clamp( minf( frustMaxZ, bbMaxZ ), -1, 1 );

		// Zoom-in slice to make better use of available shadow map space
		float scaleX = 2.0f / (maxX - minX);
		float scaleY = 2.0f / (maxY - minY);
		float scaleZ = 2.0f / (maxZ - minZ);

		float offsetX = -0.5f * (maxX + minX) * scaleX;
		float offsetY = -0.5f * (maxY + minY) * scaleY;
		float offsetZ = -0.5f * (maxZ + minZ) * scaleZ;

		// Build final matrix
		float cropMat[16] = { scaleX, 0, 0, 0,
			0, scaleY, 0, 0,
			0, 0, scaleZ, 0,
			offsetX, offsetY, offsetZ, 1 };

		return Matrix4f( cropMat );
	}


	void OpenGLRenderer::updateShadowMap()
	{
		if( _curLight == 0x0 ) return;

		uint32 prevRendBuf = _rd->currentRenderBufferHandle();
		int prevVPX = _rd->viewportX(), prevVPY = _rd->viewportY(), prevVPWidth = _rd->viewportWidth(), prevVPHeight = _rd->viewportHeight();
		const RDIRenderBuffer &shadowRT = _rd->getRenderBuffer( _shadowRB );
		_rd->setViewport( 0, 0, shadowRT.width, shadowRT.height );
		_rd->setRenderBuffer( _shadowRB );

		glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
		glDepthMask( GL_TRUE );
		_rd->clear( CLR_DEPTH, 0x0, 1.f );

		// ********************************************************************************************
		// Cascaded Shadow Maps
		// ********************************************************************************************

		// Find AABB of lit geometry
		BoundingBox aabb;
		Modules::sceneMan().updateQueues( _curCamera->getFrustum(), &_curLight->getFrustum(),
			RenderingOrder::None, SceneNodeFlags::NoDraw | SceneNodeFlags::NoCastShadow, false, true );
		for( size_t j = 0, s = Modules::sceneMan().getRenderQueue().size(); j < s; ++j )
		{
			aabb.makeUnion( Modules::sceneMan().getRenderQueue()[j].node->getBBox() );
		}

		// Find depth range of lit geometry
		float minDist = Math::MaxFloat, maxDist = 0.0f;
		for( uint32 i = 0; i < 8; ++i )
		{
			float dist = -(_curCamera->getViewMat() * aabb.getCorner( i )).z;
			if( dist < minDist ) minDist = dist;
			if( dist > maxDist ) maxDist = dist;
		}

		// Don't adjust near plane; this means less precision if scene is far away from viewer but that
		// shouldn't be too noticeable and brings better performance since the nearer split volumes are empty
		minDist = _curCamera->frustNear();

		// Calculate split distances using PSSM scheme
		const float nearDist = maxf( minDist, _curCamera->frustNear() );
		const float farDist = maxf( maxDist, minDist + 0.01f );
		const uint32 numMaps = _curLight->shadowMapCount();
		const float lambda = _curLight->shadowSplitLambda();

		_splitPlanes[0] = nearDist;
		_splitPlanes[numMaps] = farDist;

		for( uint32 i = 1; i < numMaps; ++i )
		{
			float f = (float)i / numMaps;
			float logDist = nearDist * powf( farDist / nearDist, f );
			float uniformDist = nearDist + (farDist - nearDist) * f;

			_splitPlanes[i] = (1 - lambda) * uniformDist + lambda * logDist;  // Lerp
		}

		// Prepare shadow map rendering
		glEnable( GL_DEPTH_TEST );
		//glCullFace( GL_FRONT );	// Front face culling reduces artefacts but produces more "peter-panning"

		// Split viewing frustum into slices and render shadow maps
		Frustum frustum;
		for( uint32 i = 0; i < numMaps; ++i )
		{
			// Create frustum slice
			if( !_curCamera->ortographic() )
			{
				float newLeft = _curCamera->frustLeft() * _splitPlanes[i] / _curCamera->frustNear();
				float newRight = _curCamera->frustRight() * _splitPlanes[i] / _curCamera->frustNear();
				float newBottom = _curCamera->frustBottom() * _splitPlanes[i] / _curCamera->frustNear();
				float newTop = _curCamera->frustTop() * _splitPlanes[i] / _curCamera->frustNear();
				frustum.buildViewFrustum( _curCamera->getAbsTrans(), newLeft, newRight, newBottom, newTop,
					_splitPlanes[i], _splitPlanes[i + 1] );
			}
			else
			{
				frustum.buildBoxFrustum( _curCamera->getAbsTrans(), _curCamera->frustLeft(), _curCamera->frustRight(),
					_curCamera->frustBottom(), _curCamera->frustTop(),
					-_splitPlanes[i], -_splitPlanes[i + 1] );
			}

			// Get light projection matrix
			float ymax = _curCamera->frustNear() * tanf( degToRad( _curLight->fov() / 2 ) );
			float xmax = ymax * 1.0f;  // ymax * aspect
			Matrix4f lightProjMat = Matrix4f::PerspectiveMat(
				-xmax, xmax, -ymax, ymax, _curCamera->frustNear(), _curLight->radius() );

			// Build optimized light projection matrix
			Matrix4f lightViewProjMat = lightProjMat * _curLight->getViewMat();
			lightProjMat = calcCropMatrix( frustum, _curLight->absolutePos(), lightViewProjMat ) * lightProjMat;

			// Generate render queue with shadow casters for current slice
			frustum.buildViewFrustum( _curLight->getViewMat(), lightProjMat );
			Modules::sceneMan().updateQueues( frustum, 0x0, RenderingOrder::None,
				SceneNodeFlags::NoDraw | SceneNodeFlags::NoCastShadow, false, true );

			// Create texture atlas if several splits are enabled
			if( numMaps > 1 )
			{
				const int hsm = Modules::config().shadowMapSize / 2;
				const int scissorXY[8] = { 0, 0,  hsm, 0,  hsm, hsm,  0, hsm };
				const float transXY[8] = { -0.5f, -0.5f,  0.5f, -0.5f,  0.5f, 0.5f,  -0.5f, 0.5f };

				glEnable( GL_SCISSOR_TEST );

				// Select quadrant of shadow map
				lightProjMat.scale( 0.5f, 0.5f, 1.0f );
				lightProjMat.translate( transXY[i * 2], transXY[i * 2 + 1], 0.0f );
				_rd->setScissorRect( scissorXY[i * 2], scissorXY[i * 2 + 1], hsm, hsm );
			}

			_lightMats[i] = lightProjMat * _curLight->getViewMat();
			setupViewMatrices( _curLight->getViewMat(), lightProjMat );

			// Render
			drawRenderables( _curLight->shadowContext(), "", false, &frustum, 0x0, RenderingOrder::None, -1 );
		}

		// Map from post-projective space [-1,1] to texture space [0,1]
		for( uint32 i = 0; i < numMaps; ++i )
		{
			_lightMats[i].scale( 0.5f, 0.5f, 1.0f );
			_lightMats[i].translate( 0.5f, 0.5f, 0.0f );
		}

		// ********************************************************************************************

		glCullFace( GL_BACK );
		glDisable( GL_SCISSOR_TEST );

		_rd->setViewport( prevVPX, prevVPY, prevVPWidth, prevVPHeight );
		_rd->setRenderBuffer( prevRendBuf );
		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	}


	// =================================================================================================
	// Occlusion Culling
	// =================================================================================================

	int OpenGLRenderer::registerOccSet()
	{
		for( int i = 0; i < (int)_occSets.size(); ++i )
		{
			if( _occSets[i] == 0 )
			{
				_occSets[i] = 1;
				return i;
			}
		}

		_occSets.push_back( 1 );
		return (int)_occSets.size() - 1;
	}


	void OpenGLRenderer::unregisterOccSet( int occSet )
	{
		if( occSet >= 0 && occSet < (int)_occSets.size() )
			_occSets[occSet] = 0;
	}


	void OpenGLRenderer::drawOccProxies( uint32 list )
	{
		ASSERT( list < 2 );

		GLboolean colMask[4], depthMask;
		glGetBooleanv( GL_COLOR_WRITEMASK, colMask );
		glGetBooleanv( GL_DEPTH_WRITEMASK, &depthMask );

		setMaterial( 0x0, "" );
		glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
		glDepthMask( GL_FALSE );

		setShaderComb( &_defColorShader );
		commitGeneralUniforms();
		_rd->setVertexBuffer( 0, _vbCube, 0, 12 );
		_rd->setIndexBuffer( _ibCube, IDXFMT_16 );
		_rd->setVertexLayout( _vlPosOnly );

		// Draw occlusion proxies
		for( size_t i = 0, s = _occProxies[list].size(); i < s; ++i )
		{
			OccProxy &proxy = _occProxies[list][i];

			_rd->beginQuery( proxy.queryObj );

			Matrix4f mat = Matrix4f::TransMat( proxy.bbMin.x, proxy.bbMin.y, proxy.bbMin.z ) *
				Matrix4f::ScaleMat( proxy.bbMax.x - proxy.bbMin.x, proxy.bbMax.y - proxy.bbMin.y, proxy.bbMax.z - proxy.bbMin.z );
			_rd->setShaderConst( _curShader->uni_worldMat, CONST_FLOAT44, &mat.x[0] );

			// Draw AABB
			_rd->drawIndexed( PRIM_TRILIST, 0, 36, 0, 8 );

			_rd->endQuery( proxy.queryObj );
		}

		setShaderComb( 0x0 );
		glDepthMask( depthMask );
		glColorMask( colMask[0], colMask[1], colMask[2], colMask[3] );

		_occProxies[list].resize( 0 );
	}


	// =================================================================================================
	// Overlays
	// =================================================================================================

	void OpenGLRenderer::showOverlays( const float *verts, uint32 vertCount, float *colRGBA,
		MaterialResource *matRes, int flags )
	{
		uint32 numOverlayVerts = 0;
		if( !_overlayBatches.empty() )
			numOverlayVerts = _overlayBatches.back().firstVert + _overlayBatches.back().vertCount;

		if( numOverlayVerts + vertCount > MaxNumOverlayVerts ) return;

		memcpy( &_overlayVerts[numOverlayVerts], verts, vertCount * sizeof( OverlayVert ) );

		// Check if previous batch can be extended
		if( !_overlayBatches.empty() )
		{
			OverlayBatch &prevBatch = _overlayBatches.back();
			if( matRes == prevBatch.materialRes && flags == prevBatch.flags &&
				memcmp( colRGBA, prevBatch.colRGBA, 4 * sizeof( float ) ) == 0 )
			{
				prevBatch.vertCount += vertCount;
				return;
			}
		}

		// Create new batch
		_overlayBatches.push_back( OverlayBatch( numOverlayVerts, vertCount, colRGBA, matRes, flags ) );
	}


	void OpenGLRenderer::clearOverlays()
	{
		_overlayBatches.resize( 0 );
	}


	void OpenGLRenderer::drawOverlays( const string &shaderContext )
	{
		uint32 numOverlayVerts = 0;
		if( !_overlayBatches.empty() )
			numOverlayVerts = _overlayBatches.back().firstVert + _overlayBatches.back().vertCount;

		if( numOverlayVerts == 0 ) return;

		// Upload overlay vertices
		_rd->updateBufferData( _overlayVB, 0, MaxNumOverlayVerts * sizeof( OverlayVert ), _overlayVerts );

		_rd->setVertexBuffer( 0, _overlayVB, 0, sizeof( OverlayVert ) );
		_rd->setIndexBuffer( _quadIdxBuf, IDXFMT_16 );
		ASSERT( QuadIndexBufCount >= MaxNumOverlayVerts * 6 );

		float aspect = (float)_curCamera->viewportWidth() / (float)_curCamera->viewportHeight();
		setupViewMatrices( Matrix4f(), Matrix4f::OrthoMat( 0, aspect, 1, 0, -1, 1 ) );

		MaterialResource *curMatRes = 0x0;

		for( size_t i = 0, s = _overlayBatches.size(); i < s; ++i )
		{
			OverlayBatch &ob = _overlayBatches[i];

			if( curMatRes != ob.materialRes )
			{
				if( !setMaterial( ob.materialRes, shaderContext ) ) continue;
				_rd->setVertexLayout( _vlOverlay );
				curMatRes = ob.materialRes;
			}

			if( _curShader->uni_olayColor >= 0 )
				_rd->setShaderConst( _curShader->uni_olayColor, CONST_FLOAT4, ob.colRGBA );

			// Draw batch
			_rd->drawIndexed( PRIM_TRILIST, ob.firstVert * 6/4, ob.vertCount * 6/4, ob.firstVert, ob.vertCount );
		}
	}


	// =================================================================================================
	// Pipeline Functions
	// =================================================================================================

	void OpenGLRenderer::bindPipeBuffer( uint32 rbObj, const string &sampler, uint32 bufIndex )
	{
		if( rbObj == 0 )
		{
			// Clear buffer bindings
			_pipeSamplerBindings.resize( 0 );
		}
		else
		{
			// Check if binding is already existing
			for( size_t i = 0, s = _pipeSamplerBindings.size(); i < s; ++i )
			{
				if( strcmp( _pipeSamplerBindings[i].sampler, sampler.c_str() ) == 0 )
				{
					_pipeSamplerBindings[i].rbObj = rbObj;
					_pipeSamplerBindings[i].bufIndex = bufIndex;
					return;
				}
			}

			// Add binding
			_pipeSamplerBindings.push_back( PipeSamplerBinding() );
			size_t len = std::min( sampler.length(), (size_t)63 );
			strncpy_s( _pipeSamplerBindings.back().sampler, 63, sampler.c_str(), len );
			_pipeSamplerBindings.back().sampler[len] = '\0';
			_pipeSamplerBindings.back().rbObj = rbObj;
			_pipeSamplerBindings.back().bufIndex = bufIndex;
		}
	}


	void OpenGLRenderer::clear( bool depth, bool buf0, bool buf1, bool buf2, bool buf3,
		float r, float g, float b, float a )
	{
		uint32 mask = 0;
		uint32 prevBuffers[4] = { 0 };
		float clrColor[] = { r, g, b, a };

		glDisable( GL_BLEND );	// Clearing floating point buffers causes problems when blending is enabled on Radeon 9600
		glDepthMask( GL_TRUE );

		if( _rd->currentRenderBufferHandle() != 0x0 )
		{
			// Store state of glDrawBuffers
			for( uint32 i = 0; i < 4; ++i ) glGetIntegerv( GL_DRAW_BUFFER0 + i, (int *)&prevBuffers[i] );

			const RDIRenderBuffer &rb = _rd->getRenderBuffer(_rd->currentRenderBufferHandle());
			uint32 buffers[4], cnt = 0;

			if( depth && rb.depthTex != 0 ) mask |= CLR_DEPTH;

			if( buf0 && rb.colTexs[0] != 0 ) buffers[cnt++] = GL_COLOR_ATTACHMENT0_EXT;
			if( buf1 && rb.colTexs[1] != 0 ) buffers[cnt++] = GL_COLOR_ATTACHMENT1_EXT;
			if( buf2 && rb.colTexs[2] != 0 ) buffers[cnt++] = GL_COLOR_ATTACHMENT2_EXT;
			if( buf3 && rb.colTexs[3] != 0 ) buffers[cnt++] = GL_COLOR_ATTACHMENT3_EXT;

			if( cnt > 0 )
			{	
				mask |= CLR_COLOR;
				glDrawBuffers( cnt, buffers );
			}
		}
		else
		{
			if( depth ) mask |= CLR_DEPTH;
			if( buf0 ) mask |= CLR_COLOR;
			_rd->setScissorRect( _curCamera->viewportX(), _curCamera->viewportY(), _curCamera->viewportWidth(), _curCamera->viewportHeight() );
			glEnable( GL_SCISSOR_TEST );
		}

		_rd->clear( mask, clrColor, 1.f );
		glDisable( GL_SCISSOR_TEST );

		// Restore state of glDrawBuffers
		if( _rd->currentRenderBufferHandle() != 0x0 )
			glDrawBuffers( 4, prevBuffers );
	}


	void OpenGLRenderer::drawFSQuad( Resource *matRes, const string &shaderContext )
	{
		if( matRes == 0x0 || matRes->getType() != ResourceTypes::Material ) return;

		setupViewMatrices( _curCamera->getViewMat(), Matrix4f::OrthoMat( 0, 1, 0, 1, -1, 1 ) );

		if( !setMaterial( (MaterialResource *)matRes, shaderContext ) ) return;

		_rd->setVertexBuffer( 0, _vbFSPoly, 0, 12 );
		_rd->setIndexBuffer( 0, IDXFMT_16 );
		_rd->setVertexLayout( _vlPosOnly );

		_rd->draw( PRIM_TRILIST, 0, 3 );
	}


	void OpenGLRenderer::drawGeometry( const string &shaderContext, const string &theClass,
		RenderingOrder::List order, int occSet )
	{
		Modules::sceneMan().updateQueues( _curCamera->getFrustum(), 0x0, order,
			SceneNodeFlags::NoDraw , false, true );

		setupViewMatrices( _curCamera->getViewMat(), _curCamera->getProjMat() );
		drawRenderables( shaderContext, theClass, false, &_curCamera->getFrustum(), 0x0, order, occSet );
	}


	void OpenGLRenderer::drawLightGeometry( const string &shaderContext, const string &theClass,
		bool noShadows, RenderingOrder::List order, int occSet )
	{
		Modules::sceneMan().updateQueues( _curCamera->getFrustum(), 0x0, RenderingOrder::None,
			SceneNodeFlags::NoDraw, true, false );

		IGPUTimer *timer = Modules::stats().getGPUTimer( EngineStats::FwdLightsGPUTime );
		if( Modules::config().gatherTimeStats ) timer->beginQuery( _frameID );

		for( size_t i = 0, s = Modules::sceneMan().getLightQueue().size(); i < s; ++i )
		{
			_curLight = (LightNode *)Modules::sceneMan().getLightQueue()[i];

			// Check if light is not visible
			if( _curCamera->getFrustum().cullFrustum( _curLight->getFrustum() ) ) continue;

			// Check if light is occluded
			if( occSet >= 0 )
			{
				std::vector< uint32 >& occQueries = _curLight->occQueries();
				std::vector< uint32 >& lastVisited = _curLight->lastVisited();

				if( occSet > (int)occQueries.size() - 1 )
				{
					occQueries.resize( occSet + 1, 0 );
					lastVisited.resize( occSet + 1, 0 );
				}
				if( occQueries[occSet] == 0 )
				{
					occQueries[occSet] = _rd->createOcclusionQuery();
					lastVisited[occSet] = 0;
				}
				else
				{
					if( lastVisited[occSet] != _frameID )
					{
						lastVisited[occSet] = _frameID;

						Vec3f bbMin, bbMax;
						_curLight->getFrustum().calcAABB( bbMin, bbMax );

						// Check that viewer is outside light bounds
						if( nearestDistToAABB( _curCamera->getFrustum().getOrigin(), bbMin, bbMax ) > 0 )
						{
							Modules::renderer().pushOccProxy( 1, bbMin, bbMax, occQueries[occSet] );

							// Check query result from previous frame
							if( _rd->getQueryResult( occQueries[occSet] ) < 1 )
							{
								continue;
							}
						}
					}
				}
			}

			// Update shadow map
			if( !noShadows && _curLight->shadowMapCount() > 0 )
			{
				timer->endQuery();
				IGPUTimer *timerShadows = Modules::stats().getGPUTimer( EngineStats::ShadowsGPUTime );
				if( Modules::config().gatherTimeStats ) timerShadows->beginQuery( _frameID );

				updateShadowMap();
				setupShadowMap( false );

				timerShadows->endQuery();
				if( Modules::config().gatherTimeStats ) timer->beginQuery( _frameID );
			}
			else
			{
				setupShadowMap( true );
			}

			// Calculate light screen space position
			float bbx, bby, bbw, bbh;
			_curLight->calcScreenSpaceAABB( _curCamera->getProjMat() * _curCamera->getViewMat(),
				bbx, bby, bbw, bbh );

			// Set scissor rectangle
			if( bbx != 0 || bby != 0 || bbw != 1 || bbh != 1 )
			{
				_rd->setScissorRect( ftoi_r( bbx * _rd->frameBufferWidth() ), ftoi_r( bby * _rd->frameBufferHeight() ),
					ftoi_r( bbw * _rd->frameBufferWidth() ), ftoi_r( bbh * _rd->frameBufferHeight() ) );
				glEnable( GL_SCISSOR_TEST );
			}

			// Render
			Modules::sceneMan().updateQueues( _curCamera->getFrustum(), &_curLight->getFrustum(),
				order, SceneNodeFlags::NoDraw, false, true );
			setupViewMatrices( _curCamera->getViewMat(), _curCamera->getProjMat() );
			drawRenderables( shaderContext.empty() ? _curLight->lightingContext() : shaderContext,
				theClass, false, &_curCamera->getFrustum(),
				&_curLight->getFrustum(), order, occSet );
			Modules().stats().incStat( EngineStats::LightPassCount, 1 );

			// Reset
			glDisable( GL_SCISSOR_TEST );
		}

		_curLight = 0x0;

		timer->endQuery();

		// Draw occlusion proxies
		if( occSet >= 0 )
		{
			setupViewMatrices( _curCamera->getViewMat(), _curCamera->getProjMat() );
			Modules::renderer().drawOccProxies( 1 );
		}
	}


	void OpenGLRenderer::drawLightShapes( const string &shaderContext, bool noShadows, int occSet )
	{
		MaterialResource *curMatRes = 0x0;

		Modules::sceneMan().updateQueues( _curCamera->getFrustum(), 0x0, RenderingOrder::None,
			SceneNodeFlags::NoDraw, true, false );

		IGPUTimer *timer = Modules::stats().getGPUTimer( EngineStats::DefLightsGPUTime );
		if( Modules::config().gatherTimeStats ) timer->beginQuery( _frameID );

		for( size_t i = 0, s = Modules::sceneMan().getLightQueue().size(); i < s; ++i )
		{
			_curLight = (LightNode *)Modules::sceneMan().getLightQueue()[i];

			// Check if light is not visible
			if( _curCamera->getFrustum().cullFrustum( _curLight->getFrustum() ) ) continue;

			// Check if light is occluded
			if( occSet >= 0 )
			{
				std::vector< uint32 >& occQueries = _curLight->occQueries();
				std::vector< uint32 >& lastVisited = _curLight->lastVisited();

				if( occSet > (int)occQueries.size() - 1 )
				{
					occQueries.resize( occSet + 1, 0 );
					lastVisited.resize( occSet + 1, 0 );
				}
				if( occQueries[occSet] == 0 )
				{
					occQueries[occSet] = _rd->createOcclusionQuery();
					lastVisited[occSet] = 0;
				}
				else
				{
					if( lastVisited[occSet] != _frameID )
					{
						lastVisited[occSet] = _frameID;

						Vec3f bbMin, bbMax;
						_curLight->getFrustum().calcAABB( bbMin, bbMax );

						// Check that viewer is outside light bounds
						if( nearestDistToAABB( _curCamera->getFrustum().getOrigin(), bbMin, bbMax ) > 0 )
						{
							Modules::renderer().pushOccProxy( 1, bbMin, bbMax, occQueries[occSet] );

							// Check query result from previous frame
							if( _rd->getQueryResult( occQueries[occSet] ) < 1 )
							{
								continue;
							}
						}
					}
				}
			}

			// Update shadow map
			if( !noShadows && _curLight->shadowMapCount() > 0 )
			{	
				timer->endQuery();
				IGPUTimer *timerShadows = Modules::stats().getGPUTimer( EngineStats::ShadowsGPUTime );
				if( Modules::config().gatherTimeStats ) timerShadows->beginQuery( _frameID );

				updateShadowMap();
				setupShadowMap( false );
				curMatRes = 0x0;

				timerShadows->endQuery();
				if( Modules::config().gatherTimeStats ) timer->beginQuery( _frameID );
			}
			else
			{
				setupShadowMap( true );
			}

			setupViewMatrices( _curCamera->getViewMat(), _curCamera->getProjMat() );

			if( curMatRes != _curLight->materialRes() )
			{
				if( !setMaterial( _curLight->materialRes(),
					shaderContext.empty() ? _curLight->lightingContext() : shaderContext ) )
				{
					continue;
				}
				curMatRes = _curLight->materialRes();
			}
			else
			{
				commitGeneralUniforms();
			}

			glCullFace( GL_FRONT );
			glDisable( GL_DEPTH_TEST );

			if( _curLight->fov() < 180 )
			{
				float r = _curLight->radius() * tanf( degToRad( _curLight->fov() / 2 ) );
				drawCone( _curLight->radius(), r, _curLight->getAbsTrans() );
			}
			else
			{
				drawSphere( _curLight->absolutePos(), _curLight->radius() );
			}

			Modules().stats().incStat( EngineStats::LightPassCount, 1 );

			// Reset
			glEnable( GL_DEPTH_TEST );
			glCullFace( GL_BACK );
		}

		_curLight = 0x0;

		timer->endQuery();

		// Draw occlusion proxies
		if( occSet >= 0 )
		{
			setupViewMatrices( _curCamera->getViewMat(), _curCamera->getProjMat() );
			Modules::renderer().drawOccProxies( 1 );
		}
	}


	// =================================================================================================
	// Scene Node Rendering Functions
	// =================================================================================================

	void OpenGLRenderer::drawRenderables( const string &shaderContext, const string &theClass, bool debugView,
		const Frustum *frust1, const Frustum *frust2, RenderingOrder::List order,
		int occSet )
	{
		ASSERT( _curCamera != 0x0 );

		const RenderQueue &renderQueue = Modules::sceneMan().getRenderQueue();
		uint32 queueSize = (uint32)renderQueue.size();
		if( queueSize == 0 ) return;

		// Set global render states
		if( Modules::config().wireframeMode && !Modules::config().debugViewMode )
		{
			glDisable( GL_CULL_FACE );
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		else
		{
			glEnable( GL_CULL_FACE );
		}

		// Process all render queue items
		uint32 firstItem = 0, lastItem = 0;
		do
		{
			lastItem = firstItem;
			while( (lastItem + 1 < queueSize) && (renderQueue[firstItem].type == renderQueue[lastItem + 1].type) )
			{
				++lastItem;
			}

			for( uint32 i = 0, si = (uint32)_renderFuncRegistry.size(); i < si; ++i )
			{
				if( _renderFuncRegistry[i].nodeType == renderQueue[firstItem].type )
				{
					_renderFuncRegistry[i].renderFunc(
						firstItem, lastItem, shaderContext, theClass, debugView, frust1, frust2, order, occSet );
					break;
				}
			}

			firstItem = lastItem + 1;
		} while( firstItem < queueSize );

		// Reset states
		if( Modules::config().wireframeMode && !Modules::config().debugViewMode )
		{
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
	}


	void OpenGLRenderer::drawMeshes( uint32 firstItem, uint32 lastItem, const string &shaderContext, const string &theClass,
		bool debugView, const Frustum *frust1, const Frustum *frust2, RenderingOrder::List order,
		int occSet )
	{
		if( frust1 == 0x0 ) return;

		// we know this is the renderer otherwise the method is not called for this class
		OpenGLRenderer& renderer = static_cast< OpenGLRenderer& >( Modules::renderer() );

		const RenderQueue &renderQueue = Modules::sceneMan().getRenderQueue();
		GeometryResource *curGeoRes = 0x0;
		MaterialResource *curMatRes = 0x0;

		// Loop over mesh queue
		for( size_t i = firstItem; i <= lastItem; ++i )
		{
			MeshNode *meshNode = (MeshNode *)renderQueue[i].node;
			ModelNode *modelNode = meshNode->getParentModel();

			// Check that mesh is valid
			if( modelNode->getGeometryResource() == 0x0 )
				continue;
			if( meshNode->getBatchStart() + meshNode->getBatchCount() > modelNode->getGeometryResource()->getIndexCount() )
				continue;

			bool modelChanged = true;
			uint32 queryObj = 0;

			// Occlusion culling
			if( occSet >= 0 )
			{
				std::vector< uint32 >& occQueries = meshNode->occQueries();
				std::vector< uint32 >& lastVisited = meshNode->lastVisited();
				if( occSet > (int)occQueries.size() - 1 )
				{
					occQueries.resize( occSet + 1, 0 );
					lastVisited.resize( occSet + 1, 0 );
				}
				if( occQueries[occSet] == 0 )
				{
					queryObj = gRDI->createOcclusionQuery();
					occQueries[occSet] = queryObj;
					lastVisited[occSet] = 0;
				}
				else
				{
					if( lastVisited[occSet] != renderer._frameID )
					{
						lastVisited[occSet] = renderer._frameID;

						// Check query result (viewer must be outside of bounding box)
						if( nearestDistToAABB( frust1->getOrigin(), meshNode->getBBox().min,
							meshNode->getBBox().max ) > 0 &&
							gRDI->getQueryResult( occQueries[occSet] ) < 1 )
						{
							Modules::renderer().pushOccProxy( 0, meshNode->getBBox().min, meshNode->getBBox().max,
								occQueries[occSet] );
							continue;
						}
						else
							queryObj = occQueries[occSet];
					}
				}
			}

			// Bind geometry
			if( curGeoRes != modelNode->getGeometryResource() )
			{
				curGeoRes = modelNode->getGeometryResource();
				ASSERT( curGeoRes != 0x0 );

				// Indices
				gRDI->setIndexBuffer( curGeoRes->getIndexBuf(),
					curGeoRes->using16BitIndices() ? IDXFMT_16 : IDXFMT_32 );

				// Vertices
				uint32 posVBuf = curGeoRes->getPosVBuf();
				uint32 tanVBuf = curGeoRes->getTanVBuf();
				uint32 staticVBuf = curGeoRes->getStaticVBuf();

				gRDI->setVertexBuffer( 0, posVBuf, 0, sizeof( Vec3f ) );
				gRDI->setVertexBuffer( 1, tanVBuf, 0, sizeof( VertexDataTan ) );
				gRDI->setVertexBuffer( 2, tanVBuf, sizeof( Vec3f ), sizeof( VertexDataTan ) );
				gRDI->setVertexBuffer( 3, staticVBuf, 0, sizeof( VertexDataStatic ) );
			}

			gRDI->setVertexLayout( renderer._vlModel );

			ShaderCombination *prevShader = renderer._curShader;

			if( !debugView )
			{
				if( !meshNode->getMaterialRes()->isOfClass( theClass ) ) continue;

				// Set material
				if( curMatRes != meshNode->getMaterialRes() )
				{
					if( !renderer.setMaterial( meshNode->getMaterialRes(), shaderContext ) )
					{	
						curMatRes = 0x0;
						continue;
					}
					curMatRes = meshNode->getMaterialRes();
				}
			}
			else
			{
				renderer.setShaderComb( &renderer._defColorShader );
				renderer.commitGeneralUniforms();

				uint32 curLod = meshNode->getLodLevel();
				Vec4f color;
				if( curLod == 0 ) color = Vec4f( 0.5f, 0.75f, 1, 1 );
				else if( curLod == 1 ) color = Vec4f( 0.25f, 0.75, 0.75f, 1 );
				else if( curLod == 2 ) color = Vec4f( 0.25f, 0.75, 0.5f, 1 );
				else if( curLod == 3 ) color = Vec4f( 0.5f, 0.5f, 0.25f, 1 );
				else color = Vec4f( 0.75f, 0.5, 0.25f, 1 );

				// Darken models with skeleton so that bones are more noticable
				if( !modelNode->jointList().empty() ) color = color * 0.3f;

				gRDI->setShaderConst( renderer._defColShader_color, CONST_FLOAT4, &color.x );
			}

			ShaderCombination *curShader = renderer._curShader;

			if( modelChanged || curShader != prevShader )
			{
				// Skeleton
				if( curShader->uni_skinMatRows >= 0 && !modelNode->skinMatRows().empty() )
				{
					// Note:	OpenGL 2.1 supports mat4x3 but it is internally realized as mat4 on most
					//			hardware so it would require 4 instead of 3 uniform slots per joint
					
					gRDI->setShaderConst( curShader->uni_skinMatRows, CONST_FLOAT4,
						&modelNode->skinMatRows()[0], (int)modelNode->skinMatRows().size() );
				}

				modelChanged = false;
			}

			// World transformation
			if( curShader->uni_worldMat >= 0 )
			{
				gRDI->setShaderConst( curShader->uni_worldMat, CONST_FLOAT44, &meshNode->getAbsTrans().x[0] );
			}
			if( curShader->uni_worldNormalMat >= 0 )
			{
				// TODO: Optimize this
				Matrix4f normalMat4 = meshNode->getAbsTrans().inverted().transposed();
				float normalMat[9] = { normalMat4.x[0], normalMat4.x[1], normalMat4.x[2],
					normalMat4.x[4], normalMat4.x[5], normalMat4.x[6],
					normalMat4.x[8], normalMat4.x[9], normalMat4.x[10] };
				gRDI->setShaderConst( curShader->uni_worldNormalMat, CONST_FLOAT33, normalMat );
			}
			if( curShader->uni_nodeId >= 0 )
			{
				float id = (float)meshNode->getHandle();
				gRDI->setShaderConst( curShader->uni_nodeId, CONST_FLOAT, &id );
			}
			if( curShader->uni_customInstData >= 0 )
			{
				gRDI->setShaderConst( curShader->uni_customInstData, CONST_FLOAT4,
					(void *)(modelNode->customInstData()), ModelCustomVecCount );
			}

			if( queryObj )
				gRDI->beginQuery( queryObj );

			// Render
			gRDI->drawIndexed( PRIM_TRILIST, meshNode->getBatchStart(), meshNode->getBatchCount(),
				meshNode->getVertRStart(), meshNode->getVertREnd() - meshNode->getVertRStart() + 1 );
			Modules::stats().incStat( EngineStats::BatchCount, 1 );
			Modules::stats().incStat( EngineStats::TriCount, meshNode->getBatchCount() / 3.0f );

			if( queryObj )
				gRDI->endQuery( queryObj );
		}

		// Draw occlusion proxies
		if( occSet >= 0 )
			renderer.drawOccProxies( 0 );

		gRDI->setVertexLayout( 0 );
	}


	void OpenGLRenderer::drawParticles( uint32 firstItem, uint32 lastItem, const string &shaderContext, const string &theClass,
		bool debugView, const Frustum *frust1, const Frustum * /*frust2*/, RenderingOrder::List /*order*/,
		int occSet )
	{
		// we know this is the renderer otherwise the method is not called for this class
		OpenGLRenderer& renderer = static_cast< OpenGLRenderer& >( Modules::renderer() );

		if( frust1 == 0x0 || renderer._curCamera == 0x0 ) return;
		if( debugView ) return;  // Don't render particles in debug view

		const RenderQueue &renderQueue = Modules::sceneMan().getRenderQueue();
		MaterialResource *curMatRes = 0x0;

		IGPUTimer *timer = Modules::stats().getGPUTimer( EngineStats::ParticleGPUTime );
		if( Modules::config().gatherTimeStats ) timer->beginQuery( renderer._frameID );

		// Bind particle geometry
		gRDI->setVertexBuffer( 0, renderer._particleVBO, 0, sizeof( ParticleVert ) );
		gRDI->setIndexBuffer( renderer._quadIdxBuf, IDXFMT_16 );
		ASSERT( QuadIndexBufCount >= ParticlesPerBatch * 6 );

		// Loop through emitter queue
		for( uint32 i = firstItem; i <= lastItem; ++i )
		{
			EmitterNode *emitter = (EmitterNode *)renderQueue[i].node;

			if( emitter->particleCount() == 0 ) continue;
			if( !emitter->materialRes()->isOfClass( theClass ) ) continue;

			// Occlusion culling
			uint32 queryObj = 0;
			if( occSet >= 0 )
			{
				std::vector< uint32 > &occQueries = emitter->occQueries();
				std::vector< uint32 > &lastVisited = emitter->lastVisited();
				if( occSet > (int)occQueries.size() - 1 )
				{
					occQueries.resize( occSet + 1, 0 );
					lastVisited.resize( occSet + 1, 0 );
				}
				if( occQueries[occSet] == 0 )
				{
					queryObj = gRDI->createOcclusionQuery();
					occQueries[occSet] = queryObj;
					lastVisited[occSet] = 0;
				}
				else
				{
					if( lastVisited[occSet] != renderer._frameID )
					{
						lastVisited[occSet] = renderer._frameID;

						// Check query result (viewer must be outside of bounding box)
						if( nearestDistToAABB( frust1->getOrigin(), emitter->getBBox().min,
							emitter->getBBox().max ) > 0 &&
							gRDI->getQueryResult( occQueries[occSet] ) < 1 )
						{
							Modules::renderer().pushOccProxy( 0, emitter->getBBox().min,
								emitter->getBBox().max, occQueries[occSet] );
							continue;
						}
						else
							queryObj = occQueries[occSet];
					}
				}
			}

			// Set material
			if( curMatRes != emitter->materialRes() )
			{
				if( !renderer.setMaterial( emitter->materialRes(), shaderContext ) ) continue;
				curMatRes = emitter->materialRes();
			}

			// Set vertex layout
			gRDI->setVertexLayout( renderer._vlParticle );

			if( queryObj )
				gRDI->beginQuery( queryObj );

			// Shader uniforms
			ShaderCombination *curShader = Modules::renderer().getCurShader();
			if( curShader->uni_nodeId >= 0 )
			{
				float id = (float)emitter->getHandle();
				gRDI->setShaderConst( curShader->uni_nodeId, CONST_FLOAT, &id );
			}

			// Divide particles in batches and render them
			for( uint32 j = 0; j < emitter->particleCount() / ParticlesPerBatch; ++j )
			{
				// Check if batch needs to be rendered
				bool allDead = true;
				for( uint32 k = 0; k < ParticlesPerBatch; ++k )
				{
					if( emitter->particles()[j*ParticlesPerBatch + k].life > 0 )
					{
						allDead = false;
						break;
					}
				}
				if( allDead ) continue;

				// Render batch
				if( curShader->uni_parPosArray >= 0 )
					gRDI->setShaderConst( curShader->uni_parPosArray, CONST_FLOAT3,
					(float *)emitter->parPositions() + j*ParticlesPerBatch*3, ParticlesPerBatch );
				if( curShader->uni_parSizeAndRotArray >= 0 )
					gRDI->setShaderConst( curShader->uni_parSizeAndRotArray, CONST_FLOAT2,
					(float *)emitter->parSizesANDRotations() + j*ParticlesPerBatch*2, ParticlesPerBatch );
				if( curShader->uni_parColorArray >= 0 )
					gRDI->setShaderConst( curShader->uni_parColorArray, CONST_FLOAT4,
					(float *)emitter->parColors() + j*ParticlesPerBatch*4, ParticlesPerBatch );

				gRDI->drawIndexed( PRIM_TRILIST, 0, ParticlesPerBatch * 6, 0, ParticlesPerBatch * 4 );
				Modules::stats().incStat( EngineStats::BatchCount, 1 );
				Modules::stats().incStat( EngineStats::TriCount, ParticlesPerBatch * 2.0f );
			}

			uint32 count = emitter->particleCount() % ParticlesPerBatch;
			if( count > 0 )
			{
				uint32 offset = (emitter->particleCount() / ParticlesPerBatch) * ParticlesPerBatch;

				// Check if batch needs to be rendered
				bool allDead = true;
				for( uint32 k = 0; k < count; ++k )
				{
					if( emitter->particles()[offset + k].life > 0 )
					{
						allDead = false;
						break;
					}
				}

				if( !allDead )
				{
					// Render batch
					if( curShader->uni_parPosArray >= 0 )
						gRDI->setShaderConst( curShader->uni_parPosArray, CONST_FLOAT3,
						(float *)emitter->parPositions() + offset*3, count );
					if( curShader->uni_parSizeAndRotArray >= 0 )
						gRDI->setShaderConst( curShader->uni_parSizeAndRotArray, CONST_FLOAT2,
						(float *)emitter->parSizesANDRotations() + offset*2, count );
					if( curShader->uni_parColorArray >= 0 )
						gRDI->setShaderConst( curShader->uni_parColorArray, CONST_FLOAT4,
						(float *)emitter->parColors() + offset*4, count );

					gRDI->drawIndexed( PRIM_TRILIST, 0, count * 6, 0, count * 4 );
					Modules::stats().incStat( EngineStats::BatchCount, 1 );
					Modules::stats().incStat( EngineStats::TriCount, count * 2.0f );
				}
			}

			if( queryObj )
				gRDI->endQuery( queryObj );
		}

		timer->endQuery();

		// Draw occlusion proxies
		if( occSet >= 0 )
			Modules::renderer().drawOccProxies( 0 );

		gRDI->setVertexLayout( 0 );
	}


	// =================================================================================================
	// Main Rendering Functions
	// =================================================================================================

	void OpenGLRenderer::render( CameraNode *camNode )
	{
		_curCamera = camNode;
		if( _curCamera == 0x0 ) return;

		// Build sampler anisotropy mask from anisotropy value
		int maxAniso = Modules::config().maxAnisotropy;
		if( maxAniso <= 1 ) _maxAnisoMask = SS_ANISO1;
		else if( maxAniso <= 2 ) _maxAnisoMask = SS_ANISO2;
		else if( maxAniso <= 4 ) _maxAnisoMask = SS_ANISO4;
		else if( maxAniso <= 8 ) _maxAnisoMask = SS_ANISO8;
		else _maxAnisoMask = SS_ANISO16;
		_rd->beginRendering();
		_rd->setViewport( _curCamera->viewportX(), _curCamera->viewportY(), _curCamera->viewportWidth(), _curCamera->viewportHeight() );
		if( Modules::config().debugViewMode || _curCamera->pipelineRes() == 0x0 )
		{
			renderDebugView();
			finishRendering();
			return;
		}

		// Initialize
		_rd->setOutputBufferIndex(_curCamera->outputBufferIndex());
		if( _curCamera->outputTexture() != 0x0 )
			_rd->setRenderBuffer( _curCamera->outputTexture()->getRBObject() );
		else 
			_rd->setRenderBuffer( 0 );

		// Process pipeline commands
		std::vector< PipelineStage >& pipelineStages = _curCamera->pipelineRes()->stages();
		for( uint32 i = 0; i < pipelineStages.size(); ++i )
		{
			PipelineStage &stage = pipelineStages[i];
			if( !stage.enabled ) continue;
			_curStageMatLink = stage.matLink;

			for( uint32 j = 0; j < stage.commands.size(); ++j )
			{
				PipelineCommand &pc = stage.commands[j];
				RenderTarget *rt;

				switch( pc.command )
				{
				case PipelineCommands::SwitchTarget:
					// Unbind all textures
					bindPipeBuffer( 0x0, "", 0 );

					// Bind new render target
					rt = (RenderTarget *)pc.params[0].getPtr();
					_curRenderTarget = rt;

					if( rt != 0x0 )
					{
						const RDIRenderBuffer &rendBuf = _rd->getRenderBuffer( rt->rendBuf );
						_rd->setOutputBufferIndex(_curCamera->outputBufferIndex());
						_rd->setViewport( 0, 0, rendBuf.width, rendBuf.height );
						_rd->setRenderBuffer( rt->rendBuf );
					}
					else
					{
						_rd->setViewport( _curCamera->viewportX(), _curCamera->viewportY(), _curCamera->viewportWidth(), _curCamera->viewportHeight() );
						_rd->setRenderBuffer( _curCamera->outputTexture() != 0x0 ?
							_curCamera->outputTexture()->getRBObject() : 0 );
					}
					break;

				case PipelineCommands::BindBuffer:
					rt = (RenderTarget *)pc.params[0].getPtr();
					bindPipeBuffer( rt->rendBuf, pc.params[1].getString(), (uint32)pc.params[2].getInt() );
					break;

				case PipelineCommands::UnbindBuffers:
					bindPipeBuffer( 0x0, "", 0 );
					break;

				case PipelineCommands::ClearTarget:
					clear( pc.params[0].getBool(), pc.params[1].getBool(), pc.params[2].getBool(),
						pc.params[3].getBool(), pc.params[4].getBool(), pc.params[5].getFloat(),
						pc.params[6].getFloat(), pc.params[7].getFloat(), pc.params[8].getFloat() );
					break;

				case PipelineCommands::DrawGeometry:
					drawGeometry( pc.params[0].getString(), pc.params[1].getString(),
						(RenderingOrder::List)pc.params[2].getInt(), _curCamera->occSet() );
					break;

				case PipelineCommands::DrawOverlays:
					drawOverlays( pc.params[0].getString() );
					break;

				case PipelineCommands::DrawQuad:
					drawFSQuad( pc.params[0].getResource(), pc.params[1].getString() );
					break;

				case PipelineCommands::DoForwardLightLoop:
					drawLightGeometry( pc.params[0].getString(), pc.params[1].getString(),
						pc.params[2].getBool(), (RenderingOrder::List)pc.params[3].getInt(),
						_curCamera->occSet() );
					break;

				case PipelineCommands::DoDeferredLightLoop:
					drawLightShapes( pc.params[0].getString(), pc.params[1].getBool(), _curCamera->occSet() );
					break;

				case PipelineCommands::SetUniform:
					if( pc.params[0].getResource() && pc.params[0].getResource()->getType() == ResourceTypes::Material )
					{
						((MaterialResource *)pc.params[0].getResource())->setUniform( pc.params[1].getString(),
							pc.params[2].getFloat(), pc.params[3].getFloat(),
							pc.params[4].getFloat(), pc.params[5].getFloat() );
					}
					break;
				}
			}
		}

		finishRendering();
	}


	void OpenGLRenderer::finalizeFrame()
	{
		++_frameID;

		// Reset frame timer
		Timer *timer = Modules::stats().getTimer( EngineStats::FrameTime );
		ASSERT( timer != 0x0 );
		Modules::stats().getStat( EngineStats::FrameTime, true );  // Reset
		Modules::stats().incStat( EngineStats::FrameTime, timer->getElapsedTimeMS() );
		timer->reset();
	}


	void OpenGLRenderer::renderDebugView()
	{
		float color[4] = { 0 };

		_rd->setRenderBuffer( 0 );
		setMaterial( 0x0, "" );
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

		_rd->clear( CLR_DEPTH | CLR_COLOR );

		Modules::sceneMan().updateQueues( _curCamera->getFrustum(), 0x0, RenderingOrder::None,
			SceneNodeFlags::NoDraw, true, true );

		// Draw renderable nodes as wireframe
		setupViewMatrices( _curCamera->getViewMat(), _curCamera->getProjMat() );
		drawRenderables( "", "", true, &_curCamera->getFrustum(), 0x0, RenderingOrder::None, -1 );

		// Draw bounding boxes
		glDisable( GL_CULL_FACE );
		setMaterial( 0x0, "" );
		setShaderComb( &_defColorShader );
		commitGeneralUniforms();
		_rd->setShaderConst( _defColorShader.uni_worldMat, CONST_FLOAT44, &Matrix4f().x[0] );
		color[0] = 0.4f; color[1] = 0.4f; color[2] = 0.4f; color[3] = 1;
		_rd->setShaderConst( _defColShader_color, CONST_FLOAT4, color );
		for( uint32 i = 0, s = (uint32)Modules::sceneMan().getRenderQueue().size(); i < s; ++i )
		{
			SceneNode *sn = Modules::sceneMan().getRenderQueue()[i].node;

			drawAABB( sn->getBBox().min, sn->getBBox().max );
		}
		glEnable( GL_CULL_FACE );

		/*// Draw skeleton
		setShaderConst( _defColorShader.uni_worldMat, CONST_FLOAT44, &Matrix4f().x[0] );
		color[0] = 1; color[1] = 0; color[2] = 0; color[3] = 1;
		setShaderConst( Modules::renderer()._defColShader_color, CONST_FLOAT, color );
		glLineWidth( 2.0f );
		glPointSize( 5.0f );
		for( uint32 i = 0, si = (uint32)Modules::sceneMan().getRenderableQueue().size(); i < si; ++i )
		{
		SceneNode *sn = Modules::sceneMan().getRenderableQueue()[i].node;

		if( sn->getType() == SceneNodeTypes::Model )
		{
		ModelNode *model = (ModelNode *)sn;

		for( uint32 j = 0, sj = (uint32)model->_jointList.size(); j < sj; ++j )
		{
		if( model->_jointList[j]->_parent->getType() != SceneNodeTypes::Model )
		{
		Vec3f pos1 = model->_jointList[j]->_absTrans * Vec3f( 0, 0, 0 );
		Vec3f pos2 = model->_jointList[j]->_parent->_absTrans * Vec3f( 0, 0, 0 );

		glBegin( GL_LINES );
		glVertex3fv( (float *)&pos1.x );
		glVertex3fv( (float *)&pos2.x );
		glEnd();

		glBegin( GL_POINTS );
		glVertex3fv( (float *)&pos1.x );
		glEnd();
		}
		}
		}
		}
		glLineWidth( 1.0f );
		glPointSize( 1.0f );*/

		// Draw light volumes
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
		glCullFace( GL_FRONT );
		color[0] = 1; color[1] = 1; color[2] = 0; color[3] = 0.25f;
		_rd->setShaderConst( _defColShader_color, CONST_FLOAT4, color );
		for( size_t i = 0, s = Modules::sceneMan().getLightQueue().size(); i < s; ++i )
		{
			LightNode *lightNode = (LightNode *)Modules::sceneMan().getLightQueue()[i];

			if( lightNode->fov() < 180 )
			{
				float r = lightNode->radius() * tanf( degToRad( lightNode->fov() / 2 ) );
				drawCone( lightNode->radius(), r, lightNode->getAbsTrans() );
			}
			else
			{
				drawSphere( lightNode->absolutePos(), lightNode->radius() );
			}
		}
		glCullFace( GL_BACK );
		glDisable( GL_BLEND );
	}


	void OpenGLRenderer::finishRendering()
	{
		_rd->setRenderBuffer( 0 );
		setMaterial( 0x0, "" );
		_rd->resetStates();
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}

}
