
#include "DX11_vertexStreamStateObject.h"
#include "../DX11_gpuDevice.h"

namespace ts3::gpuapi
{

	DX11VertexStreamStateObject::DX11VertexStreamStateObject( DX11GPUDevice & pGPUDevice,
	                                                          DX11VertexDataSourceBinding pDX11VertexDataSourceBinding )
	: VertexStreamStateObject( pGPUDevice )
	, mDX11VertexDataSourceBinding( std::move( pDX11VertexDataSourceBinding ) )
	{}

	GpaHandle<DX11VertexStreamStateObject> DX11VertexStreamStateObject::create( DX11GPUDevice & pGPUDevice,
	                                                                            const VertexStreamStateObjectCreateInfo & pCreateInfo )
	{
		CommonSSOState commonSSOState;
		if( !createCommonSSOState( pCreateInfo, commonSSOState ) )
		{
			return nullptr;
		}

		auto dx11VertexDataSourceBinding = createDX11VertexDataSourceBinding( commonSSOState.vertexDataSourceBinding );
		if( !dx11VertexDataSourceBinding )
		{
			return nullptr;
		}

		auto vertexStreamStateObject = createDynamicInterfaceObject<DX11VertexStreamStateObject>( pGPUDevice,
		                                                                                          std::move( dx11VertexDataSourceBinding ) );

		return vertexStreamStateObject;
	}

} // namespace ts3::gpuapi
