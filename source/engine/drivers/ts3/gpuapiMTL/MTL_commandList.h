
#pragma once

#ifndef __TS3DRIVER_GPUAPI_MTLCOMMON_GPU_COMMAND_LIST_H__
#define __TS3DRIVER_GPUAPI_MTLCOMMON_GPU_COMMAND_LIST_H__

#include "MTL_prerequisites.h"
#include <ts3/gpuapi/commandList.h>

#include <Metal/MTLCommandBuffer.h>
#include <Metal/MTLCommandEncoder.h>

namespace ts3::gpuapi
{

	/// @brief
	class MetalCommandList : public CommandList
	{
	public:
		///
		id<MTLDevice> mMTLDevice;
		///
		id<MTLCommandQueue> const mMTLCommandQueue;

	public:
		MetalCommandList( MetalCommandSystem & pMTLCommandSystem, ECommandListType pListType, id<MTLCommandQueue> pMTLCommandQueue );
		virtual ~MetalCommandList();

		virtual bool beginRenderPass(
				const RenderPassConfigurationImmutableState & pRenderPassState,
				Bitmask<ECommandListActionFlags> pFlags ) override;

		virtual bool beginRenderPass(
				const RenderPassConfigurationDynamicState & pRenderPassState,
				Bitmask<ECommandListActionFlags> pFlags ) override;

		virtual void endRenderPass() override;

		virtual void beginCommandSequence() override;
		virtual void endCommandSequence() override;

		virtual void cmdDrawDirectIndexed( native_uint pIndicesNum, native_uint pIndicesOffset ) override;
		virtual void cmdDrawDirectIndexedInstanced( native_uint pIndicesNumPerInstance, native_uint pInstancesNum, native_uint pIndicesOffset ) override;
		virtual void cmdDrawDirectNonIndexed( native_uint pVerticesNum, native_uint pVerticesOffset ) override;
		virtual void cmdDrawDirectNonIndexedInstanced( native_uint pVerticesNumPerInstance, native_uint pInstancesNum, native_uint pVerticesOffset ) override;

		virtual void cmdExecuteDeferredContext( CommandContextDeferred & pDeferredContext ) override;

	private:
		id<MTLRenderCommandEncoder> _currentPassCommandEncoder;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_MTLCOMMON_GPU_COMMAND_LIST_H__
