
#pragma once

#ifndef __TS3_ENGINE_SHADOW_RENDERER_H__
#define __TS3_ENGINE_SHADOW_RENDERER_H__

#include "../commonRendererDefs.h"

#include <ts3/gpuapi/state/renderTargetDynamicStates.h>

namespace ts3
{

	struct ShadowConfig
	{
		gpuapi::TextureSize2D screenSize;
		gpuapi::TextureSize2D shadowMapSize;
	};

	class ShadowRenderer
	{
	public:
		struct Pass1LightConstantBufferData
		{
			math::Vec3f v3fObjectSpaceLightPos;
			math::Vec3f v3fLightDiffuseColor;
			math::Mat4f m4fModel;
			math::Mat4f m4fSpace;
		};

		struct Pass2ShadowConstantBufferData
		{
			math::Vec3f v3fObjectSpaceLightPos;
			math::Vec3f v3fLightDiffuseColor;

			math::Mat4f m4fModel;
			math::Mat4f m4fModelView;
			math::Mat4f m4fModelViewProjection;
			math::Mat3f m4fNormal;
			math::Mat4f m4fShadow;
		};

		struct CurrentState
		{
			math::Vec3f vLightPosition;
			math::Vec3f vLightDiffuseColor;
			math::Mat4f mModel;
			math::Mat4f mView;
			math::Mat4f mProjection;
			math::Mat4f mSpace;
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
		ShadowRenderer( ShaderLibraryHandle pShaderLibrary, const ShadowConfig & pShadowConfig );
		virtual ~ShadowRenderer();

		virtual void createRendererResources();

		void setCSLightDiffuseColor( math::Vec3f pColor );
		void setCSLightPosition( math::Vec3f pLightPosition );

		void setCSProjectionMatrix( math::Mat4f pProjectionMatrix );
		void setCSProjectionMatrixLightOrthoDefault();
		void setCSProjectionMatrixLightPerspectiveDefault();

		void setCSModelMatrix( math::Mat4f pModelMatrix );
		void setCSViewMatrix( math::Mat4f pViewMatrix );

		void updateMatricesForLightPass();

		void updateMatricesForShadowPass();

		void beginRenderPass1Light( gpuapi::CommandContext & pCommandContext );

		void beginRenderPass2Shadow(
				gpuapi::CommandContext & pCommandContext,
				const gpuapi::RenderTargetBindingDynamicState & pRenderTargetBinding );

		void endRenderPass( gpuapi::CommandContext & pCommandContext );

	private:
		void initializeResources();

		void initializeRenderPassStates();

		void initializePipelineStateObjects();

	protected:
		gpuapi::GPUDevice & _gpuDevice;
		ShaderLibraryHandle _shaderLibrary;
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