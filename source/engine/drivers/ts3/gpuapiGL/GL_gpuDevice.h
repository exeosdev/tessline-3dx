
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
	class GLGraphicsPipelineStateController;
	class GLGraphicsPipelineStateObject;
	class GLVertexStreamStateObject;

	/// @brief
	class GLGPUDevice : public GPUDevice
	{
		friend class GLCommandContext;
		friend class GLCommandList;
		friend class GLGraphicsPipelineStateObject;

	public:
		system::OpenGLSystemDriverHandle const mSysGLDriver;

		system::OpenGLVersionSupportInfo const mSysGLSupportInfo;

		Bitmask<EGLRuntimeSupportFlags> const mGLRuntimeSupportFlags;

	public:
		explicit GLGPUDevice( GLGPUDriver & pGPUDriver, GLPipelineImmutableStateFactory & pImmutableStateFactory );
		virtual ~GLGPUDevice();

		TS3_ATTR_NO_DISCARD virtual bool isCompatibilityDevice() const noexcept = 0;

		GLDebugOutput * getDebugOutputInterface() const;

		virtual void waitForCommandSync( CommandSync & pCommandSync ) override;

	protected:
		bool initializeGLDebugOutput();

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
		GLPipelineImmutableStateFactory * _immutableStateFactoryGL;
		PipelineImmutableStateCache _immutableStateCache;
		std::unique_ptr<GLDebugOutput> _glDebugOutput;
	};

	class GLGPUDeviceCore : public GLGPUDevice
	{
	public:
		explicit GLGPUDeviceCore( GLGPUDriver & pGPUDriver );
		virtual ~GLGPUDeviceCore();

		virtual bool isCompatibilityDevice() const noexcept override final;

	private:
		GLPipelineImmutableStateFactoryCore _immutableStateFactoryCore;
	};

	class GLGPUDeviceCompat : public GLGPUDevice
	{
	public:
		explicit GLGPUDeviceCompat( GLGPUDriver & pGPUDriver );
		virtual ~GLGPUDeviceCompat();

		virtual bool isCompatibilityDevice() const noexcept override final;

	private:
		GLPipelineImmutableStateFactoryCompat _immutableStateFactoryCompat;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_GPU_DEVICE_H__
