
#include "GL_apiTranslationLayer.h"
#include <ts3/gpuapi/resources/gpuBufferCommon.h>
#include <ts3/gpuapi/state/samplerCommon.h>
#include <ts3/gpuapi/resources/shaderCommon.h>
#include <ts3/gpuapi/resources/textureCommon.h>
#include <ts3/stdext/stlHelperAlgo.h>
#include <unordered_map>

namespace ts3::gpuapi
{

	GLenum atl::chooseGLBufferInitFlagsCoreES( GLenum pBindTarget, Bitmask<resource_flags_value_t> pBufferFlags, Bitmask<EGPUMemoryFlags> pMemoryFlags )
	{
	#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
		return chooseGLBufferUsagePolicy( pBindTarget, pBufferFlags );
	#else
		return chooseGLBufferStorageFlags( pBindTarget, pBufferFlags, pMemoryFlags );
	#endif
	}

#if( TS3GX_GL_TARGET == TS3GX_GL_TARGET_GL43 )
	GLenum atl::chooseGLBufferStorageFlags( GLenum pBindTarget, Bitmask<resource_flags_value_t> pBufferFlags, Bitmask<EGPUMemoryFlags> pMemoryFlags )
	{
		Bitmask<GLenum> storageFlags = 0;

		if( !pBufferFlags.isSet( E_GPU_RESOURCE_CONTENT_FLAG_IMMUTABLE_BIT ) )
		{
			// DYNAMIC_STORAGE is required in order to perform *ANY* kind of buffer data update.
			// Without this flag, it is impossible to change the buffer's content even via glBufferSubData.
			// Because of that, we apply this to every buffer, except those specified explicitly as IMMUTABLE.
			storageFlags.set( GL_DYNAMIC_STORAGE_BIT );
		}
		if( pBufferFlags.isSet( E_GPU_RESOURCE_CONTENT_FLAG_TEMPORARY_BIT ) )
		{
			storageFlags.set( GL_CLIENT_STORAGE_BIT );
		}
		if( pMemoryFlags.isSet( E_GPU_MEMORY_ACCESS_FLAG_CPU_READ_BIT ) )
		{
			storageFlags.set( GL_MAP_READ_BIT );
		}
		if( pMemoryFlags.isSet( E_GPU_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT ) )
		{
			storageFlags.set( GL_MAP_WRITE_BIT );
		}
		if( pMemoryFlags.isSet( E_GPU_MEMORY_HEAP_PROPERTY_FLAG_CPU_CACHED_BIT ) )
		{
			storageFlags.set( GL_CLIENT_STORAGE_BIT );
		}
		if( pMemoryFlags.isSet( E_GPU_MEMORY_HEAP_PROPERTY_FLAG_PERSISTENT_MAP_BIT ) )
		{
			storageFlags.set( GL_MAP_PERSISTENT_BIT );
			if( pMemoryFlags.isSetAnyOf( E_GPU_MEMORY_HEAP_PROPERTY_FLAG_CPU_COHERENT_BIT | E_GPU_MEMORY_HEAP_PROPERTY_FLAG_GPU_COHERENT_BIT ) )
			{
				storageFlags.set( GL_MAP_COHERENT_BIT );
			}
		}

		return storageFlags;
	}
#endif

	GLenum atl::chooseGLBufferUsagePolicy( GLenum pBindTarget, Bitmask<resource_flags_value_t> pBufferFlags )
	{
		// Usage constants have standard values. Use that fact to avoid huge if-else.
		// Basically, we first select between DYNAMIC, STREAM and STATIC and then advance
		// selected value according to buffer bind target (i.e. how the data will be used).
		//
		// GL_STREAM_DRAW  = 0x88E0
		// GL_STREAM_READ  = 0x88E1
		// GL_STREAM_COPY  = 0x88E2
		// GL_STATIC_DRAW  = 0x88E4
		// GL_STATIC_READ  = 0x88E5
		// GL_STATIC_COPY  = 0x88E6
		// GL_DYNAMIC_DRAW = 0x88E8
		// GL_DYNAMIC_READ = 0x88E9
		// GL_DYNAMIC_COPY = 0x88EA

		GLenum usagePolicy = 0;

		// The nature of the data has been explicitly specified as dynamic.
		if( pBufferFlags.isSet( E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT ) )
		{
			usagePolicy = GL_DYNAMIC_DRAW;
		}
		// Temporary buffers, updated once and used few times. That fits with STREAM policy.
		else if( pBufferFlags.isSet( E_GPU_RESOURCE_CONTENT_FLAG_TEMPORARY_BIT ) )
		{
			usagePolicy = GL_STREAM_DRAW;
		}
		// The most common usage will be a static buffer (usually a VBO/IBO), updated from time to time and used for drawing.
		else
		{
			usagePolicy = GL_STATIC_DRAW;
		}

		if( ( pBindTarget == GL_COPY_READ_BUFFER ) || ( pBindTarget == GL_TRANSFORM_FEEDBACK_BUFFER ) )
		{
			// Advance from GL_XXX_DRAW to GL_XXX_READ
			usagePolicy += 1;
		}
		else if( ( pBindTarget == GL_COPY_WRITE_BUFFER ) || ( pBindTarget == GL_PIXEL_UNPACK_BUFFER ) )
		{
			// Advance from GL_XXX_DRAW to GL_XXX_COPY
			usagePolicy += 2;
		}

		return usagePolicy;
	}

	GLenum atl::chooseGLTextureMagFilter( ETextureFilter pMagFilter, ETextureMipMode pMipMode )
	{
		GLenum magFilter = GL_INVALID_VALUE;

		if( pMagFilter == ETextureFilter::Point )
		{
			magFilter = GL_NEAREST;
		}
		else if( pMagFilter == ETextureFilter::Linear )
		{
			magFilter = GL_LINEAR;
		}
		else if( pMagFilter == ETextureFilter::Anisotropic )
		{
			magFilter = GL_LINEAR;
		}

		return magFilter;
	}

