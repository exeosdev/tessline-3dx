
#pragma once

#ifndef __TS3_ENGINE_SHADOW_RENDERER_H__
#define __TS3_ENGINE_SHADOW_RENDERER_H__

#include "commonRendererDefs.h"
#include <ts3/gpuapi/state/renderTargetDynamicStates.h>

namespace ts3
{

	struct RLConstantBufferDataCommonModel
	{
		math::Mat4f m4fModelMatrix;
	};

	struct RLConstantBufferDataCommonScene
	{
		math::Mat4f m4fViewMatrix;
		math::Mat4f m4fProjectionMatrix;
	};

	class Renderer
	{
	public:
		gpuapi::GPUDeviceHandle const mGPUDevice;
		ShaderLibraryHandle const mShaderLibrary;

	public:
		Renderer( ShaderLibraryHandle pShaderLibrary );
		virtual ~Renderer();

		virtual void createRendererResources();

		void beginRendering( gpuapi::CommandContext & pCommandContext );
		void endRendering();

		void assignConstantBuffer( native_uint pCBIndex, gpuapi::GPUBufferHandle pConstantBuffer );

		template <typename TCBData>
		void createConstantBuffer();

		template <typename TCBData>
		bool setConstantBufferData( native_uint pCBIndex, const TCBData & pCBData );

		template <typename TCBData>
		TCBData * updateConstantBufferData( native_uint pCBIndex );

	protected:
		struct GpuAPIStateCommon
		{
		};

		struct ConstantBufferLocalDataRef
		{
			uint32 bufferSize = 0;
			void * dataPtr = nullptr;
			uint32 dataSize = 0;
			uint32 modified = 0;
		};

		struct LocalDataCache
		{
			RLConstantBufferDataCommonModel cbDataCommonModel;
			RLConstantBufferDataCommonScene cbDataCommonScene;
		};

	protected:
		ShaderLibraryHandle _shaderLibrary;
		GpuAPIStateCommon _gpaStateCommon;
		gpuapi::CommandContextHandle _currentCmdContext;
	};


} // namespace ts3

#endif // __TS3_ENGINE_SHADOW_RENDERER_H__
