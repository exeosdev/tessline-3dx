
#include "GL_shaderPipelineObject.h"
#include "GL_shaderProgramObject.h"

namespace ts3::gpuapi
{

	GLShaderPipelineObject::GLShaderPipelineObject( GLuint pHandle )
	: GLObject( GLObjectBaseType::ShaderPipeline, pHandle )
	{}

	GLShaderPipelineObject::~GLShaderPipelineObject() = default;

	GLShaderPipelineObjectHandle GLShaderPipelineObject::create()
	{
		GLuint shaderPipelineHandle = 0;

		glGenProgramPipelines( 1, &shaderPipelineHandle );
		ts3OpenGLHandleLastError();

		return createGLObject<GLShaderPipelineObject>( shaderPipelineHandle );
	}

	bool GLShaderPipelineObject::release()
	{
		glDeleteProgramPipelines( 1, &mGLHandle );
		ts3OpenGLHandleLastError();

		return true;
	}

	bool GLShaderPipelineObject::validateHandle() const
	{
		auto isProgram = glIsProgramPipeline( mGLHandle );
		ts3OpenGLHandleLastError();

		return isProgram != GL_FALSE;
	}

	void GLShaderPipelineObject::attachProgram( GLShaderProgramObject & pProgram )
	{
		auto programStageMask = pProgram.getLinkedShaderStageMask();
		if( programStageMask == 0 )
		{
			return;
		}

		glUseProgramStages( mGLHandle, programStageMask, pProgram.mGLHandle );
		ts3OpenGLHandleLastError();
	}

	void GLShaderPipelineObject::attachProgram( GLShaderProgramObject & pProgram, Bitmask<GLenum> pStageMask )
	{
		auto programStageMask = pProgram.getLinkedShaderStageMask();
		if( programStageMask == 0 )
		{
			// WARN!
			return;
		}
		if( !programStageMask.isSet( pStageMask ) )
		{
			// WARN!
			return;
		}

		glUseProgramStages( mGLHandle, pStageMask, pProgram.mGLHandle );
		ts3OpenGLHandleLastError();
	}

	void GLShaderPipelineObject::setActiveProgram( GLShaderProgramObject & pProgram )
	{
		glActiveShaderProgram( mGLHandle, pProgram.mGLHandle );
		ts3OpenGLHandleLastError();
	}

	void GLShaderPipelineObject::resetActiveProgram()
	{
		glActiveShaderProgram( mGLHandle, 0 );
		ts3OpenGLHandleLastError();
	}

} // namespace ts3::gpuapi