	GLenum atl::chooseGLTextureMinFilter( ETextureFilter pMinFilter, ETextureMipMode pMipMode )
	{
		GLenum minFilter = GL_INVALID_VALUE;

		if( pMinFilter == ETextureFilter::Point )
		{
			if( pMipMode == ETextureMipMode::Disable )
			{
				minFilter = GL_NEAREST;
			}
			else if( pMipMode == ETextureMipMode::Nearest )
			{
				minFilter = GL_NEAREST_MIPMAP_NEAREST;
			}
			else if( pMipMode == ETextureMipMode::Linear )
			{
				minFilter = GL_NEAREST_MIPMAP_LINEAR;
			}
		}
		else if( pMinFilter == ETextureFilter::Linear )
		{
			if( pMipMode == ETextureMipMode::Disable )
			{
				minFilter = GL_LINEAR;
			}
			else if( pMipMode == ETextureMipMode::Nearest )
			{
				minFilter = GL_LINEAR_MIPMAP_NEAREST;
			}
			else if( pMipMode == ETextureMipMode::Linear )
			{
				minFilter = GL_LINEAR_MIPMAP_LINEAR;
			}
		}
		else if( pMinFilter == ETextureFilter::Anisotropic )
		{
			if( pMipMode == ETextureMipMode::Disable )
			{
				minFilter = GL_LINEAR;
			}
			else if( pMipMode == ETextureMipMode::Nearest )
			{
				minFilter = GL_LINEAR_MIPMAP_NEAREST;
			}
			else if( pMipMode == ETextureMipMode::Linear )
			{
				minFilter = GL_LINEAR_MIPMAP_LINEAR;
			}
		}

		return minFilter;
	}

	GLsizei atl::queryGLTextureInternalFormatBPP( GLenum pGLInternalFormat )
	{
		switch( pGLInternalFormat )
		{
			ts3CaseReturn( GL_R32F         , 32  );
			ts3CaseReturn( GL_R32I         , 32  );
			ts3CaseReturn( GL_R32UI        , 32  );
			ts3CaseReturn( GL_RG32F        , 64  );
			ts3CaseReturn( GL_RG32I        , 64  );
			ts3CaseReturn( GL_RG32UI       , 64  );
			ts3CaseReturn( GL_RGB32F       , 96  );
			ts3CaseReturn( GL_RGB32I       , 96  );
			ts3CaseReturn( GL_RGB32UI      , 96  );
			ts3CaseReturn( GL_RGBA32F      , 128 );
			ts3CaseReturn( GL_RGBA32I      , 128 );
			ts3CaseReturn( GL_RGBA32UI     , 128 );
			ts3CaseReturn( GL_R16F         , 16  );
			ts3CaseReturn( GL_R16I         , 16  );
			ts3CaseReturn( GL_R16UI        , 16  );
			ts3CaseReturn( GL_RG16F        , 32  );
			ts3CaseReturn( GL_RG16I        , 32  );
			ts3CaseReturn( GL_RG16UI       , 32  );
			ts3CaseReturn( GL_RGBA16F      , 64  );
			ts3CaseReturn( GL_RGBA16I      , 64  );
			ts3CaseReturn( GL_RGBA16UI     , 64  );
			ts3CaseReturn( GL_R8I          , 8   );
			ts3CaseReturn( GL_R8UI         , 8   );
			ts3CaseReturn( GL_R8_SNORM     , 8   );
			ts3CaseReturn( GL_R8           , 8   );
			ts3CaseReturn( GL_RG8I         , 16  );
			ts3CaseReturn( GL_RG8UI        , 16  );
			ts3CaseReturn( GL_RG8_SNORM    , 16  );
			ts3CaseReturn( GL_RG8          , 16  );
			ts3CaseReturn( GL_RGB8         , 24  );
			ts3CaseReturn( GL_SRGB8        , 24  );
			ts3CaseReturn( GL_RGBA8        , 32  );
			ts3CaseReturn( GL_SRGB8_ALPHA8 , 32  );
			ts3CaseReturn( GL_RGBA8I       , 32  );
			ts3CaseReturn( GL_RGBA8UI      , 32  );
			ts3CaseReturn( GL_RGBA8_SNORM  , 32  );

			ts3CaseReturn( GL_RGB5_A1            , 16 );
			ts3CaseReturn( GL_RGB565             , 16 );
			ts3CaseReturn( GL_RGB9_E5            , 32 );
			ts3CaseReturn( GL_RGB10_A2UI         , 32 );
			ts3CaseReturn( GL_RGB10_A2           , 32 );
			ts3CaseReturn( GL_R11F_G11F_B10F     , 32 );
			ts3CaseReturn( GL_DEPTH_COMPONENT16  , 16 );
			ts3CaseReturn( GL_DEPTH24_STENCIL8   , 32 );
			ts3CaseReturn( GL_DEPTH_COMPONENT24  , 24 );
			ts3CaseReturn( GL_DEPTH_COMPONENT32F , 32 );
			ts3CaseReturn( GL_DEPTH32F_STENCIL8  , 64 );

		#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_DESKTOP )
			ts3CaseReturn( GL_TEX_FORMAT_COMPRESSED_BC1        , 4 );
			ts3CaseReturn( GL_TEX_FORMAT_COMPRESSED_BC1_SRGBA  , 4 );
			ts3CaseReturn( GL_TEX_FORMAT_COMPRESSED_BC2        , 4 );
			ts3CaseReturn( GL_TEX_FORMAT_COMPRESSED_BC2_SRGB   , 8 );
			ts3CaseReturn( GL_TEX_FORMAT_COMPRESSED_BC3        , 8 );
			ts3CaseReturn( GL_TEX_FORMAT_COMPRESSED_BC3_SRGB   , 8 );
			ts3CaseReturn( GL_TEX_FORMAT_COMPRESSED_BC4_SNORM  , 8 );
			ts3CaseReturn( GL_TEX_FORMAT_COMPRESSED_BC4_UNORM  , 8 );
			ts3CaseReturn( GL_TEX_FORMAT_COMPRESSED_BC5_SNORM  , 8 );
			ts3CaseReturn( GL_TEX_FORMAT_COMPRESSED_BC5_UNORM  , 0 );
			ts3CaseReturn( GL_TEX_FORMAT_COMPRESSED_BC6H_SFLOAT, 0 );
			ts3CaseReturn( GL_TEX_FORMAT_COMPRESSED_BC6H_UFLOAT, 0 );
			ts3CaseReturn( GL_TEX_FORMAT_COMPRESSED_BC7        , 0 );
			ts3CaseReturn( GL_TEX_FORMAT_COMPRESSED_BC7_SRGB   , 0 );
		#endif
		}
		return GL_TS3_ERR_INVALID_PARAM;
	}

