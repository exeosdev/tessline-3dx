
#include "GL_coreAPIProxy.h"
#include <ts3/gpuapi/resources/gpuBufferCommon.h>
#include <ts3/gpuapi/resources/samplerCommon.h>
#include <ts3/gpuapi/resources/shaderCommon.h>
#include <ts3/gpuapi/resources/textureCommon.h>
#include <ts3/stdext/mapUtils.h>

namespace ts3::gpuapi
{

	GLenum GLCoreAPIProxy::chooseGLBufferInitFlagsCoreES( GLenum pBindTarget, Bitmask<resource_flags_value_t> pBufferFlags, Bitmask<EMemoryFlags> pMemoryFlags )
	{
	#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
		return chooseGLBufferUsagePolicy( pBindTarget, pBufferFlags );
	#else
		return chooseGLBufferStorageFlags( pBindTarget, pBufferFlags, pMemoryFlags );
	#endif
	}

	GLenum GLCoreAPIProxy::chooseGLBufferStorageFlags( GLenum pBindTarget, Bitmask<resource_flags_value_t> pBufferFlags, Bitmask<EMemoryFlags> pMemoryFlags )
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
		if( pMemoryFlags.isSet( E_MEMORY_ACCESS_FLAG_CPU_READ_BIT ) )
		{
			storageFlags.set( GL_MAP_READ_BIT );
		}
		if( pMemoryFlags.isSet( E_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT ) )
		{
			storageFlags.set( GL_MAP_WRITE_BIT );
		}
		if( pMemoryFlags.isSet( E_MEMORY_HEAP_PROPERTY_FLAG_CPU_CACHED_BIT ) )
		{
			storageFlags.set( GL_CLIENT_STORAGE_BIT );
		}
		if( pMemoryFlags.isSet( E_MEMORY_HEAP_PROPERTY_FLAG_PERSISTENT_MAP_BIT ) )
		{
			storageFlags.set( GL_MAP_PERSISTENT_BIT );
			if( pMemoryFlags.isSetAnyOf( E_MEMORY_HEAP_PROPERTY_FLAG_CPU_COHERENT_BIT | E_MEMORY_HEAP_PROPERTY_FLAG_GPU_COHERENT_BIT ) )
			{
				storageFlags.set( GL_MAP_COHERENT_BIT );
			}
		}

