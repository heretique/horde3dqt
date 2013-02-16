#ifndef _egRenderDeviceBase_H_
#define _egRenderDeviceBase_H_

#include "egPrerequisites.h"
#include <vector>

namespace Horde3D
{
	const uint32 MaxNumVertexLayouts = 16;

	class IGPUTimer
	{
	public:
		virtual ~IGPUTimer();

		virtual void beginQuery( uint32 frameID ) = 0;
		virtual void endQuery() = 0;
		virtual bool updateResults() = 0;

		virtual void reset() = 0;
		virtual float getTimeMS() = 0;
	};

	// =================================================================================================
	// Render Device Interface
	// =================================================================================================

	// ---------------------------------------------------------
	// General
	// ---------------------------------------------------------

	template< class T > class RDIObjects
	{
	public:

		uint32 add( const T &obj )
		{
			if( !_freeList.empty() )
			{
				uint32 index = _freeList.back();
				_freeList.pop_back();
				_objects[index] = obj;
				return index + 1;
			}
			else
			{
				_objects.push_back( obj );
				return (uint32)_objects.size();
			}
		}

		void remove( uint32 handle )
		{
			ASSERT( handle > 0 && handle <= _objects.size() );

			_objects[handle - 1] = T();  // Destruct and replace with default object
			_freeList.push_back( handle - 1 );
		}

		T &getRef( uint32 handle )
		{
			ASSERT( handle > 0 && handle <= _objects.size() );

			return _objects[handle - 1];
		}

		friend class RenderDevice;

	private:
		std::vector< T >       _objects;
		std::vector< uint32 >  _freeList;
	};


	struct DeviceCaps
	{
		bool  texFloat;
		bool  texNPOT;
		bool  rtMultisampling;
        bool  texDepth;
        bool  texBGRA8;
        bool  texPVRTC;
        bool  texDXT1;
        bool  texATC;
        bool  tex3DC;
        bool  texETC;
        bool  texPVRTC2;
        bool  tex3D;
        bool  texAnisotropic;
        bool  MRT;
        bool  texS3TC;
        bool  occQuery;
        bool  timerQuery;
	};


	// ---------------------------------------------------------
	// Vertex layout
	// ---------------------------------------------------------

	struct VertexLayoutAttrib
	{
		std::string  semanticName;
		uint32       vbSlot;
		uint32       size;
		uint32       offset;
	};

	struct RDIVertexLayout
	{
		uint32              numAttribs;
		VertexLayoutAttrib  attribs[16];
	};


	// ---------------------------------------------------------
	// Buffers
	// ---------------------------------------------------------

	struct RDIBuffer
	{
		uint32  type;
		uint32  glObj;
		uint32  size;
	};

	struct RDIVertBufSlot
	{
		uint32  vbObj;
		uint32  offset;
		uint32  stride;

		RDIVertBufSlot() : vbObj( 0 ), offset( 0 ), stride( 0 ) {}
		RDIVertBufSlot( uint32 vbObj, uint32 offset, uint32 stride ) :
		vbObj( vbObj ), offset( offset ), stride( stride ) {}
	};


	// ---------------------------------------------------------
	// Textures
	// ---------------------------------------------------------

	struct TextureTypes
	{
		enum List
		{
			Tex2D, // = GL_TEXTURE_2D,
			Tex3D, // = GL_TEXTURE_3D,
			TexCube // = GL_TEXTURE_CUBE_MAP
		};
	};

	struct TextureFormats
	{
		enum List
		{
			Unknown,
            RGBA,
			BGRA8,
			DXT1,
			DXT3,
			DXT5,
			RGBA16F,
			RGBA32F,
            DEPTH,
            PVRTC_2BPP,
            PVRTC_A2BPP,
            PVRTC_4BPP,
            PVRTC_A4BPP,
            RGB5_A1,
            RGB565,
            RGBA4,
            ATC_RGB,
            ATC_RGBeA,
            ATC_RGBiA,
            T3DC_X,
            T3DC_XY,
            ETC,
            PVRTC2_2BPP,
            PVRTC2_A2BPP,
            PVRTC2_4BPP,
            PVRTC2_A4BPP
		};
	};

