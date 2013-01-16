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

#include "utOpenGLES.h"
#include "renderdeviceES.h"
#include "egModules.h"
#include "egCom.h"
#include "utMath.h"
#include "utDebug.h"


namespace Horde3DOpenGLESRenderer {

#ifdef H3D_VALIDATE_DRAWCALLS
#	define CHECK_GL_ERROR checkGLError();
#else
#	define CHECK_GL_ERROR
#endif

// =================================================================================================
// GPUTimer
// =================================================================================================

OpenGLESGPUTimer::OpenGLESGPUTimer() : _numQueries( 0 ),  _queryFrame( 0 ), _time( 0 ), _activeQuery( false )
{
    reset();
}


OpenGLESGPUTimer::~OpenGLESGPUTimer()
{
    //	if( !_queryPool.empty() )
    //		glDeleteQueries( (uint32)_queryPool.size(), &_queryPool[0] );
}

IGPUTimer* OpenGLESGPUTimer::factoryFunc()
{
    return new OpenGLESGPUTimer();
}

void OpenGLESGPUTimer::beginQuery( uint32 frameID )
{
    //	if( !glExt::ARB_timer_query ) return;
    //	ASSERT( !_activeQuery );

    //	if( _queryFrame != frameID )
    //	{
    //		if( !updateResults() ) return;

    //		_queryFrame = frameID;
    //		_numQueries = 0;
    //	}

    //	// Create new query pair if necessary
    //	uint32 queryObjs[2];
    //	if( _numQueries++ * 2 == _queryPool.size() )
    //	{
    //		glGenQueries( 2, queryObjs );
    //		_queryPool.push_back( queryObjs[0] );
    //		_queryPool.push_back( queryObjs[1] );
    //	}
    //	else
    //	{
    //		queryObjs[0] = _queryPool[(_numQueries - 1) * 2];
    //	}

    //	_activeQuery = true;
    //	 glQueryCounter( queryObjs[0], GL_TIMESTAMP );
}


void OpenGLESGPUTimer::endQuery()
{
    //	if( _activeQuery )
    //	{
    //		glQueryCounter( _queryPool[_numQueries * 2 - 1], GL_TIMESTAMP );
    //		_activeQuery = false;
    //	}
}


bool OpenGLESGPUTimer::updateResults()
{
    //    if( !glExt::ARB_timer_query ) return false;

    //	if( _numQueries == 0 )
    //	{
    //		_time = 0;
    //		return true;
    //	}

    //	// Make sure that last query is available
    //	GLint available;
    //	glGetQueryObjectiv( _queryPool[_numQueries * 2 - 1], GL_QUERY_RESULT_AVAILABLE, &available );
    //	if( !available ) return false;

    //	//  Accumulate time
    //	GLuint64 timeStart = 0, timeEnd = 0, timeAccum = 0;
    //	for( uint32 i = 0; i < _numQueries; ++i )
    //	{
    //		glGetQueryObjectui64v( _queryPool[i * 2], GL_QUERY_RESULT, &timeStart );
    //		glGetQueryObjectui64v( _queryPool[i * 2 + 1], GL_QUERY_RESULT, &timeEnd );
    //		timeAccum += timeEnd - timeStart;
    //	}

    //	_time = (float)((double)timeAccum / 1000000.0);
    return true;
}


void OpenGLESGPUTimer::reset()
{
    //	_time = glExt::ARB_timer_query ? 0.f : -1.f;
}


// =================================================================================================
// OpenGLESRenderDevice
// =================================================================================================

OpenGLESRenderDevice::OpenGLESRenderDevice()
{
    _numVertexLayouts = 0;

    _vpX = 0; _vpY = 0; _vpWidth = 320; _vpHeight = 240;
    _scX = 0; _scY = 0; _scWidth = 320; _scHeight = 240;
    _prevShaderId = _curShaderId = 0;
    _curRendBuf = 0; _outputBufferIndex = 0;
    _textureMem = 0; _bufferMem = 0;
    _curVertLayout = _newVertLayout = 0;
    _curIndexBuf = _newIndexBuf = 0;
    _defaultFBO = 0;
    _indexFormat = (uint32)IDXFMT_16;
    _pendingMask = 0;
}


OpenGLESRenderDevice::~OpenGLESRenderDevice()
{
}

IRenderDevice* OpenGLESRenderDevice::factoryFunc()
{
    return new OpenGLESRenderDevice();
}

void OpenGLESRenderDevice::initStates()
{
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
}


bool OpenGLESRenderDevice::init()
{
    bool failed = false;

    char *vendor = (char *)glGetString( GL_VENDOR );
    char *renderer = (char *)glGetString( GL_RENDERER );
    char *version = (char *)glGetString( GL_VERSION );

    Modules::log().writeInfo( "Initializing GLES2 backend using OpenGL ES 2.0 driver '%s' by '%s' on '%s'",
                              version, vendor, renderer );

    // Init extensions
    if( !initOpenGLExtensions() )
    {
        Modules::log().writeError( "Could not find all required OpenGL ES 2.0 function entry points" );
        failed = true;
    }

    // Check that OpenGL ES 2.0 is available
    if( glExt::majorVersion * 10 + glExt::minorVersion < 20 )
    {
        Modules::log().writeError( "OpenGL ES 2.0 not available" );
        failed = true;
    }

    char *exts = (char *)glGetString( GL_EXTENSIONS );
    Modules::log().writeInfo( "Supported extensions: '%s'", exts );

    if( failed )
    {
        Modules::log().writeError( "Failed to init renderer backend" );
        return false;
    }

    // Get capabilities
    _caps.texFloat = glExt::OES_texture_float ? 1 : 0;
    _caps.texNPOT = glExt::OES_texture_npot ? 1 : 0;

    _caps.rtMultisampling = glExt::EXT_multisampled_render_to_texture ? 1 : 0;
    if ( !_caps.rtMultisampling )
        _caps.rtMultisampling = glExt::IMG_multisampled_render_to_texture ? 1 : 0;
    if( !_caps.rtMultisampling )
        _caps.rtMultisampling = glExt::APPLE_framebuffer_multisample ? 1 : 0;
    if( !_caps.rtMultisampling )
        _caps.rtMultisampling = glExt::ANGLE_framebuffer_multisample ? 1 : 0;


    _caps.texBGRA8 = glExt::EXT_texture_format_BGRA8888 ? 1 : 0;
    if( !_caps.texBGRA8 )
        _caps.texBGRA8 = glExt::APPLE_texture_format_BGRA8888 ? 1 : 0;

    _caps.texDepth = glExt::OES_depth_texture ? 1 : 0;
    _caps.texPVRTC = glExt::IMG_texture_compression_pvrtc ? 1 : 0;
    _caps.texPVRTC2 = glExt::IMG_texture_compression_pvrtc2 ? 1 : 0;
    _caps.texDXT1 = glExt::EXT_texture_compression_dxt1 ? 1 : 0;
    _caps.texS3TC = glExt::EXT_texture_compression_s3tc ? 1 : 0;

    if( !_caps.texS3TC )
        _caps.texS3TC = glExt::ANGLE_texture_compression_dxt5 ? 1 : 0;

    _caps.texATC = glExt::AMD_compressed_ATC_texture ? 1 : 0;
    _caps.tex3DC = glExt::AMD_compressed_3DC_texture ? 1 : 0;
    _caps.texETC = glExt::OES_compressed_ETC1_RGB8_texture ? 1 : 0;
    _caps.tex3D = glExt::OES_texture_3D ? 1 : 0;

    _caps.occQuery = glExt::EXT_occlusion_query_boolean ? 1 : 0;
    if( !_caps.occQuery )
        Modules::log().writeWarning( "Extension EXT_occlusion_query_boolean not supported" );

    _caps.timerQuery = glExt::ANGLE_timer_query;

    _caps.texAnisotropic = glExt::EXT_texture_filter_anisotropic ? 1 : 0;
    if( !_caps.texAnisotropic )
        Modules::log().writeWarning( "Extension EXT_texture_filter_anisotropic not supported" );

    _caps.MRT = glExt::NV_draw_buffers ? 1 : 0;
    if( !_caps.MRT )
        Modules::log().writeWarning( "Multiple render targets not supported" );

    _depthFormat = GL_DEPTH_COMPONENT16;
    if (glExt::OES_depth24)
        _depthFormat = GL_DEPTH_COMPONENT24_OES;
    if (glExt::OES_depth32)
        _depthFormat = GL_DEPTH_COMPONENT32_OES;

    initStates();
    resetStates();

    return true;
}


// =================================================================================================
// Vertex layouts
// =================================================================================================

uint32 OpenGLESRenderDevice::registerVertexLayout( uint32 numAttribs, VertexLayoutAttrib *attribs )
{
    if( _numVertexLayouts == MaxNumVertexLayouts )
        return 0;

    _vertexLayouts[_numVertexLayouts].numAttribs = numAttribs;

    for( uint32 i = 0; i < numAttribs; ++i )
        _vertexLayouts[_numVertexLayouts].attribs[i] = attribs[i];

    return ++_numVertexLayouts;
}


// =================================================================================================
// Buffers
// =================================================================================================

void OpenGLESRenderDevice::beginRendering()
{	
    //	Get the currently bound frame buffer object.
    glGetIntegerv( GL_FRAMEBUFFER_BINDING, &_defaultFBO );
}

uint32 OpenGLESRenderDevice::createVertexBuffer( uint32 size, const void *data )
{
    RDIBuffer buf;

    buf.type = GL_ARRAY_BUFFER;
    buf.size = size;
    glGenBuffers( 1, &buf.glObj );
    glBindBuffer( buf.type, buf.glObj );
    glBufferData( buf.type, size, data, GL_DYNAMIC_DRAW );
    glBindBuffer( buf.type, 0 );

    _bufferMem += size;
    return _buffers.add( buf );
}


uint32 OpenGLESRenderDevice::createIndexBuffer( uint32 size, const void *data )
{
    RDIBuffer buf;

    buf.type = GL_ELEMENT_ARRAY_BUFFER;
    buf.size = size;
    glGenBuffers( 1, &buf.glObj );
    glBindBuffer( buf.type, buf.glObj );
    glBufferData( buf.type, size, data, GL_DYNAMIC_DRAW );
    glBindBuffer( buf.type, 0 );

    _bufferMem += size;
    return _buffers.add( buf );
}


void OpenGLESRenderDevice::destroyBuffer( uint32 bufObj )
{
    if( bufObj == 0 ) return;

    RDIBuffer &buf = _buffers.getRef( bufObj );
    glDeleteBuffers( 1, &buf.glObj );

    _bufferMem -= buf.size;
    _buffers.remove( bufObj );
}


void OpenGLESRenderDevice::updateBufferData( uint32 bufObj, uint32 offset, uint32 size, void *data )
{
    const RDIBuffer &buf = _buffers.getRef( bufObj );
    ASSERT( offset + size <= buf.size );

    glBindBuffer( buf.type, buf.glObj );

    if( offset == 0 &&  size == buf.size )
    {
        // Replacing the whole buffer can help the driver to avoid pipeline stalls
        glBufferData( buf.type, size, data, GL_DYNAMIC_DRAW );
        return;
    }

    glBufferSubData( buf.type, offset, size, data );
}


// =================================================================================================
// Textures
// =================================================================================================

uint32 OpenGLESRenderDevice::calcTextureSize( TextureFormats::List format, int width, int height, int depth )
{
    switch( format )
    {
    case TextureFormats::BGRA8:
        return width * height * depth * 4;
    case TextureFormats::DXT1:
        return std::max( width / 4, 1 ) * std::max( height / 4, 1 ) * depth * 8;
    case TextureFormats::DXT3:
        return std::max( width / 4, 1 ) * std::max( height / 4, 1 ) * depth * 16;
    case TextureFormats::DXT5:
        return std::max( width / 4, 1 ) * std::max( height / 4, 1 ) * depth * 16;
    case TextureFormats::RGBA16F:
        return width * height * depth * 8;
    case TextureFormats::RGBA32F:
        return width * height * depth * 16;
    case TextureFormats::PVRTC_2BPP:
    case TextureFormats::PVRTC_A2BPP:
    case TextureFormats::PVRTC2_2BPP:
    case TextureFormats::PVRTC2_A2BPP:
        return (std::max( width, 16 ) * std::max( height, 8 ) * 2 + 7) / 8;
    case TextureFormats::PVRTC_4BPP:
    case TextureFormats::PVRTC_A4BPP:
    case TextureFormats::PVRTC2_4BPP:
    case TextureFormats::PVRTC2_A4BPP:
        return (std::max( width, 8 ) * std::max( height, 8 ) * 4 + 7) / 8;
    case TextureFormats::ETC:
        return (std::max( width + 3, 2 ) * std::max( height + 3, 2 )) * 8; // wonder if this is correct?
    case TextureFormats::RGB5_A1:
        return width * height * depth * 2;
    case TextureFormats::RGB565:
        return width * height * depth * 2;
    case TextureFormats::RGBA4:
        return width * height * depth * 2;
    default:
        return 0;
    }
}


uint32 OpenGLESRenderDevice::createTexture( TextureTypes::List type, int width, int height, int depth,
                                            TextureFormats::List format,
                                            bool hasMips, bool genMips, bool compress, bool sRGB )
{
    if( !_caps.texNPOT )
    {
        // Check if texture is NPOT
        if( (width & (width-1)) != 0 || (height & (height-1)) != 0 )
            Modules::log().writeWarning( "Texture has non-power-of-two dimensions although NPOT is not supported by GPU" );
    }

    RDITexture tex;
    tex.type = type;
    tex.format = format;
    tex.width = width;
    tex.height = height;
    if( type == TextureTypes::Tex3D && !_caps.tex3D )
    {
        return 0;
        Modules::log().writeWarning( "3D textures are not supported on the GPU" );
    }
    tex.depth = (type == TextureTypes::Tex3D ? depth : 1);
    //      tex.sRGB = sRGB && Modules::config().sRGBLinearization;
    tex.sRGB = false;
    tex.genMips = genMips;
    tex.hasMips = hasMips;;

    switch( format )
    {
    case TextureFormats::BGRA8:
        if( _caps.texBGRA8 )
            tex.glFmt = GL_BGRA_EXT;
        else
            tex.glFmt = GL_RGBA;
        break;
    case TextureFormats::DXT1:
        //              tex.glFmt = tex.sRGB ? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT : GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        if( !_caps.texDXT1 || !_caps.texS3TC )
        {
            Modules::log().writeWarning( "Unsupported texture format: DXT1" );
            return 0;
        }
        tex.glFmt = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case TextureFormats::DXT3:
        if( !_caps.texS3TC )
        {
            Modules::log().writeWarning( "Unsupported texture format: DXT3" );
            return 0;
        }
        //            tex.glFmt = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case TextureFormats::DXT5:
        if( !_caps.texS3TC )
        {
            Modules::log().writeWarning( "Unsupported texture format: DXT5" );
            return 0;
        }
        //            tex.glFmt = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    case TextureFormats::RGBA16F:
        if( !_caps.texFloat )
        {
            Modules::log().writeWarning( "Unsupported texture format: RGBA16F" );
            return 0;
        }
        tex.glFmt = GL_RGBA;
        break;
    case TextureFormats::RGBA32F:
        if( !_caps.texFloat )
        {
            Modules::log().writeWarning( "Unsupported texture format: RGBA32F" );
            return 0;
        }
        tex.glFmt = GL_RGBA;
        break;
    case TextureFormats::DEPTH:
        if( !_caps.texDepth )
        {
            Modules::log().writeWarning( "Depth texture format is unsupported" );
            return 0;
        }
        tex.glFmt = GL_DEPTH_COMPONENT;
        break;
    case TextureFormats::RGBA4:
        tex.glFmt = GL_RGBA;
        break;
    case TextureFormats::RGB5_A1:
        tex.glFmt = GL_RGBA;
        break;
    case TextureFormats::RGB565:
        tex.glFmt = GL_RGB;
        break;
    case TextureFormats::PVRTC_2BPP:
        if( !_caps.texPVRTC )
        {
            Modules::log().writeWarning( "Unsupported texture format: PVRTC2" );
            return 0;
        }
        tex.glFmt = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
        break;
    case TextureFormats::PVRTC_4BPP:
        if( !_caps.texPVRTC )
        {
            Modules::log().writeWarning( "Unsupported texture format: PVRTC4" );
            return 0;
        }
        tex.glFmt = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
        break;
    case TextureFormats::PVRTC_A2BPP:
        if( !_caps.texPVRTC )
        {
            Modules::log().writeWarning( "Unsupported texture format: PVRTCA2" );
            return 0;
        }
        tex.glFmt = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
        break;
    case TextureFormats::PVRTC_A4BPP:
        if( !_caps.texPVRTC )
        {
            Modules::log().writeWarning( "Unsupported texture format: PVRTCA4" );
            return 0;
        }
        tex.glFmt = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
        break;
    case TextureFormats::ATC_RGB:
        if( !_caps.texATC )
        {
            Modules::log().writeWarning( "Unsupported texture format: ATC RGB" );
            return 0;
        }
        tex.glFmt = GL_ATC_RGB_AMD;
        break;
    case TextureFormats::ATC_RGBeA:
        if( !_caps.texATC )
        {
            Modules::log().writeWarning( "Unsupported texture format: ATC RGB explicit A" );
            return 0;
        }
        tex.glFmt = GL_ATC_RGBA_EXPLICIT_ALPHA_AMD;
        break;
    case TextureFormats::ATC_RGBiA:
        if( !_caps.texATC )
        {
            Modules::log().writeWarning( "Unsupported texture format: ATC RGB interpolated A" );
            return 0;
        }
        tex.glFmt = GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD;
        break;
    case TextureFormats::T3DC_X:
        if( !_caps.tex3DC )
        {
            Modules::log().writeWarning( "Unsupported texture format: 3DC X" );
            return 0;
        }
        tex.glFmt = GL_3DC_X_AMD;
        break;
    case TextureFormats::T3DC_XY:
        if( !_caps.tex3DC )
        {
            Modules::log().writeWarning( "Unsupported texture format: 3DC XY" );
            return 0;
        }
        tex.glFmt = GL_3DC_XY_AMD;
        break;
    case TextureFormats::ETC:
        if( !_caps.texETC )
        {
            Modules::log().writeWarning( "Unsupported texture format: ETC" );
            return 0;
        }
        tex.glFmt = GL_ETC1_RGB8_OES;
        break;
    case TextureFormats::PVRTC2_2BPP:
        if( !_caps.texPVRTC2 )
        {
            Modules::log().writeWarning( "Unsupported texture format: PVRTC2_2BPP" );
            return 0;
        }
        //            tex.glFmt = GL_COMPRESSED_RGB_PVRTC_2BPPV2_IMG;
        break;
    case TextureFormats::PVRTC2_4BPP:
        if( !_caps.texPVRTC2 )
        {
            Modules::log().writeWarning( "Unsupported texture format: PVRTC2_4BPP" );
            return 0;
        }
        //            tex.glFmt = GL_COMPRESSED_RGB_PVRTC_4BPPV2_IMG;
        break;
    case TextureFormats::PVRTC2_A2BPP:
        if( !_caps.texPVRTC2 )
        {
            Modules::log().writeWarning( "Unsupported texture format: PVRTC2_A2BPP" );
            return 0;
        }
        //            tex.glFmt = GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG;
        break;
    case TextureFormats::PVRTC2_A4BPP:
        if( !_caps.texPVRTC2 )
        {
            Modules::log().writeWarning( "Unsupported texture format: PVRTC2_A4BPP" );
            return 0;
        }
        //            tex.glFmt = GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG;
        break;
    default:
        ASSERT( 0 );
        break;
    };

    glGenTextures( 1, &tex.glObj );
    glActiveTexture( GL_TEXTURE7 );
    glBindTexture( tex.type, tex.glObj );

    //      float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    //      glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );

    tex.samplerState = 0;
    applySamplerState( tex );

    glBindTexture( tex.type, 0 );
    if( _texSlots[7].texObj )
        glBindTexture( _textures.getRef( _texSlots[7].texObj ).type, _textures.getRef( _texSlots[7].texObj ).glObj );

    // Calculate memory requirements
    tex.memSize = calcTextureSize( format, width, height, depth );
    if( hasMips || genMips ) tex.memSize += ftoi_r( tex.memSize * 1.0f / 3.0f );
    if( type == TextureTypes::TexCube ) tex.memSize *= 6;
    _textureMem += tex.memSize;

    return _textures.add( tex );
}


void OpenGLESRenderDevice::uploadTextureData( uint32 texObj, int slice, int mipLevel, const void *pixels )
{
    const RDITexture &tex = _textures.getRef( texObj );
    TextureFormats::List format = tex.format;

    glActiveTexture( GL_TEXTURE15 );
    glBindTexture( tex.type, tex.glObj );

    int inputFormat = GL_BGRA, inputType = GL_UNSIGNED_BYTE;
    bool compressed = (format == TextureFormats::DXT1) ||
            (format == TextureFormats::DXT3) ||
            (format == TextureFormats::DXT5) ||
            (format == TextureFormats::PVRTC_2BPP) ||
            (format == TextureFormats::PVRTC_4BPP) ||
            (format == TextureFormats::PVRTC_A2BPP) ||
            (format == TextureFormats::PVRTC_A4BPP) ||
            (format == TextureFormats::ATC_RGB) ||
            (format == TextureFormats::ATC_RGBeA) ||
            (format == TextureFormats::ATC_RGBiA) ||
            (format == TextureFormats::T3DC_X) ||
            (format == TextureFormats::T3DC_XY) ||
            (format == TextureFormats::ETC) ||
            (format == TextureFormats::PVRTC2_2BPP) ||
            (format == TextureFormats::PVRTC2_4BPP) ||
            (format == TextureFormats::PVRTC2_A2BPP) ||
            (format == TextureFormats::PVRTC2_A4BPP);

    switch( format )
    {
    case TextureFormats::RGBA16F:
        inputFormat = GL_RGBA;
        inputType = GL_HALF_FLOAT_OES;
        break;
    case TextureFormats::RGBA32F:
        inputFormat = GL_RGBA;
        inputType = GL_FLOAT;
        break;
    case TextureFormats::DEPTH:
        inputFormat = GL_DEPTH_COMPONENT;
        inputType = GL_UNSIGNED_INT;
    case TextureFormats::RGB5_A1:
        inputFormat = GL_RGBA;
        inputType = GL_UNSIGNED_SHORT_5_5_5_1;
        break;
    case TextureFormats::RGB565:
        inputFormat = GL_RGB;
        inputType = GL_UNSIGNED_SHORT_5_6_5;
        break;
    case TextureFormats::RGBA4:
        inputFormat = GL_RGBA;
        inputType = GL_UNSIGNED_SHORT_4_4_4_4;
        break;
    };

    // Calculate size of next mipmap using "floor" convention
    int width = std::max( tex.width >> mipLevel, 1 ), height = std::max( tex.height >> mipLevel, 1 );

    if( tex.type == TextureTypes::Tex2D || tex.type == TextureTypes::TexCube )
    {
        int target = (tex.type == TextureTypes::Tex2D) ?
                    GL_TEXTURE_2D : (GL_TEXTURE_CUBE_MAP_POSITIVE_X + slice);

        if( compressed )
            glCompressedTexImage2D( target, mipLevel, tex.glFmt, width, height, 0,
                                    calcTextureSize( format, width, height, 1 ), pixels );
        else
            glTexImage2D( target, mipLevel, tex.glFmt, width, height, 0, inputFormat, inputType, pixels );
    }
    else if( tex.type == TextureTypes::Tex3D && glExt::OES_texture_3D)
    {
        int depth = std::max( tex.depth >> mipLevel, 1 );

        if( compressed )
            glExt::glCompressedTexImage3DOES( GL_TEXTURE_3D_OES, mipLevel, tex.glFmt, width, height, depth, 0,
                                              calcTextureSize( format, width, height, depth ), pixels );
        else
            glExt::glTexImage3DOES( GL_TEXTURE_3D_OES, mipLevel, tex.glFmt, width, height, depth, 0,
                                    inputFormat, inputType, pixels );
    }

    if( tex.genMips && (tex.type != GL_TEXTURE_CUBE_MAP || slice == 5) )
    {
        // Note: for cube maps mips are only generated when the side with the highest index is uploaded
        //        glEnable( tex.type );  // Workaround for ATI driver bug
        glGenerateMipmap( tex.type );
        //        glDisable( tex.type );
    }

    glBindTexture( tex.type, 0 );
    if( _texSlots[15].texObj )
        glBindTexture( _textures.getRef( _texSlots[15].texObj ).type, _textures.getRef( _texSlots[15].texObj ).glObj );
}


void OpenGLESRenderDevice::destroyTexture( uint32 texObj )
{
    if( texObj == 0 ) return;

    const RDITexture &tex = _textures.getRef( texObj );
    glDeleteTextures( 1, &tex.glObj );

    _textureMem -= tex.memSize;
    _textures.remove( texObj );
}


void OpenGLESRenderDevice::updateTextureData( uint32 texObj, int slice, int mipLevel, const void *pixels )
{
    uploadTextureData( texObj, slice, mipLevel, pixels );
}


bool OpenGLESRenderDevice::getTextureData( uint32 texObj, int slice, int mipLevel, void *buffer )
{
    Modules::log().writeWarning( "getTextureData is not supported with OpenGL ES 2.0 renderer" );
    return false;
}


// =================================================================================================
// Shaders
// =================================================================================================

uint32 OpenGLESRenderDevice::createShaderProgram( const char *vertexShaderSrc, const char *fragmentShaderSrc )
{
    int infologLength = 0;
    int charsWritten = 0;
    char *infoLog = 0x0;
    int status;

    _shaderLog = "";

    // Vertex shader
    uint32 vs = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vs, 1, &vertexShaderSrc, 0x0 );
    glCompileShader( vs );
    glGetShaderiv( vs, GL_COMPILE_STATUS, &status );
    if( !status )
    {
        // Get info
        glGetShaderiv( vs, GL_INFO_LOG_LENGTH, &infologLength );
        if( infologLength > 1 )
        {
            infoLog = new char[infologLength];
            glGetShaderInfoLog( vs, infologLength, &charsWritten, infoLog );
            _shaderLog = _shaderLog + "[Vertex Shader]\n" + infoLog;
            delete[] infoLog; infoLog = 0x0;
        }

        glDeleteShader( vs );
        return 0;
    }

