
#pragma once

#ifndef __TS3_ENGINE_GRID_GENERATOR_H__
#define __TS3_ENGINE_GRID_GENERATOR_H__

#include "../prerequisites.h"
#include <ts3/gpuapi/state/commonGPUStateDefs.h>
#include <ts3/math/vector.h>
#include <ts3/stdext/memoryBuffer.h>

namespace ts3
{

	struct GridGeometry
	{
		using VertexType = math::Vec3f;
		using IndexType = uint32;

		DynamicMemoryBuffer vertices;

		DynamicMemoryBuffer indices;

		native_uint verticesNum;

		native_uint indicesNum;

		gpuapi::ETriangleVerticesOrder verticesOrder;
	};

	class GridGenerator
	{
	public:
		static GridGeometry generate(
				const math::Vec3f & pOrigin,
				const math::Vec2u32 & pGridSize,
				const math::Vec2f & pAdvance,
				gpuapi::ETriangleVerticesOrder pVerticesOrder = gpuapi::ETriangleVerticesOrder::CounterClockwise);

		static GridGeometry generateIndexed(
				const math::Vec3f& pOrigin,
				const math::Vec2u32& pGridSize,
				const math::Vec2f & pAdvance,
				gpuapi::ETriangleVerticesOrder pVerticesOrder = gpuapi::ETriangleVerticesOrder::CounterClockwise);

	private:
		static void _generateVerticesIndexed(
				const math::Vec3f & pOrigin,
				const math::Vec2u32 & pGridSize,
				const math::Vec2f & pAdvance,
				GridGeometry * pGeometry);

		static void _generateVerticesNoIndexClockwise(
				const math::Vec3f & pOrigin,
				const math::Vec2u32 & pGridSize,
				const math::Vec2f & pAdvance,
				GridGeometry * pGeometry);

		static void _generateVerticesNoIndexCounterClockwise(
				const math::Vec3f & pOrigin,
				const math::Vec2u32 & pGridSize,
				const math::Vec2f & pAdvance,
				GridGeometry * pGeometry);

		static void _generateIndicesClockwise(
				const math::Vec2u32 & pGridSize,
				GridGeometry * pGeometry);

		static void _generateIndicesCounterClockwise(
				const math::Vec2u32 & pGridSize,
				GridGeometry * pGeometry);
	};

} // namespace ts3

#endif // __TS3_ENGINE_GRID_GENERATOR_H__
