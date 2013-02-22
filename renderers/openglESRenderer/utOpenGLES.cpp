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

#define _CRT_SECURE_NO_WARNINGS
#include "utOpenGLES.h"
#include <cstdlib>
#include <cstring>


namespace glExt
{
    // Generic
    bool OES_compressed_ETC1_RGB8_texture = false;
    bool OES_compressed_paletted_texture = false;
    bool OES_depth24 = false;
    bool OES_depth32 = false;
    bool OES_depth_texture = false;
    bool OES_EGL_image = false;
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = 0x0;
    PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC glEGLImageTargetRenderbufferStorageOES = 0x0;
    bool OES_EGL_image_external = false;
    bool OES_element_index_uint = false;
    bool OES_get_program_binary = false;
    PFNGLGETPROGRAMBINARYOESPROC glGetProgramBinaryOES = 0x0;
    PFNGLPROGRAMBINARYOESPROC glProgramBinaryOES = 0x0;
    bool OES_mapbuffer = false;
    PFNGLMAPBUFFEROESPROC glMapBufferOES = 0x0;
    PFNGLUNMAPBUFFEROESPROC glUnmapBufferOES = 0x0;
    PFNGLGETBUFFERPOINTERVOESPROC glGetBufferPointervOES = 0x0;
    bool OES_packed_depth_stencil = false;
    bool OES_rgb8_rgba8 = false;
    bool OES_standard_derivatives = false;
    bool OES_stencil1 = false;
    bool OES_stencil4 = false;
    bool OES_texture_3D = false;
    PFNGLTEXIMAGE3DOESPROC glTexImage3DOES = 0x0;
    PFNGLTEXSUBIMAGE3DOESPROC glTexSubImage3DOES = 0x0;
    PFNGLCOPYTEXSUBIMAGE3DOESPROC glCopyTexSubImage3DOES = 0x0;
    PFNGLCOMPRESSEDTEXIMAGE3DOESPROC glCompressedTexImage3DOES = 0x0;
    PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC glCompressedTexSubImage3DOES = 0x0;
    PFNGLFRAMEBUFFERTEXTURE3DOES glFramebufferTexture3DOES = 0x0;
    bool OES_texture_float = false;
    bool OES_texture_float_linear = false;
    bool OES_texture_half_float = false;
    bool OES_texture_half_float_linear = false;
    bool OES_texture_npot = false;
    bool OES_vertex_array_object = false;
    PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES = 0x0;
    PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES = 0x0;
    PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES = 0x0;
    PFNGLISVERTEXARRAYOESPROC glIsVertexArrayOES = 0x0;
    bool OES_vertex_type_10_10_10_2 = false;

    // Ext
    bool EXT_blend_minmax = false; //iOS SGX Supported
    bool EXT_discard_framebuffer = false; //iOS SGX Supported
    PFNGLDISCARDFRAMEBUFFEREXTPROC glDiscardFramebufferEXT = 0x0;
    bool EXT_shader_texture_lod = false; //iOS SGX Supported
    bool EXT_texture_filter_anisotropic = false; //iOS SGX Supported
    bool EXT_texture_compression_dxt1 = false;
    bool EXT_texture_format_BGRA8888 = false;
    bool EXT_texture_compression_s3tc = false;
    bool EXT_occlusion_query_boolean = false; //Newer iOS models supported (and Chrome/NaCL!)
    bool EXT_multisampled_render_to_texture = false;

    // IMG Extensions
    bool IMG_read_format = false; //iOS SGX Supported
     bool IMG_texture_compression_pvrtc = false; //iOS SGX Supported
     bool IMG_texture_stream = false;
     bool IMG_texture_compression_pvrtc2 = false;
     bool IMG_multisampled_render_to_texture = false;
     PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMG glRenderbufferStorageMultisampleIMG = 0x0;
     PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMG glFramebufferTexture2DMultisampleIMG = 0x0;

    // ANGLE Extensions (Chrome/NaCL)
     bool ANGLE_framebuffer_multisample = false;
     PFNGLRENDERBUFFERSTORAGEMULTISAMPLEANGLEPROC glRenderbufferStorageMultisampleANGLE = 0x0;
     bool ANGLE_framebuffer_blit = false;
     PFNGLBLITFRAMEBUFFERANGLEPROC glBlitFramebufferANGLE = 0x0;
     bool ANGLE_texture_compression_dxt5 = false;
     bool ANGLE_timer_query = false;
    //APPLE
     bool APPLE_framebuffer_multisample = false; //iOS SGX Supported
     PFNGLRENDERBUFFERSTORAGEMULTISAMPLEAPPLEPROC glRenderbufferStorageMultisampleAPPLE = 0x0;
     PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC glResolveMultisampleFramebufferAPPLE = 0x0;
     bool APPLE_texture_max_level = false; //iOS SGX Supported
     bool APPLE_rgb_422 = false; //iOS SGX Supported
     bool APPLE_texture_format_BGRA8888 = false; //iOS SGX Supported
    // AMD Extensions
     bool AMD_compressed_3DC_texture = false;
     bool AMD_compressed_ATC_texture = false;
    // NV Extensions
     bool NV_fbo_color_attachments = false; // Android Tegra 2 Supported
     bool NV_read_buffer = false;