    // Fragment shader
    uint32 fs = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fs, 1, &fragmentShaderSrc, 0x0 );
    glCompileShader( fs );
    glGetShaderiv( fs, GL_COMPILE_STATUS, &status );
    if( !status )
    {
        glGetShaderiv( fs, GL_INFO_LOG_LENGTH, &infologLength );
        if( infologLength > 1 )
        {
            infoLog = new char[infologLength];
            glGetShaderInfoLog( fs, infologLength, &charsWritten, infoLog );
            _shaderLog = _shaderLog + "[Fragment Shader]\n" + infoLog;
            delete[] infoLog; infoLog = 0x0;
        }

        glDeleteShader( vs );
        glDeleteShader( fs );
        return 0;
    }

    // Shader program
    uint32 program = glCreateProgram();
    glAttachShader( program, vs );
    glAttachShader( program, fs );
    glDeleteShader( vs );
    glDeleteShader( fs );

    return program;
}


bool OpenGLESRenderDevice::linkShaderProgram( uint32 programObj )
{
    int infologLength = 0;
    int charsWritten = 0;
    char *infoLog = 0x0;
    int status;

    _shaderLog = "";

    glLinkProgram( programObj );
    glGetProgramiv( programObj, GL_INFO_LOG_LENGTH, &infologLength );
    if( infologLength > 1 )
    {
        infoLog = new char[infologLength];
        glGetProgramInfoLog( programObj, infologLength, &charsWritten, infoLog );
        _shaderLog = _shaderLog + "[Linking]\n" + infoLog;
        delete[] infoLog; infoLog = 0x0;
    }

    glGetProgramiv( programObj, GL_LINK_STATUS, &status );
    if( !status ) return false;

    return true;
}