	GLenum atl::translateGLBaseDataType( EBaseDataType pBaseDataType )
	{
		auto baseDataTypeIndex = cxdefs::getBaseDataTypeIndex( pBaseDataType );
		switch( baseDataTypeIndex )
		{
			ts3CaseReturn( 0u, GL_BYTE                           );
			ts3CaseReturn( 1u, GL_UNSIGNED_BYTE                  );
			ts3CaseReturn( 2u, GL_SHORT                          );
			ts3CaseReturn( 3u, GL_UNSIGNED_SHORT                 );
			ts3CaseReturn( 4u, GL_INT                            );
			ts3CaseReturn( 5u, GL_UNSIGNED_INT                   );
			ts3CaseReturn( 6u, GL_HALF_FLOAT                     );
			ts3CaseReturn( 7u, GL_FLOAT                          );
			ts3CaseReturn( 8u, GL_UNSIGNED_INT_24_8              );
			ts3CaseReturn( 9u, GL_FLOAT_32_UNSIGNED_INT_24_8_REV );

			default:
			{
				break;
			}
		}
		return GL_TS3_ERR_INVALID_PARAM;
	}

	GLenum atl::translateGLBlendFactor( EBlendFactor pBlendFactor )
	{
		switch( pBlendFactor )
		{
			ts3CaseReturn( EBlendFactor::Zero        , GL_ZERO                     );
			ts3CaseReturn( EBlendFactor::One         , GL_ONE                      );
			ts3CaseReturn( EBlendFactor::Const       , GL_CONSTANT_COLOR           );
			ts3CaseReturn( EBlendFactor::ConstInv    , GL_ONE_MINUS_CONSTANT_COLOR );
			ts3CaseReturn( EBlendFactor::SrcColor    , GL_SRC_COLOR                );
			ts3CaseReturn( EBlendFactor::SrcAlpha    , GL_SRC_ALPHA                );
			ts3CaseReturn( EBlendFactor::DstColor    , GL_DST_COLOR                );
			ts3CaseReturn( EBlendFactor::DstAlpha    , GL_DST_ALPHA                );
			ts3CaseReturn( EBlendFactor::SrcColorInv , GL_ONE_MINUS_SRC_COLOR      );
			ts3CaseReturn( EBlendFactor::SrcAlphaInv , GL_ONE_MINUS_SRC_ALPHA      );
			ts3CaseReturn( EBlendFactor::DstColorInv , GL_ONE_MINUS_DST_COLOR      );
			ts3CaseReturn( EBlendFactor::DstAlphaInv , GL_ONE_MINUS_DST_ALPHA      );

			default:
			{
				break;
			}
		};
		return GL_TS3_ERR_INVALID_PARAM;
	}

	GLenum atl::translateGLBlendOp( EBlendOp pBlendOp )
	{
		switch( pBlendOp )
		{
			ts3CaseReturn( EBlendOp::Add         , GL_FUNC_ADD              );
			ts3CaseReturn( EBlendOp::Min         , GL_MIN                   );
			ts3CaseReturn( EBlendOp::Max         , GL_MAX                   );
			ts3CaseReturn( EBlendOp::Subtract    , GL_FUNC_SUBTRACT         );
			ts3CaseReturn( EBlendOp::SubtractRev , GL_FUNC_REVERSE_SUBTRACT );

			default:
			{
				break;
			}
		};
		return GL_TS3_ERR_INVALID_PARAM;
	}

	GLenum atl::translateGLBufferBindTarget( EGPUBufferTarget pBufferTarget )
	{
		switch( pBufferTarget )
		{
			ts3CaseReturn( EGPUBufferTarget::ConstantBuffer         , GL_UNIFORM_BUFFER            );
			ts3CaseReturn( EGPUBufferTarget::VertexBuffer           , GL_ARRAY_BUFFER              );
			ts3CaseReturn( EGPUBufferTarget::IndexBuffer            , GL_ELEMENT_ARRAY_BUFFER      );
			ts3CaseReturn( EGPUBufferTarget::ShaderInputBuffer      , GL_SHADER_STORAGE_BUFFER     );
			ts3CaseReturn( EGPUBufferTarget::ShaderUAVBuffer        , GL_SHADER_STORAGE_BUFFER     );
			ts3CaseReturn( EGPUBufferTarget::StreamOutputBuffer     , GL_TRANSFORM_FEEDBACK_BUFFER );
			ts3CaseReturn( EGPUBufferTarget::IndirectDispatchBuffer , GL_DISPATCH_INDIRECT_BUFFER  );
			ts3CaseReturn( EGPUBufferTarget::IndirectDrawBuffer     , GL_DRAW_INDIRECT_BUFFER      );
			ts3CaseReturn( EGPUBufferTarget::TransferSourceBuffer   , GL_COPY_READ_BUFFER          );
			ts3CaseReturn( EGPUBufferTarget::TransferTargetBuffer   , GL_COPY_WRITE_BUFFER         );

			default:
			{
				break;
			}
		};
		return GL_TS3_ERR_INVALID_PARAM;
	}