		return storageFlags;
	}

	GLenum GLCoreAPIProxy::chooseGLBufferUsagePolicy( GLenum pBindTarget, Bitmask<resource_flags_value_t> pBufferFlags )
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

		if( ( pBindTarget == GL_COPY_READ_BUFFER ) || ( pBindTarget == GL_QUERY_BUFFER ) || ( pBindTarget == GL_TRANSFORM_FEEDBACK_BUFFER ) )
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

	GLenum GLCoreAPIProxy::chooseGLTextureMagFilter( ETextureFilter pMagFilter, ETextureMipMode pMipMode )
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

	GLenum GLCoreAPIProxy::chooseGLTextureMinFilter( ETextureFilter pMinFilter, ETextureMipMode pMipMode )
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

	GLsizei GLCoreAPIProxy::queryGLTextureInternalFormatBPP( GLenum pGLInternalFormat )
	{
		static const std::unordered_map<GLenum, GLsizei> internalFormatSizeMap =
		{
			{ GL_R32F         , 32  },
			{ GL_R32I         , 32  },
			{ GL_R32UI        , 32  },
			{ GL_RG32F        , 64  },
			{ GL_RG32I        , 64  },
			{ GL_RG32UI       , 64  },
			{ GL_RGB32F       , 96  },
			{ GL_RGB32I       , 96  },
			{ GL_RGB32UI      , 96  },
			{ GL_RGBA32F      , 128 },
			{ GL_RGBA32I      , 128 },
			{ GL_RGBA32UI     , 128 },
			{ GL_R16F         , 16  },
			{ GL_R16I         , 16  },
			{ GL_R16UI        , 16  },
			{ GL_R16_SNORM    , 16  },
			{ GL_R16          , 16  },
			{ GL_RG16F        , 32  },
			{ GL_RG16I        , 32  },
			{ GL_RG16UI       , 32  },
			{ GL_RG16_SNORM   , 32  },
			{ GL_RG16         , 32  },
			{ GL_RGBA16F      , 64  },
			{ GL_RGBA16I      , 64  },
			{ GL_RGBA16UI     , 64  },
			{ GL_RGBA16_SNORM , 64  },
			{ GL_RGBA16       , 64  },
			{ GL_R8I          , 8   },
			{ GL_R8UI         , 8   },
			{ GL_R8_SNORM     , 8   },
			{ GL_R8           , 8   },
			{ GL_RG8I         , 16  },
			{ GL_RG8UI        , 16  },
			{ GL_RG8_SNORM    , 16  },
			{ GL_RG8          , 16  },
			{ GL_RGB8         , 24  },
			{ GL_SRGB8        , 24  },
			{ GL_RGBA8        , 32  },
			{ GL_SRGB8_ALPHA8 , 32  },
			{ GL_RGBA8I       , 32  },
			{ GL_RGBA8UI      , 32  },
			{ GL_RGBA8_SNORM  , 32  },
			{ GL_RGBA8        , 32  },
			{ GL_SRGB8_ALPHA8 , 32  },

			{ GL_RGB5_A1            , 16 },
			{ GL_RGB565             , 16 },
			{ GL_RGB9_E5            , 32 },
			{ GL_RGB10_A2UI         , 32 },
			{ GL_RGB10_A2           , 32 },
			{ GL_R11F_G11F_B10F     , 32 },
			{ GL_DEPTH_COMPONENT16  , 16 },
			{ GL_DEPTH24_STENCIL8   , 32 },
			{ GL_DEPTH_COMPONENT24  , 24 },
			{ GL_DEPTH_COMPONENT32F , 32 },
			{ GL_DEPTH32F_STENCIL8  , 64 },

			{ GL_TEXFMT_COMPRESSED_BC1        , 4 },
			{ GL_TEXFMT_COMPRESSED_BC1_SRGBA  , 4 },
			{ GL_TEXFMT_COMPRESSED_BC2        , 4 },
			{ GL_TEXFMT_COMPRESSED_BC2_SRGB   , 8 },
			{ GL_TEXFMT_COMPRESSED_BC3        , 8 },
			{ GL_TEXFMT_COMPRESSED_BC3_SRGB   , 8 },
			{ GL_TEXFMT_COMPRESSED_BC4_SNORM  , 8 },
			{ GL_TEXFMT_COMPRESSED_BC4_UNORM  , 8 },
			{ GL_TEXFMT_COMPRESSED_BC5_SNORM  , 8 },
			{ GL_TEXFMT_COMPRESSED_BC5_UNORM  , 0 },
			{ GL_TEXFMT_COMPRESSED_BC6H_SFLOAT, 0 },
			{ GL_TEXFMT_COMPRESSED_BC6H_UFLOAT, 0 },
			{ GL_TEXFMT_COMPRESSED_BC7        , 0 },
			{ GL_TEXFMT_COMPRESSED_BC7_SRGB   , 0 },
		};
		return ts3::getMapValueOrDefault( internalFormatSizeMap, pGLInternalFormat, 0 );
	}

	GLenum GLCoreAPIProxy::translateGLBaseDataType( EBaseDataType pBaseDataType )
	{
		static const GLenum baseDataTypeArray[] =
		{
			GL_BYTE,
			GL_UNSIGNED_BYTE,
			GL_SHORT,
			GL_UNSIGNED_SHORT,
			GL_INT,
			GL_UNSIGNED_INT,
			GL_INT64_ARB,
			GL_UNSIGNED_INT64_ARB,
			GL_HALF_FLOAT,
			GL_FLOAT,
			GL_DOUBLE,
			GL_UNSIGNED_INT_24_8,
			GL_FLOAT_32_UNSIGNED_INT_24_8_REV,
		};
		auto baseDataTypeIndex = ecGetBaseDataTypeIndex( pBaseDataType );
		return ts3::staticArrayElementOrDefault( baseDataTypeArray, baseDataTypeIndex, GL_TS3_ERR_INVALID_PARAM );
	}

	GLenum GLCoreAPIProxy::translateGLBlendFactor( EBlendFactor pBlendFactor )
	{
		static const GLenum blendFactorArray[] =
		{
			GL_ZERO,
			GL_ONE,
			GL_CONSTANT_COLOR,
			GL_ONE_MINUS_CONSTANT_COLOR,
			GL_SRC_COLOR,
			GL_SRC_ALPHA,
			GL_DST_COLOR,
			GL_DST_ALPHA,
			GL_ONE_MINUS_SRC_COLOR,
			GL_ONE_MINUS_SRC_ALPHA,
			GL_ONE_MINUS_DST_COLOR,
			GL_ONE_MINUS_DST_ALPHA,
		};
		return ts3::staticArrayElementOrDefault( blendFactorArray, pBlendFactor, GL_TS3_ERR_INVALID_PARAM );
	}

	GLenum GLCoreAPIProxy::translateGLBlendOp( EBlendOp pBlendOp )
	{
		static const GLenum blendOpArray[] =
		{
			GL_FUNC_ADD,
			GL_MIN,
			GL_MAX,
			GL_FUNC_SUBTRACT,
			GL_FUNC_REVERSE_SUBTRACT
		};
		return ts3::staticArrayElementOrDefault( blendOpArray, pBlendOp, GL_TS3_ERR_INVALID_PARAM );
	}

	GLenum GLCoreAPIProxy::translateGLBufferBindTarget( EGPUBufferTarget pBufferTarget )
	{
		static const std::unordered_map<EGPUBufferTarget, GLenum> bufferTargetMap =
		{
			{ EGPUBufferTarget::ConstantBuffer         , GL_UNIFORM_BUFFER            },
			{ EGPUBufferTarget::VertexBuffer           , GL_ARRAY_BUFFER              },
			{ EGPUBufferTarget::IndexBuffer            , GL_ELEMENT_ARRAY_BUFFER      },
			{ EGPUBufferTarget::ShaderInputBuffer      , GL_TEXTURE_BUFFER            },
			{ EGPUBufferTarget::ShaderUAVBuffer        , GL_SHADER_STORAGE_BUFFER     },
			{ EGPUBufferTarget::StreamOutputBuffer     , GL_TRANSFORM_FEEDBACK_BUFFER },
			{ EGPUBufferTarget::IndirectDispatchBuffer , GL_DISPATCH_INDIRECT_BUFFER  },
			{ EGPUBufferTarget::IndirectDrawBuffer     , GL_DRAW_INDIRECT_BUFFER      },
			{ EGPUBufferTarget::TransferSourceBuffer   , GL_COPY_READ_BUFFER          },
			{ EGPUBufferTarget::TransferTargetBuffer   , GL_COPY_WRITE_BUFFER         },
		};
		return ts3::getMapValueOrDefault( bufferTargetMap, pBufferTarget, GL_TS3_ERR_INVALID_PARAM );
	}

	GLenum GLCoreAPIProxy::translateGLBufferMapFlags( EMemoryMapMode pMapMode, Bitmask<EMemoryFlags> pMemoryFlags )
	{
		Bitmask<uint32> resourceMapFlags = static_cast<uint32>( pMapMode );
		Bitmask<GLenum> openglMapFlags = 0;

		if( resourceMapFlags.isSet( E_MEMORY_MAP_FLAG_ACCESS_READ_BIT ) )
		{
			openglMapFlags.set( GL_MAP_READ_BIT );
		}
		if( resourceMapFlags.isSet( E_MEMORY_MAP_FLAG_ACCESS_WRITE_BIT ) )
		{
			openglMapFlags.set( GL_MAP_WRITE_BIT );
		}
		if( resourceMapFlags.isSet( E_MEMORY_MAP_FLAG_WRITE_INVALIDATE_BIT ) )
		{
			openglMapFlags.set( GL_MAP_INVALIDATE_BUFFER_BIT );
		}
		if( pMemoryFlags.isSet( E_MEMORY_HEAP_PROPERTY_FLAG_PERSISTENT_MAP_BIT ) )
		{
			openglMapFlags.set( GL_MAP_PERSISTENT_BIT );
		}
		if( pMemoryFlags.isSet( E_MEMORY_HEAP_PROPERTY_FLAG_CPU_COHERENT_BIT ) )
		{
			openglMapFlags.set( GL_MAP_COHERENT_BIT );
		}
		if( openglMapFlags.isSet( GL_MAP_PERSISTENT_BIT ) && !openglMapFlags.isSet( GL_MAP_COHERENT_BIT ) )
		{
			openglMapFlags.set( GL_MAP_FLUSH_EXPLICIT_BIT );
		}

		return openglMapFlags;
	}

	GLenum GLCoreAPIProxy::translateGLCompFunc( ECompFunc pCompFunc )
	{
		static const GLenum compFuncArray[] =
		{
			GL_NEVER,
			GL_ALWAYS,
			GL_EQUAL,
			GL_NOTEQUAL,
			GL_GREATER,
			GL_GEQUAL,
			GL_LESS,
			GL_LEQUAL,
		};
		return ts3::staticArrayElementOrDefault( compFuncArray, pCompFunc, GL_TS3_ERR_INVALID_PARAM );
	}

	GLenum GLCoreAPIProxy::translateGLCullMode( ECullMode pCullMode )
	{
		static const GLenum cullModeArray[] =
		{
			0,
			GL_BACK,
			GL_FRONT,
		};
		return ts3::staticArrayElementOrDefault( cullModeArray, pCullMode, GL_TS3_ERR_INVALID_PARAM );
	}

	GLenum GLCoreAPIProxy::translateGLPrimitiveFillMode( EPrimitiveFillMode pFillMode )
	{
		static const GLenum fillModeArray[] =
		{
			GL_FILL,
			GL_LINE,
		};
		return ts3::staticArrayElementOrDefault( fillModeArray, pFillMode, GL_TS3_ERR_INVALID_PARAM );
	}

	GLenum GLCoreAPIProxy::translateGLPrimitiveTopology( EPrimitiveTopology pTopology )
	{
		static const GLenum primitiveTopologyArray[] =
		{
			GL_POINTS,
			GL_LINES,
			GL_LINES_ADJACENCY,
			GL_LINE_STRIP,
			GL_LINE_STRIP_ADJACENCY,
			GL_TRIANGLES,
			GL_TRIANGLES_ADJACENCY,
			GL_TRIANGLE_STRIP,
			GL_TRIANGLE_STRIP_ADJACENCY,
			GL_PATCHES
		};
		return ts3::staticArrayElementOrDefault( primitiveTopologyArray, pTopology, GL_TS3_ERR_INVALID_PARAM );
	}

	GLenum GLCoreAPIProxy::translateGLEShaderType( EShaderType pShaderType )
	{
		static const GLenum shaderTypeArray[] =
		{
			GL_VERTEX_SHADER,
			GL_TESS_CONTROL_SHADER,
			GL_TESS_EVALUATION_SHADER,
			GL_GEOMETRY_SHADER,
			GL_FRAGMENT_SHADER,
			GL_COMPUTE_SHADER,
		};
		const auto stageIndex = ecGetEShaderStageIndex( pShaderType );
		return ts3::staticArrayElementOrDefault( shaderTypeArray, stageIndex, GL_TS3_ERR_INVALID_PARAM );
	}

	GLenum GLCoreAPIProxy::translateGLStencilOp( EStencilOp pStencilOp )
	{
		static const GLenum stencilOpArray[] =
		{
			GL_ZERO,
			GL_KEEP,
			GL_REPLACE,
			GL_INCR,
			GL_INCR_WRAP,
			GL_DECR,
			GL_DECR_WRAP,
			GL_INVERT,
		};
		return ts3::staticArrayElementOrDefault( stencilOpArray, pStencilOp, GL_TS3_ERR_INVALID_PARAM );
	}

	GLenum GLCoreAPIProxy::translateGLETextureAddressMode( ETextureAddressMode pAddressMode )
	{
		static const GLenum addressModeArray[] =
		{
			GL_CLAMP_TO_BORDER,
			GL_CLAMP_TO_EDGE,
			GL_MIRRORED_REPEAT,
			GL_MIRROR_CLAMP_TO_EDGE,
			GL_REPEAT,
		};
		return ts3::staticArrayElementOrDefault( addressModeArray, pAddressMode, GL_TS3_ERR_INVALID_PARAM );
	}

	GLenum GLCoreAPIProxy::translateGLTextureBindTarget( ETextureDimensionClass pTextureDimensionClass )
	{
		static const std::unordered_map<ETextureDimensionClass, GLenum> bindTargetMap =
		{
			{ ETextureDimensionClass::Texture2D      , GL_TEXTURE_2D             },
			{ ETextureDimensionClass::Texture2DArray , GL_TEXTURE_2D_ARRAY       },
			{ ETextureDimensionClass::Texture2DMS    , GL_TEXTURE_2D_MULTISAMPLE },
			{ ETextureDimensionClass::Texture3D      , GL_TEXTURE_3D             },
			{ ETextureDimensionClass::TextureCubeMap    , GL_TEXTURE_CUBE_MAP       },
		};
		return ts3::getMapValueOrDefault( bindTargetMap, pTextureDimensionClass, GL_TS3_ERR_INVALID_PARAM );
	}

	GLenum GLCoreAPIProxy::translateGLTextureInternalFormat( ETextureFormat pTextureFormat )
	{
		static const std::unordered_map<ETextureFormat, GLenum> internalFormatMap =
		{
			{ ETextureFormat::R32_FLOAT           , GL_R32F         },
			{ ETextureFormat::R32_SINT            , GL_R32I         },
			{ ETextureFormat::R32_UINT            , GL_R32UI        },
			{ ETextureFormat::R32G32_FLOAT        , GL_RG32F        },
			{ ETextureFormat::R32G32_SINT         , GL_RG32I        },
			{ ETextureFormat::R32G32_UINT         , GL_RG32UI       },
			{ ETextureFormat::R32G32B32_FLOAT     , GL_RGB32F       },
			{ ETextureFormat::R32G32B32_SINT      , GL_RGB32I       },
			{ ETextureFormat::R32G32B32_UINT      , GL_RGB32UI      },
			{ ETextureFormat::R32G32B32A32_FLOAT  , GL_RGBA32F      },
			{ ETextureFormat::R32G32B32A32_SINT   , GL_RGBA32I      },
			{ ETextureFormat::R32G32B32A32_UINT   , GL_RGBA32UI     },
			{ ETextureFormat::R16_FLOAT           , GL_R16F         },
			{ ETextureFormat::R16_SINT            , GL_R16I         },
			{ ETextureFormat::R16_UINT            , GL_R16UI        },
			{ ETextureFormat::R16_SNORM           , GL_R16_SNORM    },
			{ ETextureFormat::R16_UNORM           , GL_R16          },
			{ ETextureFormat::R16G16_FLOAT        , GL_RG16F        },
			{ ETextureFormat::R16G16_SINT         , GL_RG16I        },
			{ ETextureFormat::R16G16_UINT         , GL_RG16UI       },
			{ ETextureFormat::R16G16_SNORM        , GL_RG16_SNORM   },
			{ ETextureFormat::R16G16_UNORM        , GL_RG16         },
			{ ETextureFormat::R16G16B16A16_FLOAT  , GL_RGBA16F      },
			{ ETextureFormat::R16G16B16A16_SINT   , GL_RGBA16I      },
			{ ETextureFormat::R16G16B16A16_UINT   , GL_RGBA16UI     },
			{ ETextureFormat::R16G16B16A16_SNORM  , GL_RGBA16_SNORM },
			{ ETextureFormat::R16G16B16A16_UNORM  , GL_RGBA16       },
			{ ETextureFormat::R8_SINT             , GL_R8I          },
			{ ETextureFormat::R8_UINT             , GL_R8UI         },
			{ ETextureFormat::R8_SNORM            , GL_R8_SNORM     },
			{ ETextureFormat::R8_UNORM            , GL_R8           },
			{ ETextureFormat::R8G8_SINT           , GL_RG8I         },
			{ ETextureFormat::R8G8_UINT           , GL_RG8UI        },
			{ ETextureFormat::R8G8_SNORM          , GL_RG8_SNORM    },
			{ ETextureFormat::R8G8_UNORM          , GL_RG8          },
			{ ETextureFormat::B8G8R8X8_UNORM      , GL_RGB8         },
			{ ETextureFormat::B8G8R8X8_SRGB       , GL_SRGB8        },
			{ ETextureFormat::B8G8R8A8_UNORM      , GL_RGBA8        },
			{ ETextureFormat::B8G8R8A8_SRGB       , GL_SRGB8_ALPHA8 },
			{ ETextureFormat::R8G8B8A8_SINT       , GL_RGBA8I       },
			{ ETextureFormat::R8G8B8A8_UINT       , GL_RGBA8UI      },
			{ ETextureFormat::R8G8B8A8_SNORM      , GL_RGBA8_SNORM  },
			{ ETextureFormat::R8G8B8A8_UNORM      , GL_RGBA8        },
			{ ETextureFormat::R8G8B8A8_SRGB       , GL_SRGB8_ALPHA8 },

			{ ETextureFormat::R5G5B5A1            , GL_RGB5_A1            },
			{ ETextureFormat::R5G6B5              , GL_RGB565             },
			{ ETextureFormat::R9G9B9E5            , GL_RGB9_E5            },
			{ ETextureFormat::R10G10B10A2_UINT    , GL_RGB10_A2UI         },
			{ ETextureFormat::R10G10B10A2_UNORM   , GL_RGB10_A2           },
			{ ETextureFormat::R11G11B10_FLOAT32   , GL_R11F_G11F_B10F     },
			{ ETextureFormat::D16_UNORM           , GL_DEPTH_COMPONENT16  },
			{ ETextureFormat::D24_UNORM_S8_UINT   , GL_DEPTH24_STENCIL8   },
			{ ETextureFormat::D24_UNORM_X8        , GL_DEPTH_COMPONENT24  },
			{ ETextureFormat::D32_FLOAT           , GL_DEPTH_COMPONENT32F },

			{ ETextureFormat::BC1_RGBA_UNORM      , GL_TEXFMT_COMPRESSED_BC1         },
			{ ETextureFormat::BC1_RGBA_UNORM_SRGB , GL_TEXFMT_COMPRESSED_BC1_SRGBA   },
			{ ETextureFormat::BC2_UNORM           , GL_TEXFMT_COMPRESSED_BC2         },
			{ ETextureFormat::BC2_UNORM_SRGB      , GL_TEXFMT_COMPRESSED_BC2_SRGB    },
			{ ETextureFormat::BC3_UNORM           , GL_TEXFMT_COMPRESSED_BC3         },
			{ ETextureFormat::BC3_UNORM_SRGB      , GL_TEXFMT_COMPRESSED_BC3_SRGB    },
			{ ETextureFormat::BC4_SNORM           , GL_TEXFMT_COMPRESSED_BC4_SNORM   },
			{ ETextureFormat::BC4_UNORM           , GL_TEXFMT_COMPRESSED_BC4_UNORM   },
			{ ETextureFormat::BC5_SNORM           , GL_TEXFMT_COMPRESSED_BC5_SNORM   },
			{ ETextureFormat::BC5_UNORM           , GL_TEXFMT_COMPRESSED_BC5_UNORM   },
			{ ETextureFormat::BC6H_SFLOAT         , GL_TEXFMT_COMPRESSED_BC6H_SFLOAT },
			{ ETextureFormat::BC6H_UFLOAT         , GL_TEXFMT_COMPRESSED_BC6H_UFLOAT },
			{ ETextureFormat::BC7_UNORM           , GL_TEXFMT_COMPRESSED_BC7         },
			{ ETextureFormat::BC7_UNORM_SRGB      , GL_TEXFMT_COMPRESSED_BC7_SRGB    },
		};
		return ts3::getMapValueOrDefault( internalFormatMap, pTextureFormat, GL_TS3_ERR_INVALID_PARAM );
	}

	GLenum GLCoreAPIProxy::translateGLTexturePixelDataLayout( ETextureFormat pTextureFormat )
	{
		static const std::unordered_map<ETextureFormat, GLenum> pixelDataLayoutMap =
		{
			{ ETextureFormat::R32_FLOAT           , GL_RED  },
			{ ETextureFormat::R32_SINT            , GL_RED  },
			{ ETextureFormat::R32_UINT            , GL_RED  },
			{ ETextureFormat::R32G32_FLOAT        , GL_RG   },
			{ ETextureFormat::R32G32_SINT         , GL_RG   },
			{ ETextureFormat::R32G32_UINT         , GL_RG   },
			{ ETextureFormat::R32G32B32_FLOAT     , GL_RGB  },
			{ ETextureFormat::R32G32B32_SINT      , GL_RGB  },
			{ ETextureFormat::R32G32B32_UINT      , GL_RGB  },
			{ ETextureFormat::R32G32B32A32_FLOAT  , GL_RGBA },
			{ ETextureFormat::R32G32B32A32_SINT   , GL_RGBA },
			{ ETextureFormat::R32G32B32A32_UINT   , GL_RGBA },
			{ ETextureFormat::R16_FLOAT           , GL_RED  },
			{ ETextureFormat::R16_SINT            , GL_RED  },
			{ ETextureFormat::R16_UINT            , GL_RED  },
			{ ETextureFormat::R16_SNORM           , GL_RED  },
			{ ETextureFormat::R16_UNORM           , GL_RED  },
			{ ETextureFormat::R16G16_FLOAT        , GL_RG   },
			{ ETextureFormat::R16G16_SINT         , GL_RG   },
			{ ETextureFormat::R16G16_UINT         , GL_RG   },
			{ ETextureFormat::R16G16_SNORM        , GL_RG   },
			{ ETextureFormat::R16G16_UNORM        , GL_RG   },
			{ ETextureFormat::R16G16B16A16_FLOAT  , GL_RGBA },
			{ ETextureFormat::R16G16B16A16_SINT   , GL_RGBA },
			{ ETextureFormat::R16G16B16A16_UINT   , GL_RGBA },
			{ ETextureFormat::R16G16B16A16_SNORM  , GL_RGBA },
			{ ETextureFormat::R16G16B16A16_UNORM  , GL_RGBA },
			{ ETextureFormat::R8_SINT             , GL_RED  },
			{ ETextureFormat::R8_UINT             , GL_RED  },
			{ ETextureFormat::R8_SNORM            , GL_RED  },
			{ ETextureFormat::R8_UNORM            , GL_RED  },
			{ ETextureFormat::R8G8_SINT           , GL_RG   },
			{ ETextureFormat::R8G8_UINT           , GL_RG   },
			{ ETextureFormat::R8G8_SNORM          , GL_RG   },
			{ ETextureFormat::R8G8_UNORM          , GL_RG   },
			{ ETextureFormat::B8G8R8X8_UNORM      , GL_BGR  },
			{ ETextureFormat::B8G8R8X8_SRGB       , GL_BGR  },
			{ ETextureFormat::B8G8R8A8_UNORM      , GL_BGRA },
			{ ETextureFormat::B8G8R8A8_SRGB       , GL_BGRA },
			{ ETextureFormat::R8G8B8A8_SINT       , GL_RGBA },
			{ ETextureFormat::R8G8B8A8_UINT       , GL_RGBA },
			{ ETextureFormat::R8G8B8A8_SNORM      , GL_RGBA },
			{ ETextureFormat::R8G8B8A8_UNORM      , GL_RGBA },
			{ ETextureFormat::R8G8B8A8_SRGB       , GL_RGBA },

			{ ETextureFormat::R5G5B5A1            , GL_RGB5_A1            },
			{ ETextureFormat::R5G6B5              , GL_RGB565             },
			{ ETextureFormat::R9G9B9E5            , GL_RGB9_E5            },
			{ ETextureFormat::R10G10B10A2_UINT    , GL_RGB10_A2UI         },
			{ ETextureFormat::R10G10B10A2_UNORM   , GL_RGB10_A2           },
			{ ETextureFormat::R11G11B10_FLOAT32   , GL_R11F_G11F_B10F     },
			{ ETextureFormat::D16_UNORM           , GL_DEPTH_COMPONENT16  },
			{ ETextureFormat::D24_UNORM_S8_UINT   , GL_DEPTH24_STENCIL8   },
			{ ETextureFormat::D24_UNORM_X8        , GL_DEPTH_COMPONENT24  },
			{ ETextureFormat::D32_FLOAT           , GL_DEPTH_COMPONENT32F },

			{ ETextureFormat::BC1_RGBA_UNORM      , GL_TEXFMT_COMPRESSED_BC1         },
			{ ETextureFormat::BC1_RGBA_UNORM_SRGB , GL_TEXFMT_COMPRESSED_BC1_SRGBA   },
			{ ETextureFormat::BC2_UNORM           , GL_TEXFMT_COMPRESSED_BC2         },
			{ ETextureFormat::BC2_UNORM_SRGB      , GL_TEXFMT_COMPRESSED_BC2_SRGB    },
			{ ETextureFormat::BC3_UNORM           , GL_TEXFMT_COMPRESSED_BC3         },
			{ ETextureFormat::BC3_UNORM_SRGB      , GL_TEXFMT_COMPRESSED_BC3_SRGB    },
			{ ETextureFormat::BC4_SNORM           , GL_TEXFMT_COMPRESSED_BC4_SNORM   },
			{ ETextureFormat::BC4_UNORM           , GL_TEXFMT_COMPRESSED_BC4_UNORM   },
			{ ETextureFormat::BC5_SNORM           , GL_TEXFMT_COMPRESSED_BC5_SNORM   },
			{ ETextureFormat::BC5_UNORM           , GL_TEXFMT_COMPRESSED_BC5_UNORM   },
			{ ETextureFormat::BC6H_SFLOAT         , GL_TEXFMT_COMPRESSED_BC6H_SFLOAT },
			{ ETextureFormat::BC6H_UFLOAT         , GL_TEXFMT_COMPRESSED_BC6H_UFLOAT },
			{ ETextureFormat::BC7_UNORM           , GL_TEXFMT_COMPRESSED_BC7         },
			{ ETextureFormat::BC7_UNORM_SRGB      , GL_TEXFMT_COMPRESSED_BC7_SRGB    },
		};
		return ts3::getMapValueOrDefault( pixelDataLayoutMap, pTextureFormat, GL_TS3_ERR_INVALID_PARAM );
	}

	GLenum GLCoreAPIProxy::translateGLTriangleVerticesOrder( ETriangleVerticesOrder pVerticesOrder )
	{
		static const GLenum verticesOrderArray[] =
		{
			GL_CW,
			GL_CCW,
		};
		return ts3::staticArrayElementOrDefault( verticesOrderArray, pVerticesOrder, GL_TS3_ERR_INVALID_PARAM );
	}

	const char * GLCoreAPIProxy::translateGLDebugOutputExtensionName( GLDebugOutputVersion pDebugOutputVersion )
	{
		switch( pDebugOutputVersion )
		{
			case GLDebugOutputVersion::AMDExt:
				return "GL_AMD_debug_output";
			case GLDebugOutputVersion::ARBExt:
				return "GL_ARB_debug_output";
			case GLDebugOutputVersion::KHRCore:
				return "GL_KHR_debug";
			default:
				return "Unknown";
		}
	}

	const char * GLCoreAPIProxy::translateGLDebugEventCategoryStrAMD( GLenum pEventCategory )
	{
		switch( pEventCategory )
		{
			case GL_DEBUG_CATEGORY_API_ERROR_AMD:
				return "API error";
			case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
				return "Window system";
			case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
				return "Deprecation";
			case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
				return "Undefined behavior";
			case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
				return "Performance";
			case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
				return "Shader compiler";
			case GL_DEBUG_CATEGORY_APPLICATION_AMD:
				return "Application";
			case GL_DEBUG_CATEGORY_OTHER_AMD:
				return "Other";
			default:
				return "Unknown";
		}
	}

	const char * GLCoreAPIProxy::translateGLDebugEventSeverityStr( GLenum pEventSeverity )
	{
		switch( pEventSeverity )
		{
			case GL_DEBUG_SEVERITY_HIGH_ARB:
				return "High";
			case GL_DEBUG_SEVERITY_MEDIUM_ARB:
				return "Medium";
			case GL_DEBUG_SEVERITY_LOW_ARB:
				return "Low";
			case GL_DEBUG_SEVERITY_NOTIFICATION:
				return "Notification";
			default:
				return "Unknown";
		}
	}

	const char * GLCoreAPIProxy::translateGLDebugEventSourceStr( GLenum pEventSource )
	{
		switch( pEventSource )
		{
			case GL_DEBUG_SOURCE_API_ARB:
				return "GL API";
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
				return "Window system";
			case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
				return "Shader compiler";
			case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
				return "Third party";
			case GL_DEBUG_SOURCE_APPLICATION_ARB:
				return "Application";
			case GL_DEBUG_SOURCE_OTHER_ARB:
				return "Other";
			default:
				return "Unknown";
		}
	}

	const char * GLCoreAPIProxy::translateGLDebugEventTypeStr( GLenum pEventType )
	{
		switch( pEventType )
		{
			case GL_DEBUG_TYPE_ERROR_ARB:
				return "Error";
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
				return "Deprecated behavior";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
				return "Undefined behavior";
			case GL_DEBUG_TYPE_PORTABILITY_ARB:
				return "Portability";
			case GL_DEBUG_TYPE_PERFORMANCE_ARB:
				return "Performance";
			case GL_DEBUG_TYPE_OTHER_ARB:
				return "Other";
			default:
				return "Unknown";
		}
	}

}
