
#pragma once

#ifndef __TS3_GPUAPI_GPU_DATA_FORMATS_H__
#define __TS3_GPUAPI_GPU_DATA_FORMATS_H__

namespace ts3::gpuapi
{

	using base_data_type_value_t = uint8;
	using gpu_pixel_format_value_t = uint32;
	using vertex_attrib_format_value_t = uint32;

	enum class EBaseDataType : base_data_type_value_t;

	inline constexpr base_data_type_value_t ecDeclareBaseDataType( uint8 pIndex, uint8 pByteSize ) noexcept
	{
		return ( ( pByteSize & 0xF ) << 4 ) | ( pIndex & 0xF );
	}

	inline constexpr gpu_pixel_format_value_t ecDeclareTextureFormat( uint8 pIndex, EBaseDataType pBaseType, uint8 pByteSize, uint8 pFlags ) noexcept
	{
		return ( ( ( uint32 )pBaseType ) << 24 ) | ( ( ( uint32 )pByteSize ) << 16 ) | ( ( ( uint32 )pFlags ) << 8 ) | ( uint32 )pIndex;
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
		Byte         = ecDeclareBaseDataType( 0,  1 ),
		Ubyte        = ecDeclareBaseDataType( 1,  1 ),
		Int16        = ecDeclareBaseDataType( 2,  2 ),
		Uint16       = ecDeclareBaseDataType( 3,  2 ),
		Int32        = ecDeclareBaseDataType( 4,  4 ),
		Uint32       = ecDeclareBaseDataType( 5,  4 ),
		Int64        = ecDeclareBaseDataType( 6,  8 ),
		Uint64       = ecDeclareBaseDataType( 7,  8 ),
		Float16      = ecDeclareBaseDataType( 8,  2 ),
		Float32      = ecDeclareBaseDataType( 9,  4 ),
		Double       = ecDeclareBaseDataType( 10, 8 ),
		Uint24S8     = ecDeclareBaseDataType( 11, 4 ),
		Undefined    = ecDeclareBaseDataType( 12, 0 ),
	};

	enum class EIndexDataFormat : vertex_attrib_format_value_t
	{
		Uint16 = static_cast<vertex_attrib_format_value_t>( EBaseDataType::Uint16 ),
		Uint32 = static_cast<vertex_attrib_format_value_t>( EBaseDataType::Uint32 )
	};