	GLenum atl::translateGLBufferMapFlags( EGPUMemoryMapMode pMapMode, Bitmask<EGPUMemoryFlags> pMemoryFlags )
	{
		Bitmask<uint32> resourceMapFlags = static_cast<uint32>( pMapMode );
		Bitmask<GLenum> openglMapFlags = 0;

		if( resourceMapFlags.isSet( E_GPU_MEMORY_MAP_FLAG_ACCESS_READ_BIT ) )
		{
			openglMapFlags.set( GL_MAP_READ_BIT );
		}
		if( resourceMapFlags.isSet( E_GPU_MEMORY_MAP_FLAG_ACCESS_WRITE_BIT ) )
		{
			openglMapFlags.set( GL_MAP_WRITE_BIT );
		}
		if( resourceMapFlags.isSet( E_GPU_MEMORY_MAP_FLAG_WRITE_INVALIDATE_BIT ) )
		{
			openglMapFlags.set( GL_MAP_INVALIDATE_BUFFER_BIT );
		}

	#if( TS3GX_GL_FEATURE_SUPPORT_BUFFER_PERSISTENT_MAP )
		if( pMemoryFlags.isSet( E_GPU_MEMORY_HEAP_PROPERTY_FLAG_PERSISTENT_MAP_BIT ) )
		{
			openglMapFlags.set( GL_MAP_PERSISTENT_BIT );
		}
		if( pMemoryFlags.isSet( E_GPU_MEMORY_HEAP_PROPERTY_FLAG_CPU_COHERENT_BIT ) )
		{
			openglMapFlags.set( GL_MAP_COHERENT_BIT );
		}
		if( openglMapFlags.isSet( GL_MAP_PERSISTENT_BIT ) && !openglMapFlags.isSet( GL_MAP_COHERENT_BIT ) )
		{
			openglMapFlags.set( GL_MAP_FLUSH_EXPLICIT_BIT );
		}
	#endif

		return openglMapFlags;
	}

	GLenum atl::translateGLCompFunc( ECompFunc pCompFunc )
	{
		switch( pCompFunc )
		{
			ts3CaseReturn( ECompFunc::Never        , GL_NEVER    );
			ts3CaseReturn( ECompFunc::Always       , GL_ALWAYS   );
			ts3CaseReturn( ECompFunc::Equal        , GL_EQUAL    );
			ts3CaseReturn( ECompFunc::NotEqual     , GL_NOTEQUAL );
			ts3CaseReturn( ECompFunc::Greater      , GL_GREATER  );
			ts3CaseReturn( ECompFunc::GreaterEqual , GL_GEQUAL   );
			ts3CaseReturn( ECompFunc::Less         , GL_LESS     );
			ts3CaseReturn( ECompFunc::LessEqual    , GL_LEQUAL   );
		};
		return GL_TS3_ERR_INVALID_PARAM;
	}

	GLenum atl::translateGLCullMode( ECullMode pCullMode )
	{
		switch( pCullMode )
		{
			ts3CaseReturn( ECullMode::None  , 0 );
			ts3CaseReturn( ECullMode::Back  , GL_BACK  );
			ts3CaseReturn( ECullMode::Front , GL_FRONT );
		};
		return GL_TS3_ERR_INVALID_PARAM;
	}

	GLenum atl::translateGLIndexDataFormat( EIndexDataFormat pIndexDataFormat )
	{
		switch( pIndexDataFormat )
		{
			ts3CaseReturn( EIndexDataFormat::Uint16, GL_UNSIGNED_SHORT );
			ts3CaseReturn( EIndexDataFormat::Uint32, GL_UNSIGNED_INT   );

			default:
			{
				break;
			}
		}
		return GL_TS3_ERR_INVALID_PARAM;
	}

	GLenum atl::translateGLPrimitiveFillMode( EPrimitiveFillMode pFillMode )
	{
	#if( TS3GX_GL_FEATURE_SUPPORT_PRIMITIVE_FILL_MODE )
		switch( pFillMode )
		{
			ts3CaseReturn( EPrimitiveFillMode::Solid     , GL_FILL );
			ts3CaseReturn( EPrimitiveFillMode::Wireframe , GL_LINE );
		};
	#endif
		return GL_TS3_ERR_INVALID_PARAM;
	}

	GLenum atl::translateGLPrimitiveTopology( EPrimitiveTopology pTopology )
	{
		switch( pTopology )
		{
			ts3CaseReturn( EPrimitiveTopology::Undefined        , 0u );
			ts3CaseReturn( EPrimitiveTopology::PointList        , GL_POINTS );
			ts3CaseReturn( EPrimitiveTopology::LineList         , GL_LINES );
			ts3CaseReturn( EPrimitiveTopology::LineStrip        , GL_LINE_STRIP );
			ts3CaseReturn( EPrimitiveTopology::TriangleList     , GL_TRIANGLES );
			ts3CaseReturn( EPrimitiveTopology::TriangleStrip    , GL_TRIANGLE_STRIP );
		#if( TS3GX_GL_FEATURE_SUPPORT_PRIMITIVE_TYPE_ADJACENCY )
			ts3CaseReturn( EPrimitiveTopology::LineListAdj      , GL_LINES_ADJACENCY );
			ts3CaseReturn( EPrimitiveTopology::LineStripAdj     , GL_LINE_STRIP_ADJACENCY );
			ts3CaseReturn( EPrimitiveTopology::TriangleListAdj  , GL_TRIANGLES_ADJACENCY );
			ts3CaseReturn( EPrimitiveTopology::TriangleStripAdj , GL_TRIANGLE_STRIP_ADJACENCY );
        #endif
		#if( TS3GX_GL_FEATURE_SUPPORT_PRIMITIVE_TYPE_PATCHES )
			ts3CaseReturn( EPrimitiveTopology::TesselationPatch , GL_PATCHES );
		#endif
		};
		return GL_TS3_ERR_INVALID_PARAM;
	}