uint32 OpenGLESRenderDevice::createShader( const char *vertexShaderSrc, const char *fragmentShaderSrc )
{
    // Compile and link shader
    uint32 programObj = createShaderProgram( vertexShaderSrc, fragmentShaderSrc );
    if( programObj == 0 ) return 0;
    if( !linkShaderProgram( programObj ) ) return 0;

    uint32 shaderId = _shaders.add( RDIShader() );
    RDIShader &shader = _shaders.getRef( shaderId );
    shader.oglProgramObj = programObj;

    int attribCount;
    glGetProgramiv( programObj, GL_ACTIVE_ATTRIBUTES, &attribCount );

    for( uint32 i = 0; i < _numVertexLayouts; ++i )
    {
        RDIVertexLayout &vl = _vertexLayouts[i];
        bool allAttribsFound = true;

        for( uint32 j = 0; j < 16; ++j )
            shader.inputLayouts[i].attribIndices[j] = -1;

        for( int j = 0; j < attribCount; ++j )
        {
            char name[32];
            uint32 size, type;
            glGetActiveAttrib( programObj, j, 32, 0x0, (int *)&size, &type, name );

            bool attribFound = false;
            for( uint32 k = 0; k < vl.numAttribs; ++k )
            {
                if( vl.attribs[k].semanticName.compare(name) == 0 )
                {
                    shader.inputLayouts[i].attribIndices[k] = glGetAttribLocation( programObj, name );
                    attribFound = true;
                }
            }

            if( !attribFound )
            {
                allAttribsFound = false;
                break;
            }
        }

        shader.inputLayouts[i].valid = allAttribsFound;
    }

    return shaderId;
}


