
#include <ts3/gpuapiGL/GL_prerequisites.h>

namespace ts3::gpuapi
{

	namespace coreutil
	{

		Bitmask<EGLRuntimeSupportFlags> queryGLRuntimeSupportFlags( const system::OpenGLVersionSupportInfo & pSupportInfo )
		{
			Bitmask<EGLRuntimeSupportFlags> supportFlags = 0;
			if( pSupportInfo.apiVersion >= Version{4,1} )
			{
				supportFlags.set( E_GL_RUNTIME_SUPPORT_FLAG_SEPARATE_SHADER_STAGES_BIT );
			}
			if( pSupportInfo.apiVersion >= Version{4,2} )
			{
				supportFlags.set( E_GL_RUNTIME_SUPPORT_FLAG_EXPLICIT_SHADER_ATTRIBUTE_LOCATION_BIT );
				supportFlags.set( E_GL_RUNTIME_SUPPORT_FLAG_EXPLICIT_SHADER_FRAG_DATA_LOCATION_BIT );
			}
			if( pSupportInfo.apiVersion >= Version{4,4} )
			{
				supportFlags.set( E_GL_RUNTIME_SUPPORT_FLAG_BUFFER_IMMUTABLE_STORAGE_BIT );
				supportFlags.set( E_GL_RUNTIME_SUPPORT_FLAG_EXPLICIT_SHADER_UNIFORM_BINDING_BIT );
			}
			return supportFlags;
		}

	}

}