    // ARB Extensions (ARB in ES 2.0 land? LOL Nvidia! But thank you for allowing MRTs!)
//       bool ARB_draw_buffers; // Android Tegra 2 Supported
     bool NV_draw_buffers = false;

    int	majorVersion = 2, minorVersion = 0;
}



bool isExtensionSupported( const char * const extension )
{
    // The recommended technique for querying OpenGL extensions;
    // from http://opengl.org/resources/features/OGLextensions/
    const GLubyte *extensions = NULL;
    const GLubyte *start;
    GLubyte *where, *terminator;

    /* Extension names should not have spaces. */
    where = (GLubyte *) strchr(extension, ' ');
    if (where || *extension == '\0')
        return 0;

    extensions = glGetString(GL_EXTENSIONS);

    /* It takes a bit of care to be fool-proof about parsing the
    OpenGL extensions string. Don't be fooled by sub-strings, etc. */
    start = extensions;
    for (;;) {
        where = (GLubyte *) strstr((const char *) start, extension);
        if (!where)
            break;
        terminator = where + strlen(extension);
        if (where == start || *(where - 1) == ' ')
            if (*terminator == ' ' || *terminator == '\0')
                return true;
        start = terminator;
    }

    return false;
}


void getOpenGLESVersion()
{
	char version[8];
	size_t len = strlen( (char *)glGetString( GL_VERSION ) );
	if( len >= 8 ) len = 7;
	
	strncpy( version, (char *)glGetString( GL_VERSION ), len );
	version[len] = '\0';

	char *pos1 = strtok( version, "." );
    if( pos1 && strlen(pos1) < len)
	{
		glExt::majorVersion = atoi( pos1 );
		char *pos2 = strtok( 0x0, ". " );
		if( pos2 ) glExt::minorVersion = atoi( pos2 );
	}
}


void *platGetProcAddress( const char *funcName )
{
#if defined( PLATFORM_WIN )
	return (void *)eglGetProcAddress( funcName );
#elif defined( PLATFORM_MAC )
	CFStringRef functionName = CFStringCreateWithCString( kCFAllocatorDefault, funcName, kCFStringEncodingASCII );
	CFURLRef bundleURL = CFURLCreateWithFileSystemPath(
		kCFAllocatorDefault, CFSTR( "/System/Library/Frameworks/OpenGL.framework" ), kCFURLPOSIXPathStyle, true );
	CFBundleRef bundle = CFBundleCreate( kCFAllocatorDefault, bundleURL );
   
	void *function = CFBundleGetFunctionPointerForName( bundle, functionName );
   
	CFRelease( bundle );
	CFRelease( bundleURL );
	CFRelease( functionName );
   
	return function; 
#elif defined( PLATFORM_LINUX )
    return (void *)eglGetProcAddress( funcName );
#else
	return (void *)glXGetProcAddressARB( (const GLubyte *)funcName );
#endif
}


