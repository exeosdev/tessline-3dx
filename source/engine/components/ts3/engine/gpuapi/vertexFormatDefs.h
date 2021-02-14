
#ifndef __TS3_ENGINE_VERTEX_FORMAT_DEFS_H__
#define __TS3_ENGINE_VERTEX_FORMAT_DEFS_H__

#include "commonGPUDefs.h"
#include <ts3/gpuapi/state/pipelineStateDesc.h>

namespace ts3
{

	struct alignas( 16 ) GPUVertexFormat2DDefault
	{
		math::Vec2f position;
		math::Vec2f texCoord0;
		math::RGBAColorU8 color;
	};

	struct alignas( 16 ) GPUVertexFormat2DUI
	{
		math::Vec3f position;
		math::Vec3f texCoord0;
		math::RGBAColorU8 color;
	};

	struct alignas( 16 ) GPUVertexFormat3DDefault
	{
		math::Vec3f position;
		math::Vec3f texCoord0;
		math::RGBAColorU8 color;
	};

	template <typename TpVertexStruct>
	struct GPUVertexInputFormatDescForVertexStruct;

	template <>
	struct GPUVertexInputFormatDescForVertexStruct<GPUVertexFormat2DDefault>
	{
		TS3_ENGINE_API static const gpuapi::VertexInputFormatDesc sFormat;
	};

	template <>
	struct GPUVertexInputFormatDescForVertexStruct<GPUVertexFormat2DUI>
	{
		TS3_ENGINE_API static const gpuapi::VertexInputFormatDesc sFormat;
	};

	template <>
	struct GPUVertexInputFormatDescForVertexStruct<GPUVertexFormat3DDefault>
	{
		TS3_ENGINE_API static const gpuapi::VertexInputFormatDesc sFormat;
	};

}

#endif // __TS3_ENGINE_VERTEX_FORMAT_DEFS_H__