void OpenGLESRenderDevice::destroyShader( uint32 shaderId )
{
    if( shaderId == 0 ) return;

    RDIShader &shader = _shaders.getRef( shaderId );
    glDeleteProgram( shader.oglProgramObj );
    _shaders.remove( shaderId );
}


void OpenGLESRenderDevice::bindShader( uint32 shaderId )
{
    if( shaderId != 0 )
    {
        RDIShader &shader = _shaders.getRef( shaderId );
        glUseProgram( shader.oglProgramObj );
    }
    else
    {
        glUseProgram( 0 );
    }

    _curShaderId = shaderId;
    _pendingMask |= PM_VERTLAYOUT;
} 


int OpenGLESRenderDevice::getShaderConstLoc( uint32 shaderId, const char *name )
{
    RDIShader &shader = _shaders.getRef( shaderId );
    return glGetUniformLocation( shader.oglProgramObj, name );
}


int OpenGLESRenderDevice::getShaderSamplerLoc( uint32 shaderId, const char *name )
{
    RDIShader &shader = _shaders.getRef( shaderId );
    return glGetUniformLocation( shader.oglProgramObj, name );
}


void OpenGLESRenderDevice::setShaderConst( int loc, RDIShaderConstType type, void *values, uint32 count )
{
    switch( type )
    {
    case CONST_FLOAT:
        glUniform1fv( loc, count, (float *)values );
        break;
    case CONST_FLOAT2:
        glUniform2fv( loc, count, (float *)values );
        break;
    case CONST_FLOAT3:
        glUniform3fv( loc, count, (float *)values );
        break;
    case CONST_FLOAT4:
        glUniform4fv( loc, count, (float *)values );
        break;
    case CONST_FLOAT44:
        glUniformMatrix4fv( loc, count, false, (float *)values );
        break;
    case CONST_FLOAT33:
        glUniformMatrix3fv( loc, count, false, (float *)values );
        break;
    }
}