	GLenum atl::translateShaderType( EShaderType pShaderType )
	{
		switch( pShaderType )
		{
			ts3CaseReturn( EShaderType::GSVertex   , GL_VERTEX_SHADER );
			ts3CaseReturn( EShaderType::GSPixel    , GL_FRAGMENT_SHADER );
		#if( TS3GX_GL_FEATURE_SUPPORT_SHADER_TYPE_GEOMETRY )
			ts3CaseReturn( EShaderType::GSGeometry , GL_GEOMETRY_SHADER );
        #endif
		#if( TS3GX_GL_FEATURE_SUPPORT_SHADER_TYPE_TESSELATION )
			ts3CaseReturn( EShaderType::GSHull   , GL_TESS_CONTROL_SHADER );
			ts3CaseReturn( EShaderType::GSDomain , GL_TESS_EVALUATION_SHADER );
        #endif
		#if( TS3GX_GL_FEATURE_SUPPORT_SHADER_TYPE_COMPUTE )
			ts3CaseReturn( EShaderType::CSCompute , GL_COMPUTE_SHADER );
		#endif

			default:
			{
				break;
			}
		};
		return GL_TS3_ERR_INVALID_PARAM;
	}

	GLenum atl::translateGLStencilOp( EStencilOp pStencilOp )
	{
		switch( pStencilOp )
		{
			ts3CaseReturn( EStencilOp::Zero      , GL_ZERO      );
			ts3CaseReturn( EStencilOp::Keep      , GL_KEEP      );
			ts3CaseReturn( EStencilOp::Replace   , GL_REPLACE   );
			ts3CaseReturn( EStencilOp::IncrClamp , GL_INCR      );
			ts3CaseReturn( EStencilOp::IncrWrap  , GL_INCR_WRAP );
			ts3CaseReturn( EStencilOp::DecrClamp , GL_DECR      );
			ts3CaseReturn( EStencilOp::DecrWrap  , GL_DECR_WRAP );
			ts3CaseReturn( EStencilOp::Invert    , GL_INVERT    );
		};
		return GL_TS3_ERR_INVALID_PARAM;
	}

	GLenum atl::translateGLTextureAddressMode( ETextureAddressMode pAddressMode )
	{
		switch( pAddressMode )
		{
			ts3CaseReturn( ETextureAddressMode::Clamp  , GL_CLAMP_TO_EDGE   );
			ts3CaseReturn( ETextureAddressMode::Mirror , GL_MIRRORED_REPEAT );
			ts3CaseReturn( ETextureAddressMode::Wrap   , GL_REPEAT          );
		#if( TS3GX_GL_FEATURE_SUPPORT_TEXTURE_EXTENDED_ADDRESS_MODE )
			ts3CaseReturn( ETextureAddressMode::BorderColor     , GL_CLAMP_TO_BORDER      );
			ts3CaseReturn( ETextureAddressMode::MirrorOnceClamp , GL_MIRROR_CLAMP_TO_EDGE );
		#endif
		};
		return GL_TS3_ERR_INVALID_PARAM;
	}

	GLenum atl::translateGLTextureBindTarget( ETextureClass pTextureDimensionClass )
	{
		switch( pTextureDimensionClass )
		{
			ts3CaseReturn( ETextureClass::T2D      , GL_TEXTURE_2D             );
			ts3CaseReturn( ETextureClass::T2DArray , GL_TEXTURE_2D_ARRAY       );
			ts3CaseReturn( ETextureClass::T2DMS    , GL_TEXTURE_2D_MULTISAMPLE );
			ts3CaseReturn( ETextureClass::T3D      , GL_TEXTURE_3D             );
			ts3CaseReturn( ETextureClass::TCubeMap , GL_TEXTURE_CUBE_MAP       );
		};
		return GL_TS3_ERR_INVALID_PARAM;
	}

