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

#ifndef _utOpenGL_H_
#define _utOpenGL_H_

#if defined( __gl2_h_ ) || defined( __GL2_H__ )
#   error gl2.h included before utOpenGLES.h
#endif

#include "utPlatform.h"

#if defined(BUILD_OGLES2)
    #if defined(PLATFORM_IOS)
        #import <OpenGLES/ES2/gl.h>
        #import <OpenGLES/ES2/glext.h>
    #elif defined(PLATFORM_MAC)	//OSX
        #include <GLES2/gl2.h>
        #include <GLES2/gl2ext.h>
        #include <GLES2/gl2extimg.h>
    #elif defined (PLATFORM_SYMBIAN)
        #include <EGL/egl.h>
        #include <GLES2/gl2.h>
        #include <GLES2/gl2ext.h>
//        #include <GLES2/gl2extimg.h>
    #else
        #if defined( PLATFORM_WIN ) || defined( PLATFORM_WIN_CE )
        #   define WIN32_LEAN_AND_MEAN 1
        #   ifndef NOMINMAX
        #       define NOMINMAX
        #   endif
        #endif
        #if !defined(EGL_NOT_PRESENT)
            #include <EGL/egl.h>
        #endif
        #include <GLES2/gl2.h>
        #include <GLES2/gl2ext.h>
//        #include <GLES2/gl2extimg.h>
    #endif
#elif defined(BUILD_OGLES3)
    #include <GLES3/gl3.h>
    #include <GLES2/gl2ext.h>
    #include <GLES3/gl3extimg.h>
#endif


namespace glExt
{
    // Generic
    extern bool OES_compressed_ETC1_RGB8_texture;
    extern bool OES_compressed_paletted_texture;
    extern bool OES_depth24;
    extern bool OES_depth32;
    extern bool OES_depth_texture;
    extern bool OES_EGL_image;
    extern PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;
    extern PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC glEGLImageTargetRenderbufferStorageOES;
    extern bool OES_EGL_image_external;
    extern bool OES_element_index_uint;
    extern bool OES_get_program_binary;
    extern PFNGLGETPROGRAMBINARYOESPROC glGetProgramBinaryOES;
    extern PFNGLPROGRAMBINARYOESPROC glProgramBinaryOES;
    extern bool OES_mapbuffer;
    extern PFNGLMAPBUFFEROESPROC glMapBufferOES;
    extern PFNGLUNMAPBUFFEROESPROC glUnmapBufferOES;
    extern PFNGLGETBUFFERPOINTERVOESPROC glGetBufferPointervOES;
    extern bool OES_packed_depth_stencil;
    extern bool OES_rgb8_rgba8;
    extern bool OES_standard_derivatives;
    extern bool OES_stencil1;
    extern bool OES_stencil4;
    extern bool OES_texture_3D;
    extern PFNGLTEXIMAGE3DOESPROC glTexImage3DOES;
    extern PFNGLTEXSUBIMAGE3DOESPROC glTexSubImage3DOES;
    extern PFNGLCOPYTEXSUBIMAGE3DOESPROC glCopyTexSubImage3DOES;
    extern PFNGLCOMPRESSEDTEXIMAGE3DOESPROC glCompressedTexImage3DOES;
    extern PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC glCompressedTexSubImage3DOES;
    extern PFNGLFRAMEBUFFERTEXTURE3DOES glFramebufferTexture3DOES;
    extern bool OES_texture_float;
    extern bool OES_texture_float_linear;
    extern bool OES_texture_half_float;
    extern bool OES_texture_half_float_linear;
    extern bool OES_texture_npot;
    extern bool OES_vertex_array_object;
    extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES;
    extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES;
    extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES;
    extern PFNGLISVERTEXARRAYOESPROC glIsVertexArrayOES;
    extern bool OES_vertex_type_10_10_10_2;

    // Ext
    extern bool EXT_blend_minmax; //iOS SGX Supported
    extern bool EXT_discard_framebuffer; //iOS SGX Supported
    extern PFNGLDISCARDFRAMEBUFFEREXTPROC glDiscardFramebufferEXT;
    extern bool EXT_shader_texture_lod; //iOS SGX Supported
    extern bool EXT_texture_filter_anisotropic; //iOS SGX Supported
    extern bool EXT_texture_compression_dxt1;
    extern bool EXT_texture_format_BGRA8888;
    extern bool EXT_texture_compression_s3tc;
    extern bool EXT_occlusion_query_boolean; //Newer iOS models supported (and Chrome/NaCL!)
    extern bool EXT_multisampled_render_to_texture;

    // IMG Extensions
    extern bool IMG_read_format; //iOS SGX Supported
    extern bool IMG_texture_compression_pvrtc; //iOS SGX Supported
    extern bool IMG_texture_stream;
    extern bool IMG_texture_compression_pvrtc2;
    extern bool IMG_multisampled_render_to_texture;
    extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMG glRenderbufferStorageMultisampleIMG;
    extern PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMG glFramebufferTexture2DMultisampleIMG;

    // ANGLE Extensions (Chrome/NaCL)
    extern bool ANGLE_framebuffer_multisample;
    extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEANGLEPROC glRenderbufferStorageMultisampleANGLE;
    extern bool ANGLE_framebuffer_blit;
    extern PFNGLBLITFRAMEBUFFERANGLEPROC glBlitFramebufferANGLE;
    extern bool ANGLE_texture_compression_dxt5;
    extern bool ANGLE_timer_query;
    //APPLE
    extern bool APPLE_framebuffer_multisample; //iOS SGX Supported
    extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEAPPLEPROC glRenderbufferStorageMultisampleAPPLE;
    extern PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC glResolveMultisampleFramebufferAPPLE;
    extern bool APPLE_texture_max_level; //iOS SGX Supported
    extern bool APPLE_rgb_422; //iOS SGX Supported
    extern bool APPLE_texture_format_BGRA8888; //iOS SGX Supported
    // AMD Extensions
    extern bool AMD_compressed_3DC_texture;
    extern bool AMD_compressed_ATC_texture;
    // NV Extensions
    extern bool NV_fbo_color_attachments; // Android Tegra 2 Supported
    extern bool NV_read_buffer;

    // ARB Extensions (ARB in ES 2.0 land? LOL Nvidia! But thank you for allowing MRTs!)
//      extern bool ARB_draw_buffers; // Android Tegra 2 Supported
    extern bool NV_draw_buffers;

	extern int  majorVersion, minorVersion;
}

bool initOpenGLExtensions();

/****************************************************************************
** Macros
****************************************************************************/
#define PVRTRGBA(r, g, b, a)   ((GLuint) (((a) << 24) | ((b) << 16) | ((g) << 8) | (r)))


#endif // _utOpenGL_H_