void OpenGLESRenderDevice::setShaderSampler( int loc, uint32 texUnit )
{
    glUniform1i( loc, (int)texUnit );
}


// =================================================================================================
// Renderbuffers
// =================================================================================================

uint32 OpenGLESRenderDevice::createRenderBuffer( uint32 width, uint32 height, TextureFormats::List format,
                                                 bool depth, uint32 numColBufs, uint32 samples )
{
    if( (format == TextureFormats::RGBA16F || format == TextureFormats::RGBA32F) && !_caps.texFloat )
    {
        return 0;
    }

    if( numColBufs > RDIRenderBuffer::MaxColorAttachmentCount ) return 0;

    uint32 maxSamples = 0;
    if( _caps.rtMultisampling )
    {
        GLint value;
        if (glExt::ANGLE_framebuffer_multisample)
            glGetIntegerv( GL_MAX_SAMPLES_ANGLE, &value );
        else if (glExt::APPLE_framebuffer_multisample)
            glGetIntegerv( GL_MAX_SAMPLES_APPLE, &value );
        else if (glExt::IMG_multisampled_render_to_texture)
            glGetIntegerv( GL_MAX_SAMPLES_IMG, &value );

        maxSamples = (uint32)value;
    }
    if( samples > maxSamples )
    {
        samples = maxSamples;
        Modules::log().writeWarning( "GPU does not support desired multisampling quality for render target" );
    }

    RDIRenderBuffer rb;
    rb.width = width;
    rb.height = height;
    rb.samples = samples;

    // Create framebuffers
    glGenFramebuffers( 1, &rb.fbo );
    if( samples > 0 ) glGenFramebuffers( 1, &rb.fboMS );

    if( numColBufs > 0 )
    {
        // Attach color buffers
        for( uint32 j = 0; j < numColBufs; ++j )
        {
            glBindFramebuffer( GL_FRAMEBUFFER, rb.fbo );
            // Create a color texture
            uint32 texObj = createTexture( TextureTypes::Tex2D, rb.width, rb.height, 1, format, false, false, false, false );
            ASSERT( texObj != 0 );
            uploadTextureData( texObj, 0, 0, 0x0 );
            rb.colTexs[j] = texObj;
            RDITexture &tex = _textures.getRef( texObj );
            // Attach the texture
            glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + j, GL_TEXTURE_2D, tex.glObj, 0 );

            if( samples > 0 )
            {
                glBindFramebuffer( GL_FRAMEBUFFER, rb.fboMS );
                // Create a multisampled renderbuffer
                glGenRenderbuffers( 1, &rb.colBufs[j] );
                glBindRenderbuffer( GL_RENDERBUFFER, rb.colBufs[j] );
                if (glExt::ANGLE_framebuffer_multisample)
                    glExt::glRenderbufferStorageMultisampleANGLE( GL_RENDERBUFFER, rb.samples, tex.glFmt, rb.width, rb.height );
                else if (glExt::APPLE_framebuffer_multisample)
                    glExt::glRenderbufferStorageMultisampleAPPLE( GL_RENDERBUFFER, rb.samples, tex.glFmt, rb.width, rb.height );
                else if (glExt::IMG_multisampled_render_to_texture)
                    glExt::glRenderbufferStorageMultisampleIMG( GL_RENDERBUFFER, rb.samples, tex.glFmt, rb.width, rb.height );
                // Attach the renderbuffer
                glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + j,
                                           GL_RENDERBUFFER, rb.colBufs[j] );
            }
        }

        //        uint32 buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1_EXT,
        //                             GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT };
        glBindFramebuffer( GL_FRAMEBUFFER, rb.fbo );
        //        glDrawBuffers( numColBufs, buffers );

        if( samples > 0 )
        {
            glBindFramebuffer( GL_FRAMEBUFFER, rb.fboMS );
            //            glDrawBuffers( numColBufs, buffers );
        }
    }
    else
    {
        glBindFramebuffer( GL_FRAMEBUFFER, rb.fbo );
        //        glDrawBuffer( GL_NONE );
        //        glReadBuffer( GL_NONE );

        if( samples > 0 )
        {
            glBindFramebuffer( GL_FRAMEBUFFER, rb.fboMS );
            //            glDrawBuffer( GL_NONE );
            //            glReadBuffer( GL_NONE );
        }
    }

    // Attach depth buffer
    if( depth )
    {
        glBindFramebuffer( GL_FRAMEBUFFER, rb.fbo );
        // Create a depth texture
        uint32 texObj = createTexture( TextureTypes::Tex2D, rb.width, rb.height, 1, TextureFormats::DEPTH, false, false, false, false );
        ASSERT( texObj != 0 );
        //        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );
        uploadTextureData( texObj, 0, 0, 0x0 );
        rb.depthTex = texObj;
        RDITexture &tex = _textures.getRef( texObj );
        // Attach the texture
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex.glObj, 0 );

        if( samples > 0 )
        {
            glBindFramebuffer( GL_FRAMEBUFFER, rb.fboMS );
            // Create a multisampled renderbuffer
            glGenRenderbuffers( 1, &rb.depthBuf );
            glBindRenderbuffer( GL_RENDERBUFFER, rb.depthBuf );
            if (glExt::ANGLE_framebuffer_multisample)
                glExt::glRenderbufferStorageMultisampleANGLE( GL_RENDERBUFFER, rb.samples, _depthFormat, rb.width, rb.height );
            else if (glExt::APPLE_framebuffer_multisample)
                glExt::glRenderbufferStorageMultisampleAPPLE( GL_RENDERBUFFER, rb.samples, _depthFormat, rb.width, rb.height );
            else if (glExt::IMG_multisampled_render_to_texture)
                glExt::glRenderbufferStorageMultisampleIMG( GL_RENDERBUFFER, rb.samples, _depthFormat, rb.width, rb.height );
            // Attach the renderbuffer
            glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                       GL_RENDERBUFFER, rb.depthBuf );
        }
    }

    uint32 rbObj = _rendBufs.add( rb );

    // Check if FBO is complete
    bool valid = true;
    glBindFramebuffer( GL_FRAMEBUFFER, rb.fbo );
    uint32 status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
    glBindFramebuffer( GL_FRAMEBUFFER, _defaultFBO );
    if( status != GL_FRAMEBUFFER_COMPLETE ) valid = false;

    if( samples > 0 )
    {
        glBindFramebuffer( GL_FRAMEBUFFER, rb.fboMS );
        status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
        glBindFramebuffer( GL_FRAMEBUFFER, _defaultFBO );
        if( status != GL_FRAMEBUFFER_COMPLETE ) valid = false;
    }

    if( !valid )
    {
        destroyRenderBuffer( rbObj );
        return 0;
    }

    return rbObj;
}


