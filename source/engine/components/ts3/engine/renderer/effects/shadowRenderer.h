
#pragma once

#ifndef __TS3_ENGINE_SHADOW_RENDERER_H__
#define __TS3_ENGINE_SHADOW_RENDERER_H__

#include "../commonRendererDefs.h"

#include <ts3/gpuapi/state/renderTargetDynamicStates.h>

namespace ts3
{

	struct CBShadowData
	{
		math::Mat4f m4fLightSpaceMatrix;
		math::Vec4f v4fShadowProperties;
	};

	struct ShadowConfig
	{
		gpuapi::TextureSize2D screenSize;
		gpuapi::TextureSize2D shadowMapSize;
	};

	class ShadowRenderer
	{
	public:
		struct CurrentState
		{
			math::Vec3f vLightPosition;
			math::Vec3f vLightTarget;
			math::Mat4f mLightView;
			math::Mat4f mLightProjection;
			math::Mat4f mLightSpace;
		};

		struct GpuAPIState
		{
			gpuapi::RenderTargetBindingDynamicState rtBindingPass1Light;
			gpuapi::GraphicsPipelineStateObjectHandle psoPass1Light;
			gpuapi::GraphicsPipelineStateObjectHandle psoPass2Shadow;
			gpuapi::RenderPassConfigurationImmutableStateHandle renderPass1Light;
			gpuapi::RenderPassConfigurationImmutableStateHandle renderPass2Shadow;
			gpuapi::SamplerHandle samplerPass2Shadow;
		};

		struct Resources
		{
			gpuapi::GPUBufferHandle constantBuffer;
			gpuapi::TextureHandle shadowMapTexture;
			gpuapi::RenderTargetTextureHandle shadowMapRTT;
		};

	public:
		ShadowRenderer( ShaderLibrary & pShaderLibrary, const ShadowConfig & pShadowConfig );
		virtual ~ShadowRenderer();

		virtual void createRendererResources();

		const math::Mat4f & getLightProjectionMatrix() const
		{
			return _currentState.mLightProjection;
		}

		const math::Mat4f & getLightViewMatrix() const
		{
			return _currentState.mLightView;
		}

		const math::Mat4f & getLightSpaceMatrix() const
		{
			return _currentState.mLightSpace;
		}

		const math::Vec3f & getLightPosition() const
		{
			return _currentState.vLightPosition;
		}

		void setCSLightPosition( math::Vec3f pLightPosition );
		void setCSLightTarget( math::Vec3f pLightTarget );

		void setCSProjectionMatrixLightOrthoDefault();
		void setCSProjectionMatrixLightPerspectiveDefault();

		void updateMatricesForLightPass( gpuapi::CommandContext & pCommandContext );

		void updateMatricesForShadowPass( gpuapi::CommandContext & pCommandContext );

		void beginRenderPass1Light( gpuapi::CommandContext & pCommandContext );

		void beginRenderPass2Shadow( gpuapi::CommandContext & pCommandContext );

		void endRenderPass( gpuapi::CommandContext & pCommandContext );

	private:
		void initializeResources();

		void initializeRenderPassStates();

		void initializePipelineStateObjects();

	protected:
		gpuapi::GPUDevice & _gpuDevice;
		ShaderLibrary & _shaderLibrary;
		ShadowConfig _shadowConfig;
		GpuAPIState _gpuAPIState;
		Resources _resources;
		CurrentState _currentState;
	};

//	class ShadowRenderer0 : public ShadowRenderer
//	{
//	public:
//		ShadowRenderer0( ShaderLibraryHandle pShaderLibrary );
//		virtual ~ShadowRenderer0();
//
//	};


} // namespace ts3

#endif // __TS3_ENGINE_SHADOW_RENDERER_H__
