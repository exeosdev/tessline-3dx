
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DXCOMMON_PIPELINE_STATE_CONTROLLER_H__
#define __TS3DRIVER_GPUAPI_DXCOMMON_PIPELINE_STATE_CONTROLLER_H__

#include "../DX11_prerequisites.h"
#include <ts3/gpuapi/state/pipelineStateController.h>
#include <ts3/gpuapiDX11/state/DX11_pipelineStateDesc.h>

namespace ts3::gpuapi
{

	/// @brief
	class DX11GraphicsPipelineStateController : public SeparableGraphicsPipelineStateController
	{
		friend class DX11CommandList;

	public:
		DX11CommandList * const mDX11CommandList = nullptr;

		DX11GraphicsPipelineStateController( DX11CommandList & pDX11CommandList, DX11GraphicsPipelineStateDescriptorCache & pDescriptorCache );
		~DX11GraphicsPipelineStateController();

		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO ) override;

		virtual bool setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO ) override;

	protected:
		virtual bool updatePipelineState() override;

	protected:
		DX11GraphicsPipelineStateDescriptorCache * _descriptorCache;
	};

}

#endif // __TS3DRIVER_GPUAPI_DXCOMMON_PIPELINE_STATE_CONTROLLER_H__