void OpenGLESRenderDevice::destroyRenderBuffer( uint32 rbObj )
{
    RDIRenderBuffer &rb = _rendBufs.getRef( rbObj );

    glBindFramebuffer( GL_FRAMEBUFFER, _defaultFBO );

    if( rb.depthTex != 0 ) destroyTexture( rb.depthTex );
    if( rb.depthBuf != 0 ) glDeleteRenderbuffers( 1, &rb.depthBuf );
    rb.depthTex = rb.depthBuf = 0;

    for( uint32 i = 0; i < RDIRenderBuffer::MaxColorAttachmentCount; ++i )
    {
        if( rb.colTexs[i] != 0 ) destroyTexture( rb.colTexs[i] );
        if( rb.colBufs[i] != 0 ) glDeleteRenderbuffers( 1, &rb.colBufs[i] );
        rb.colTexs[i] = rb.colBufs[i] = 0;
    }

    if( rb.fbo != 0 ) glDeleteFramebuffers( 1, &rb.fbo );
    if( rb.fboMS != 0 ) glDeleteFramebuffers( 1, &rb.fboMS );
    rb.fbo = rb.fboMS = 0;

    _rendBufs.remove( rbObj );
}


uint32 OpenGLESRenderDevice::getRenderBufferTex( uint32 rbObj, uint32 bufIndex )
{
    RDIRenderBuffer &rb = _rendBufs.getRef( rbObj );

    if( bufIndex < RDIRenderBuffer::MaxColorAttachmentCount ) return rb.colTexs[bufIndex];
    else if( bufIndex == 32 ) return rb.depthTex;
    else return 0;
}


void OpenGLESRenderDevice::resolveRenderBuffer( uint32 rbObj )
{
    RDIRenderBuffer &rb = _rendBufs.getRef( rbObj );

    if( rb.fboMS == 0 ) return;

    //    glBindFramebuffer( GL_READ_FRAMEBUFFER_EXT, rb.fboMS );
    //    glBindFramebuffer( GL_DRAW_FRAMEBUFFER_EXT, rb.fbo );

    bool depthResolved = false;
    for( uint32 i = 0; i < RDIRenderBuffer::MaxColorAttachmentCount; ++i )
    {
        if( rb.colBufs[i] != 0 )
        {
            //			glReadBuffer( GL_COLOR_ATTACHMENT0_EXT + i );
            //			glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + i );

            int mask = GL_COLOR_BUFFER_BIT;
            if( !depthResolved && rb.depthBuf != 0 )
            {
                mask |= GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
                depthResolved = true;
            }
            //			glBlitFramebufferEXT( 0, 0, rb.width, rb.height, 0, 0, rb.width, rb.height, mask, GL_NEAREST );
        }
    }

    if( !depthResolved && rb.depthBuf != 0 )
    {
        //		glReadBuffer( GL_NONE );
        //		glDrawBuffer( GL_NONE );
        //		glBlitFramebufferEXT( 0, 0, rb.width, rb.height, 0, 0, rb.width, rb.height,
        //							  GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST );
    }

    //    glBindFramebuffer( GL_READ_FRAMEBUFFER_EXT, _defaultFBO );
    //    glBindFramebuffer( GL_DRAW_FRAMEBUFFER_EXT, _defaultFBO );
}


