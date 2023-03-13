
#include "GL_graphicsShaderState.h"
#include <ts3/gpuapiGL/GL_gpuDevice.h>
#include <ts3/gpuapiGL/resources/GL_shader.h>
#include <ts3/gpuapiGL/objects/GL_shaderPipelineObject.h>

namespace ts3::gpuapi
{

	GLGraphicsShaderLinkageImmutableState::GLGraphicsShaderLinkageImmutableState(
			GLGPUDevice & pGPUDevice,
			const GraphicsShaderLinkageCommonProperties & pCommonProperties,
			GLShaderPipelineObjectHandle pGLShaderPipelineObject,
			GLShaderProgramObjectHandle pGLShaderProgramObject )
	: GraphicsShaderLinkageImmutableState( pGPUDevice, pCommonProperties )
	, mGLShaderPipelineObject( std::move( pGLShaderPipelineObject ) )
	, mGLShaderProgramObject( std::move( pGLShaderProgramObject ) )
	{}

	GLGraphicsShaderLinkageImmutableState::~GLGraphicsShaderLinkageImmutableState() = default;

	GpaHandle<GLGraphicsShaderLinkageImmutableState> GLGraphicsShaderLinkageImmutableState::createInstance(
			GLGPUDevice & pGPUDevice,
			const GraphicsShaderSet & pShaderSet )
	{
		const auto & commonProperties = smutil::getGraphicsShaderLinkageCommonPropertiesForShaderSet( pShaderSet );

		GLShaderPipelineObjectHandle shaderPipelineObject;
		GLShaderProgramObjectHandle shaderProgramObject;

	#if( TS3GX_GL_FEATURE_SUPPORT_SHADER_PIPELINE_OBJECT )
		shaderPipelineObject = smutil::createGraphicsShaderPipelineObject( pShaderSet );
		if( !shaderPipelineObject )
		{
			return nullptr;
		}
	#else
		shaderProgramObject = smutil::createGraphicsShaderProgramObject( pShaderSet );
		if( !shaderProgramObject )
		{
			return nullptr;
		}
	#endif

		auto immutableState = createGPUAPIObject<GLGraphicsShaderLinkageImmutableState>(
				pGPUDevice,
				commonProperties,
				std::move( shaderPipelineObject ),
				std::move( shaderProgramObject ) );

		return immutableState;
	}

	namespace smutil
	{

		GLShaderPipelineObjectHandle createGraphicsShaderPipelineObject( const GraphicsShaderSet & pShaderSet )
		{
			auto shaderPipelineObject = GLShaderPipelineObject::create();
			if( !shaderPipelineObject )
			{
				return nullptr;
			}

			glBindProgramPipeline( shaderPipelineObject->mGLHandle );
			ts3OpenGLHandleLastError();

			for( auto & shaderHandle : pShaderSet.commonShaderArray )
			{
				if( shaderHandle )
				{
					auto * openglShader = shaderHandle->queryInterface<GLShader>();
					ts3DebugAssert( openglShader->mGLShaderProgramObject );
					shaderPipelineObject->attachProgram( *( openglShader->mGLShaderProgramObject ) );
				}
			}

			glBindProgramPipeline( 0 );
			ts3OpenGLHandleLastError();

			return shaderPipelineObject;
		}

		GLShaderProgramObjectHandle createGraphicsShaderProgramObject( const GraphicsShaderSet & pShaderSet )
		{
			auto shaderProgramObject = GLShaderProgramObject::create( GLShaderProgramType::Combined );
			if( !shaderProgramObject )
			{
				return nullptr;
			}

			for( auto & shaderHandle : pShaderSet.commonShaderArray )
			{
				if( shaderHandle )
				{
					auto * openglShader = shaderHandle->queryInterface<GLShader>();
					ts3DebugAssert( openglShader->mGLShaderProgramObject );
					shaderProgramObject->attachShader( *( openglShader->mGLShaderObject ) );
				}
			}

			if( !shaderProgramObject->link() )
			{
				ts3DebugInterrupt();
				return nullptr;
			}
			if( !shaderProgramObject->validate() )
			{
				ts3DebugInterrupt();
				return nullptr;
			}

			shaderProgramObject->detachAllShaders();

			return shaderProgramObject;
		}

