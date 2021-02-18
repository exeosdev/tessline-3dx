
#ifndef __TS3_ENGINE_MESH_PART_H__
#define __TS3_ENGINE_MESH_PART_H__

#include "commonRDUtils.h"

namespace ts3
{

	class HWVertexBuffer;
	class HWIndexBuffer;

	class MeshPart
	{
	public:

	private:
		SharedHandle<HWVertexBuffer> _hwVertexBuffer;
		SharedHandle<HWIndexBuffer> _hwIndexBuffer;
	};

}

#endif // __TS3_ENGINE_MESH_PART_H__
