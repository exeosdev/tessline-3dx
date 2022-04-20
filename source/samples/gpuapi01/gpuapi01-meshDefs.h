
#ifndef __TS3SAMPLE_TEST1_MESH_DEFS_H__
#define __TS3SAMPLE_TEST1_MESH_DEFS_H__

#include <ts3/math/vector.h>
#include <ts3/math/color.h>

struct MeshVertex
{
	ts3::math::Vec3f position;
	ts3::math::RGBAColorU8 colorU8;
};

inline const MeshVertex cvMeshUnitCubeVertexData[] =
{
	MeshVertex
	{
		ts3::math::Vec3f{ -0.5f, -0.5f, 0.5f },
		ts3::math::generateRandomColor()
	},
	MeshVertex
	{
		ts3::math::Vec3f{ 0.5f, -0.5f, 0.5f },
		ts3::math::generateRandomColor()
	},
	MeshVertex
	{
		ts3::math::Vec3f{ 0.5f, 0.5f, 0.5f },
		ts3::math::generateRandomColor()
	},
	MeshVertex
	{
		ts3::math::Vec3f{ -0.5f, 0.5f, 0.5f },
		ts3::math::generateRandomColor()
	},
	MeshVertex
	{
		ts3::math::Vec3f{ -0.5f, -0.5f, -0.5f },
		ts3::math::generateRandomColor()
	},
	MeshVertex
	{
		ts3::math::Vec3f{ 0.5f, -0.5f, -0.5f },
		ts3::math::generateRandomColor()
	},
	MeshVertex
	{
		ts3::math::Vec3f{ 0.5f, 0.5f, -0.5f },
		ts3::math::generateRandomColor()
	},
	MeshVertex
	{
		ts3::math::Vec3f{ -0.5f, 0.5f, -0.5f },
		ts3::math::generateRandomColor()
	}
};
inline const uint32 cvMeshUnitCubeIndexData[] =
{
	0, 1, 2, 2, 3, 0,
	1, 5, 6, 6, 2, 1,
	5, 4, 7, 7, 6, 5,
	4, 0, 3, 3, 7, 4,
	3, 2, 6, 6, 7, 3,
	4, 5, 1, 1, 0, 4
};
inline const size_t cvMeshUnitCubeVertexDataSize = sizeof( MeshVertex ) * ts3::staticArraySize( cvMeshUnitCubeVertexData );
inline const size_t cvMeshUnitCubeIndexDataSize = sizeof( uint32 ) * ts3::staticArraySize( cvMeshUnitCubeIndexData );

inline const MeshVertex cvTriangleVertexData[] =
{
	MeshVertex
	{
		ts3::math::Vec3f{-0.5f, -0.5f, 0.5f},
		ts3::math::generateRandomColor()
	},
	MeshVertex
	{
		ts3::math::Vec3f{0.5f, -0.5f, 0.5f},
		ts3::math::generateRandomColor()
	},
	MeshVertex
	{
		ts3::math::Vec3f{0.5f, 0.5f, 0.5f},
		ts3::math::generateRandomColor()
	}
};
uint32 cvTriangleIndexData[] =
{
	0, 1, 2, 0, 1, 2
};
inline const size_t cvTriangleVertexDataSize = sizeof( MeshVertex ) * ts3::staticArraySize( cvTriangleVertexData );
inline const size_t cvTriangleIndexDataSize = sizeof( uint32 ) * ts3::staticArraySize( cvTriangleIndexData );

struct TexturedMeshVertex
{
	ts3::math::Vec3f position;
	ts3::math::Vec2f texCoord0;
};

