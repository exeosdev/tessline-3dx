
#ifndef __TS3_ENGINE_MESH_PART_H__
#define __TS3_ENGINE_MESH_PART_H__

#include "commonRDUtils.h"

namespace ts3
{

	class HardwareVertexBuffer;
	class HardwareIndexBuffer;

	class MeshPart
	{
	public:

	private:
		SharedHandle<HardwareVertexBuffer> _hwVertexBuffer;
		SharedHandle<HardwareIndexBuffer> _hwIndexBuffer;
	};

}

#endif // __TS3_ENGINE_MESH_PART_H__
