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

#ifndef _egOpenGLRenderDevice_H_
#define _egOpenGLRenderDevice_H_

#include "egRenderDeviceBase.h"


namespace Horde3DOpenGLRenderer {

	using namespace Horde3D;

	class OpenGLGPUTimer : public IGPUTimer
	{
	public:
		OpenGLGPUTimer();
		~OpenGLGPUTimer();

		static IGPUTimer* factoryFunc();

		void beginQuery( uint32 frameID );
		void endQuery();
		bool updateResults();

		void reset();
		float getTimeMS() { return _time; }

	private:
		std::vector < uint32 >  _queryPool;
		uint32                  _numQueries;
		uint32                  _queryFrame;
		float                   _time;
		bool                    _activeQuery;
	};

	class OpenGLRenderDevice: public IRenderDevice
	{
	public:

		OpenGLRenderDevice();
		virtual ~OpenGLRenderDevice();

		static IRenderDevice* factoryFunc();

		void initStates();
		bool init();

		// -----------------------------------------------------------------------------
		// Resources
		// -----------------------------------------------------------------------------

		// Vertex layouts
		uint32 registerVertexLayout( uint32 numAttribs, VertexLayoutAttrib *attribs );

		// Buffers
		void beginRendering();
		uint32 createVertexBuffer( uint32 size, const void *data );
		uint32 createIndexBuffer( uint32 size, const void *data );
		void destroyBuffer( uint32 bufObj );
		void updateBufferData( uint32 bufObj, uint32 offset, uint32 size, void *data );
		uint32 getBufferMem() { return _bufferMem; }

		// Textures
		uint32 calcTextureSize( TextureFormats::List format, int width, int height, int depth );
		uint32 createTexture( TextureTypes::List type, int width, int height, int depth, TextureFormats::List format,
			bool hasMips, bool genMips, bool compress, bool sRGB );
		void uploadTextureData( uint32 texObj, int slice, int mipLevel, const void *pixels );
		void destroyTexture( uint32 texObj );
		void updateTextureData( uint32 texObj, int slice, int mipLevel, const void *pixels );
		bool getTextureData( uint32 texObj, int slice, int mipLevel, void *buffer );
		uint32 getTextureMem() { return _textureMem; }

		// Shaders
		uint32 createShader( const char *vertexShaderSrc, const char *fragmentShaderSrc );
		void destroyShader( uint32 shaderId );
		void bindShader( uint32 shaderId );
		std::string &getShaderLog() { return _shaderLog; }
		int getShaderConstLoc( uint32 shaderId, const char *name );
		int getShaderSamplerLoc( uint32 shaderId, const char *name );
		void setShaderConst( int loc, RDIShaderConstType type, void *values, uint32 count = 1 );
		void setShaderSampler( int loc, uint32 texUnit );

		// Renderbuffers
		uint32 createRenderBuffer( uint32 width, uint32 height, TextureFormats::List format,
			bool depth, uint32 numColBufs, uint32 samples );
		void destroyRenderBuffer( uint32 rbObj );
		uint32 getRenderBufferTex( uint32 rbObj, uint32 bufIndex );
		void setRenderBuffer( uint32 rbObj );
		bool getRenderBufferData( uint32 rbObj, int bufIndex, int *width, int *height,
			int *compCount, void *dataBuffer, int bufferSize );

		// Queries
		uint32 createOcclusionQuery();
		void destroyQuery( uint32 queryObj );
		void beginQuery( uint32 queryObj );
		void endQuery( uint32 queryObj );
		uint32 getQueryResult( uint32 queryObj );

		// -----------------------------------------------------------------------------
		// Commands
		// -----------------------------------------------------------------------------

