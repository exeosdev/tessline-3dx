
#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_GRAPHICS_PIPELINE_IMMUTABLE_STATE_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_GRAPHICS_PIPELINE_IMMUTABLE_STATE_H__

#include "GL_commonPipelineStateDefs.h"
#include "../objects/GL_vertexArrayObject.h"
#include <ts3/gpuapi/state/immutable/commonGraphicsImmutableState.h>
#include <ts3/gpuapi/state/immutable/graphicsShaderLinkageImmutableState.h>
#include <ts3/gpuapi/state/immutable/iaInputLayoutImmutableState.h>

namespace ts3::gpuapi
{

	///
	class GLBlendImmutableState : public BlendImmutableState
	{
	public:
		GLBlendConfig const mGLBlendConfig;

	public:
		GLBlendImmutableState( GPUDevice & pGPUDevice, const GLBlendConfig & pGLBlendConfig )
		: BlendImmutableState( pGPUDevice )
		, mGLBlendConfig( pGLBlendConfig )
		{}

		virtual ~GLBlendImmutableState() = default;
	};

	///
	class GLDepthStencilImmutableState : public DepthStencilImmutableState
	{
	public:
		GLDepthStencilConfig const mGLDepthStencilConfig;

	public:
		GLDepthStencilImmutableState( GPUDevice & pGPUDevice, const GLDepthStencilConfig & pGLDepthStencilConfig )
		: DepthStencilImmutableState( pGPUDevice )
		, mGLDepthStencilConfig( pGLDepthStencilConfig )
		{}

		virtual ~GLDepthStencilImmutableState() = default;
	};

	///
	class GLIAInputLayoutImmutableState : public IAInputLayoutImmutableState
	{
	public:
		GLIAInputLayoutDefinition const mGLLayoutDefinition;
		GLVertexArrayObjectHandle mVertexArrayObject;

	public:
		GLIAInputLayoutImmutableState( GPUDevice & pGPUDevice, const GLIAInputLayoutDefinition & pGLLayoutDefinition )
		: IAInputLayoutImmutableState( pGPUDevice )
		, mGLLayoutDefinition( pGLLayoutDefinition )
		{}

		virtual ~GLIAInputLayoutImmutableState() = default;
	};

	///
	class GLRasterizerImmutableState : public RasterizerImmutableState
	{
	public:
		GLRasterizerConfig const mGLRasterizerConfig;

	public:
		GLRasterizerImmutableState( GPUDevice & pGPUDevice, const GLRasterizerConfig & pGLRasterizerConfig )
		: RasterizerImmutableState( pGPUDevice )
		, mGLRasterizerConfig( pGLRasterizerConfig )
		{}

		virtual ~GLRasterizerImmutableState() = default;
	};

	namespace smutil
	{

		TS3_ATTR_NO_DISCARD GLVertexArrayObjectHandle createGLVertexArrayObjectFromLayoutDefinition(
				const GLIAInputLayoutDefinition & pGLLayoutDefinition ) noexcept;

	}

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_GRAPHICS_PIPELINE_IMMUTABLE_STATE_H__
