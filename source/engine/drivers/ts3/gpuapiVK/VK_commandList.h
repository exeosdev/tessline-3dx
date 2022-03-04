
#pragma once

#ifndef __TS3DRIVER_GPUAPI_VKCOMMON_COMMAND_LIST_H__
#define __TS3DRIVER_GPUAPI_VKCOMMON_COMMAND_LIST_H__

#include "VK_prerequisites.h"
#include <ts3/gpuapi/commandList.h>

namespace ts3
{
namespace gpuapi
{

	/// @brief
	class TS3_GPUAPI_CLASS VKCommandList : public CommandList
	{
		friend class VKCommandSystem;

	public:
		VKCommandList( VKCommandSystem & pVKCommandSystem, ECommandListType pListType );
		virtual ~VKCommandList();

		virtual void beginCommandSequence() override;
		virtual void endCommandSequence() override;

		virtual void executeDeferredContext( CommandContextDeferred & pDeferredContext ) override;

		virtual void clearRenderTarget( Bitmask<ERenderTargetAttachmentFlags> pAttachmentMask ) override;
		virtual void setViewport( const ViewportDesc & pViewportDesc ) override;
		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO ) override;
		virtual bool setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO ) override;
		virtual bool setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData ) override;
		virtual bool setShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer ) override;
		virtual bool setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture ) override;
		virtual bool setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler ) override;
		virtual void drawDirectIndexed( uint32 pIndicesNum, uint32 pIndicesOffset ) override;
		virtual void drawDirectIndexedInstanced( uint32 pIndicesNumPerInstance, uint32 pInstancesNum, uint32 pIndicesOffset ) override;
		virtual void drawDirectNonIndexed( uint32 pVerticesNum, uint32 pVerticesOffset ) override;
		virtual void drawDirectNonIndexedInstanced( uint32 pVerticesNumPerInstance, uint32 pInstancesNum, uint32 pVerticesOffset ) override;
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_VKCOMMON_COMMAND_LIST_H__
