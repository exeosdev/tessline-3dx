
#pragma once

#ifndef __TS3_GPUAPI_GPU_DATA_FORMATS_H__
#define __TS3_GPUAPI_GPU_DATA_FORMATS_H__

namespace ts3::gpuapi
{

	using base_data_type_value_t = uint8;
	using gpu_pixel_format_value_t = uint64;
	using vertex_attrib_format_value_t = uint32;

	enum class EBaseDataType : base_data_type_value_t;

	inline constexpr base_data_type_value_t ecDeclareBaseDataType( uint8 pIndex, uint8 pByteSize ) noexcept
	{
		return ( ( pByteSize & 0xF ) << 4 ) | ( pIndex & 0xF );
	}

	inline constexpr gpu_pixel_format_value_t ecDeclareTextureFormat( uint8 pIndex, EPixelDataLayout pPixelLayout, EBaseDataType pBaseType, uint8 pByteSize, uint8 pFlags ) noexcept
	{
	    return ( ( ( uint64 )pPixelLayout ) << 32 ) | ( ( ( uint64 )pBaseType ) << 24 ) | ( ( ( uint64 )pByteSize ) << 16 ) | ( ( ( uint64 )pFlags ) << 8 ) | ( uint64 )pIndex;
	}

	inline constexpr vertex_attrib_format_value_t ecDeclareVertexAttribFormat( uint8 pIndex, EBaseDataType pBaseType, uint8 pLength, uint8 pFlags ) noexcept
	{
	    return ( ( ( uint32 )pBaseType ) << 24 ) | ( ( ( uint32 )pLength ) << 16 ) | ( ( ( uint32 )pFlags ) << 8 ) | ( uint32 )pIndex;
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
		Byte         = ecDeclareBaseDataType( 0, 1 ),
		Ubyte        = ecDeclareBaseDataType( 1, 1 ),
		Int16        = ecDeclareBaseDataType( 2, 2 ),
		Uint16       = ecDeclareBaseDataType( 3, 2 ),
		Int32        = ecDeclareBaseDataType( 4, 4 ),
		Uint32       = ecDeclareBaseDataType( 5, 4 ),
		Float16      = ecDeclareBaseDataType( 6, 2 ),
		Float32      = ecDeclareBaseDataType( 7, 4 ),
		Uint24S8     = ecDeclareBaseDataType( 8, 4 ),
		Undefined    = ecDeclareBaseDataType( 9, 0 ),
	};

	enum class EIndexDataFormat : vertex_attrib_format_value_t
	{
		Uint16 = static_cast<vertex_attrib_format_value_t>( EBaseDataType::Uint16 ),
		Uint32 = static_cast<vertex_attrib_format_value_t>( EBaseDataType::Uint32 )
	};