	GLenum atl::translateGLTextureInternalFormat( ETextureFormat pTextureFormat )
	{
		switch( pTextureFormat )
		{
			ts3CaseReturn( ETextureFormat::R32F       , GL_R32F         );
			ts3CaseReturn( ETextureFormat::R32I       , GL_R32I         );
			ts3CaseReturn( ETextureFormat::R32U       , GL_R32UI        );
			ts3CaseReturn( ETextureFormat::RG32F      , GL_RG32F        );
			ts3CaseReturn( ETextureFormat::RG32I      , GL_RG32I        );
			ts3CaseReturn( ETextureFormat::RG32U      , GL_RG32UI       );
			ts3CaseReturn( ETextureFormat::RGB32F     , GL_RGB32F       );
			ts3CaseReturn( ETextureFormat::RGB32I     , GL_RGB32I       );
			ts3CaseReturn( ETextureFormat::RGB32U     , GL_RGB32UI      );
			ts3CaseReturn( ETextureFormat::RGBA32F    , GL_RGBA32F      );
			ts3CaseReturn( ETextureFormat::RGBA32I    , GL_RGBA32I      );
			ts3CaseReturn( ETextureFormat::RGBA32U    , GL_RGBA32UI     );
			ts3CaseReturn( ETextureFormat::R16F       , GL_R16F         );
			ts3CaseReturn( ETextureFormat::R16I       , GL_R16I         );
			ts3CaseReturn( ETextureFormat::R16U       , GL_R16UI        );
			ts3CaseReturn( ETextureFormat::RG16F      , GL_RG16F        );
			ts3CaseReturn( ETextureFormat::RG16I      , GL_RG16I        );
			ts3CaseReturn( ETextureFormat::RG16U      , GL_RG16UI       );
			ts3CaseReturn( ETextureFormat::RGBA16F    , GL_RGBA16F      );
			ts3CaseReturn( ETextureFormat::RGBA16I    , GL_RGBA16I      );
			ts3CaseReturn( ETextureFormat::RGBA16U    , GL_RGBA16UI     );
			ts3CaseReturn( ETextureFormat::R8I        , GL_R8I          );
			ts3CaseReturn( ETextureFormat::R8U        , GL_R8UI         );
			ts3CaseReturn( ETextureFormat::R8IN       , GL_R8_SNORM     );
			ts3CaseReturn( ETextureFormat::R8UN       , GL_R8           );
			ts3CaseReturn( ETextureFormat::RG8I       , GL_RG8I         );
			ts3CaseReturn( ETextureFormat::RG8U       , GL_RG8UI        );
			ts3CaseReturn( ETextureFormat::RG8IN      , GL_RG8_SNORM    );
			ts3CaseReturn( ETextureFormat::RG8UN      , GL_RG8          );
			ts3CaseReturn( ETextureFormat::RGBA8I     , GL_RGBA8I       );
			ts3CaseReturn( ETextureFormat::RGBA8U     , GL_RGBA8UI      );
			ts3CaseReturn( ETextureFormat::RGBA8IN    , GL_RGBA8_SNORM  );
			ts3CaseReturn( ETextureFormat::RGBA8UN    , GL_RGBA8        );
			ts3CaseReturn( ETextureFormat::RGBA8SRGB  , GL_SRGB8_ALPHA8 );
		#if( TS3GX_GL_FEATURE_SUPPORT_TEXTURE_FORMAT_BGR )
			ts3CaseReturn( ETextureFormat::BGRX8UN    , GL_RGB8         );
			ts3CaseReturn( ETextureFormat::BGRX8SRGB  , GL_SRGB8        );
			ts3CaseReturn( ETextureFormat::BGRA8UN    , GL_RGBA8        );
			ts3CaseReturn( ETextureFormat::BGRA8SRGB  , GL_SRGB8_ALPHA8 );
		#endif
			ts3CaseReturn( ETextureFormat::R5G5B5A1   , GL_RGB5_A1            );
			ts3CaseReturn( ETextureFormat::R5G6B5     , GL_RGB565             );
			ts3CaseReturn( ETextureFormat::R9G9B9E5   , GL_RGB9_E5            );
			ts3CaseReturn( ETextureFormat::RGB10A2U   , GL_RGB10_A2UI         );
			ts3CaseReturn( ETextureFormat::RGB10A2UN  , GL_RGB10_A2           );
			ts3CaseReturn( ETextureFormat::R11G11B10F , GL_R11F_G11F_B10F     );
			ts3CaseReturn( ETextureFormat::D16UN      , GL_DEPTH_COMPONENT16  );
			ts3CaseReturn( ETextureFormat::D24UNS8U   , GL_DEPTH24_STENCIL8   );
			ts3CaseReturn( ETextureFormat::D24UNX8    , GL_DEPTH_COMPONENT24  );
			ts3CaseReturn( ETextureFormat::D32F       , GL_DEPTH_COMPONENT32F );
		#if( TS3GX_GL_FEATURE_SUPPORT_TEXTURE_FORMAT_COMPRESSED_BCX )
			ts3CaseReturn( ETextureFormat::BC1        , GL_TEX_FORMAT_COMPRESSED_BC1         );
			ts3CaseReturn( ETextureFormat::BC1SRGB    , GL_TEX_FORMAT_COMPRESSED_BC1_SRGBA   );
			ts3CaseReturn( ETextureFormat::BC2        , GL_TEX_FORMAT_COMPRESSED_BC2         );
			ts3CaseReturn( ETextureFormat::BC2SRGB    , GL_TEX_FORMAT_COMPRESSED_BC2_SRGB    );
			ts3CaseReturn( ETextureFormat::BC3        , GL_TEX_FORMAT_COMPRESSED_BC3         );
			ts3CaseReturn( ETextureFormat::BC3SRGB    , GL_TEX_FORMAT_COMPRESSED_BC3_SRGB    );
			ts3CaseReturn( ETextureFormat::BC4IN      , GL_TEX_FORMAT_COMPRESSED_BC4_SNORM   );
			ts3CaseReturn( ETextureFormat::BC4UN      , GL_TEX_FORMAT_COMPRESSED_BC4_UNORM   );
			ts3CaseReturn( ETextureFormat::BC5IN      , GL_TEX_FORMAT_COMPRESSED_BC5_SNORM   );
			ts3CaseReturn( ETextureFormat::BC5UN      , GL_TEX_FORMAT_COMPRESSED_BC5_UNORM   );
			ts3CaseReturn( ETextureFormat::BC6HSF     , GL_TEX_FORMAT_COMPRESSED_BC6H_SFLOAT );
			ts3CaseReturn( ETextureFormat::BC6HUF     , GL_TEX_FORMAT_COMPRESSED_BC6H_UFLOAT );
			ts3CaseReturn( ETextureFormat::BC7        , GL_TEX_FORMAT_COMPRESSED_BC7         );
			ts3CaseReturn( ETextureFormat::BC7SRGB    , GL_TEX_FORMAT_COMPRESSED_BC7_SRGB    );
		#endif
		};
		return GL_TS3_ERR_INVALID_PARAM;
	}

