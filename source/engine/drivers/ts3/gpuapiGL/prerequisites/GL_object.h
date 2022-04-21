
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_OBJECT_H__

namespace ts3::gpuapi
{

	constexpr GLuint cxGLObjectHandleInvalid = ts3::Limits<GLuint>::maxValue;

	constexpr GLenum cxGLBindTargetUnknown = ts3::Limits<GLenum>::maxValue;

	enum class GLObjectBaseType : enum_default_value_t
	{
		// Type for buffer objects (glGenBuffers).
		Buffer,
		// Type for framebuffer objects (glGenFamebuffers).
		Framebuffer,
		// Type for buffer objects (glGenRenderbuffers).
		Renderbuffer,
		// Type for buffer objects (glGenSamplers).
		Sampler,
		// Type for buffer objects (glCreateShader).
		Shader,
		// Type for buffer objects (glGenProgramPipelines).
		ShaderPipeline,
		// Type for shader program objects (glCreateProgram).
		ShaderProgram,
		// Type for texture objects (glGenTextures).
		Texture,
		// Type for transform feedback objects (glGenTransformFeedbacks).
		TransformFeedback,
		// Type for vertex array objects (glGenVertexArrays).
		VertexArray,
		//
		Unknown
	};

	class GLObject
	{
	public:
		GLObjectBaseType const mBaseType;

		GLuint const mGLHandle;

		GLObject( GLObjectBaseType pBaseType, GLuint pHandle );
		virtual ~GLObject();

		virtual bool release();

		virtual bool validateHandle() const;

		bool checkHandle() const;

	protected:
		void invalidateHandle();
	};

	inline bool GLObject::checkHandle() const
	{
		return mGLHandle != cxGLObjectHandleInvalid;
	}

	struct GLObjectDeleter
	{
		void operator()( GLObject * pObject ) const
		{
			pObject->release();
			delete pObject;
		}
	};

	template <typename TpObject>
	using GLObjectHandle = std::unique_ptr<TpObject, GLObjectDeleter>;

#define ts3GLDeclareOpenGLObjectHandle( pObjectClass ) \
	class pObjectClass; \
	using pObjectClass##Handle = GLObjectHandle<pObjectClass>

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_OBJECT_H__
