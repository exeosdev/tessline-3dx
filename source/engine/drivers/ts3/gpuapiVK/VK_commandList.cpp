
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

	bool VKCommandList::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO )
	{
		return true;
	}

	bool VKCommandList::setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO )
	{
		return true;
	}

	void VKCommandList::clearRenderTarget( Bitmask<ERenderTargetAttachmentFlags> pAttachmentMask )
	{
	}

	void VKCommandList::setViewport( const ViewportDesc & pViewportDesc )
	{
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

	void VKCommandList::drawDirectIndexed( native_uint pIndicesNum, native_uint pIndicesOffset )
	{
	}

	void VKCommandList::drawDirectIndexedInstanced( native_uint pIndicesNumPerInstance, native_uint pInstancesNum, native_uint pIndicesOffset )
	{
	}

	void VKCommandList::drawDirectNonIndexed( native_uint pVerticesNum, native_uint pVerticesOffset )
	{
	}

	void VKCommandList::drawDirectNonIndexedInstanced( native_uint pVerticesNumPerInstance, native_uint pInstancesNum, native_uint pVerticesOffset )
	{
	}

} // namespace ts3::gpuapi