	enum class ETextureFormat : gpu_pixel_format_value_t
	{
		R32_FLOAT              = ecDeclareTextureFormat( 0,  EBaseDataType::Float32,      4,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32_SINT               = ecDeclareTextureFormat( 1,  EBaseDataType::Int32,        4,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32_UINT               = ecDeclareTextureFormat( 2,  EBaseDataType::Uint32,       4,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R32G32_FLOAT           = ecDeclareTextureFormat( 3,  EBaseDataType::Float32,      8,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32G32_SINT            = ecDeclareTextureFormat( 4,  EBaseDataType::Int32,        8,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32G32_UINT            = ecDeclareTextureFormat( 5,  EBaseDataType::Uint32,       8,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R32G32B32_FLOAT        = ecDeclareTextureFormat( 6,  EBaseDataType::Float32,      12, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32G32B32_SINT         = ecDeclareTextureFormat( 7,  EBaseDataType::Int32,        12, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32G32B32_UINT         = ecDeclareTextureFormat( 8,  EBaseDataType::Uint32,       12, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R32G32B32A32_FLOAT     = ecDeclareTextureFormat( 9,  EBaseDataType::Float32,      16, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32G32B32A32_SINT      = ecDeclareTextureFormat( 10, EBaseDataType::Int32,        16, E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R32G32B32A32_UINT      = ecDeclareTextureFormat( 11, EBaseDataType::Uint32,       16, E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R16_FLOAT              = ecDeclareTextureFormat( 12, EBaseDataType::Float16,      2,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R16_SINT               = ecDeclareTextureFormat( 13, EBaseDataType::Int16,        2,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R16_UINT               = ecDeclareTextureFormat( 15, EBaseDataType::Uint16,       2,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R16_SNORM              = ecDeclareTextureFormat( 14, EBaseDataType::Int16,        2,  E_GPU_DATA_FORMAT_FLAG_SNORM ),
		R16_UNORM              = ecDeclareTextureFormat( 16, EBaseDataType::Uint16,       2,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		R16G16_FLOAT           = ecDeclareTextureFormat( 17, EBaseDataType::Float16,      4,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R16G16_SINT            = ecDeclareTextureFormat( 18, EBaseDataType::Int16,        4,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R16G16_UINT            = ecDeclareTextureFormat( 20, EBaseDataType::Uint16,       4,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R16G16_SNORM           = ecDeclareTextureFormat( 19, EBaseDataType::Int16,        4,  E_GPU_DATA_FORMAT_FLAG_SNORM ),
		R16G16_UNORM           = ecDeclareTextureFormat( 21, EBaseDataType::Uint16,       4,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		R16G16B16A16_FLOAT     = ecDeclareTextureFormat( 22, EBaseDataType::Float16,      8,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R16G16B16A16_SINT      = ecDeclareTextureFormat( 23, EBaseDataType::Int16,        8,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R16G16B16A16_UINT      = ecDeclareTextureFormat( 25, EBaseDataType::Uint16,       8,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R16G16B16A16_SNORM     = ecDeclareTextureFormat( 24, EBaseDataType::Int16,        8,  E_GPU_DATA_FORMAT_FLAG_SNORM ),
		R16G16B16A16_UNORM     = ecDeclareTextureFormat( 26, EBaseDataType::Uint16,       8,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		R8_SINT                = ecDeclareTextureFormat( 27, EBaseDataType::Byte,         1,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R8_UINT                = ecDeclareTextureFormat( 29, EBaseDataType::Ubyte,        1,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R8_SNORM               = ecDeclareTextureFormat( 28, EBaseDataType::Byte,         1,  E_GPU_DATA_FORMAT_FLAG_SNORM ),
		R8_UNORM               = ecDeclareTextureFormat( 30, EBaseDataType::Ubyte,        1,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		R8G8_SINT              = ecDeclareTextureFormat( 32, EBaseDataType::Byte,         2,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R8G8_UINT              = ecDeclareTextureFormat( 34, EBaseDataType::Ubyte,        2,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R8G8_SNORM             = ecDeclareTextureFormat( 33, EBaseDataType::Byte,         2,  E_GPU_DATA_FORMAT_FLAG_SNORM ),
		R8G8_UNORM             = ecDeclareTextureFormat( 35, EBaseDataType::Ubyte,        2,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		B8G8R8X8_UNORM         = ecDeclareTextureFormat( 45, EBaseDataType::Ubyte,        4,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		B8G8R8X8_SRGB          = ecDeclareTextureFormat( 46, EBaseDataType::Ubyte,        4,  E_GPU_DATA_FORMAT_FLAG_SRGB_BIT ),
		B8G8R8A8_UNORM         = ecDeclareTextureFormat( 55, EBaseDataType::Ubyte,        4,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		B8G8R8A8_SRGB          = ecDeclareTextureFormat( 56, EBaseDataType::Ubyte,        4,  E_GPU_DATA_FORMAT_FLAG_SRGB_BIT ),
		R8G8B8A8_SINT          = ecDeclareTextureFormat( 47, EBaseDataType::Byte,         4,  E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT ),
		R8G8B8A8_UINT          = ecDeclareTextureFormat( 49, EBaseDataType::Ubyte,        4,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R8G8B8A8_SNORM         = ecDeclareTextureFormat( 48, EBaseDataType::Byte,         4,  E_GPU_DATA_FORMAT_FLAG_SNORM ),
		R8G8B8A8_UNORM         = ecDeclareTextureFormat( 50, EBaseDataType::Ubyte,        4,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		R8G8B8A8_SRGB          = ecDeclareTextureFormat( 51, EBaseDataType::Ubyte,        4,  E_GPU_DATA_FORMAT_FLAG_SRGB_BIT ),
		R5G5B5A1               = ecDeclareTextureFormat( 57, EBaseDataType::Ubyte,        2,  0 ),
		R5G6B5                 = ecDeclareTextureFormat( 58, EBaseDataType::Ubyte,        2,  0 ),
		R9G9B9E5               = ecDeclareTextureFormat( 59, EBaseDataType::Uint32,       4,  0 ),
		R10G10B10A2_UINT       = ecDeclareTextureFormat( 60, EBaseDataType::Uint32,       4,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		R10G10B10A2_UNORM      = ecDeclareTextureFormat( 61, EBaseDataType::Float32,      4,  E_GPU_DATA_FORMAT_FLAG_UNORM ),
		R11G11B10_FLOAT32      = ecDeclareTextureFormat( 62, EBaseDataType::Float32,      4,  E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT ),
		D16_UNORM              = ecDeclareTextureFormat( 64, EBaseDataType::Uint16,       2,  0 ),
		D24_UNORM_S8_UINT      = ecDeclareTextureFormat( 65, EBaseDataType::Uint32,       4,  0 ),
		D24_UNORM_X8           = ecDeclareTextureFormat( 66, EBaseDataType::Uint32,       4,  0 ),
		D32_FLOAT              = ecDeclareTextureFormat( 67, EBaseDataType::Float32,      4,  0 ),
		BC1_RGBA_UNORM         = ecDeclareTextureFormat( 80, EBaseDataType::Undefined,    0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC1_RGBA_UNORM_SRGB    = ecDeclareTextureFormat( 81, EBaseDataType::Undefined,    0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_SRGB ),
		BC2_UNORM              = ecDeclareTextureFormat( 82, EBaseDataType::Undefined,    0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC2_UNORM_SRGB         = ecDeclareTextureFormat( 83, EBaseDataType::Undefined,    0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_SRGB ),
		BC3_UNORM              = ecDeclareTextureFormat( 84, EBaseDataType::Undefined,    0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC3_UNORM_SRGB         = ecDeclareTextureFormat( 85, EBaseDataType::Undefined,    0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_SRGB ),
		BC4_SNORM              = ecDeclareTextureFormat( 86, EBaseDataType::Undefined,    0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC4_UNORM              = ecDeclareTextureFormat( 87, EBaseDataType::Undefined,    0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC5_SNORM              = ecDeclareTextureFormat( 88, EBaseDataType::Undefined,    0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC5_UNORM              = ecDeclareTextureFormat( 89, EBaseDataType::Undefined,    0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC6H_SFLOAT            = ecDeclareTextureFormat( 90, EBaseDataType::Undefined,    0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC6H_UFLOAT            = ecDeclareTextureFormat( 91, EBaseDataType::Undefined,    0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC7_UNORM              = ecDeclareTextureFormat( 92, EBaseDataType::Undefined,    0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ),
		BC7_UNORM_SRGB         = ecDeclareTextureFormat( 93, EBaseDataType::Undefined,    0,  E_GPU_DATA_FORMAT_FLAG_COMPRESSED_SRGB ),
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
