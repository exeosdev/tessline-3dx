
#include "../prerequisites.h"

namespace ts3::GpuAPI
{

//	TS3_EnumDefineTypeInfoGx( EBaseDataType )
//	{
//		TS3_EnumTypeInfoBegin( EBaseDataType );
//		TS3_EnumTypeInfoRegisterClassConstant( EBaseDataType::Byte );
//		TS3_EnumTypeInfoRegisterClassConstant( EBaseDataType::Ubyte );
//		TS3_EnumTypeInfoRegisterClassConstant( EBaseDataType::Int16 );
//		TS3_EnumTypeInfoRegisterClassConstant( EBaseDataType::Uint16 );
//		TS3_EnumTypeInfoRegisterClassConstant( EBaseDataType::Int32 );
//		TS3_EnumTypeInfoRegisterClassConstant( EBaseDataType::Uint32 );
//		TS3_EnumTypeInfoRegisterClassConstant( EBaseDataType::Int64 );
//		TS3_EnumTypeInfoRegisterClassConstant( EBaseDataType::Uint64 );
//		TS3_EnumTypeInfoRegisterClassConstant( EBaseDataType::Float );
//		TS3_EnumTypeInfoRegisterClassConstant( EBaseDataType::Double );
//	}
//
//	TS3_EnumDefineTypeInfoGx( EIndexDataFormat )
//	{
//		TS3_EnumTypeInfoBegin( EIndexDataFormat );
//		TS3_EnumTypeInfoRegisterClassConstant( EIndexDataFormat::Uint16 );
//		TS3_EnumTypeInfoRegisterClassConstant( EIndexDataFormat::Uint32 );
//	}
//
//	TS3_EnumDefineTypeInfoGx( ETextureFormat )
//	{
//		TS3_EnumTypeInfoBegin( ETextureFormat );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R32_SFLOAT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R32_SINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R32_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R32G32_SFLOAT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R32G32_SINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R32G32_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R32G32B32_SFLOAT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R32G32B32_SINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R32G32B32_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R32G32B32A32_SFLOAT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R32G32B32A32_SINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R32G32B32A32_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R16_SFLOAT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R16_SINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R16_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R16_SNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R16_UNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R16G16_SFLOAT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R16G16_SINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R16G16_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R16G16_SNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R16G16_UNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R16G16B16A16_SFLOAT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R16G16B16A16_SINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R16G16B16A16_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R16G16B16A16_SNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R16G16B16A16_UNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8_SINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8_SNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8_UNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8_SRGB );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8G8_SINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8G8_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8G8_SNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8G8_UNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8G8_SRGB );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8G8B8_SINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8G8B8_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8G8B8_SNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8G8B8_UNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8G8B8_SRGB );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::B8G8R8_SINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::B8G8R8_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::B8G8R8_SNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::B8G8R8_UNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::B8G8R8_SRGB );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8G8B8A8_SINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8G8B8A8_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8G8B8A8_SNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8G8B8A8_UNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R8G8B8A8_SRGB );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::B8G8R8A8_SINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::B8G8R8A8_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::B8G8R8A8_SNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::B8G8R8A8_UNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::B8G8R8A8_SRGB );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R5G5B5A1 );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R5G6B5 );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R9G9B9E5 );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R10G10B10A2_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R10G10B10A2_UNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::R11G11B10_SFLOAT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::D16_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::D16_UNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::D16_UNORM_S8_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::D24_UNORM_S8_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::D24_UNORM_X8 );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::D32_SFLOAT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::D32_SFLOAT_S8_UINT );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::A8_UNORM );
//		TS3_EnumTypeInfoRegisterClassConstant( ETextureFormat::UNKNOWN );
//	}
//
//	TS3_EnumDefineTypeInfoGx( EVertexAttribFormat )
//	{
//		TS3_EnumTypeInfoBegin( EVertexAttribFormat );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::SFLOAT );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::SBYTE );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::SINT16 );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::SINT32 );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::UBYTE );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::UINT16 );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::UINT32 );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::SBYTE_NORM );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::SINT16_NORM );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::UBYTE_NORM );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::UINT16_NORM );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC2_SFLOAT );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC2_SBYTE );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC2_SINT16 );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC2_SINT32 );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC2_UBYTE );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC2_UINT16 );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC2_UINT32 );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC2_SBYTE_NORM );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC2_SINT16_NORM );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC2_UBYTE_NORM );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC2_UINT16_NORM );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC3_SFLOAT );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC3_SBYTE );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC3_SINT16 );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC3_SINT32 );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC3_UBYTE );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC3_UINT16 );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC3_UINT32 );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC3_SBYTE_NORM );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC3_SINT16_NORM );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC3_UBYTE_NORM );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC3_UINT16_NORM );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC4_SFLOAT );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC4_SBYTE );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC4_SINT16 );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC4_SINT32 );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC4_UBYTE );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC4_UINT16 );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC4_UINT32 );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC4_SBYTE_NORM );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC4_SINT16_NORM );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC4_UBYTE_NORM );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::VEC4_UINT16_NORM );
//		TS3_EnumTypeInfoRegisterClassConstant( EVertexAttribFormat::UNKNOWN );
//	}

} // namespace ts3::GpuAPI
