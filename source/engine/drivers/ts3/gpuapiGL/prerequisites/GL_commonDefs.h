
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_COMMON_DEFS_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_COMMON_DEFS_H__

#define GL_TEXFMT_COMPRESSED_BC1         GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#define GL_TEXFMT_COMPRESSED_BC1_SRGBA   GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT
#define GL_TEXFMT_COMPRESSED_BC2         GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
#define GL_TEXFMT_COMPRESSED_BC2_SRGB    GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT
#define GL_TEXFMT_COMPRESSED_BC3         GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
#define GL_TEXFMT_COMPRESSED_BC3_SRGB    GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
#define GL_TEXFMT_COMPRESSED_BC4_SNORM   GL_COMPRESSED_SIGNED_RED_RGTC1
#define GL_TEXFMT_COMPRESSED_BC4_UNORM   GL_COMPRESSED_RED_RGTC1
#define GL_TEXFMT_COMPRESSED_BC5_SNORM   GL_COMPRESSED_SIGNED_RG_RGTC2
#define GL_TEXFMT_COMPRESSED_BC5_UNORM   GL_COMPRESSED_RG_RGTC2
#define GL_TEXFMT_COMPRESSED_BC6H_SFLOAT GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB
#define GL_TEXFMT_COMPRESSED_BC6H_UFLOAT GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB
#define GL_TEXFMT_COMPRESSED_BC7         GL_COMPRESSED_RGBA_BPTC_UNORM_ARB
#define GL_TEXFMT_COMPRESSED_BC7_SRGB    GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB

#define GL_TS3_ERR_INVALID_PARAM ( ( GLenum ) 0xFFFFFFFFu )

namespace ts3::gpuapi
{

	constexpr GLenum cvGLInvalidValue = Limits<GLenum>::maxValue;

	constexpr GLuint cvGLInvalidProgramLocation = Limits<GLuint>::maxValue;

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_COMMON_DEFS_H__
