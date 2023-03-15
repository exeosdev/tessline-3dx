
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_GPU_DEVICE_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_GPU_DEVICE_H__

#include "GL_apiTranslationLayer.h"
#include "state/GL_pipelineImmutableStateFactory.h"
#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/gpuapi/state/pipelineImmutableStateCache.h>

namespace ts3::gpuapi
{

	class GLCommandContext;
	class GLGraphicsPipelineStateObject;
	class GLVertexStreamStateObject;

	/// @brief
	class TS3GX_GL_CLASS GLGPUDevice : public GPUDevice
	{
		friend class GLCommandContext;
		friend class GLCommandList;
		friend class GLGraphicsPipelineStateObject;

	public:
		system::OpenGLSystemDriverHandle const mSysGLDriver;

	public:
		explicit GLGPUDevice( GLGPUDriver & pGPUDriver );
		virtual ~GLGPUDevice();

		GLDebugOutput * getDebugOutputInterface() const;

		bool initializeGLDebugOutput();

		virtual void waitForCommandSync( CommandSync & pCommandSync ) override;

	protected:
		virtual void initializeCommandSystem() override;

	private:
	    virtual bool _drvOnSetPresentationLayer( PresentationLayerHandle pPresentationLayer ) override;

	    virtual GPUBufferHandle _drvCreateGPUBuffer( const GPUBufferCreateInfo & pCreateInfo ) override final;
	    virtual SamplerHandle _drvCreateSampler( const SamplerCreateInfo & pCreateInfo ) override final;
	    virtual ShaderHandle _drvCreateShader( const ShaderCreateInfo & pCreateInfo ) override final;
	    virtual TextureHandle _drvCreateTexture( const TextureCreateInfo & pCreateInfo ) override final;

		virtual RenderTargetTextureHandle _drvCreateRenderTargetTexture(
				const RenderTargetTextureCreateInfo & pCreateInfo ) override final;

		virtual GraphicsPipelineStateObjectHandle _drvCreateGraphicsPipelineStateObject(
				const GraphicsPipelineStateObjectCreateInfo & pCreateInfo ) override final;

	private:
		GLPipelineImmutableStateFactory _immutableStateFactory;
		PipelineImmutableStateCache _immutableStateCache;
		std::unique_ptr<GLDebugOutput> _glDebugOutput;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_GPU_DEVICE_H__
