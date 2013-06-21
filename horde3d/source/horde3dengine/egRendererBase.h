// *************************************************************************************************
//
// Horde3D
//   Next-Generation Graphics Engine
// --------------------------------------
// Copyright (C) 2006-2011 Nicolas Schulz
//
// This software is distributed under the terms of the Eclipse Public License v1.0.
// A copy of the license may be obtained at: http://www.eclipse.org/legal/epl-v10.html
//
// *************************************************************************************************

#ifndef _egRenderer_H_
#define _egRenderer_H_

#include "egPrerequisites.h"
#include "egRenderDeviceBase.h"
#include "egPrimitives.h"
#include "egModel.h"
#include <vector>
#include <algorithm>


namespace Horde3D {

	class MaterialResource;
	class LightNode;
	class CameraNode;
	struct ShaderContext;

	const uint32 MaxNumOverlayVerts = 2048;
	const uint32 ParticlesPerBatch = 64;	// Warning: The GPU must have enough registers
	const uint32 QuadIndexBufCount = MaxNumOverlayVerts * 6;

	extern const char *vsDefColor;
	extern const char *fsDefColor;
	extern const char *vsOccBox;
	extern const char *fsOccBox;	


	// =================================================================================================
	// Renderer
	// =================================================================================================

	typedef void (*RenderFunc)( uint32 firstItem, uint32 lastItem, const std::string &shaderContext,
		const std::string &theClass, bool debugView, const Frustum *frust1,
		const Frustum *frust2, RenderingOrder::List order, int occSet );

	struct RenderFuncListItem
	{
		int         nodeType;
		RenderFunc  renderFunc;
	};

	// =================================================================================================

	struct OverlayBatch
	{
		PMaterialResource  materialRes;
		uint32             firstVert, vertCount;
		float              colRGBA[4];
		int                flags;

		OverlayBatch() {}

		OverlayBatch( uint32 firstVert, uint32 vertCount, float *col, MaterialResource *materialRes, int flags ) :
		materialRes( materialRes ), firstVert( firstVert ), vertCount( vertCount ), flags( flags )
		{
			colRGBA[0] = col[0]; colRGBA[1] = col[1]; colRGBA[2] = col[2]; colRGBA[3] = col[3];
		}
	};

	struct OverlayVert
	{
		float  x, y;  // Position
		float  u, v;  // Texture coordinates
	};


	struct ParticleVert
	{
		float  u, v;         // Texture coordinates
		float  index;        // Index in property array

		ParticleVert() {}

		ParticleVert( float u, float v ):
		u( u ), v( v ), index( 0 )
		{
		}
	};

	// =================================================================================================

	struct OccProxy
	{
		Vec3f   bbMin, bbMax;
		uint32  queryObj;

		OccProxy() {}
		OccProxy( const Vec3f &bbMin, const Vec3f &bbMax, uint32 queryObj ) :
		bbMin( bbMin ), bbMax( bbMax ), queryObj( queryObj )
		{
		}
	};

	struct PipeSamplerBinding
	{
		char    sampler[64];
		uint32  rbObj;
		uint32  bufIndex;
	};

	class IRenderDevice;

	class IRenderer
	{
	public:
        virtual ~IRenderer() {}
		virtual IRenderDevice* renderDevice() = 0;
		virtual void registerRenderFunc( int nodeType, RenderFunc rf ) = 0;

		virtual unsigned char *useScratchBuf( uint32 minSize ) = 0;

		virtual bool init() = 0;
		virtual void initStates() = 0;
		virtual void initInternalRenderFuncs(int nodeType) = 0;

		virtual void drawAABB( const Vec3f &bbMin, const Vec3f &bbMax ) = 0;
		virtual void drawSphere( const Vec3f &pos, float radius ) = 0;
		virtual void drawCone( float height, float fov, const Matrix4f &transMat ) = 0;

		virtual bool createShaderComb( const char *vertexShader, const char *fragmentShader, ShaderCombination &sc ) = 0;
		virtual void releaseShaderComb( ShaderCombination &sc ) = 0;
		virtual void setShaderComb( ShaderCombination *sc ) = 0;
		virtual void commitGeneralUniforms() = 0;
		virtual bool setMaterial( MaterialResource *materialRes, const std::string &shaderContext ) = 0;

		virtual bool createShadowRB( uint32 width, uint32 height ) = 0;
		virtual void releaseShadowRB() = 0;

		virtual int registerOccSet() = 0;
		virtual void unregisterOccSet( int occSet ) = 0;
		virtual void drawOccProxies( uint32 list ) = 0;
		virtual void pushOccProxy( uint32 list, const Vec3f &bbMin, const Vec3f &bbMax, uint32 queryObj ) = 0;

		virtual void showOverlays( const float *verts, uint32 vertCount, float *colRGBA,
			MaterialResource *matRes, int flags ) = 0;
		virtual void clearOverlays() = 0;
		virtual void render( CameraNode *camNode ) = 0;
		virtual void finalizeFrame() = 0;

		virtual ShaderCombination *getCurShader() = 0;
		virtual CameraNode *getCurCamera() = 0;

		virtual void drawOverlays( const std::string &shaderContext ) = 0;

		virtual void bindPipeBuffer( uint32 rbObj, const std::string &sampler, uint32 bufIndex ) = 0;
		virtual void clear( bool depth, bool buf0, bool buf1, bool buf2, bool buf3, float r, float g, float b, float a ) = 0;
		virtual void drawFSQuad( Resource *matRes, const std::string &shaderContext ) = 0;
		virtual void drawGeometry( const std::string &shaderContext, const std::string &theClass,
			RenderingOrder::List order, int occSet ) = 0;
		virtual void drawLightGeometry( const std::string &shaderContext, const std::string &theClass,
			bool noShadows, RenderingOrder::List order, int occSet ) = 0;
		virtual void drawLightShapes( const std::string &shaderContext, bool noShadows, int occSet ) = 0;

		virtual void drawRenderables( const std::string &shaderContext, const std::string &theClass, bool debugView,
			const Frustum *frust1, const Frustum *frust2, RenderingOrder::List order, int occSet ) = 0;

		virtual void renderDebugView() = 0;
		virtual void finishRendering() = 0;
		virtual uint32 getFrameID() = 0;
	};

}
#endif // _egRenderer_H_