	struct RDITexture
	{
		uint32                glObj;
		uint32                glFmt;
		int                   type;
		TextureFormats::List  format;
		int                   width, height, depth;
		int                   memSize;
		uint32                samplerState;
		bool                  sRGB;
		bool                  hasMips, genMips;
	};

	struct RDITexSlot
	{
		uint32  texObj;
		uint32  samplerState;

		RDITexSlot() : texObj( 0 ), samplerState( 0 ) {}
		RDITexSlot( uint32 texObj, uint32 samplerState ) :
		texObj( texObj ), samplerState( samplerState ) {}
	};


	// ---------------------------------------------------------
	// Shaders
	// ---------------------------------------------------------

	enum RDIShaderConstType
	{
		CONST_FLOAT,
		CONST_FLOAT2,
		CONST_FLOAT3,
		CONST_FLOAT4,
		CONST_FLOAT44,
		CONST_FLOAT33
	};

	struct RDIInputLayout
	{
		bool  valid;
		int8  attribIndices[16];
	};

	struct RDIShader
	{
		uint32          oglProgramObj;
		RDIInputLayout  inputLayouts[MaxNumVertexLayouts];
	};


	// ---------------------------------------------------------
	// Render buffers
	// ---------------------------------------------------------

	struct RDIRenderBuffer
	{
		static const uint32 MaxColorAttachmentCount = 4;

		uint32  fbo, fboMS;  // fboMS: Multisampled FBO used when samples > 0
		uint32  width, height;
		uint32  samples;

		uint32  depthTex, colTexs[MaxColorAttachmentCount];
		uint32  depthBuf, colBufs[MaxColorAttachmentCount];  // Used for multisampling

		RDIRenderBuffer() : fbo( 0 ), fboMS( 0 ), width( 0 ), height( 0 ), depthTex( 0 ), depthBuf( 0 )
		{
			for( uint32 i = 0; i < MaxColorAttachmentCount; ++i ) colTexs[i] = colBufs[i] = 0;
		}
	};


	// ---------------------------------------------------------
	// Render states
	// ---------------------------------------------------------

	enum RDISamplerState
	{
		SS_FILTER_BILINEAR   = 0x0,
		SS_FILTER_TRILINEAR  = 0x0001,
		SS_FILTER_POINT      = 0x0002,
		SS_ANISO1            = 0x0,
		SS_ANISO2            = 0x0004,
		SS_ANISO4            = 0x0008,
		SS_ANISO8            = 0x0010,
		SS_ANISO16           = 0x0020,
		SS_ADDRU_CLAMP       = 0x0,
		SS_ADDRU_WRAP        = 0x0040,
		SS_ADDRU_CLAMPCOL    = 0x0080,
		SS_ADDRV_CLAMP       = 0x0,
		SS_ADDRV_WRAP        = 0x0100,
		SS_ADDRV_CLAMPCOL    = 0x0200,
		SS_ADDRW_CLAMP       = 0x0,
		SS_ADDRW_WRAP        = 0x0400,
		SS_ADDRW_CLAMPCOL    = 0x0800,
		SS_ADDR_CLAMP        = SS_ADDRU_CLAMP | SS_ADDRV_CLAMP | SS_ADDRW_CLAMP,
		SS_ADDR_WRAP         = SS_ADDRU_WRAP | SS_ADDRV_WRAP | SS_ADDRW_WRAP,
		SS_ADDR_CLAMPCOL     = SS_ADDRU_CLAMPCOL | SS_ADDRV_CLAMPCOL | SS_ADDRW_CLAMPCOL,
		SS_COMP_LEQUAL       = 0x1000
	};

