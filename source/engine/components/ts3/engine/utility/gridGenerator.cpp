
#include "gridGenerator.h"

namespace ts3
{

	GridGeometry GridGenerator::generate(
			const math::Vec3f & pOrigin,
			const math::Vec2u32 & pGridSize,
			const math::Vec2f & pAdvance,
			gpuapi::ETriangleVerticesOrder pVerticesOrder )
	{
		GridGeometry gridGeometry;
		gridGeometry.indicesNum = 0;
		gridGeometry.verticesNum = 0;

		if( pVerticesOrder == gpuapi::ETriangleVerticesOrder::Clockwise )
		{
			_generateVerticesNoIndexClockwise( pOrigin, pGridSize, pAdvance, &gridGeometry );
			gridGeometry.verticesOrder = gpuapi::ETriangleVerticesOrder::Clockwise;
		}
		else
		{
			_generateVerticesNoIndexCounterClockwise( pOrigin, pGridSize, pAdvance, &gridGeometry );
			gridGeometry.verticesOrder = gpuapi::ETriangleVerticesOrder::CounterClockwise;
		}

		return gridGeometry;
	}

	GridGeometry GridGenerator::generateIndexed(
			const math::Vec3f& pOrigin,
			const math::Vec2u32& pGridSize,
			const math::Vec2f & pAdvance,
			gpuapi::ETriangleVerticesOrder pVerticesOrder )
	{
		GridGeometry gridGeometry;
		gridGeometry.indicesNum = 0;
		gridGeometry.verticesNum = 0;

		if( pVerticesOrder == gpuapi::ETriangleVerticesOrder::Clockwise )
		{
			_generateVerticesIndexed( pOrigin, pGridSize, pAdvance, &gridGeometry );
			_generateIndicesClockwise( pGridSize, &gridGeometry );
			gridGeometry.verticesOrder = gpuapi::ETriangleVerticesOrder::Clockwise;
		}
		else
		{
			_generateVerticesIndexed( pOrigin, pGridSize, pAdvance, &gridGeometry );
			_generateIndicesCounterClockwise( pGridSize, &gridGeometry );
			gridGeometry.verticesOrder = gpuapi::ETriangleVerticesOrder::CounterClockwise;
		}

		return gridGeometry;
	}

	void GridGenerator::_generateVerticesIndexed(
			const math::Vec3f & pOrigin,
			const math::Vec2u32 & pGridSize,
			const math::Vec2f & pAdvance,
			GridGeometry * pGeometry)
	{
	}

	void GridGenerator::_generateVerticesNoIndexClockwise(
			const math::Vec3f & pOrigin,
			const math::Vec2u32 & pGridSize,
			const math::Vec2f & pAdvance,
			GridGeometry * pGeometry)
	{
	}

	void GridGenerator::_generateVerticesNoIndexCounterClockwise(
			const math::Vec3f & pOrigin,
			const math::Vec2u32 & pGridSize,
			const math::Vec2f & pAdvance,
			GridGeometry * pGeometry)
	{
	}

	void GridGenerator::_generateIndicesClockwise(
			const math::Vec2u32 & pGridSize,
			GridGeometry * pGeometry)
	{
	}

	void GridGenerator::_generateIndicesCounterClockwise(
			const math::Vec2u32 & pGridSize,
			GridGeometry * pGeometry)
	{
	}

}