	GLenum atl::translateGLTexturePixelDataLayout( ETextureFormat pTextureFormat )
	{
		switch( pTextureFormat )
		{
			ts3CaseReturn( ETextureFormat::R32F       , GL_RED  );
			ts3CaseReturn( ETextureFormat::R32I       , GL_RED  );
			ts3CaseReturn( ETextureFormat::R32U       , GL_RED  );
			ts3CaseReturn( ETextureFormat::RG32F      , GL_RG   );
			ts3CaseReturn( ETextureFormat::RG32I      , GL_RG   );
			ts3CaseReturn( ETextureFormat::RG32U      , GL_RG   );
			ts3CaseReturn( ETextureFormat::RGB32F     , GL_RGB  );
			ts3CaseReturn( ETextureFormat::RGB32I     , GL_RGB  );
			ts3CaseReturn( ETextureFormat::RGB32U     , GL_RGB  );
			ts3CaseReturn( ETextureFormat::RGBA32F    , GL_RGBA );
			ts3CaseReturn( ETextureFormat::RGBA32I    , GL_RGBA );
			ts3CaseReturn( ETextureFormat::RGBA32U    , GL_RGBA );
			ts3CaseReturn( ETextureFormat::R16F       , GL_RED  );
			ts3CaseReturn( ETextureFormat::R16I       , GL_RED  );
			ts3CaseReturn( ETextureFormat::R16U       , GL_RED  );
			ts3CaseReturn( ETextureFormat::RG16F      , GL_RG   );
			ts3CaseReturn( ETextureFormat::RG16I      , GL_RG   );
			ts3CaseReturn( ETextureFormat::RG16U      , GL_RG   );
			ts3CaseReturn( ETextureFormat::RGBA16F    , GL_RGBA );
			ts3CaseReturn( ETextureFormat::RGBA16I    , GL_RGBA );
			ts3CaseReturn( ETextureFormat::RGBA16U    , GL_RGBA );
			ts3CaseReturn( ETextureFormat::R8I        , GL_RED  );
			ts3CaseReturn( ETextureFormat::R8U        , GL_RED  );
			ts3CaseReturn( ETextureFormat::R8IN       , GL_RED  );
			ts3CaseReturn( ETextureFormat::R8UN       , GL_RED  );
			ts3CaseReturn( ETextureFormat::RG8I       , GL_RG   );
			ts3CaseReturn( ETextureFormat::RG8U       , GL_RG   );
			ts3CaseReturn( ETextureFormat::RG8IN      , GL_RG   );
			ts3CaseReturn( ETextureFormat::RG8UN      , GL_RG   );
		#if( TS3GX_GL_FEATURE_SUPPORT_TEXTURE_FORMAT_BGR )
			ts3CaseReturn( ETextureFormat::BGRX8UN    , GL_BGR  );
			ts3CaseReturn( ETextureFormat::BGRX8SRGB  , GL_BGR  );
			ts3CaseReturn( ETextureFormat::BGRA8UN    , GL_BGRA );
			ts3CaseReturn( ETextureFormat::BGRA8SRGB  , GL_BGRA );
		#endif
			ts3CaseReturn( ETextureFormat::RGBA8I     , GL_RGBA );
			ts3CaseReturn( ETextureFormat::RGBA8U     , GL_RGBA );
			ts3CaseReturn( ETextureFormat::RGBA8IN    , GL_RGBA );
			ts3CaseReturn( ETextureFormat::RGBA8UN    , GL_RGBA );
			ts3CaseReturn( ETextureFormat::RGBA8SRGB  , GL_RGBA );
			ts3CaseReturn( ETextureFormat::R5G5B5A1   , GL_RGB5_A1            );
			ts3CaseReturn( ETextureFormat::R5G6B5     , GL_RGB565             );
			ts3CaseReturn( ETextureFormat::R9G9B9E5   , GL_RGB9_E5            );
			ts3CaseReturn( ETextureFormat::RGB10A2U   , GL_RGB10_A2UI         );
			ts3CaseReturn( ETextureFormat::RGB10A2UN  , GL_RGB10_A2           );
			ts3CaseReturn( ETextureFormat::R11G11B10F , GL_R11F_G11F_B10F     );
			ts3CaseReturn( ETextureFormat::D16UN      , GL_DEPTH_COMPONENT16  );
			ts3CaseReturn( ETextureFormat::D24UNS8U   , GL_DEPTH24_STENCIL8   );
			ts3CaseReturn( ETextureFormat::D24UNX8    , GL_DEPTH_COMPONENT24  );
			ts3CaseReturn( ETextureFormat::D32F       , GL_DEPTH_COMPONENT32F );
		#if( TS3GX_GL_FEATURE_SUPPORT_TEXTURE_FORMAT_COMPRESSED_BCX )
			ts3CaseReturn( ETextureFormat::BC1        , GL_TEX_FORMAT_COMPRESSED_BC1         );
			ts3CaseReturn( ETextureFormat::BC1SRGB    , GL_TEX_FORMAT_COMPRESSED_BC1_SRGBA   );
			ts3CaseReturn( ETextureFormat::BC2        , GL_TEX_FORMAT_COMPRESSED_BC2         );
			ts3CaseReturn( ETextureFormat::BC2SRGB    , GL_TEX_FORMAT_COMPRESSED_BC2_SRGB    );
			ts3CaseReturn( ETextureFormat::BC3        , GL_TEX_FORMAT_COMPRESSED_BC3         );
			ts3CaseReturn( ETextureFormat::BC3SRGB    , GL_TEX_FORMAT_COMPRESSED_BC3_SRGB    );
			ts3CaseReturn( ETextureFormat::BC4IN      , GL_TEX_FORMAT_COMPRESSED_BC4_SNORM   );
			ts3CaseReturn( ETextureFormat::BC4UN      , GL_TEX_FORMAT_COMPRESSED_BC4_UNORM   );
			ts3CaseReturn( ETextureFormat::BC5IN      , GL_TEX_FORMAT_COMPRESSED_BC5_SNORM   );
			ts3CaseReturn( ETextureFormat::BC5UN      , GL_TEX_FORMAT_COMPRESSED_BC5_UNORM   );
			ts3CaseReturn( ETextureFormat::BC6HSF     , GL_TEX_FORMAT_COMPRESSED_BC6H_SFLOAT );
			ts3CaseReturn( ETextureFormat::BC6HUF     , GL_TEX_FORMAT_COMPRESSED_BC6H_UFLOAT );
			ts3CaseReturn( ETextureFormat::BC7        , GL_TEX_FORMAT_COMPRESSED_BC7         );
			ts3CaseReturn( ETextureFormat::BC7SRGB    , GL_TEX_FORMAT_COMPRESSED_BC7_SRGB    );
		#endif
		};
		return GL_TS3_ERR_INVALID_PARAM;
	}