bool initOpenGLExtensions()
{
	bool r = true;
	
    getOpenGLESVersion();
	
	// Extensions
    glExt::OES_compressed_ETC1_RGB8_texture = isExtensionSupported( "GL_OES_compressed_ETC1_RGB8_texture" );
    glExt::OES_compressed_paletted_texture = isExtensionSupported( "GL_OES_compressed_paletted_texture" );
    glExt::OES_depth24 = isExtensionSupported( "GL_OES_depth24" );
    glExt::OES_depth32 = isExtensionSupported( "GL_OES_depth32" );
    glExt::OES_depth_texture = isExtensionSupported( "GL_OES_depth_texture" );
    glExt::OES_EGL_image = isExtensionSupported( "GL_OES_EGL_image" );
    if (glExt::OES_EGL_image)
    {
        glExt::glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)platGetProcAddress("glEGLImageTargetTexture2DOES");
        glExt::glEGLImageTargetRenderbufferStorageOES = (PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC)platGetProcAddress("glEGLImageTargetRenderbufferStorageOES");
    }
    glExt::OES_EGL_image_external = isExtensionSupported( "GL_OES_EGL_image_external" );
    glExt::OES_element_index_uint = isExtensionSupported( "GL_OES_element_index_uint" );
    glExt::OES_get_program_binary = isExtensionSupported( "GL_OES_get_program_binary" );
    if (glExt::OES_get_program_binary)
    {
        glExt::glGetProgramBinaryOES = (PFNGLGETPROGRAMBINARYOESPROC)platGetProcAddress("glGetProgramBinaryOES");
        glExt::glProgramBinaryOES = (PFNGLPROGRAMBINARYOESPROC)platGetProcAddress("glProgramBinaryOES");
    }
    glExt::OES_mapbuffer = isExtensionSupported( "GL_OES_mapbuffer" );
    if (glExt::OES_mapbuffer)
    {
        glExt::glMapBufferOES = (PFNGLMAPBUFFEROESPROC)platGetProcAddress("glMapBufferOES");
        glExt::glUnmapBufferOES = (PFNGLUNMAPBUFFEROESPROC)platGetProcAddress("glUnmapBufferOES");
        glExt::glGetBufferPointervOES = (PFNGLGETBUFFERPOINTERVOESPROC)platGetProcAddress("glGetBufferPointervOES");
    }
    glExt::OES_packed_depth_stencil = isExtensionSupported( "GL_OES_packed_depth_stencil" );
    glExt::OES_rgb8_rgba8 = isExtensionSupported( "GL_OES_rgb8_rgba8" );
    glExt::OES_standard_derivatives = isExtensionSupported( "GL_OES_standard_derivatives" );
    glExt::OES_stencil1 = isExtensionSupported( "GL_OES_stencil1" );
    glExt::OES_stencil4 = isExtensionSupported( "GL_OES_stencil4" );
    glExt::OES_texture_3D = isExtensionSupported( "GL_OES_texture_3D" );
    if (glExt::OES_texture_3D)
    {
        glExt::glTexImage3DOES = (PFNGLTEXIMAGE3DOESPROC)platGetProcAddress("glTexImage3DOES");
        glExt::glTexSubImage3DOES = (PFNGLTEXSUBIMAGE3DOESPROC)platGetProcAddress("glTexSubImage3DOES");
        glExt::glCopyTexSubImage3DOES = (PFNGLCOPYTEXSUBIMAGE3DOESPROC)platGetProcAddress("glCopyTexSubImage3DOES");
        glExt::glCompressedTexImage3DOES = (PFNGLCOMPRESSEDTEXIMAGE3DOESPROC)platGetProcAddress("glCompressedTexImage3DOES");
        glExt::glCompressedTexSubImage3DOES = (PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC)platGetProcAddress("glCompressedTexSubImage3DOES");
        glExt::glFramebufferTexture3DOES = (PFNGLFRAMEBUFFERTEXTURE3DOES)platGetProcAddress("glFramebufferTexture3DOES");
    }
    glExt::OES_texture_float = isExtensionSupported( "GL_OES_texture_float" );
    glExt::OES_texture_float_linear = isExtensionSupported( "GL_OES_texture_float_linear" );
    glExt::OES_texture_half_float = isExtensionSupported( "GL_OES_texture_half_float" );
    glExt::OES_texture_half_float_linear = isExtensionSupported( "GL_OES_texture_half_float_linear" );
    glExt::OES_texture_npot = isExtensionSupported( "GL_OES_texture_npot" );
    glExt::OES_vertex_array_object = isExtensionSupported( "GL_OES_vertex_array_object" );
    if (glExt::OES_vertex_array_object)
    {
        glExt::glBindVertexArrayOES = (PFNGLBINDVERTEXARRAYOESPROC)platGetProcAddress("glBindVertexArrayOES");
        glExt::glDeleteVertexArraysOES = (PFNGLDELETEVERTEXARRAYSOESPROC)platGetProcAddress("glDeleteVertexArraysOES");
        glExt::glGenVertexArraysOES = (PFNGLGENVERTEXARRAYSOESPROC)platGetProcAddress("glGenVertexArraysOES");
        glExt::glIsVertexArrayOES = (PFNGLISVERTEXARRAYOESPROC)platGetProcAddress("glIsVertexArrayOES");
    }
    glExt::OES_vertex_type_10_10_10_2 = isExtensionSupported( "GL_OES_vertex_type_10_10_10_2" );

    // Ext
    glExt::EXT_blend_minmax = isExtensionSupported( "GL_EXT_blend_minmax" );
    glExt::EXT_discard_framebuffer = isExtensionSupported( "GL_EXT_discard_framebuffer" );
    if (glExt::EXT_discard_framebuffer)
    {
        glExt::glDiscardFramebufferEXT = (PFNGLDISCARDFRAMEBUFFEREXTPROC)platGetProcAddress("glDiscardFramebufferEXT");
    }
    glExt::EXT_shader_texture_lod = isExtensionSupported( "GL_EXT_shader_texture_lod" );
    glExt::EXT_texture_filter_anisotropic = isExtensionSupported( "GL_EXT_texture_filter_anisotropic" );
    glExt::EXT_texture_compression_dxt1 = isExtensionSupported( "GL_EXT_texture_compression_dxt1" );
    glExt::EXT_texture_format_BGRA8888 = isExtensionSupported( "GL_EXT_texture_format_BGRA8888" );
    glExt::EXT_texture_compression_s3tc = isExtensionSupported( "GL_EXT_texture_compression_s3tc" );
    glExt::EXT_occlusion_query_boolean = isExtensionSupported( "GL_EXT_occlusion_query_boolean" );
    glExt::EXT_multisampled_render_to_texture = isExtensionSupported( "GL_EXT_multisampled_render_to_texture" );

    // IMG Extensions
    glExt::IMG_read_format = isExtensionSupported( "GL_IMG_read_format" );
    glExt::IMG_texture_compression_pvrtc = isExtensionSupported( "GL_IMG_texture_compression_pvrtc" );
    glExt::IMG_texture_stream = isExtensionSupported( "GL_IMG_texture_stream" );
    glExt::IMG_texture_compression_pvrtc2 = isExtensionSupported( "GL_IMG_texture_compression_pvrtc2" );
    glExt::IMG_multisampled_render_to_texture = isExtensionSupported( "GL_IMG_multisampled_render_to_texture" );
    if (glExt::IMG_multisampled_render_to_texture)
    {
        glExt::glRenderbufferStorageMultisampleIMG = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMG)platGetProcAddress("glRenderbufferStorageMultisampleIMG");
        glExt::glFramebufferTexture2DMultisampleIMG = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMG)platGetProcAddress("glFramebufferTexture2DMultisampleIMG");
    }

    // ANGLE Extensions (Chrome/NaCL)
    glExt::ANGLE_framebuffer_multisample = isExtensionSupported( "GL_ANGLE_framebuffer_multisample" );
    if (glExt::ANGLE_framebuffer_multisample)
    {
        glExt::glRenderbufferStorageMultisampleANGLE = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEANGLEPROC)platGetProcAddress("glRenderbufferStorageMultisampleANGLE");
    }
    glExt::ANGLE_framebuffer_blit = isExtensionSupported( "GL_ANGLE_framebuffer_blit" );
    if (glExt::ANGLE_framebuffer_blit)
    {
       glExt:: glBlitFramebufferANGLE = (PFNGLBLITFRAMEBUFFERANGLEPROC)platGetProcAddress("glBlitFramebufferANGLE");
    }
    glExt::ANGLE_texture_compression_dxt5 = isExtensionSupported( "GL_ANGLE_texture_compression_dxt5" );
    glExt::ANGLE_timer_query = isExtensionSupported( "GL_ANGLE_timer_query" );
    //APPLE
    glExt::APPLE_framebuffer_multisample = isExtensionSupported( "GL_APPLE_framebuffer_multisample" );
    if (glExt::APPLE_framebuffer_multisample)
    {
        glExt::glRenderbufferStorageMultisampleAPPLE = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEAPPLEPROC)platGetProcAddress("glRenderbufferStorageMultisampleAPPLE");
        glExt::glResolveMultisampleFramebufferAPPLE = (PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC)platGetProcAddress("glResolveMultisampleFramebufferAPPLE");
    }
    glExt::APPLE_texture_max_level = isExtensionSupported( "GL_APPLE_texture_max_level" );
    glExt::APPLE_rgb_422 = isExtensionSupported( "GL_APPLE_rgb_422" ); //iOS SGX Supported
    glExt::APPLE_texture_format_BGRA8888 = isExtensionSupported( "GL_APPLE_texture_format_BGRA8888" );
    // AMD Extensions
    glExt::AMD_compressed_3DC_texture = isExtensionSupported( "GL_AMD_compressed_3DC_texture" );
    glExt::AMD_compressed_ATC_texture = isExtensionSupported( "GL_AMD_compressed_ATC_texture" );
    // NV Extensions
    glExt::NV_fbo_color_attachments = isExtensionSupported( "GL_NV_fbo_color_attachments" );
    glExt::NV_read_buffer = isExtensionSupported( "GL_NV_read_buffer" );
    glExt::NV_draw_buffers = isExtensionSupported( "GL_NV_draw_buffers" );

	return r;
}
