
#pragma once

#ifndef __TS3_GPUAPI_GPU_DATA_FORMATS_H__
#define __TS3_GPUAPI_GPU_DATA_FORMATS_H__

namespace ts3::GpuAPI
{

	using base_data_type_value_t = uint8;
	using gpu_pixel_format_value_t = uint64;
	using vertex_attrib_format_value_t = uint32;

	enum class EBaseDataType : base_data_type_value_t;

	namespace CxDefs
	{

		TS3_ATTR_NO_DISCARD inline constexpr base_data_type_value_t declareBaseDataType( uint8 pIndex, uint8 pByteSize ) noexcept
		{
			return ( ( pByteSize & 0xF ) << 4 ) | ( pIndex & 0xF );
		}

		TS3_ATTR_NO_DISCARD inline constexpr gpu_pixel_format_value_t declareTextureFormat(
				uint8 pIndex, EPixelDataLayout pPixelLayout, EBaseDataType pBaseType, uint8 pByteSize, uint8 pFlags ) noexcept
		{
			return ( ( ( uint64 )pPixelLayout ) << 32 ) | ( ( ( uint64 )pBaseType ) << 24 ) | ( ( ( uint64 )pByteSize ) << 16 ) | ( ( ( uint64 )pFlags ) << 8 ) | ( uint64 )pIndex;
		}

		TS3_ATTR_NO_DISCARD inline constexpr vertex_attrib_format_value_t declareVertexAttribFormat(
				uint8 pIndex, EBaseDataType pBaseType, uint8 pLength, uint8 pFlags ) noexcept
		{
			return ( ( ( uint32 )pBaseType ) << 24 ) | ( ( ( uint32 )pLength ) << 16 ) | ( ( ( uint32 )pFlags ) << 8 ) | ( uint32 )pIndex;
		}

	}

	enum EGPUDataFormatFlags : uint8
	{
		E_GPU_DATA_FORMAT_FLAG_NORMALIZED_BIT = 0x01,
		E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT = 0x02,
		E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT = 0x04,
		E_GPU_DATA_FORMAT_FLAG_SRGB_BIT = 0x08,
		E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT = 0x10,
		E_GPU_DATA_FORMAT_FLAG_SNORM = E_GPU_DATA_FORMAT_FLAG_NORMALIZED_BIT | E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT,
		E_GPU_DATA_FORMAT_FLAG_UNORM = E_GPU_DATA_FORMAT_FLAG_NORMALIZED_BIT | E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT,
		E_GPU_DATA_FORMAT_FLAG_COMPRESSED_SRGB = E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT | E_GPU_DATA_FORMAT_FLAG_SRGB_BIT,
	};

	enum class EBaseDataType : base_data_type_value_t
	{
		Byte         = CxDefs::declareBaseDataType( 0, 1 ),
		Ubyte        = CxDefs::declareBaseDataType( 1, 1 ),
		Int16        = CxDefs::declareBaseDataType( 2, 2 ),
		Uint16       = CxDefs::declareBaseDataType( 3, 2 ),
		Int32        = CxDefs::declareBaseDataType( 4, 4 ),
		Uint32       = CxDefs::declareBaseDataType( 5, 4 ),
		Float16      = CxDefs::declareBaseDataType( 6, 2 ),
		Float32      = CxDefs::declareBaseDataType( 7, 4 ),
		Uint24S8     = CxDefs::declareBaseDataType( 8, 4 ),
		Undefined    = CxDefs::declareBaseDataType( 9, 0 ),
	};

	enum class EIndexDataFormat : vertex_attrib_format_value_t
	{
		Uint16 = static_cast<vertex_attrib_format_value_t>( EBaseDataType::Uint16 ),
		Uint32 = static_cast<vertex_attrib_format_value_t>( EBaseDataType::Uint32 ),
		Undefined = 0
	};

