
#include "GL_vertexStreamStateObject.h"
#include "../GL_gpuDevice.h"
#include "../resources/GL_gpuBuffer.h"

namespace ts3::gpuapi
{

	GLVertexStreamStateObject::GLVertexStreamStateObject( GLGPUDevice & pGPUDevice, GLVertexStreamBindingDescriptor pGLVertexStreamBindingDescriptor )
    : VertexStreamStateObject( pGPUDevice )
    , mGLVertexStreamBindingDescriptor( std::move( pGLVertexStreamBindingDescriptor ) )
	{}

	GpaHandle<GLVertexStreamStateObject> GLVertexStreamStateObject::create( GLGPUDevice & pGPUDevice,
	                                                                        const VertexStreamStateObjectCreateInfo & pCreateInfo )
	{
		CommonSSOState commonSSOState;
		if( !createCommonSSOState( pCreateInfo, commonSSOState ) )
		{
			return nullptr;
		}

		auto openglVSBDescriptor = createGLVertexStreamBindingDescriptor( commonSSOState.vertexDataSourceBinding );
		auto vertexStreamStateObject = createDynamicInterfaceObject<GLVertexStreamStateObject>( pGPUDevice, openglVSBDescriptor );

		return vertexStreamStateObject;
	}

} // namespace ts3::gpuapi
