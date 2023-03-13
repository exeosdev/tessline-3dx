
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_OBJECT_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/state/separablePipelineState.h>

namespace ts3::gpuapi
{

	ts3GLDeclareOpenGLObjectHandle( GLShaderPipelineObject );
	ts3GLDeclareOpenGLObjectHandle( GLShaderProgramObject );

	class GLGraphicsPipelineStateObject : public GraphicsPipelineStateObjectSeparable
	{
	public:
		GLGraphicsPipelineStateObject(
				GLGPUDevice & pGPUDevice,
				RenderTargetLayout pRenderTargetLayout,
				ShaderInputSignature pShaderInputSignature,
				const SeparablePSOStateSet & pPSOImmutableStates );

		virtual ~GLGraphicsPipelineStateObject();

		TS3_ATTR_NO_DISCARD const GLBlendImmutableState & getBlendState() const noexcept;

		TS3_ATTR_NO_DISCARD const GLDepthStencilImmutableState & getDepthStencilState() const noexcept;

		TS3_ATTR_NO_DISCARD const GLRasterizerImmutableState & getRasterizerState() const noexcept;

		TS3_ATTR_NO_DISCARD const GLGraphicsShaderLinkageImmutableState & getGraphicsShaderLinkageState() const noexcept;

		TS3_ATTR_NO_DISCARD const GLIAInputLayoutImmutableState & getIAInputLayoutState() const noexcept;

		static GpaHandle<GLGraphicsPipelineStateObject> create(
				GLGPUDevice & pGPUDevice,
				const GraphicsPipelineStateObjectCreateInfo & pCreateInfo );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_OBJECT_H__
