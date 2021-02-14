
#include "vertexFormatDefs.h"

namespace ts3
{

	const gpuapi::VertexInputFormatDesc GPUVertexInputFormatDescForVertexStruct<GPUVertexFormat2DDefault>::sFormat =
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

	const gpuapi::VertexInputFormatDesc GPUVertexInputFormatDescForVertexStruct<GPUVertexFormat2DUI>::sFormat =
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

	const gpuapi::VertexInputFormatDesc GPUVertexInputFormatDescForVertexStruct<GPUVertexFormat3DDefault>::sFormat =
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

}
