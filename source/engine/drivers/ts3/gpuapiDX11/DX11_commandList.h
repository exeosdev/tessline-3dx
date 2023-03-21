
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_COMMAND_LIST_H__
#define __TS3DRIVER_GPUAPI_DX11_COMMAND_LIST_H__

#include "DX11_prerequisites.h"
#include "state/DX11_pipelineStateController.h"
#include <ts3/gpuapi/commandList.h>

namespace ts3::gpuapi
{

	/// @brief
	class TS3_GPUAPI_CLASS DX11CommandList : public CommandList
	{
		friend class DX11CommandSystem;

	public:
		ComPtr<ID3D11Device1> const mD3D11Device1 = nullptr;
		ComPtr<ID3D11DeviceContext1> const mD3D11DeviceContext1 = nullptr;

		DX11CommandList( DX11CommandSystem & pDX11CommandSystem, ECommandListType pListType, ComPtr<ID3D11DeviceContext1> pD3D11DeviceContext1 );
		virtual ~DX11CommandList();

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

	friendapi:
		ID3D11Query * releaseExecutionSyncQuery();

	private:
		void updatePipelineState();

		static ComPtr<ID3D11Query> _createExecutionSyncQuery( ComPtr<ID3D11Device1> pD3D11Device );

	private:
		ComPtr<ID3D11Query> _d3d11ExecutionSyncQuery;
		DX11GraphicsPipelineStateController _stateController;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX11_COMMAND_LIST_H__
