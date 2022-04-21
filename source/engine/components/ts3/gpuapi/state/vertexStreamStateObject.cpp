
#include "vertexStreamStateObject.h"

namespace ts3::gpuapi
{

	VertexStreamStateObject::VertexStreamStateObject( GPUDevice & pGPUDevice )
	: GPUStateObject( pGPUDevice )
	{}

	VertexStreamStateObject::~VertexStreamStateObject() = default;

	bool VertexStreamStateObject::createCommonSSOState( const VertexStreamStateObjectCreateInfo & pCreateInfo,
	                                                    CommonSSOState & pOutputState )
	{
		auto vertexDataSourceBinding = createVertexDataSourceBinding( pCreateInfo.vertexDataSourceBindingDesc );
		if( !vertexDataSourceBinding )
		{
			return false;
		}

		pOutputState.vertexDataSourceBinding = std::move( vertexDataSourceBinding );

		return true;
	}


	CommonVertexStreamStateObject::CommonVertexStreamStateObject( GPUDevice & pGPUDevice, VertexDataSourceBinding pVertexDataSourceBinding )
	: VertexStreamStateObject( pGPUDevice )
	, mVertexDataSourceBinding( pVertexDataSourceBinding )
	{}

	CommonVertexStreamStateObject::~CommonVertexStreamStateObject() = default;

	CommonVertexStreamStateObjectHandle CommonVertexStreamStateObject::create( GPUDevice & pGPUDevice,
	                                                                           const VertexStreamStateObjectCreateInfo & pCreateInfo )
	{
		CommonSSOState commonSSOState;
		if( !createCommonSSOState( pCreateInfo, commonSSOState ) )
		{
			return nullptr;
		}

		auto streamStateObject = createDynamicInterfaceObject<CommonVertexStreamStateObject>( pGPUDevice,
		                                                                                      std::move( commonSSOState.vertexDataSourceBinding ) );
		return streamStateObject;
	}

} // namespace ts3::gpuapi
