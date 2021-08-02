
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_GPU_DEVICE_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_GPU_DEVICE_H__

#include "GL_coreAPIProxy.h"
#include "state/GL_pipelineStateDesc.h"
#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/gpuapi/state/graphicsPipelineStateDescriptorCache.h>

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
		friend class GLVertexStreamStateObject;

	public:
		SysGLDriverHandle const mSysGLDriver;

	public:
		explicit GLGPUDevice( GLGPUDriver & pGLGPUDriver );
		virtual ~GLGPUDevice();

		virtual GPUBufferHandle createGPUBuffer( const GPUBufferCreateInfo & pCreateInfo ) override;
		virtual SamplerHandle createSampler( const SamplerCreateInfo & pCreateInfo ) override;
		virtual ShaderHandle createShader( const ShaderCreateInfo & pCreateInfo ) override;
		virtual TextureHandle createTexture( const TextureCreateInfo & pCreateInfo ) override;

		virtual GraphicsPipelineStateObjectHandle createGraphicsPipelineStateObject( const GraphicsPipelineStateObjectCreateInfo & pCreateInfo ) override;
		virtual VertexStreamStateObjectHandle createVertexStreamStateObject( const VertexStreamStateObjectCreateInfo & pCreateInfo ) override;
		virtual RenderTargetStateObjectHandle createRenderTargetStateObject( const RenderTargetStateObjectCreateInfo & pCreateInfo ) override;

		virtual void waitForCommandSync( CommandSync & pCommandSync ) override;

		bool initializeGLDebugOutput();

		GLDebugOutput * getDebugOutputInterface() const;

		const GLBlendStateDescriptor & getBlendDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const;
		const GLDepthStencilStateDescriptor & getDepthStencilDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const;
		const GLRasterizerStateDescriptor & getRasterizerDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const;
		const GLVertexInputFormatStateDescriptor & getVertexInputFormatDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const;

	protected:
		virtual void initializeCommandSystem() override;
		virtual bool onSetPresentationLayer( PresentationLayerHandle pPresentationLayer ) override;

	friendapi:
		pipeline_state_descriptor_id_t createBlendDescriptor( const BlendConfigDesc & pConfigDesc );
		pipeline_state_descriptor_id_t createDepthStencilDescriptor( const DepthStencilConfigDesc & pConfigDesc );
		pipeline_state_descriptor_id_t createRasterizerDescriptor( const RasterizerConfigDesc & pConfigDesc );
		pipeline_state_descriptor_id_t createVertexInputFormatDescriptor( const VertexInputFormatDesc & pInputFormatDesc );

		GLGraphicsPipelineStateDescriptorCache & getDescriptorCache();

	private:
		GLGraphicsPipelineStateDescriptorCache _descriptorCache;
		std::unique_ptr<GLDebugOutput> _glDebugOutput;
	};

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_GPU_DEVICE_H__