	GLenum atl::translateGLTriangleVerticesOrder( ETriangleVerticesOrder pVerticesOrder )
	{
		switch( pVerticesOrder )
		{
			ts3CaseReturn( ETriangleVerticesOrder::Clockwise        , GL_CW  );
			ts3CaseReturn( ETriangleVerticesOrder::CounterClockwise , GL_CCW );
		}
		return GL_TS3_ERR_INVALID_PARAM;
	}

	const char * atl::translateGLDebugOutputExtensionName( GLDebugOutputVersion pDebugOutputVersion )
	{
		switch( pDebugOutputVersion )
		{
			ts3CaseReturn( GLDebugOutputVersion::AMDExt  , "GL_AMD_debug_outputr" );
			ts3CaseReturn( GLDebugOutputVersion::ARBExt  , "GL_ARB_debug_output"  );
			ts3CaseReturn( GLDebugOutputVersion::KHRCore , "GL_KHR_debug"         );
		}
		return "Unknown";
	}

	const char * atl::translateGLDebugEventCategoryStrAMD( GLenum pEventCategory )
	{
	#if( TS3GX_GL_FEATURE_SUPPORT_DEBUG_OUTPUT )
		switch( pEventCategory )
		{
			ts3CaseReturn( GL_DEBUG_CATEGORY_API_ERROR_AMD          , "API error"          );
			ts3CaseReturn( GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD      , "Window system"      );
			ts3CaseReturn( GL_DEBUG_CATEGORY_DEPRECATION_AMD        , "Deprecation"        );
			ts3CaseReturn( GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD , "Undefined behavior" );
			ts3CaseReturn( GL_DEBUG_CATEGORY_PERFORMANCE_AMD        , "Performance"        );
			ts3CaseReturn( GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD    , "Shader compiler"    );
			ts3CaseReturn( GL_DEBUG_CATEGORY_APPLICATION_AMD        , "Application"        );
			ts3CaseReturn( GL_DEBUG_CATEGORY_OTHER_AMD              , "Other"              );
		}
	#endif
		return "Unknown";
	}

	const char * atl::translateGLDebugEventSeverityStr( GLenum pEventSeverity )
	{
	#if( TS3GX_GL_FEATURE_SUPPORT_DEBUG_OUTPUT )
	    switch( pEventSeverity )
		{
			ts3CaseReturn( GL_DEBUG_SEVERITY_HIGH_ARB     , "High"         );
			ts3CaseReturn( GL_DEBUG_SEVERITY_MEDIUM_ARB   , "Medium"       );
			ts3CaseReturn( GL_DEBUG_SEVERITY_LOW_ARB      , "Low"          );
			ts3CaseReturn( GL_DEBUG_SEVERITY_NOTIFICATION , "Notification" );
		}
	#endif
		return "Unknown";
	}

	const char * atl::translateGLDebugEventSourceStr( GLenum pEventSource )
	{
	#if( TS3GX_GL_FEATURE_SUPPORT_DEBUG_OUTPUT )
	    switch( pEventSource )
		{
			ts3CaseReturn( GL_DEBUG_SOURCE_API_ARB             , "GL API"          );
			ts3CaseReturn( GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB   , "Window system"   );
			ts3CaseReturn( GL_DEBUG_SOURCE_SHADER_COMPILER_ARB , "Shader compiler" );
			ts3CaseReturn( GL_DEBUG_SOURCE_THIRD_PARTY_ARB     , "Third party"     );
			ts3CaseReturn( GL_DEBUG_SOURCE_APPLICATION_ARB     , "Application"     );
			ts3CaseReturn( GL_DEBUG_SOURCE_OTHER_ARB           , "Other"           );
		}
	#endif
		return "Unknown";
	}

	const char * atl::translateGLDebugEventTypeStr( GLenum pEventType )
	{
	#if( TS3GX_GL_FEATURE_SUPPORT_DEBUG_OUTPUT )
	    switch( pEventType )
		{
			ts3CaseReturn( GL_DEBUG_TYPE_ERROR_ARB               , "GL API"          );
			ts3CaseReturn( GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB , "Window system"   );
			ts3CaseReturn( GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB  , "Shader compiler" );
			ts3CaseReturn( GL_DEBUG_TYPE_PORTABILITY_ARB         , "Third party"     );
			ts3CaseReturn( GL_DEBUG_TYPE_PERFORMANCE_ARB         , "Application"     );
			ts3CaseReturn( GL_DEBUG_TYPE_OTHER_ARB               , "Other"           );
		}
	#endif
		return "Unknown";
	}

} // namespace ts3::gpuapi
