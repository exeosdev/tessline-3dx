
#include "VK_commandList.h"
#include "VK_commandSystem.h"
#include <ts3/gpuapi/commandContext.h>
#include <ts3/gpuapi/resources/gpuBuffer.h>
#include <ts3/gpuapi/resources/shader.h>
#include <ts3/gpuapi/resources/texture.h>

namespace ts3::gpuapi
{

	VKCommandList::VKCommandList( VKCommandSystem & pVKCommandSystem, ECommandListType pListType )
	: CommandList( pVKCommandSystem, pListType )
	, _stateController( *this, mGPUDevice.queryInterface<VKGPUDevice>()->getDescriptorCache() )
	{ }

	VKCommandList::~VKCommandList() = default;

	void VKCommandList::beginCommandSequence()
	{
		CommandList::beginCommandSequence();
	}

	void VKCommandList::endCommandSequence()
	{
		CommandList::endCommandSequence();
	}

	void VKCommandList::executeDeferredContext( CommandContextDeferred & pDeferredContext )
	{
	}

	void VKCommandList::clearRenderTarget( Bitmask<ERenderTargetAttachmentFlags> pAttachmentMask )
	{
	}

	void VKCommandList::setViewport( const ViewportDesc & pViewportDesc )
	{
	}

	bool VKCommandList::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO )
	{
		return true;
	}

	bool VKCommandList::setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO )
	{
		return true;
	}

	bool VKCommandList::setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData )
	{
		return true;
	}

	bool VKCommandList::setShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer )
	{
		return true;
	}

	bool VKCommandList::setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture )
	{
		return true;
	}

	bool VKCommandList::setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler )
	{
		return true;
	}

	void VKCommandList::drawDirectIndexed( uint32 pIndicesNum, uint32 pIndicesOffset )
	{
	}

	void VKCommandList::drawDirectIndexedInstanced( uint32 pIndicesNumPerInstance, uint32 pInstancesNum, uint32 pIndicesOffset )
	{
	}

	void VKCommandList::drawDirectNonIndexed( uint32 pVerticesNum, uint32 pVerticesOffset )
	{
	}

	void VKCommandList::drawDirectNonIndexedInstanced( uint32 pVerticesNumPerInstance, uint32 pInstancesNum, uint32 pVerticesOffset )
	{
	}

}