	enum class ETextureFormat : gpu_pixel_format_value_t
	{
		R32F       = CxDefs::declareTextureFormat( 0,  EPixelDataLayout::Red,   EBaseDataType::Float32,   4,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32I       = CxDefs::declareTextureFormat( 1,  EPixelDataLayout::Red,   EBaseDataType::Int32,     4,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32U       = CxDefs::declareTextureFormat( 2,  EPixelDataLayout::Red,   EBaseDataType::Uint32,    4,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		RG32F      = CxDefs::declareTextureFormat( 3,  EPixelDataLayout::RG,    EBaseDataType::Float32,   8,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		RG32I      = CxDefs::declareTextureFormat( 4,  EPixelDataLayout::RG,    EBaseDataType::Int32,     8,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		RG32U      = CxDefs::declareTextureFormat( 5,  EPixelDataLayout::RG,    EBaseDataType::Uint32,    8,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		RGB32F     = CxDefs::declareTextureFormat( 6,  EPixelDataLayout::RGB,   EBaseDataType::Float32,   12, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		RGB32I     = CxDefs::declareTextureFormat( 7,  EPixelDataLayout::RGB,   EBaseDataType::Int32,     12, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		RGB32U     = CxDefs::declareTextureFormat( 8,  EPixelDataLayout::RGB,   EBaseDataType::Uint32,    12, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		RGBA32F    = CxDefs::declareTextureFormat( 9,  EPixelDataLayout::RGBA,  EBaseDataType::Float32,   16, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		RGBA32I    = CxDefs::declareTextureFormat( 10, EPixelDataLayout::RGBA,  EBaseDataType::Int32,     16, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		RGBA32U    = CxDefs::declareTextureFormat( 11, EPixelDataLayout::RGBA,  EBaseDataType::Uint32,    16, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R16F       = CxDefs::declareTextureFormat( 12, EPixelDataLayout::RG,    EBaseDataType::Float16,   2,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R16I       = CxDefs::declareTextureFormat( 13, EPixelDataLayout::RG,    EBaseDataType::Int16,     2,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R16U       = CxDefs::declareTextureFormat( 14, EPixelDataLayout::RG,    EBaseDataType::Uint16,    2,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		RG16F      = CxDefs::declareTextureFormat( 15, EPixelDataLayout::RG,    EBaseDataType::Float16,   4,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		RG16I      = CxDefs::declareTextureFormat( 16, EPixelDataLayout::RG,    EBaseDataType::Int16,     4,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		RG16U      = CxDefs::declareTextureFormat( 17, EPixelDataLayout::RG,    EBaseDataType::Uint16,    4,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		RGBA16F    = CxDefs::declareTextureFormat( 18, EPixelDataLayout::RGBA,  EBaseDataType::Float16,   8,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		RGBA16I    = CxDefs::declareTextureFormat( 19, EPixelDataLayout::RGBA,  EBaseDataType::Int16,     8,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		RGBA16U    = CxDefs::declareTextureFormat( 20, EPixelDataLayout::RGBA,  EBaseDataType::Uint16,    8,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R8I        = CxDefs::declareTextureFormat( 21, EPixelDataLayout::Red,   EBaseDataType::Byte,      1,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R8U        = CxDefs::declareTextureFormat( 22, EPixelDataLayout::Red,   EBaseDataType::Ubyte,     1,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R8IN       = CxDefs::declareTextureFormat( 23, EPixelDataLayout::Red,   EBaseDataType::Byte,      1,  E_GPU_DATA_FORMAT_FLAG_SNORM ),
		R8UN       = CxDefs::declareTextureFormat( 24, EPixelDataLayout::Red,   EBaseDataType::Ubyte,     1,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		RG8I       = CxDefs::declareTextureFormat( 25, EPixelDataLayout::RG,    EBaseDataType::Byte,      2,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		RG8U       = CxDefs::declareTextureFormat( 26, EPixelDataLayout::RG,    EBaseDataType::Ubyte,     2,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		RG8IN      = CxDefs::declareTextureFormat( 27, EPixelDataLayout::RG,    EBaseDataType::Byte,      2,  E_GPU_DATA_FORMAT_FLAG_SNORM ),
		RG8UN      = CxDefs::declareTextureFormat( 28, EPixelDataLayout::RG,    EBaseDataType::Ubyte,     2,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		BGRX8UN    = CxDefs::declareTextureFormat( 29, EPixelDataLayout::RGBA,  EBaseDataType::Ubyte,     4,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		BGRX8SRGB  = CxDefs::declareTextureFormat( 30, EPixelDataLayout::RGBA,  EBaseDataType::Ubyte,     4,  E_GPU_DATA_FORMAT_FLAG_SRGB_BIT ),
		BGRA8UN    = CxDefs::declareTextureFormat( 31, EPixelDataLayout::RGBA,  EBaseDataType::Ubyte,     4,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		BGRA8SRGB  = CxDefs::declareTextureFormat( 32, EPixelDataLayout::RGBA,  EBaseDataType::Ubyte,     4,  E_GPU_DATA_FORMAT_FLAG_SRGB_BIT ),
		RGBA8I     = CxDefs::declareTextureFormat( 33, EPixelDataLayout::RGBA,  EBaseDataType::Byte,      4,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		RGBA8U     = CxDefs::declareTextureFormat( 34, EPixelDataLayout::RGBA,  EBaseDataType::Ubyte,     4,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		RGBA8IN    = CxDefs::declareTextureFormat( 35, EPixelDataLayout::RGBA,  EBaseDataType::Byte,      4,  E_GPU_DATA_FORMAT_FLAG_SNORM ),
		RGBA8UN    = CxDefs::declareTextureFormat( 36, EPixelDataLayout::RGBA,  EBaseDataType::Ubyte,     4,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		RGBA8SRGB  = CxDefs::declareTextureFormat( 37, EPixelDataLayout::RGBA,  EBaseDataType::Ubyte,     4,  E_GPU_DATA_FORMAT_FLAG_SRGB_BIT ),
		R5G5B5A1   = CxDefs::declareTextureFormat( 38, EPixelDataLayout::RGBA,  EBaseDataType::Ubyte,     2,  0 ),
		R5G6B5     = CxDefs::declareTextureFormat( 39, EPixelDataLayout::RGB,   EBaseDataType::Ubyte,     2,  0 ),
		R9G9B9E5   = CxDefs::declareTextureFormat( 40, EPixelDataLayout::RGBA,  EBaseDataType::Uint32,    4,  0 ),
		RGB10A2U   = CxDefs::declareTextureFormat( 41, EPixelDataLayout::RGBA,  EBaseDataType::Uint32,    4,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		RGB10A2UN  = CxDefs::declareTextureFormat( 42, EPixelDataLayout::RGBA,  EBaseDataType::Float32,   4,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		R11G11B10F = CxDefs::declareTextureFormat( 43, EPixelDataLayout::RGB,   EBaseDataType::Float32,   4,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		D16UN      = CxDefs::declareTextureFormat( 44, EPixelDataLayout::Depth, EBaseDataType::Uint16,    2,  0 ),
		D24UNS8U   = CxDefs::declareTextureFormat( 45, EPixelDataLayout::DS,    EBaseDataType::Uint32,    4,  0 ),
		D24UNX8    = CxDefs::declareTextureFormat( 46, EPixelDataLayout::DS,    EBaseDataType::Uint32,    4,  0 ),
		D32F       = CxDefs::declareTextureFormat( 47, EPixelDataLayout::DS,    EBaseDataType::Float32,   4,  0 ),
		BC1        = CxDefs::declareTextureFormat( 48, EPixelDataLayout::S3TC,  EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC1SRGB    = CxDefs::declareTextureFormat( 49, EPixelDataLayout::S3TC,  EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_SRGB ),
		BC2        = CxDefs::declareTextureFormat( 50, EPixelDataLayout::S3TC,  EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC2SRGB    = CxDefs::declareTextureFormat( 51, EPixelDataLayout::S3TC,  EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_SRGB ),
		BC3        = CxDefs::declareTextureFormat( 52, EPixelDataLayout::S3TC,  EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC3SRGB    = CxDefs::declareTextureFormat( 53, EPixelDataLayout::S3TC,  EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_SRGB ),
		BC4IN      = CxDefs::declareTextureFormat( 54, EPixelDataLayout::S3TC,  EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC4UN      = CxDefs::declareTextureFormat( 55, EPixelDataLayout::S3TC,  EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC5IN      = CxDefs::declareTextureFormat( 56, EPixelDataLayout::S3TC,  EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC5UN      = CxDefs::declareTextureFormat( 57, EPixelDataLayout::S3TC,  EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC6HSF     = CxDefs::declareTextureFormat( 58, EPixelDataLayout::S3TC,  EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC6HUF     = CxDefs::declareTextureFormat( 59, EPixelDataLayout::S3TC,  EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC7        = CxDefs::declareTextureFormat( 60, EPixelDataLayout::S3TC,  EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC7SRGB    = CxDefs::declareTextureFormat( 61, EPixelDataLayout::S3TC,  EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_SRGB ),
		UNKNOWN    = 0
	};

	enum class EVertexAttribFormat : vertex_attrib_format_value_t
	{
		F16       = CxDefs::declareVertexAttribFormat( 0,  EBaseDataType::Float16, 1, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		F32       = CxDefs::declareVertexAttribFormat( 1,  EBaseDataType::Float32, 1, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		I8        = CxDefs::declareVertexAttribFormat( 2,  EBaseDataType::Byte,    1, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		I16       = CxDefs::declareVertexAttribFormat( 3,  EBaseDataType::Int16,   1, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		I32       = CxDefs::declareVertexAttribFormat( 4,  EBaseDataType::Int32,   1, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		U8        = CxDefs::declareVertexAttribFormat( 5,  EBaseDataType::Ubyte,   1, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		U16       = CxDefs::declareVertexAttribFormat( 6,  EBaseDataType::Uint16,  1, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		U32       = CxDefs::declareVertexAttribFormat( 7,  EBaseDataType::Uint32,  1, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		I8N       = CxDefs::declareVertexAttribFormat( 8,  EBaseDataType::Byte,    1, E_GPU_DATA_FORMAT_FLAG_SNORM ),
		I16N      = CxDefs::declareVertexAttribFormat( 9,  EBaseDataType::Int16,   1, E_GPU_DATA_FORMAT_FLAG_SNORM ),
		U8N       = CxDefs::declareVertexAttribFormat( 10, EBaseDataType::Ubyte,   1, E_GPU_DATA_FORMAT_FLAG_UNORM ),
		U16N      = CxDefs::declareVertexAttribFormat( 11, EBaseDataType::Uint16,  1, E_GPU_DATA_FORMAT_FLAG_UNORM ),
		Vec2F16   = CxDefs::declareVertexAttribFormat( 12, EBaseDataType::Float16, 2, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		Vec2F32   = CxDefs::declareVertexAttribFormat( 13, EBaseDataType::Float32, 2, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		Vec2I8    = CxDefs::declareVertexAttribFormat( 14, EBaseDataType::Byte,    2, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		Vec2I16   = CxDefs::declareVertexAttribFormat( 15, EBaseDataType::Int16,   2, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		Vec2I32   = CxDefs::declareVertexAttribFormat( 16, EBaseDataType::Int32,   2, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		Vec2U8    = CxDefs::declareVertexAttribFormat( 17, EBaseDataType::Ubyte,   2, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		Vec2U16   = CxDefs::declareVertexAttribFormat( 18, EBaseDataType::Uint16,  2, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		Vec2U32   = CxDefs::declareVertexAttribFormat( 19, EBaseDataType::Uint32,  2, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		Vec2I8N   = CxDefs::declareVertexAttribFormat( 20, EBaseDataType::Byte,    2, E_GPU_DATA_FORMAT_FLAG_SNORM ),
		Vec2I16N  = CxDefs::declareVertexAttribFormat( 21, EBaseDataType::Int16,   2, E_GPU_DATA_FORMAT_FLAG_SNORM ),
		Vec2U8N   = CxDefs::declareVertexAttribFormat( 22, EBaseDataType::Ubyte,   2, E_GPU_DATA_FORMAT_FLAG_UNORM ),
		Vec2U16N  = CxDefs::declareVertexAttribFormat( 23, EBaseDataType::Uint16,  2, E_GPU_DATA_FORMAT_FLAG_UNORM ),
		Vec3F32   = CxDefs::declareVertexAttribFormat( 24, EBaseDataType::Float32, 3, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		Vec3I32   = CxDefs::declareVertexAttribFormat( 25, EBaseDataType::Int32,   3, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		Vec3U32   = CxDefs::declareVertexAttribFormat( 26, EBaseDataType::Uint32,  3, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		Vec4F16   = CxDefs::declareVertexAttribFormat( 27, EBaseDataType::Float16, 4, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		Vec4F32   = CxDefs::declareVertexAttribFormat( 28, EBaseDataType::Float32, 4, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		Vec4I8    = CxDefs::declareVertexAttribFormat( 29, EBaseDataType::Byte,    4, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		Vec4I16   = CxDefs::declareVertexAttribFormat( 30, EBaseDataType::Int16,   4, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		Vec4I32   = CxDefs::declareVertexAttribFormat( 31, EBaseDataType::Int32,   4, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		Vec4U8    = CxDefs::declareVertexAttribFormat( 32, EBaseDataType::Ubyte,   4, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		Vec4U16   = CxDefs::declareVertexAttribFormat( 33, EBaseDataType::Uint16,  4, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		Vec4U32   = CxDefs::declareVertexAttribFormat( 34, EBaseDataType::Uint32,  4, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		Vec4I8N   = CxDefs::declareVertexAttribFormat( 35, EBaseDataType::Byte,    4, E_GPU_DATA_FORMAT_FLAG_SNORM ),
		Vec4I16N  = CxDefs::declareVertexAttribFormat( 36, EBaseDataType::Int16,   4, E_GPU_DATA_FORMAT_FLAG_SNORM ),
		Vec4U8N   = CxDefs::declareVertexAttribFormat( 37, EBaseDataType::Ubyte,   4, E_GPU_DATA_FORMAT_FLAG_UNORM ),
		Vec4U16N  = CxDefs::declareVertexAttribFormat( 38, EBaseDataType::Uint16,  4, E_GPU_DATA_FORMAT_FLAG_UNORM ),
		Undefined = 0
	};

	using EShaderInputConstantFormat = EVertexAttribFormat;

	namespace CxDefs
	{

		TS3_ATTR_NO_DISCARD inline constexpr uint8 getBaseDataTypeIndex( EBaseDataType pBaseType ) noexcept
		{
			return ( uint8 )( ( base_data_type_value_t )pBaseType & 0xF );
		}

		TS3_ATTR_NO_DISCARD inline constexpr uint16 getBaseDataTypeByteSize( EBaseDataType pBaseType ) noexcept
		{
			return ( uint16 )( ( ( base_data_type_value_t )pBaseType >> 4 ) & 0xF );
		}

		TS3_ATTR_NO_DISCARD inline constexpr uint16 getIndexDataFormatByteSize( EIndexDataFormat pFormat ) noexcept
		{
			return getBaseDataTypeByteSize( static_cast<EBaseDataType>( pFormat ) );
		}

		TS3_ATTR_NO_DISCARD inline constexpr EPixelDataLayout getTextureFormatPixelDataLayout( ETextureFormat pFormat ) noexcept
		{
			return ( EPixelDataLayout )( ( ( gpu_pixel_format_value_t )pFormat >> 32 ) & 0xFF );
		}

		TS3_ATTR_NO_DISCARD inline constexpr EBaseDataType getTextureFormatBaseDataType( ETextureFormat pFormat ) noexcept
		{
			return ( EBaseDataType )( ( ( gpu_pixel_format_value_t )pFormat >> 24 ) & 0xFF );
		}

		TS3_ATTR_NO_DISCARD inline constexpr uint8 getTextureFormatByteSize( ETextureFormat pFormat ) noexcept
		{
			return ( uint8 )( ( ( gpu_pixel_format_value_t )pFormat >> 16 ) & 0xFF );
		}

		TS3_ATTR_NO_DISCARD inline constexpr uint8 getTextureFormatFlags( ETextureFormat pFormat ) noexcept
		{
			return ( uint8 )( ( ( gpu_pixel_format_value_t )pFormat >> 8 ) & 0xFF );
		}

		TS3_ATTR_NO_DISCARD inline constexpr EBaseDataType getVertexAttribFormatBaseDataType( EVertexAttribFormat pFormat ) noexcept
		{
			return ( EBaseDataType )( ( ( vertex_attrib_format_value_t )pFormat >> 24 ) & 0xFF );
		}

		TS3_ATTR_NO_DISCARD inline constexpr uint8 getVertexAttribFormatLength( EVertexAttribFormat pFormat ) noexcept
		{
			return ( uint8 )( ( ( vertex_attrib_format_value_t )pFormat >> 16 ) & 0xFF );
		}

		TS3_ATTR_NO_DISCARD inline constexpr Bitmask<uint8> getVertexAttribFormatFlags( EVertexAttribFormat pFormat )
		{
			return ( uint8 )( ( ( vertex_attrib_format_value_t )pFormat >> 8 ) & 0xFF );
		}

		TS3_ATTR_NO_DISCARD inline constexpr bool isTextureFormatSRGB( ETextureFormat pFormat ) noexcept
		{
			const Bitmask<uint8> formatFlags = getTextureFormatFlags( pFormat );
			return formatFlags.isSet( E_GPU_DATA_FORMAT_FLAG_SRGB_BIT );
		}

		TS3_ATTR_NO_DISCARD inline constexpr bool isVertexAttribFormatNormalized( EVertexAttribFormat pFormat ) noexcept
		{
			const Bitmask<uint8> formatFlags = getVertexAttribFormatFlags( pFormat );
			return formatFlags.isSet( E_GPU_DATA_FORMAT_FLAG_NORMALIZED_BIT );
		}

		TS3_ATTR_NO_DISCARD inline constexpr uint32 getVertexAttribFormatByteSize( EVertexAttribFormat pFormat ) noexcept
		{
			return getBaseDataTypeByteSize( getVertexAttribFormatBaseDataType( pFormat ) ) * getVertexAttribFormatLength( pFormat );
		}

	}

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_GPU_DATA_FORMATS_H__