	const uint32 SS_FILTER_START = 0;
	const uint32 SS_FILTER_MASK = SS_FILTER_BILINEAR | SS_FILTER_TRILINEAR | SS_FILTER_POINT;
	const uint32 SS_ANISO_START = 2;
	const uint32 SS_ANISO_MASK = SS_ANISO1 | SS_ANISO2 | SS_ANISO4 | SS_ANISO8 | SS_ANISO16;
	const uint32 SS_ADDRU_START = 6;
	const uint32 SS_ADDRU_MASK = SS_ADDRU_CLAMP | SS_ADDRU_WRAP | SS_ADDRU_CLAMPCOL;
	const uint32 SS_ADDRV_START = 8;
	const uint32 SS_ADDRV_MASK = SS_ADDRV_CLAMP | SS_ADDRV_WRAP | SS_ADDRV_CLAMPCOL;
	const uint32 SS_ADDRW_START = 10;
	const uint32 SS_ADDRW_MASK = SS_ADDRW_CLAMP | SS_ADDRW_WRAP | SS_ADDRW_CLAMPCOL;
	const uint32 SS_ADDR_START = 6;
	const uint32 SS_ADDR_MASK = SS_ADDR_CLAMP | SS_ADDR_WRAP | SS_ADDR_CLAMPCOL;


	// ---------------------------------------------------------
	// Draw calls and clears
	// ---------------------------------------------------------

	enum RDIClearFlags
	{
		CLR_COLOR = 0x00000001,
		CLR_DEPTH = 0x00000002
	};

	enum RDIIndexFormat
	{
		IDXFMT_16, // = GL_UNSIGNED_SHORT,
		IDXFMT_32 // = GL_UNSIGNED_INT
	};

	enum RDIPrimType
	{
		PRIM_TRILIST, // = GL_TRIANGLES,
		PRIM_TRISTRIP // = GL_TRIANGLE_STRIP
	};


	// ---------------------------------------------------------
	// Render device interface
	// ---------------------------------------------------------

	class IRenderDevice
	{
	public:
		virtual ~IRenderDevice();

		virtual void initStates() = 0;
		virtual bool init() = 0;

		// -----------------------------------------------------------------------------
		// Resources
		// -----------------------------------------------------------------------------

		// Vertex layouts
		virtual uint32 registerVertexLayout( uint32 numAttribs, VertexLayoutAttrib *attribs ) = 0;

		// Buffers
		virtual void beginRendering() = 0;
		virtual uint32 createVertexBuffer( uint32 size, const void *data ) = 0;
		virtual uint32 createIndexBuffer( uint32 size, const void *data ) = 0;
		virtual void destroyBuffer( uint32 bufObj ) = 0;
		virtual void updateBufferData( uint32 bufObj, uint32 offset, uint32 size, void *data ) = 0;
		virtual uint32 getBufferMem() = 0;

		// Textures
		virtual uint32 calcTextureSize( TextureFormats::List format, int width, int height, int depth ) = 0;
		virtual uint32 createTexture( TextureTypes::List type, int width, int height, int depth, TextureFormats::List format,
			bool hasMips, bool genMips, bool compress, bool sRGB ) = 0;
		virtual void uploadTextureData( uint32 texObj, int slice, int mipLevel, const void *pixels ) = 0;
		virtual void destroyTexture( uint32 texObj ) = 0;
		virtual void updateTextureData( uint32 texObj, int slice, int mipLevel, const void *pixels ) = 0;
		virtual bool getTextureData( uint32 texObj, int slice, int mipLevel, void *buffer ) = 0;
		virtual uint32 getTextureMem() = 0;

		// Shaders
		virtual uint32 createShader( const char *vertexShaderSrc, const char *fragmentShaderSrc ) = 0;
		virtual void destroyShader( uint32 shaderId ) = 0;
		virtual void bindShader( uint32 shaderId ) = 0;
		virtual std::string &getShaderLog() = 0;
		virtual int getShaderConstLoc( uint32 shaderId, const char *name ) = 0;
		virtual int getShaderSamplerLoc( uint32 shaderId, const char *name ) = 0;
		virtual void setShaderConst( int loc, RDIShaderConstType type, void *values, uint32 count = 1 ) = 0;
		virtual void setShaderSampler( int loc, uint32 texUnit ) = 0;