inline const TexturedMeshVertex cvMeshTexUnitCubeVertexData[] =
{
	TexturedMeshVertex { ts3::math::Vec3f{ -0.5f, -0.5f,  -0.5f }, ts3::math::Vec2f{ 0.0f, 1.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{  0.5f, -0.5f,  -0.5f }, ts3::math::Vec2f{ 1.0f, 1.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{  0.5f,  0.5f,  -0.5f }, ts3::math::Vec2f{ 1.0f, 0.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{ -0.5f,  0.5f,  -0.5f }, ts3::math::Vec2f{ 0.0f, 0.0f } },

	TexturedMeshVertex { ts3::math::Vec3f{  0.5f, -0.5f,  -0.5f }, ts3::math::Vec2f{ 0.0f, 1.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{  0.5f, -0.5f, 0.5f }, ts3::math::Vec2f{ 1.0f, 1.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{  0.5f,  0.5f, 0.5f }, ts3::math::Vec2f{ 1.0f, 0.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{  0.5f,  0.5f,  -0.5f }, ts3::math::Vec2f{ 0.0f, 0.0f } },

	TexturedMeshVertex { ts3::math::Vec3f{  0.5f, -0.5f, 0.5f }, ts3::math::Vec2f{ 0.0f, 1.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{ -0.5f, -0.5f, 0.5f }, ts3::math::Vec2f{ 1.0f, 1.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{ -0.5f,  0.5f, 0.5f }, ts3::math::Vec2f{ 1.0f, 0.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{  0.5f,  0.5f, 0.5f }, ts3::math::Vec2f{ 0.0f, 0.0f } },

	TexturedMeshVertex { ts3::math::Vec3f{ -0.5f, -0.5f, 0.5f }, ts3::math::Vec2f{ 0.0f, 1.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{ -0.5f, -0.5f,  -0.5f }, ts3::math::Vec2f{ 1.0f, 1.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{ -0.5f,  0.5f,  -0.5f }, ts3::math::Vec2f{ 1.0f, 0.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{ -0.5f,  0.5f, 0.5f }, ts3::math::Vec2f{ 0.0f, 0.0f } },

	TexturedMeshVertex { ts3::math::Vec3f{ -0.5f,  0.5f,  -0.5f }, ts3::math::Vec2f{ 0.0f, 1.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{  0.5f,  0.5f,  -0.5f }, ts3::math::Vec2f{ 1.0f, 1.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{  0.5f,  0.5f, 0.5f }, ts3::math::Vec2f{ 1.0f, 0.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{ -0.5f,  0.5f, 0.5f }, ts3::math::Vec2f{ 0.0f, 0.0f } },

	TexturedMeshVertex { ts3::math::Vec3f{ -0.5f, -0.5f, 0.5f }, ts3::math::Vec2f{ 0.0f, 1.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{  0.5f, -0.5f, 0.5f }, ts3::math::Vec2f{ 1.0f, 1.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{  0.5f, -0.5f,  -0.5f }, ts3::math::Vec2f{ 1.0f, 0.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{ -0.5f, -0.5f,  -0.5f }, ts3::math::Vec2f{ 0.0f, 0.0f } },

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	TexturedMeshVertex { ts3::math::Vec3f{ -1.92f, -1.08f,  0.5f }, ts3::math::Vec2f{ 0.0f, 0.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{  1.92f, -1.08f,  0.5f }, ts3::math::Vec2f{ 1.0f, 0.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{  1.92f,  1.08f,  0.5f }, ts3::math::Vec2f{ 1.0f, 1.0f } },
	TexturedMeshVertex { ts3::math::Vec3f{ -1.92f,  1.08f,  0.5f }, ts3::math::Vec2f{ 0.0f, 1.0f } },
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
	24, 25, 26, 26, 27, 24
};
inline const size_t cvMeshTexUnitCubeVertexDataSize = sizeof( TexturedMeshVertex ) * ts3::staticArraySize( cvMeshTexUnitCubeVertexData );
inline const size_t cvMeshTexUnitCubeIndexDataSize = sizeof( uint32 ) * ts3::staticArraySize( cvMeshTexUnitCubeIndexData );

#endif // __TS3SAMPLE_TEST1_MESH_DEFS_H__