		void setViewport( int x, int y, int width, int height )
		{ _vpX = x; _vpY = y; _vpWidth = width; _vpHeight = height; _pendingMask |= PM_VIEWPORT; }
		int viewportX() { return _vpX; }
		int viewportY() { return _vpY; }
		int viewportWidth() { return _vpWidth; }
		int viewportHeight() { return _vpHeight; }
		void setScissorRect( int x, int y, int width, int height )
		{ _scX = x; _scY = y; _scWidth = width; _scHeight = height; _pendingMask |= PM_SCISSOR; }
		int scissorX() { return _scX; }
		int scissorY() { return _scY; }
		int scissorWidth() { return _scWidth; }
		int	scissorHeight() { return _scHeight; }
		void setIndexBuffer( uint32 bufObj, RDIIndexFormat idxFmt )
		{ _indexFormat = (uint32)idxFmt; _newIndexBuf = bufObj; _pendingMask |= PM_INDEXBUF; }
		void setVertexBuffer( uint32 slot, uint32 vbObj, uint32 offset, uint32 stride )
		{ ASSERT( slot < 16 ); _vertBufSlots[slot] = RDIVertBufSlot( vbObj, offset, stride );
		_pendingMask |= PM_VERTLAYOUT; }
		void setVertexLayout( uint32 vlObj )
		{ _newVertLayout = vlObj; }
		void setTexture( uint32 slot, uint32 texObj, uint16 samplerState )
		{ ASSERT( slot < 16 ); _texSlots[slot] = RDITexSlot( texObj, samplerState );
		_pendingMask |= PM_TEXTURES; }

		bool commitStates( uint32 filter = 0xFFFFFFFF );
		void resetStates();

		// Draw calls and clears
		void clear( uint32 flags, float *colorRGBA = 0x0, float depth = 1.0f );
		void draw( RDIPrimType primType, uint32 firstVert, uint32 numVerts );
		void drawIndexed( RDIPrimType primType, uint32 firstIndex, uint32 numIndices,
			uint32 firstVert, uint32 numVerts );

		// -----------------------------------------------------------------------------
		// Getters
		// -----------------------------------------------------------------------------

		const DeviceCaps &getCaps() { return _caps; }
		const RDIBuffer &getBuffer( uint32 bufObj ) { return _buffers.getRef( bufObj ); }
		const RDITexture &getTexture( uint32 texObj ) { return _textures.getRef( texObj ); }
		const uint32 currentRenderBufferHandle() { return _curRendBuf; }
		const RDIRenderBuffer &getRenderBuffer( uint32 rbObj ) { return _rendBufs.getRef( rbObj ); }
		const int frameBufferWidth() { return _fbWidth; }
		const int frameBufferHeight() { return _fbHeight; }
		const uint32 currentShaderHandle() { return _curShaderId; }
		void setOutputBufferIndex(const int oindex) { _outputBufferIndex = oindex; }
		const int outputBufferIndex() { return _outputBufferIndex; }

	protected:

		uint32 createShaderProgram( const char *vertexShaderSrc, const char *fragmentShaderSrc );
		bool linkShaderProgram( uint32 programObj );
		void resolveRenderBuffer( uint32 rbObj );

		void checkGLError();
		bool applyVertexLayout();
		void applySamplerState( RDITexture &tex );

	protected:

		DeviceCaps    _caps;

		uint32        _depthFormat;
		int           _vpX, _vpY, _vpWidth, _vpHeight;
		int           _scX, _scY, _scWidth, _scHeight;
		int           _fbWidth, _fbHeight;
		std::string   _shaderLog;
		uint32        _curRendBuf;
		int           _outputBufferIndex;  // Left and right eye for stereo rendering
		uint32        _textureMem, _bufferMem;

		int                            _defaultFBO;
		uint32                         _numVertexLayouts;
		RDIVertexLayout                _vertexLayouts[MaxNumVertexLayouts];
		RDIObjects< RDIBuffer >        _buffers;
		RDIObjects< RDITexture >       _textures;
		RDIObjects< RDIShader >        _shaders;
		RDIObjects< RDIRenderBuffer >  _rendBufs;

		RDIVertBufSlot    _vertBufSlots[16];
		RDITexSlot        _texSlots[16];
		uint32            _prevShaderId, _curShaderId;
		uint32            _curVertLayout, _newVertLayout;
		uint32            _curIndexBuf, _newIndexBuf;
		uint32            _indexFormat;
		uint32            _activeVertexAttribsMask;
		uint32            _pendingMask;
	};
}
#endif // _egOpenGLRenderDevice_H_