		void updateUniformDataCurrent(
				GLShaderPipelineObject & pShaderPipeline,
				uint32 pUniformIndex,
				EBaseDataType pBaseType,
				uint32 pLength,
				const void * pData )
		{
			if( ( pLength == 0 ) || !pData )
			{
				return;
			}

			const auto currentPipelineHandle = GLShaderPipelineObject::queryCurrentShaderPipelineBinding();
			if( currentPipelineHandle != pShaderPipeline.mGLHandle )
			{
				ts3Throw( 0 );
			}

			if( pBaseType == EBaseDataType::Float32 )
			{
				const auto * floatData = reinterpret_cast<const float *>( pData );
				switch( pLength )
				{
					case 1: glUniform1fv( pUniformIndex, 1, floatData ); ts3OpenGLHandleLastError(); break;
					case 2: glUniform2fv( pUniformIndex, 1, floatData ); ts3OpenGLHandleLastError(); break;
					case 3: glUniform3fv( pUniformIndex, 1, floatData ); ts3OpenGLHandleLastError(); break;
					case 4: glUniform4fv( pUniformIndex, 1, floatData ); ts3OpenGLHandleLastError(); break;
				}
			}
			else if( pBaseType == EBaseDataType::Int32 )
			{
				const auto * intData = reinterpret_cast<const GLint *>( pData );
				switch( pLength )
				{
					case 1: glUniform1iv( pUniformIndex, 1, intData ); ts3OpenGLHandleLastError(); break;
					case 2: glUniform2iv( pUniformIndex, 1, intData ); ts3OpenGLHandleLastError(); break;
					case 3: glUniform3iv( pUniformIndex, 1, intData ); ts3OpenGLHandleLastError(); break;
					case 4: glUniform4iv( pUniformIndex, 1, intData ); ts3OpenGLHandleLastError(); break;
				}
			}
			else if( pBaseType == EBaseDataType::Uint32 )
			{
				const auto * uintData = reinterpret_cast<const GLuint *>( pData );
				switch( pLength )
				{
					case 1: glUniform1uiv( pUniformIndex, 1, uintData ); ts3OpenGLHandleLastError(); break;
					case 2: glUniform2uiv( pUniformIndex, 1, uintData ); ts3OpenGLHandleLastError(); break;
					case 3: glUniform3uiv( pUniformIndex, 1, uintData ); ts3OpenGLHandleLastError(); break;
					case 4: glUniform4uiv( pUniformIndex, 1, uintData ); ts3OpenGLHandleLastError(); break;
				}
			}
		}

		void updateUniformDataExplicit(
				GLShaderProgramObject & pShaderProgram,
				uint32 pUniformIndex,
				EBaseDataType pBaseType,
				uint32 pLength,
				const void * pData )
		{
			if( ( pLength == 0 ) || !pData )
			{
				return;
			}

			const auto currentProgramHandle = GLShaderProgramObject::queryCurrentShaderProgramBinding();
			if( currentProgramHandle != pShaderProgram.mGLHandle )
			{
				ts3Throw( 0 );
			}

			if( pBaseType == EBaseDataType::Float32 )
			{
				const auto * floatData = reinterpret_cast<const float *>( pData );
				switch( pLength )
				{
					case 1: glProgramUniform1fv( pShaderProgram.mGLHandle, pUniformIndex, 1, floatData ); ts3OpenGLHandleLastError(); break;
					case 2: glProgramUniform2fv( pShaderProgram.mGLHandle, pUniformIndex, 1, floatData ); ts3OpenGLHandleLastError(); break;
					case 3: glProgramUniform3fv( pShaderProgram.mGLHandle, pUniformIndex, 1, floatData ); ts3OpenGLHandleLastError(); break;
					case 4: glProgramUniform4fv( pShaderProgram.mGLHandle, pUniformIndex, 1, floatData ); ts3OpenGLHandleLastError(); break;
				}
			}
			else if( pBaseType == EBaseDataType::Int32 )
			{
				const auto * intData = reinterpret_cast<const GLint *>( pData );
				switch( pLength )
				{
					case 1: glProgramUniform1iv( pShaderProgram.mGLHandle, pUniformIndex, 1, intData ); ts3OpenGLHandleLastError(); break;
					case 2: glProgramUniform2iv( pShaderProgram.mGLHandle, pUniformIndex, 1, intData ); ts3OpenGLHandleLastError(); break;
					case 3: glProgramUniform3iv( pShaderProgram.mGLHandle, pUniformIndex, 1, intData ); ts3OpenGLHandleLastError(); break;
					case 4: glProgramUniform4iv( pShaderProgram.mGLHandle, pUniformIndex, 1, intData ); ts3OpenGLHandleLastError(); break;
				}
			}
			else if( pBaseType == EBaseDataType::Uint32 )
			{
				const auto * uintData = reinterpret_cast<const GLuint *>( pData );
				switch( pLength )
				{
					case 1: glProgramUniform1uiv( pShaderProgram.mGLHandle, pUniformIndex, 1, uintData ); ts3OpenGLHandleLastError(); break;
					case 2: glProgramUniform2uiv( pShaderProgram.mGLHandle, pUniformIndex, 1, uintData ); ts3OpenGLHandleLastError(); break;
					case 3: glProgramUniform3uiv( pShaderProgram.mGLHandle, pUniformIndex, 1, uintData ); ts3OpenGLHandleLastError(); break;
					case 4: glProgramUniform4uiv( pShaderProgram.mGLHandle, pUniformIndex, 1, uintData ); ts3OpenGLHandleLastError(); break;
				}
			}
		}

	}

}
