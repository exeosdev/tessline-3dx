
#ifndef __TS3SAMPLE_TEST1_MESH_DEFS_H__
#define __TS3SAMPLE_TEST1_MESH_DEFS_H__

#include <ts3/math/vector.h>
#include <ts3/math/color.h>

#define V3( pVal ) ts3::math::Vec3f{ pVal, pVal, pVal }
#define V4( pVal ) ts3::math::Vec4f{ pVal, pVal, pVal, pVal }
#define VCOL() ts3::math::colorVector(ts3::math::colors::cxColorBlueSignatory)

struct VertexPNT0
{
	ts3::math::Vec3f position;
	ts3::math::Vec4f color;
	ts3::math::Vec3f normal;
	ts3::math::Vec2f texCoord0;
};

inline const VertexPNT0 cvMeshTexUnitCubeVertexData[] =
{
	// Front face
	VertexPNT0 { ts3::math::Vec3f{ -0.5f, -0.5f, -0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 0.0f, 1.0f } },
	VertexPNT0 { ts3::math::Vec3f{  0.5f, -0.5f, -0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 1.0f, 1.0f } },
	VertexPNT0 { ts3::math::Vec3f{  0.5f,  0.5f, -0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 1.0f, 0.0f } },
	VertexPNT0 { ts3::math::Vec3f{ -0.5f,  0.5f, -0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 0.0f, 0.0f } },

	// Right face
	VertexPNT0 { ts3::math::Vec3f{  0.5f, -0.5f, -0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 0.0f, 1.0f } },
	VertexPNT0 { ts3::math::Vec3f{  0.5f, -0.5f,  0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 1.0f, 1.0f } },
	VertexPNT0 { ts3::math::Vec3f{  0.5f,  0.5f,  0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 1.0f, 0.0f } },
	VertexPNT0 { ts3::math::Vec3f{  0.5f,  0.5f, -0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 0.0f, 0.0f } },

	// Back face
	VertexPNT0 { ts3::math::Vec3f{  0.5f, -0.5f, 0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 0.0f, 1.0f } },
	VertexPNT0 { ts3::math::Vec3f{ -0.5f, -0.5f, 0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 1.0f, 1.0f } },
	VertexPNT0 { ts3::math::Vec3f{ -0.5f,  0.5f, 0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 1.0f, 0.0f } },
	VertexPNT0 { ts3::math::Vec3f{  0.5f,  0.5f, 0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 0.0f, 0.0f } },

	// Left face
	VertexPNT0 { ts3::math::Vec3f{ -0.5f, -0.5f,  0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 0.0f, 1.0f } },
	VertexPNT0 { ts3::math::Vec3f{ -0.5f, -0.5f, -0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 1.0f, 1.0f } },
	VertexPNT0 { ts3::math::Vec3f{ -0.5f,  0.5f, -0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 1.0f, 0.0f } },
	VertexPNT0 { ts3::math::Vec3f{ -0.5f,  0.5f,  0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 0.0f, 0.0f } },

	// Top face
	VertexPNT0 { ts3::math::Vec3f{ -0.5f,  0.5f, -0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 0.0f, 1.0f } },
	VertexPNT0 { ts3::math::Vec3f{  0.5f,  0.5f, -0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 1.0f, 1.0f } },
	VertexPNT0 { ts3::math::Vec3f{  0.5f,  0.5f,  0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 1.0f, 0.0f } },
	VertexPNT0 { ts3::math::Vec3f{ -0.5f,  0.5f,  0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 0.0f, 0.0f } },

	// Bottom face
	VertexPNT0 { ts3::math::Vec3f{ -0.5f, -0.5f,  0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 0.0f, 1.0f } },
	VertexPNT0 { ts3::math::Vec3f{  0.5f, -0.5f,  0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 1.0f, 1.0f } },
	VertexPNT0 { ts3::math::Vec3f{  0.5f, -0.5f, -0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 1.0f, 0.0f } },
	VertexPNT0 { ts3::math::Vec3f{ -0.5f, -0.5f, -0.5f }, V4(0.0f), V3(0.0f), ts3::math::Vec2f{ 0.0f, 0.0f } },

	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Render rectangle
	VertexPNT0 { ts3::math::Vec3f{ -1.92f, -1.08f,  0.0f }, V4( 0.0f ), V3( 0.0f ), ts3::math::Vec2f{ 0.0f, 1.0f } },
	VertexPNT0 { ts3::math::Vec3f{  1.92f, -1.08f,  0.0f }, V4( 0.0f ), V3( 0.0f ), ts3::math::Vec2f{ 1.0f, 1.0f } },
	VertexPNT0 { ts3::math::Vec3f{  1.92f,  1.08f,  0.0f }, V4( 0.0f ), V3( 0.0f ), ts3::math::Vec2f{ 1.0f, 0.0f } },
	VertexPNT0 { ts3::math::Vec3f{ -1.92f,  1.08f,  0.0f }, V4( 0.0f ), V3( 0.0f ), ts3::math::Vec2f{ 0.0f, 0.0f } },

	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Plane rectangle
	VertexPNT0 { ts3::math::Vec3f{ -6.0f, -0.5f,  0.0f }, VCOL(), V3(0.0f), ts3::math::Vec2f{ 0.0f, 0.0f } },
	VertexPNT0 { ts3::math::Vec3f{  6.0f, -0.5f,  0.0f }, VCOL(), V3(0.0f), ts3::math::Vec2f{ 0.0f, 0.0f } },
	VertexPNT0 { ts3::math::Vec3f{  6.0f, -0.5f,  16.0f }, VCOL(), V3(0.0f), ts3::math::Vec2f{ 0.0f, 0.0f } },
	VertexPNT0 { ts3::math::Vec3f{ -6.0f, -0.5f,  16.0f }, VCOL(), V3(0.0f), ts3::math::Vec2f{ 0.0f, 0.0f } },
};