void OpenGLESRenderDevice::setRenderBuffer( uint32 rbObj )
{
    // Resolve render buffer if necessary
    if( _curRendBuf != 0 ) resolveRenderBuffer( _curRendBuf );

    // Set new render buffer
    _curRendBuf = rbObj;

    if( rbObj == 0 )
    {
        glBindFramebuffer( GL_FRAMEBUFFER, _defaultFBO );
        //		if( _defaultFBO == 0 ) glDrawBuffer( _outputBufferIndex == 1 ? GL_BACK_RIGHT : GL_BACK_LEFT );
        _fbWidth = _vpWidth + _vpX;
        _fbHeight = _vpHeight + _vpY;
        //		glDisable( GL_MULTISAMPLE );
    }
    else
    {
        // Unbind all textures to make sure that no FBO attachment is bound any more
        for( uint32 i = 0; i < 16; ++i ) setTexture( i, 0, 0 );
        commitStates( PM_TEXTURES );

        RDIRenderBuffer &rb = _rendBufs.getRef( rbObj );

        glBindFramebuffer( GL_FRAMEBUFFER, rb.fboMS != 0 ? rb.fboMS : rb.fbo );
        //        ASSERT( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE_EXT );
        _fbWidth = rb.width;
        _fbHeight = rb.height;

        //		if( rb.fboMS != 0 ) glEnable( GL_MULTISAMPLE );
        //		else glDisable( GL_MULTISAMPLE );
    }
}


bool OpenGLESRenderDevice::getRenderBufferData( uint32 rbObj, int bufIndex, int *width, int *height,
                                                int *compCount, void *dataBuffer, int bufferSize )
{
    int x, y, w, h;
    int format = GL_RGBA;
    int type = GL_FLOAT;
    beginRendering();
    glPixelStorei( GL_PACK_ALIGNMENT, 4 );

    if( rbObj == 0 )
    {
        if( bufIndex != 32 && bufIndex != 0 ) return false;
        if( width != 0x0 ) *width = _vpWidth;
        if( height != 0x0 ) *height = _vpHeight;

        x = _vpX; y = _vpY; w = _vpWidth; h = _vpHeight;

        glBindFramebuffer( GL_FRAMEBUFFER, _defaultFBO );
        //		if( bufIndex != 32 ) glReadBuffer( GL_BACK_LEFT );
        //format = GL_BGRA;
        //type = GL_UNSIGNED_BYTE;
    }
    else
    {
        resolveRenderBuffer( rbObj );
        RDIRenderBuffer &rb = _rendBufs.getRef( rbObj );

        if( bufIndex == 32 && rb.depthTex == 0 ) return false;
        if( bufIndex != 32 )
        {
            if( (unsigned)bufIndex >= RDIRenderBuffer::MaxColorAttachmentCount || rb.colTexs[bufIndex] == 0 )
                return false;
        }
        if( width != 0x0 ) *width = rb.width;
        if( height != 0x0 ) *height = rb.height;

        x = 0; y = 0; w = rb.width; h = rb.height;

        glBindFramebuffer( GL_FRAMEBUFFER, rb.fbo );
        //		if( bufIndex != 32 ) glReadBuffer( GL_COLOR_ATTACHMENT0_EXT + bufIndex );
    }

    if( bufIndex == 32 )
    {
        format = GL_DEPTH_COMPONENT;
        type = GL_FLOAT;
    }

    int comps = (bufIndex == 32 ? 1 : 4);
    if( compCount != 0x0 ) *compCount = comps;

    bool retVal = false;
    if( dataBuffer != 0x0 &&
            bufferSize >= w * h * comps * (type == GL_FLOAT ? 4 : 1) )
    {
        glFinish();
        glReadPixels( x, y, w, h, format, type, dataBuffer );
        retVal = true;
    }
    glBindFramebuffer( GL_FRAMEBUFFER, _defaultFBO );

    return retVal;
}


// =================================================================================================
// Queries
// =================================================================================================

uint32 OpenGLESRenderDevice::createOcclusionQuery()
{
    uint32 queryObj = 0;
    //	glGenQueries( 1, &queryObj );
    return queryObj;
}


void OpenGLESRenderDevice::destroyQuery( uint32 queryObj )
{
    if( queryObj == 0 ) return;

    //	glDeleteQueries( 1, &queryObj );
}


void OpenGLESRenderDevice::beginQuery( uint32 queryObj )
{
    //	glBeginQuery( GL_SAMPLES_PASSED, queryObj );
}


void OpenGLESRenderDevice::endQuery( uint32 /*queryObj*/ )
{
    //	glEndQuery( GL_SAMPLES_PASSED );
}


uint32 OpenGLESRenderDevice::getQueryResult( uint32 queryObj )
{
    uint32 samples = 0;
    //	glGetQueryObjectuiv( queryObj, GL_QUERY_RESULT, &samples );
    return samples;
}


// =================================================================================================
// Internal state management
// =================================================================================================

void OpenGLESRenderDevice::checkGLError()
{
    uint32 error = glGetError();
    ASSERT( error != GL_INVALID_ENUM );
    ASSERT( error != GL_INVALID_VALUE );
    ASSERT( error != GL_INVALID_OPERATION );
    ASSERT( error != GL_OUT_OF_MEMORY );
    //	ASSERT( error != GL_STACK_OVERFLOW && error != GL_STACK_UNDERFLOW );
}


bool OpenGLESRenderDevice::applyVertexLayout()
{
    if( _newVertLayout == 0 || _curShaderId == 0 ) return false;

    RDIVertexLayout &vl = _vertexLayouts[_newVertLayout - 1];
    RDIShader &shader = _shaders.getRef( _curShaderId );
    RDIInputLayout &inputLayout = shader.inputLayouts[_newVertLayout - 1];

    if( !inputLayout.valid )
        return false;

    // Set vertex attrib pointers
    uint32 newVertexAttribMask = 0;
    for( uint32 i = 0; i < vl.numAttribs; ++i )
    {
        int8 attribIndex = inputLayout.attribIndices[i];
        if( attribIndex >= 0 )
        {
            VertexLayoutAttrib &attrib = vl.attribs[i];
            const RDIVertBufSlot &vbSlot = _vertBufSlots[attrib.vbSlot];

            ASSERT( _buffers.getRef( _vertBufSlots[attrib.vbSlot].vbObj ).glObj != 0 &&
                    _buffers.getRef( _vertBufSlots[attrib.vbSlot].vbObj ).type == GL_ARRAY_BUFFER );

            glBindBuffer( GL_ARRAY_BUFFER, _buffers.getRef( _vertBufSlots[attrib.vbSlot].vbObj ).glObj );
            glVertexAttribPointer( attribIndex, attrib.size, GL_FLOAT, GL_FALSE,
                                   vbSlot.stride, (char *)0 + vbSlot.offset + attrib.offset );

            newVertexAttribMask |= 1 << attribIndex;
        }
    }

    for( uint32 i = 0; i < 16; ++i )
    {
        uint32 curBit = 1 << i;
        if( (newVertexAttribMask & curBit) != (_activeVertexAttribsMask & curBit) )
        {
            if( newVertexAttribMask & curBit ) glEnableVertexAttribArray( i );
            else glDisableVertexAttribArray( i );
        }
    }
    _activeVertexAttribsMask = newVertexAttribMask;

    return true;
}