	enum class ETextureFormat : gpu_pixel_format_value_t
	{
		R32_FLOAT              = ecDeclareTextureFormat( 0,  EPixelDataLayout::Red,          EBaseDataType::Float32,   4,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32_SINT               = ecDeclareTextureFormat( 1,  EPixelDataLayout::Red,          EBaseDataType::Int32,     4,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32_UINT               = ecDeclareTextureFormat( 2,  EPixelDataLayout::Red,          EBaseDataType::Uint32,    4,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R32G32_FLOAT           = ecDeclareTextureFormat( 3,  EPixelDataLayout::RG,           EBaseDataType::Float32,   8,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32G32_SINT            = ecDeclareTextureFormat( 4,  EPixelDataLayout::RG,           EBaseDataType::Int32,     8,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32G32_UINT            = ecDeclareTextureFormat( 5,  EPixelDataLayout::RG,           EBaseDataType::Uint32,    8,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R32G32B32_FLOAT        = ecDeclareTextureFormat( 6,  EPixelDataLayout::RGB,          EBaseDataType::Float32,   12, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32G32B32_SINT         = ecDeclareTextureFormat( 7,  EPixelDataLayout::RGB,          EBaseDataType::Int32,     12, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32G32B32_UINT         = ecDeclareTextureFormat( 8,  EPixelDataLayout::RGB,          EBaseDataType::Uint32,    12, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R32G32B32A32_FLOAT     = ecDeclareTextureFormat( 9,  EPixelDataLayout::RGBA,         EBaseDataType::Float32,   16, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32G32B32A32_SINT      = ecDeclareTextureFormat( 10, EPixelDataLayout::RGBA,         EBaseDataType::Int32,     16, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32G32B32A32_UINT      = ecDeclareTextureFormat( 11, EPixelDataLayout::RGBA,         EBaseDataType::Uint32,    16, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R16_FLOAT              = ecDeclareTextureFormat( 12, EPixelDataLayout::RG,           EBaseDataType::Float16,   2,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R16_SINT               = ecDeclareTextureFormat( 13, EPixelDataLayout::RG,           EBaseDataType::Int16,     2,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R16_UINT               = ecDeclareTextureFormat( 14, EPixelDataLayout::RG,           EBaseDataType::Uint16,    2,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R16G16_FLOAT           = ecDeclareTextureFormat( 15, EPixelDataLayout::RG,           EBaseDataType::Float16,   4,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R16G16_SINT            = ecDeclareTextureFormat( 16, EPixelDataLayout::RG,           EBaseDataType::Int16,     4,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R16G16_UINT            = ecDeclareTextureFormat( 17, EPixelDataLayout::RG,           EBaseDataType::Uint16,    4,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R16G16B16A16_FLOAT     = ecDeclareTextureFormat( 18, EPixelDataLayout::RGBA,         EBaseDataType::Float16,   8,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R16G16B16A16_SINT      = ecDeclareTextureFormat( 19, EPixelDataLayout::RGBA,         EBaseDataType::Int16,     8,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R16G16B16A16_UINT      = ecDeclareTextureFormat( 20, EPixelDataLayout::RGBA,         EBaseDataType::Uint16,    8,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R8_SINT                = ecDeclareTextureFormat( 21, EPixelDataLayout::Red,          EBaseDataType::Byte,      1,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R8_UINT                = ecDeclareTextureFormat( 22, EPixelDataLayout::Red,          EBaseDataType::Ubyte,     1,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R8_SNORM               = ecDeclareTextureFormat( 23, EPixelDataLayout::Red,          EBaseDataType::Byte,      1,  E_GPU_DATA_FORMAT_FLAG_SNORM ),
		R8_UNORM               = ecDeclareTextureFormat( 24, EPixelDataLayout::Red,          EBaseDataType::Ubyte,     1,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		R8G8_SINT              = ecDeclareTextureFormat( 25, EPixelDataLayout::RG,           EBaseDataType::Byte,      2,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R8G8_UINT              = ecDeclareTextureFormat( 26, EPixelDataLayout::RG,           EBaseDataType::Ubyte,     2,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R8G8_SNORM             = ecDeclareTextureFormat( 27, EPixelDataLayout::RG,           EBaseDataType::Byte,      2,  E_GPU_DATA_FORMAT_FLAG_SNORM ),
		R8G8_UNORM             = ecDeclareTextureFormat( 28, EPixelDataLayout::RG,           EBaseDataType::Ubyte,     2,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		B8G8R8X8_UNORM         = ecDeclareTextureFormat( 29, EPixelDataLayout::RGBA,         EBaseDataType::Ubyte,     4,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		B8G8R8X8_SRGB          = ecDeclareTextureFormat( 30, EPixelDataLayout::RGBA,         EBaseDataType::Ubyte,     4,  E_GPU_DATA_FORMAT_FLAG_SRGB_BIT ),
		B8G8R8A8_UNORM         = ecDeclareTextureFormat( 31, EPixelDataLayout::RGBA,         EBaseDataType::Ubyte,     4,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		B8G8R8A8_SRGB          = ecDeclareTextureFormat( 32, EPixelDataLayout::RGBA,         EBaseDataType::Ubyte,     4,  E_GPU_DATA_FORMAT_FLAG_SRGB_BIT ),
		R8G8B8A8_SINT          = ecDeclareTextureFormat( 33, EPixelDataLayout::RGBA,         EBaseDataType::Byte,      4,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R8G8B8A8_UINT          = ecDeclareTextureFormat( 34, EPixelDataLayout::RGBA,         EBaseDataType::Ubyte,     4,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R8G8B8A8_SNORM         = ecDeclareTextureFormat( 35, EPixelDataLayout::RGBA,         EBaseDataType::Byte,      4,  E_GPU_DATA_FORMAT_FLAG_SNORM ),
		R8G8B8A8_UNORM         = ecDeclareTextureFormat( 36, EPixelDataLayout::RGBA,         EBaseDataType::Ubyte,     4,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		R8G8B8A8_SRGB          = ecDeclareTextureFormat( 37, EPixelDataLayout::RGBA,         EBaseDataType::Ubyte,     4,  E_GPU_DATA_FORMAT_FLAG_SRGB_BIT ),
		R5G5B5A1               = ecDeclareTextureFormat( 38, EPixelDataLayout::RGBA,         EBaseDataType::Ubyte,     2,  0 ),
		R5G6B5                 = ecDeclareTextureFormat( 39, EPixelDataLayout::RGB,          EBaseDataType::Ubyte,     2,  0 ),
		R9G9B9E5               = ecDeclareTextureFormat( 40, EPixelDataLayout::RGBA,         EBaseDataType::Uint32,    4,  0 ),
		R10G10B10A2_UINT       = ecDeclareTextureFormat( 41, EPixelDataLayout::RGBA,         EBaseDataType::Uint32,    4,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R10G10B10A2_UNORM      = ecDeclareTextureFormat( 42, EPixelDataLayout::RGBA,         EBaseDataType::Float32,   4,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		R11G11B10_FLOAT32      = ecDeclareTextureFormat( 43, EPixelDataLayout::RGB,          EBaseDataType::Float32,   4,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		D16_UNORM              = ecDeclareTextureFormat( 44, EPixelDataLayout::Depth,        EBaseDataType::Uint16,    2,  0 ),
		D24_UNORM_S8_UINT      = ecDeclareTextureFormat( 45, EPixelDataLayout::DepthStencil, EBaseDataType::Uint32,    4,  0 ),
		D24_UNORM_X8           = ecDeclareTextureFormat( 46, EPixelDataLayout::DepthStencil, EBaseDataType::Uint32,    4,  0 ),
		D32_FLOAT              = ecDeclareTextureFormat( 47, EPixelDataLayout::DepthStencil, EBaseDataType::Float32,   4,  0 ),
		BC1_RGBA_UNORM         = ecDeclareTextureFormat( 48, EPixelDataLayout::S3TC,         EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC1_RGBA_UNORM_SRGB    = ecDeclareTextureFormat( 49, EPixelDataLayout::S3TC,         EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_SRGB ),
		BC2_UNORM              = ecDeclareTextureFormat( 50, EPixelDataLayout::S3TC,         EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC2_UNORM_SRGB         = ecDeclareTextureFormat( 51, EPixelDataLayout::S3TC,         EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_SRGB ),
		BC3_UNORM              = ecDeclareTextureFormat( 52, EPixelDataLayout::S3TC,         EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC3_UNORM_SRGB         = ecDeclareTextureFormat( 53, EPixelDataLayout::S3TC,         EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_SRGB ),
		BC4_SNORM              = ecDeclareTextureFormat( 54, EPixelDataLayout::S3TC,         EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC4_UNORM              = ecDeclareTextureFormat( 55, EPixelDataLayout::S3TC,         EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC5_SNORM              = ecDeclareTextureFormat( 56, EPixelDataLayout::S3TC,         EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC5_UNORM              = ecDeclareTextureFormat( 57, EPixelDataLayout::S3TC,         EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC6H_SFLOAT            = ecDeclareTextureFormat( 58, EPixelDataLayout::S3TC,         EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC6H_UFLOAT            = ecDeclareTextureFormat( 59, EPixelDataLayout::S3TC,         EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC7_UNORM              = ecDeclareTextureFormat( 60, EPixelDataLayout::S3TC,         EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC7_UNORM_SRGB         = ecDeclareTextureFormat( 61, EPixelDataLayout::S3TC,         EBaseDataType::Undefined, 0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_SRGB ),
		UNKNOWN                = 0
	};

	enum class EVertexAttribFormat : vertex_attrib_format_value_t
	{
		FLOAT16          = ecDeclareVertexAttribFormat( 0,  EBaseDataType::Float16, 1, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		FLOAT32          = ecDeclareVertexAttribFormat( 1,  EBaseDataType::Float32, 1, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		SBYTE            = ecDeclareVertexAttribFormat( 2,  EBaseDataType::Byte,    1, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		SINT16           = ecDeclareVertexAttribFormat( 3,  EBaseDataType::Int16,   1, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		SINT32           = ecDeclareVertexAttribFormat( 4,  EBaseDataType::Int32,   1, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		UBYTE            = ecDeclareVertexAttribFormat( 5,  EBaseDataType::Ubyte,   1, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		UINT16           = ecDeclareVertexAttribFormat( 6,  EBaseDataType::Uint16,  1, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		UINT32           = ecDeclareVertexAttribFormat( 7,  EBaseDataType::Uint32,  1, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		SBYTE_NORM       = ecDeclareVertexAttribFormat( 8,  EBaseDataType::Byte,    1, E_GPU_DATA_FORMAT_FLAG_SNORM ),
		SINT16_NORM      = ecDeclareVertexAttribFormat( 9,  EBaseDataType::Int16,   1, E_GPU_DATA_FORMAT_FLAG_SNORM ),
		UBYTE_NORM       = ecDeclareVertexAttribFormat( 10, EBaseDataType::Ubyte,   1, E_GPU_DATA_FORMAT_FLAG_UNORM ),
		UINT16_NORM      = ecDeclareVertexAttribFormat( 11, EBaseDataType::Uint16,  1, E_GPU_DATA_FORMAT_FLAG_UNORM ),
		VEC2_FLOAT16     = ecDeclareVertexAttribFormat( 12, EBaseDataType::Float16, 2, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		VEC2_FLOAT32     = ecDeclareVertexAttribFormat( 13, EBaseDataType::Float32, 2, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		VEC2_SBYTE       = ecDeclareVertexAttribFormat( 14, EBaseDataType::Byte,    2, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		VEC2_SINT16      = ecDeclareVertexAttribFormat( 15, EBaseDataType::Int16,   2, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		VEC2_SINT32      = ecDeclareVertexAttribFormat( 16, EBaseDataType::Int32,   2, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		VEC2_UBYTE       = ecDeclareVertexAttribFormat( 17, EBaseDataType::Ubyte,   2, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		VEC2_UINT16      = ecDeclareVertexAttribFormat( 18, EBaseDataType::Uint16,  2, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		VEC2_UINT32      = ecDeclareVertexAttribFormat( 19, EBaseDataType::Uint32,  2, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		VEC2_SBYTE_NORM  = ecDeclareVertexAttribFormat( 20, EBaseDataType::Byte,    2, E_GPU_DATA_FORMAT_FLAG_SNORM ),
		VEC2_SINT16_NORM = ecDeclareVertexAttribFormat( 21, EBaseDataType::Int16,   2, E_GPU_DATA_FORMAT_FLAG_SNORM ),
		VEC2_UBYTE_NORM  = ecDeclareVertexAttribFormat( 22, EBaseDataType::Ubyte,   2, E_GPU_DATA_FORMAT_FLAG_UNORM ),
		VEC2_UINT16_NORM = ecDeclareVertexAttribFormat( 23, EBaseDataType::Uint16,  2, E_GPU_DATA_FORMAT_FLAG_UNORM ),
		VEC3_FLOAT32     = ecDeclareVertexAttribFormat( 24, EBaseDataType::Float32, 3, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		VEC3_SINT32      = ecDeclareVertexAttribFormat( 25, EBaseDataType::Int32,   3, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		VEC3_UINT32      = ecDeclareVertexAttribFormat( 26, EBaseDataType::Uint32,  3, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		VEC4_FLOAT16     = ecDeclareVertexAttribFormat( 27, EBaseDataType::Float16, 4, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		VEC4_FLOAT32     = ecDeclareVertexAttribFormat( 28, EBaseDataType::Float32, 4, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		VEC4_SBYTE       = ecDeclareVertexAttribFormat( 29, EBaseDataType::Byte,    4, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		VEC4_SINT16      = ecDeclareVertexAttribFormat( 30, EBaseDataType::Int16,   4, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		VEC4_SINT32      = ecDeclareVertexAttribFormat( 31, EBaseDataType::Int32,   4, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		VEC4_UBYTE       = ecDeclareVertexAttribFormat( 32, EBaseDataType::Ubyte,   4, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		VEC4_UINT16      = ecDeclareVertexAttribFormat( 33, EBaseDataType::Uint16,  4, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		VEC4_UINT32      = ecDeclareVertexAttribFormat( 34, EBaseDataType::Uint32,  4, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		VEC4_SBYTE_NORM  = ecDeclareVertexAttribFormat( 35, EBaseDataType::Byte,    4, E_GPU_DATA_FORMAT_FLAG_SNORM ),
		VEC4_SINT16_NORM = ecDeclareVertexAttribFormat( 36, EBaseDataType::Int16,   4, E_GPU_DATA_FORMAT_FLAG_SNORM ),
		VEC4_UBYTE_NORM  = ecDeclareVertexAttribFormat( 37, EBaseDataType::Ubyte,   4, E_GPU_DATA_FORMAT_FLAG_UNORM ),
		VEC4_UINT16_NORM = ecDeclareVertexAttribFormat( 38, EBaseDataType::Uint16,  4, E_GPU_DATA_FORMAT_FLAG_UNORM ),
		UNKNOWN          = 0
	};

	using ShaderInputConstantFormat = EVertexAttribFormat;

	inline constexpr uint8 ecGetBaseDataTypeIndex( EBaseDataType pBaseType ) noexcept
	{
		return ( uint8 )( ( base_data_type_value_t )pBaseType & 0xF );
	}

	inline constexpr uint16 ecGetBaseDataTypeByteSize( EBaseDataType pBaseType ) noexcept
	{
		return ( uint16 )( ( ( base_data_type_value_t )pBaseType >> 4 ) & 0xF );
	}

	inline constexpr EBaseDataType ecGetIndexDataFormatBaseDataType( EIndexDataFormat pFormat ) noexcept
	{
		return static_cast< EBaseDataType >( pFormat );
	}

	inline constexpr EPixelDataLayout ecGetTextureFormatPixelDataLayout( ETextureFormat pFormat ) noexcept
	{
	    return ( EPixelDataLayout )( ( ( gpu_pixel_format_value_t )pFormat >> 32 ) & 0xFF );
	}

	inline constexpr EBaseDataType ecGetTextureFormatBaseDataType( ETextureFormat pFormat ) noexcept
	{
		return ( EBaseDataType )( ( ( gpu_pixel_format_value_t )pFormat >> 24 ) & 0xFF );
	}

	inline constexpr uint8 ecGetTextureFormatByteSize( ETextureFormat pFormat ) noexcept
	{
		return ( uint8 )( ( ( gpu_pixel_format_value_t )pFormat >> 16 ) & 0xFF );
	}

	inline constexpr uint8 ecGetTextureFormatFlags( ETextureFormat pFormat ) noexcept
	{
		return ( uint8 )( ( ( gpu_pixel_format_value_t )pFormat >> 8 ) & 0xFF );
	}

	inline constexpr EBaseDataType ecGetVertexAttribFormatBaseDataType( EVertexAttribFormat pFormat ) noexcept
	{
		return ( EBaseDataType )( ( ( vertex_attrib_format_value_t )pFormat >> 24 ) & 0xFF );
	}

	inline constexpr uint8 ecGetVertexAttribFormatLength( EVertexAttribFormat pFormat ) noexcept
	{
		return ( uint8 )( ( ( vertex_attrib_format_value_t )pFormat >> 16 ) & 0xFF );
	}

	inline constexpr Bitmask<uint8> ecGetVertexAttribFormatFlags( EVertexAttribFormat pFormat )
	{
		return ( uint8 )( ( ( vertex_attrib_format_value_t )pFormat >> 8 ) & 0xFF );
	}

	inline constexpr bool ecIsTextureFormatSRGB( ETextureFormat pFormat ) noexcept
	{
		const Bitmask<uint8> formatFlags = ecGetTextureFormatFlags( pFormat );
		return formatFlags.isSet( E_GPU_DATA_FORMAT_FLAG_SRGB_BIT );
	}

	inline constexpr bool ecIsVertexAttribFormatNormalized( EVertexAttribFormat pFormat ) noexcept
	{
		const Bitmask<uint8> formatFlags = ecGetVertexAttribFormatFlags( pFormat );
		return formatFlags.isSet( E_GPU_DATA_FORMAT_FLAG_NORMALIZED_BIT );
	}

	inline constexpr uint32 ecGetVertexAttribFormatByteSize( EVertexAttribFormat pFormat ) noexcept
	{
		return ecGetBaseDataTypeByteSize( ecGetVertexAttribFormatBaseDataType( pFormat ) ) * ecGetVertexAttribFormatLength( pFormat );
	}

}

#endif // __TS3_GPUAPI_GPU_DATA_FORMATS_H__