inline const ts3::math::Vec3f cvMeshTexUnitCubeNormals[] =
{
	ts3::math::Vec3f{ 0.0f, 0.0f, -1.0f },
	ts3::math::Vec3f{ 1.0f, 0.0f, 0.0f },
	ts3::math::Vec3f{ 0.0f, 0.0f, 1.0f },
	ts3::math::Vec3f{ -1.0f, 0.0f, 0.0f },
	ts3::math::Vec3f{ 0.0f, 1.0f, 0.0f },
	ts3::math::Vec3f{ 0.0f, -1.0f, 0.0f },
	ts3::math::Vec3f{ 0.0f, 0.0f, -1.0f },
};

inline const uint32 cvMeshTexUnitCubeIndexData[] =
{
	 0,  1,  2,  2,  3,  0,
	 4,  5,  6,  6,  7,  4,
	 8,  9, 10, 10, 11,  8,
	12, 13, 14, 14, 15, 12,
	16, 17, 18, 18, 19, 16,
	20, 21, 22, 22, 23, 20,
	///////////////////////
	24, 25, 26, 26, 27, 24,
	///////////////////////
	28, 29, 30, 30, 31, 28
};

std::vector<VertexPNT0> generateVertexPNT0Data()
{
	std::vector<VertexPNT0> result;

	const auto verticesNum = ts3::staticArraySize( cvMeshTexUnitCubeVertexData );
	result.resize( verticesNum );

	for( uint32 vertexIdx = 0; vertexIdx < verticesNum; ++vertexIdx )
	{
		result[vertexIdx] = cvMeshTexUnitCubeVertexData[vertexIdx];
		result[vertexIdx].normal = cvMeshTexUnitCubeNormals[vertexIdx/4];
	}

	return result;
}

std::vector<uint32> generateIndexPNT0Data()
{
	std::vector<uint32> result;

	const auto indicesNum = ts3::staticArraySize( cvMeshTexUnitCubeIndexData );
	result.resize( indicesNum );

	ts3::memCopy( result.data(), result.size(), cvMeshTexUnitCubeIndexData, indicesNum );

	return result;
}

#endif // __TS3SAMPLE_TEST1_MESH_DEFS_H__
