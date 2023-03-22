
#include "DX11_commandList.h"

namespace ts3::gpuapi
{

	namespace smutil
	{

		ID3D11DeviceContext1 * getD3D11DeviceContextFromCommandList( void * pDX11CommandList )
		{
			return reinterpret_cast<DX11CommandList *>( pDX11CommandList )->mD3D11DeviceContext1.Get();
		}

	}

}
