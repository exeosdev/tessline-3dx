
#include "vertexFormatDefs.h"

namespace ts3
{

	const gpuapi::VertexInputFormatDesc & queryVertexInputFormatDesc( EGPUVertexFormatID pFormatID )
	{
		switch( pFormatID )
		{
			case EGPUVertexFormatID::VF2DDefault:
				return cvVertexInputFormatDesc2DDefault;

			case EGPUVertexFormatID::VF2DUI:
				return cvVertexInputFormatDesc2DUI;

			case EGPUVertexFormatID::VF3DDefault:
				return cvVertexInputFormatDesc3DDefault;

			default:
				break;
		}

		throw 0;
	}

	const gpuapi::VertexInputFormatDesc cvVertexInputFormatDesc2DDefault =
	{
		ts3gaVertexAttributeDesc( 0,
		                          0,
		                          "POSITION",
		                          0,
		                          gpuapi::EVertexAttribFormat::VEC2_FLOAT32,
		                          0,
		                          0 ),

		ts3gaVertexAttributeDesc( 1,
		                          0,
		                          "TEXCOORD0",
		                          0,
		                          gpuapi::EVertexAttribFormat::VEC2_FLOAT32,
		                          8,
		                          0 ),

		ts3gaVertexAttributeDesc( 2,
		                          0,
		                          "COLOR",
		                          0,
		                          gpuapi::EVertexAttribFormat::VEC4_UBYTE_NORM,
		                          16,
		                          0 )
	};

	const gpuapi::VertexInputFormatDesc cvVertexInputFormatDesc2DUI =
	{
		ts3gaVertexAttributeDesc( 0,
		                          0,
		                          "POSITION",
		                          0,
		                          gpuapi::EVertexAttribFormat::VEC3_FLOAT32,
		                          0,
		                          0 ),

		ts3gaVertexAttributeDesc( 1,
		                          0,
		                          "TEXCOORD0",
		                          0,
		                          gpuapi::EVertexAttribFormat::VEC3_FLOAT32,
		                          12,
		                          0 ),

		ts3gaVertexAttributeDesc( 2,
		                          0,
		                          "COLOR",
		                          0,
		                          gpuapi::EVertexAttribFormat::VEC4_UBYTE_NORM,
		                          24,
		                          0 )
	};

	const gpuapi::VertexInputFormatDesc cvVertexInputFormatDesc3DDefault =
	{
		ts3gaVertexAttributeDesc( 0,
		                          0,
		                          "POSITION",
		                          0,
		                          gpuapi::EVertexAttribFormat::VEC3_FLOAT32,
		                          0,
		                          0 ),

		ts3gaVertexAttributeDesc( 1,
		                          0,
		                          "COLOR",
		                          0,
		                          gpuapi::EVertexAttribFormat::VEC4_UBYTE_NORM,
		                          12,
		                          0 )
	};

} // namespace ts3
