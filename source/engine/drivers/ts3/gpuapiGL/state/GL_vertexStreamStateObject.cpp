
#include "GL_vertexStreamStateObject.h"
#include "../GL_gpuDevice.h"
#include "../resources/GL_gpuBuffer.h"

namespace ts3
{
namespace gpuapi
{

	GLVertexStreamStateObject::GLVertexStreamStateObject( GLGPUDevice & pGPUDevice, GLVertexDataSourceBinding pGLVertexDataSourceBinding )
    : VertexStreamStateObject( pGPUDevice )
    , mGLVertexDataSourceBinding( std::move( pGLVertexDataSourceBinding ) )
	{}

	Handle<GLVertexStreamStateObject> GLVertexStreamStateObject::create( GLGPUDevice & pGPUDevice,
	                                                                     const VertexStreamStateObjectCreateInfo & pCreateInfo )
	{
		CommonSSOState commonSSOState;
		if( !createCommonSSOState( pCreateInfo, commonSSOState ) )
		{
			return nullptr;
		}

		auto openglVertexDataSourceBinding = createGLVertexDataSourceBinding( commonSSOState.vertexDataSourceBinding );
		if( !openglVertexDataSourceBinding )
		{
			return nullptr;
		}

		auto vertexStreamStateObject = createDynamicInterfaceObject<GLVertexStreamStateObject>( pGPUDevice,
		                                                                                        std::move( openglVertexDataSourceBinding ) );
		return vertexStreamStateObject;
	}

} /* namespace ts3 */
} /* namespace gpuapi */