void OpenGLESRenderDevice::applySamplerState( RDITexture &tex )
{
    uint32 state = tex.samplerState;
    uint32 target = tex.type;

    const uint32 magFilters[] = { GL_LINEAR, GL_LINEAR, GL_NEAREST };
    const uint32 minFiltersMips[] = { GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_NEAREST };
    const uint32 maxAniso[] = { 1, 2, 4, 0, 8, 0, 0, 0, 16 };
    const uint32 wrapModes[] = { GL_CLAMP_TO_EDGE, GL_REPEAT, GL_CLAMP_TO_EDGE };

    if( tex.hasMips )
        glTexParameteri( target, GL_TEXTURE_MIN_FILTER, minFiltersMips[(state & SS_FILTER_MASK) >> SS_FILTER_START] );
    else
        glTexParameteri( target, GL_TEXTURE_MIN_FILTER, magFilters[(state & SS_FILTER_MASK) >> SS_FILTER_START] );

    glTexParameteri( target, GL_TEXTURE_MAG_FILTER, magFilters[(state & SS_FILTER_MASK) >> SS_FILTER_START] );

    if (glExt::EXT_texture_filter_anisotropic)
        glTexParameteri( target, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso[(state & SS_ANISO_MASK) >> SS_ANISO_START] );

    glTexParameteri( target, GL_TEXTURE_WRAP_S, wrapModes[(state & SS_ADDRU_MASK) >> SS_ADDRU_START] );
    glTexParameteri( target, GL_TEXTURE_WRAP_T, wrapModes[(state & SS_ADDRV_MASK) >> SS_ADDRV_START] );
    if (glExt::OES_texture_3D)
        glTexParameteri( target, GL_TEXTURE_WRAP_R_OES, wrapModes[(state & SS_ADDRW_MASK) >> SS_ADDRW_START] );

    if( !(state & SS_COMP_LEQUAL) )
    {
        //		glTexParameteri( target, GL_TEXTURE_COMPARE_MODE, GL_NONE );
    }
    else
    {
        //		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
        //		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
    }
}


bool OpenGLESRenderDevice::commitStates( uint32 filter )
{
    if( _pendingMask & filter )
    {
        uint32 mask = _pendingMask & filter;

        // Set viewport
        if( mask & PM_VIEWPORT )
        {
            glViewport( _vpX, _vpY, _vpWidth, _vpHeight );
            _pendingMask &= ~PM_VIEWPORT;
        }

        // Set scissor rect
        if( mask & PM_SCISSOR )
        {
            glScissor( _scX, _scY, _scWidth, _scHeight );
            _pendingMask &= ~PM_SCISSOR;
        }

        // Bind index buffer
        if( mask & PM_INDEXBUF )
        {
            if( _newIndexBuf != _curIndexBuf )
            {
                if( _newIndexBuf != 0 )
                    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _buffers.getRef( _newIndexBuf ).glObj );
                else
                    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

                _curIndexBuf = _newIndexBuf;
                _pendingMask &= ~PM_INDEXBUF;
            }
        }

        // Bind vertex buffers
        if( mask & PM_VERTLAYOUT )
        {
            //if( _newVertLayout != _curVertLayout || _curShader != _prevShader )
            {
                if( !applyVertexLayout() )
                    return false;
                _curVertLayout = _newVertLayout;
                _prevShaderId = _curShaderId;
                _pendingMask &= ~PM_VERTLAYOUT;
            }
        }

        // Bind textures and set sampler state
        if( mask & PM_TEXTURES )
        {
            for( uint32 i = 0; i < 16; ++i )
            {
                glActiveTexture( GL_TEXTURE0 + i );

                if( _texSlots[i].texObj != 0 )
                {
                    RDITexture &tex = _textures.getRef( _texSlots[i].texObj );
                    glBindTexture( tex.type, tex.glObj );

                    // Apply sampler state
                    if( tex.samplerState != _texSlots[i].samplerState )
                    {
                        tex.samplerState = _texSlots[i].samplerState;
                        applySamplerState( tex );
                    }
                }
                else
                {
                    glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
                    if (glExt::OES_texture_3D)
                        glBindTexture( GL_TEXTURE_3D_OES, 0 );
                    glBindTexture( GL_TEXTURE_2D, 0 );
                }
            }

            _pendingMask &= ~PM_TEXTURES;
        }

        CHECK_GL_ERROR
    }

    return true;
}


void OpenGLESRenderDevice::resetStates()
{
    static int maxVertexAttribs = 0;
    if( maxVertexAttribs == 0 )
        glGetIntegerv( GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs );

    _curIndexBuf = 1; _newIndexBuf = 0;
    _curVertLayout = 1; _newVertLayout = 0;

    for( uint32 i = 0; i < 16; ++i )
        setTexture( i, 0, 0 );

    _activeVertexAttribsMask = 0;

    for( uint32 i = 0; i < (uint32)maxVertexAttribs; ++i )
        glDisableVertexAttribArray( i );

    glBindFramebuffer( GL_FRAMEBUFFER, _defaultFBO );

    _pendingMask = 0xFFFFFFFF;
    commitStates();
}


// =================================================================================================
// Draw calls and clears
// =================================================================================================

void OpenGLESRenderDevice::clear( uint32 flags, float *colorRGBA, float depth )
{
    uint32 mask = 0;

    if( flags & CLR_DEPTH )
    {
        mask |= GL_DEPTH_BUFFER_BIT;
        glClearDepthf( depth );
    }
    if( flags & CLR_COLOR )
    {
        mask |= GL_COLOR_BUFFER_BIT;
        if( colorRGBA ) glClearColor( colorRGBA[0], colorRGBA[1], colorRGBA[2], colorRGBA[3] );
        else glClearColor( 0, 0, 0, 0 );
    }

    if( mask )
    {
        commitStates( PM_VIEWPORT | PM_SCISSOR );
        glClear( mask );
    }

    CHECK_GL_ERROR
}


void OpenGLESRenderDevice::draw( RDIPrimType primType, uint32 firstVert, uint32 numVerts )
{
    if( commitStates() )
    {
        glDrawArrays( (uint32)primType, firstVert, numVerts );
    }

    CHECK_GL_ERROR
}


void OpenGLESRenderDevice::drawIndexed( RDIPrimType primType, uint32 firstIndex, uint32 numIndices,
                                        uint32 firstVert, uint32 numVerts )
{
    if( commitStates() )
    {
        firstIndex *= (_indexFormat == IDXFMT_16) ? sizeof( short ) : sizeof( int );
        glDrawElements( (uint32)primType, numIndices, _indexFormat, (char *)0 + firstIndex );
    }

    CHECK_GL_ERROR
}

}  // namespace
