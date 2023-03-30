
#ifndef __TS3_ENGINE_VERTEX_FORMAT_DEFS_H__
#define __TS3_ENGINE_VERTEX_FORMAT_DEFS_H__

#include "commonGpaDefs.h"
#include <ts3/gpuapi/state/pipelineStateDesc.h>

namespace ts3
{

	enum class EGPUVertexFormatID : enum_default_value_t
	{
		VF2DDefault,
		VF2DUI,
		VF3DDefault
	};

	struct VertexIndexData
	{
		EGPUVertexFormatID vertexFormatID;

		gpuapi::EIndexDataFormat indexFormat;

		gpuapi::VertexStreamStateObjectHandle vertexStreamSO;
	};

	struct alignas( 16 ) GPUVertexFormatData2DDefault
	{
		math::Vec2f position;
		math::Vec2f texCoord0;
		math::RGBAColorU8 color;
	};

	struct alignas( 16 ) GPUVertexFormatData2DUI
	{
		math::Vec3f position;
		math::Vec3f texCoord0;
		math::RGBAColorU8 color;
	};

	struct alignas( 16 ) GPUVertexFormatData3DDefault
	{
		math::Vec3f position;
		math::Vec3f texCoord0;
		math::RGBAColorU8 color;
	};

	TS3_ENGINE_OBJ const gpuapi::VertexInputFormatDesc cvVertexInputFormatDesc2DDefault;
	TS3_ENGINE_OBJ const gpuapi::VertexInputFormatDesc cvVertexInputFormatDesc2DUI;
	TS3_ENGINE_OBJ const gpuapi::VertexInputFormatDesc cvVertexInputFormatDesc3DDefault;

	TS3_ENGINE_API const gpuapi::VertexInputFormatDesc & queryVertexInputFormatDesc( EGPUVertexFormatID pFormatID );

} // namespace ts3

#endif // __TS3_ENGINE_VERTEX_FORMAT_DEFS_H__