		// Renderbuffers
		virtual uint32 createRenderBuffer( uint32 width, uint32 height, TextureFormats::List format,
			bool depth, uint32 numColBufs, uint32 samples ) = 0;
		virtual void destroyRenderBuffer( uint32 rbObj ) = 0;
		virtual uint32 getRenderBufferTex( uint32 rbObj, uint32 bufIndex ) = 0;
		virtual void setRenderBuffer( uint32 rbObj ) = 0;
		virtual bool getRenderBufferData( uint32 rbObj, int bufIndex, int *width, int *height,
			int *compCount, void *dataBuffer, int bufferSize ) = 0;

		// Queries
		virtual uint32 createOcclusionQuery() = 0;
		virtual void destroyQuery( uint32 queryObj ) = 0;
		virtual void beginQuery( uint32 queryObj ) = 0;
		virtual void endQuery( uint32 queryObj ) = 0;
		virtual uint32 getQueryResult( uint32 queryObj ) = 0;

		// -----------------------------------------------------------------------------
		// Commands
		// -----------------------------------------------------------------------------

		virtual void setViewport( int x, int y, int width, int height ) = 0;
		virtual int viewportX() = 0;
		virtual int viewportY() = 0;
		virtual int viewportWidth() = 0;
		virtual int viewportHeight() = 0;
		virtual void setScissorRect( int x, int y, int width, int height ) = 0;
		virtual int scissorX() = 0;
		virtual int scissorY() = 0;
		virtual int scissorWidth() = 0;
		virtual int	scissorHeight() = 0;
		virtual void setIndexBuffer( uint32 bufObj, RDIIndexFormat idxFmt ) = 0;
		virtual void setVertexBuffer( uint32 slot, uint32 vbObj, uint32 offset, uint32 stride ) = 0;
		virtual void setVertexLayout( uint32 vlObj ) = 0;
		virtual void setTexture( uint32 slot, uint32 texObj, uint16 samplerState ) = 0;
		virtual bool commitStates( uint32 filter = 0xFFFFFFFF ) = 0;
		virtual void resetStates() = 0;

		// Draw calls and clears
		virtual void clear( uint32 flags, float *colorRGBA = 0x0, float depth = 1.0f ) = 0;
		virtual void draw( RDIPrimType primType, uint32 firstVert, uint32 numVerts ) = 0;
		virtual void drawIndexed( RDIPrimType primType, uint32 firstIndex, uint32 numIndices,
			uint32 firstVert, uint32 numVerts ) = 0;

		// -----------------------------------------------------------------------------
		// Getters
		// -----------------------------------------------------------------------------

		virtual const DeviceCaps &getCaps() = 0;
		virtual const RDIBuffer &getBuffer( uint32 bufObj ) = 0;
		virtual const RDITexture &getTexture( uint32 texObj ) = 0;
		virtual const uint32 currentRenderBufferHandle() = 0;
		virtual const RDIRenderBuffer &getRenderBuffer( uint32 rbObj ) = 0;
		virtual const int frameBufferWidth() = 0;
		virtual const int frameBufferHeight() = 0;
		virtual const uint32 currentShaderHandle() = 0;
		virtual void setOutputBufferIndex(const int oindex) = 0;
		virtual const int outputBufferIndex() = 0; // Left and right eye for stereo rendering

	protected:
		enum RDIPendingMask
		{
			PM_VIEWPORT    = 0x00000001,
			PM_INDEXBUF    = 0x00000002,
			PM_VERTLAYOUT  = 0x00000004,
			PM_TEXTURES    = 0x00000008,
			PM_SCISSOR     = 0x00000010
		};
	};

} // namespace Horde3D



#endif // _egRenderDeviceBase_H_
