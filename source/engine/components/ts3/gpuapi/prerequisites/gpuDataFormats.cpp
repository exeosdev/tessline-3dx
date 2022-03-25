
#include "../prerequisites.h"
#include <ts3/core/reflection/enumTypeInfo.h>

namespace ts3
{
namespace gpuapi
{

	ts3TypeInfoEnumDefine( EBaseDataType )
	{
		ts3TypeInfoEnumBegin( EBaseDataType );
		ts3TypeInfoEnumRegisterClassConstant( EBaseDataType::Byte      );
		ts3TypeInfoEnumRegisterClassConstant( EBaseDataType::Ubyte     );
		ts3TypeInfoEnumRegisterClassConstant( EBaseDataType::Int16     );
		ts3TypeInfoEnumRegisterClassConstant( EBaseDataType::Uint16    );
		ts3TypeInfoEnumRegisterClassConstant( EBaseDataType::Int32     );
		ts3TypeInfoEnumRegisterClassConstant( EBaseDataType::Uint32    );
		ts3TypeInfoEnumRegisterClassConstant( EBaseDataType::Float16   );
		ts3TypeInfoEnumRegisterClassConstant( EBaseDataType::Float32   );
		ts3TypeInfoEnumRegisterClassConstant( EBaseDataType::Uint24S8  );
		ts3TypeInfoEnumRegisterClassConstant( EBaseDataType::Undefined );
	}

	ts3TypeInfoEnumDefine( EIndexDataFormat )
	{
		ts3TypeInfoEnumBegin( EIndexDataFormat );
		ts3TypeInfoEnumRegisterClassConstant( EIndexDataFormat::Uint16 );
		ts3TypeInfoEnumRegisterClassConstant( EIndexDataFormat::Uint32 );
	}

	ts3TypeInfoEnumDefine( ETextureFormat )
	{
		ts3TypeInfoEnumBegin( ETextureFormat );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R32_FLOAT           );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R32_SINT            );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R32_UINT            );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R32G32_FLOAT        );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R32G32_SINT         );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R32G32_UINT         );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R32G32B32_FLOAT     );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R32G32B32_SINT      );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R32G32B32_UINT      );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R32G32B32A32_FLOAT  );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R32G32B32A32_SINT   );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R32G32B32A32_UINT   );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R16_FLOAT           );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R16_SINT            );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R16_UINT            );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R16G16_FLOAT        );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R16G16_SINT         );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R16G16_UINT         );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R16G16B16A16_FLOAT  );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R16G16B16A16_SINT   );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R16G16B16A16_UINT   );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R8_SINT             );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R8_UINT             );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R8_SNORM            );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R8_UNORM            );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R8G8_SINT           );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R8G8_UINT           );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R8G8_SNORM          );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R8G8_UNORM          );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::B8G8R8X8_UNORM      );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::B8G8R8X8_SRGB       );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::B8G8R8A8_UNORM      );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::B8G8R8A8_SRGB       );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R8G8B8A8_SINT       );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R8G8B8A8_UINT       );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R8G8B8A8_SNORM      );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R8G8B8A8_UNORM      );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R8G8B8A8_SRGB       );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R5G5B5A1            );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R5G6B5              );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R9G9B9E5            );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R10G10B10A2_UINT    );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R10G10B10A2_UNORM   );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::R11G11B10_FLOAT32   );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::D16_UNORM           );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::D24_UNORM_S8_UINT   );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::D24_UNORM_X8        );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::D32_FLOAT           );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::BC1_RGBA_UNORM      );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::BC1_RGBA_UNORM_SRGB );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::BC2_UNORM           );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::BC2_UNORM_SRGB      );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::BC3_UNORM           );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::BC3_UNORM_SRGB      );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::BC4_SNORM           );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::BC4_UNORM           );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::BC5_SNORM           );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::BC5_UNORM           );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::BC6H_SFLOAT         );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::BC6H_UFLOAT         );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::BC7_UNORM           );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::BC7_UNORM_SRGB      );
		ts3TypeInfoEnumRegisterClassConstant( ETextureFormat::UNKNOWN             );
	}

	ts3TypeInfoEnumDefine( EVertexAttribFormat )
	{
		ts3TypeInfoEnumBegin( EVertexAttribFormat );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::FLOAT16          );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::FLOAT32          );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::SBYTE            );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::SINT16           );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::SINT32           );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::UBYTE            );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::UINT16           );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::UINT32           );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::SBYTE_NORM       );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::SINT16_NORM      );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::UBYTE_NORM       );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::UINT16_NORM      );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC2_FLOAT16     );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC2_FLOAT32     );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC2_SBYTE       );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC2_SINT16      );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC2_SINT32      );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC2_UBYTE       );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC2_UINT16      );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC2_UINT32      );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC2_SBYTE_NORM  );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC2_SINT16_NORM );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC2_UBYTE_NORM  );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC2_UINT16_NORM );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC3_FLOAT32     );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC3_SINT32      );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC3_UINT32      );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC4_FLOAT16     );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC4_FLOAT32     );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC4_SBYTE       );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC4_SINT16      );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC4_SINT32      );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC4_UBYTE       );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC4_UINT16      );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC4_UINT32      );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC4_SBYTE_NORM  );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC4_SINT16_NORM );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC4_UBYTE_NORM  );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::VEC4_UINT16_NORM );
		ts3TypeInfoEnumRegisterClassConstant( EVertexAttribFormat::UNKNOWN          );
	}

} /* namespace ts3 */
} /* namespace gpuapi */
