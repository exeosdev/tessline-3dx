
#pragma once

#ifndef __TS3_ENGINE_GEOMETRY_BUFFER_H__
#define __TS3_ENGINE_GEOMETRY_BUFFER_H__

#include "geometryCommonDefs.h"
#include <ts3/gpuapi/resources/gpuBufferCommon.h>

namespace ts3
{

	struct GeometryBufferMetrics
	{
		uint32 elementSize;
		uint32 dataSizeInElementsNum;
	};

	class GeometryBuffer
	{
	public:
		GeometryBufferMetrics const mBufferMetrics;

	public:
		void lock();
		void unlock();

	private:
		gpuapi::GPUBufferReference _gpaBufferRef;
	};

	class IndexBuffer : public GeometryBuffer
	{
	};

	class VertexBuffer : public GeometryBuffer
	{
	};

} // namespace ts3

#endif // __TS3_ENGINE_GEOMETRY_BUFFER_H__
