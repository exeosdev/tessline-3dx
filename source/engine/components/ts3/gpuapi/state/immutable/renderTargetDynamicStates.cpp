
#include "renderTargetDynamicStates.h"

namespace ts3::gpuapi
{

	RenderTargetBindingDynamicState::RenderTargetBindingDynamicState()
	{}

	RenderTargetBindingDynamicState::RenderTargetBindingDynamicState( const RenderTargetBindingDefinition & pDefinition )
	: _renderTargetBindingDefinition( pDefinition )
	{}

	RenderTargetBindingDynamicState::~RenderTargetBindingDynamicState() = default;

	bool RenderTargetBindingDynamicState::empty() const noexcept
	{
		return _renderTargetBindingDefinition.activeAttachmentsMask.empty();
	}

	uint32 RenderTargetBindingDynamicState::countActiveColorAttachments() const noexcept
	{
		return popCount( _renderTargetBindingDefinition.activeAttachmentsMask & E_RT_ATTACHMENT_MASK_COLOR_ALL );
	}

	const RenderTargetBindingDefinition & RenderTargetBindingDynamicState::getBindingDefinition() const noexcept
	{
		return _renderTargetBindingDefinition;
	}

	void RenderTargetBindingDynamicState::assign( const RenderTargetBindingDefinition & pDefinition )
	{
		_renderTargetBindingDefinition = pDefinition;
	}

	RenderTargetColorAttachmentBinding & RenderTargetBindingDynamicState::setColorAttachmentBinding( render_target_index_t pIndex )
	{
		ts3DebugAssert( cxdefs::isRTAttachmentIndexValid( pIndex ) );
		_renderTargetBindingDefinition.activeAttachmentsMask.set( cxdefs::makeRTAttachmentFlag( pIndex ) );
		return _renderTargetBindingDefinition.colorAttachments[pIndex];
	}

	void RenderTargetBindingDynamicState::setColorAttachmentBinding(
			render_target_index_t pIndex,
			const RenderTargetColorAttachmentBinding & pRPCAttachmentBinding )
	{
		_setColorAttachmentBindings( pIndex, 1, &pRPCAttachmentBinding );
	}

	void RenderTargetBindingDynamicState::setColorAttachmentBindings(
			const RenderTargetColorAttachmentBindingArray & pRPCAttachmentBindings )
	{
		_setColorAttachmentBindings( 0, pRPCAttachmentBindings.size(), pRPCAttachmentBindings.data() );
	}

	void RenderTargetBindingDynamicState::setColorAttachmentBindings(
			uint32 pFirstIndex,
			uint32 pCount,
			const RenderTargetColorAttachmentBinding * pRPCAttachmentBindings )
	{
		_setColorAttachmentBindings( pFirstIndex, pCount, pRPCAttachmentBindings );
	}

	RenderTargetDepthStencilAttachmentBinding & RenderTargetBindingDynamicState::setDepthStencilAttachmentBinding()
	{
		_renderTargetBindingDefinition.activeAttachmentsMask.set( E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT );
		return _renderTargetBindingDefinition.depthStencilAttachment;
	}

	void RenderTargetBindingDynamicState::setDepthStencilAttachmentBinding(
			const RenderTargetDepthStencilAttachmentBinding & pRPDSAttachmentBinding )
	{
		_renderTargetBindingDefinition.activeAttachmentsMask.setOrUnset( E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT, !pRPDSAttachmentBinding.empty() );
		_renderTargetBindingDefinition.depthStencilAttachment = pRPDSAttachmentBinding;
	}

	void RenderTargetBindingDynamicState::resetColorAttachmentBinding( render_target_index_t pIndex )
	{
		_resetColorAttachmentBindings( pIndex, 1 );
	}

	void RenderTargetBindingDynamicState::resetColorAttachmentBindings( uint32 pFirstIndex, uint32 pCount )
	{
		_resetColorAttachmentBindings( pFirstIndex, pCount );
	}

	void RenderTargetBindingDynamicState::resetColorAttachmentBindings()
	{
		_resetColorAttachmentBindings( 0, cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM );
	}

	void RenderTargetBindingDynamicState::resetDepthStencilAttachmentBinding()
	{
		_renderTargetBindingDefinition.activeAttachmentsMask.unset( E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT );
		_renderTargetBindingDefinition.depthStencilAttachment.reset();
	}

	void RenderTargetBindingDynamicState::resetAllAttachmentBindings()
	{
		resetColorAttachmentBindings();
		resetDepthStencilAttachmentBinding();
	}

	void RenderTargetBindingDynamicState::_setColorAttachmentBindings(
			uint32 pFirstIndex,
			uint32 pCount,
			const RenderTargetColorAttachmentBinding * pRPCAttachmentBindings )
	{
		for( uint32 caIndex = pFirstIndex; ( caIndex < cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM ) && ( pCount != 0 ); ++caIndex, --pCount )
		{
			const auto & sourceCABinding = pRPCAttachmentBindings[caIndex - pFirstIndex];

			const auto colorAttachmentBit = cxdefs::makeRTAttachmentFlag( caIndex );

			_renderTargetBindingDefinition.colorAttachments[caIndex] = sourceCABinding;
			_renderTargetBindingDefinition.activeAttachmentsMask.setOrUnset( colorAttachmentBit, !sourceCABinding.empty() );
		}
	}

	void RenderTargetBindingDynamicState::_resetColorAttachmentBindings( uint32 pFirstIndex, uint32 pCount )
	{
		for( uint32 caIndex = pFirstIndex; ( caIndex < cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM ) && ( pCount != 0 ); ++caIndex, --pCount )
		{
			const auto colorAttachmentBit = cxdefs::makeRTAttachmentFlag( caIndex );

			_renderTargetBindingDefinition.colorAttachments[caIndex].reset();
			_renderTargetBindingDefinition.activeAttachmentsMask.unset( colorAttachmentBit );
		}
	}


	RenderPassDynamicState::RenderPassDynamicState()
	{}

	RenderPassDynamicState::RenderPassDynamicState( const RenderPassConfiguration & pConfiguration )
	: _renderPassConfiguration( pConfiguration )
	{}

	RenderPassDynamicState::~RenderPassDynamicState() = default;

	bool RenderPassDynamicState::empty() const noexcept
	{
		return _renderPassConfiguration.activeAttachmentsMask.empty();
	}

	uint32 RenderPassDynamicState::countActiveColorAttachments() const noexcept
	{
		return popCount( _renderPassConfiguration.activeAttachmentsMask & E_RT_ATTACHMENT_MASK_COLOR_ALL );
	}

	const RenderPassConfiguration & RenderPassDynamicState::getRenderPassConfiguration() const noexcept
	{
		return _renderPassConfiguration;
	}

	void RenderPassDynamicState::assign( const RenderPassConfiguration & pConfiguration )
	{
		_renderPassConfiguration = pConfiguration;
	}

	RenderPassColorAttachmentUsage & RenderPassDynamicState::setColorAttachmentUsage( render_target_index_t pIndex )
	{
		ts3DebugAssert( cxdefs::isRTAttachmentIndexValid( pIndex ) );
		_renderPassConfiguration.activeAttachmentsMask.set( cxdefs::makeRTAttachmentFlag( pIndex ) );
		return _renderPassConfiguration.colorAttachments[pIndex];
	}

	void RenderPassDynamicState::setColorAttachmentUsage(
			render_target_index_t pIndex,
			const RenderPassColorAttachmentUsage & pRPCAttachmentUsage )
	{
		_setColorAttachmentUsages( pIndex, 1, &pRPCAttachmentUsage );
	}

	void RenderPassDynamicState::setColorAttachmentUsages(
			const RenderPassColorAttachmentUsageArray & pRPCAttachmentUsages )
	{
		_setColorAttachmentUsages( 0, pRPCAttachmentUsages.size(), pRPCAttachmentUsages.data() );
	}

	void RenderPassDynamicState::setColorAttachmentBindings(
			uint32 pFirstIndex,
			uint32 pCount,
			const RenderPassColorAttachmentUsage * pRPCAttachmentUsages )
	{
		_setColorAttachmentUsages( pFirstIndex, pCount, pRPCAttachmentUsages );
	}

	RenderPassDepthStencilAttachmentUsage & RenderPassDynamicState::setDepthStencilAttachmentUsage()
	{
		_renderPassConfiguration.activeAttachmentsMask.set( E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT );
		return _renderPassConfiguration.depthStencilAttachment;
	}

	void RenderPassDynamicState::setDepthStencilAttachmentUsage(
			const RenderPassDepthStencilAttachmentUsage & pRPDSAttachmentUsage )
	{
		_renderPassConfiguration.activeAttachmentsMask.setOrUnset( E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT, pRPDSAttachmentUsage.valid() );
		_renderPassConfiguration.depthStencilAttachment = pRPDSAttachmentUsage;
	}

	void RenderPassDynamicState::resetColorAttachmentUsage( render_target_index_t pIndex )
	{
		_resetColorAttachmentUsages( pIndex, 1 );
	}

	void RenderPassDynamicState::resetColorAttachmentUsages( uint32 pFirstIndex, uint32 pCount )
	{
		_resetColorAttachmentUsages( pFirstIndex, pCount );
	}

	void RenderPassDynamicState::resetColorAttachmentUsages()
	{
		_resetColorAttachmentUsages( 0, cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM );
	}

	void RenderPassDynamicState::resetDepthStencilAttachmentUsage()
	{
		_renderPassConfiguration.activeAttachmentsMask.unset( E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT );
		_renderPassConfiguration.depthStencilAttachment.reset();
	}

	void RenderPassDynamicState::resetAllAttachmentUsages()
	{
		resetColorAttachmentUsages();
		resetDepthStencilAttachmentUsage();
	}

	void RenderPassDynamicState::_setColorAttachmentUsages(
			uint32 pFirstIndex,
			uint32 pCount,
			const RenderPassColorAttachmentUsage * pRPCAttachmentUsages )
	{
		for( uint32 caIndex = pFirstIndex; ( caIndex < cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM ) && ( pCount != 0 ); ++caIndex, --pCount )
		{
			const auto & sourceCAUsage = pRPCAttachmentUsages[caIndex - pFirstIndex];

			const auto colorAttachmentBit = cxdefs::makeRTAttachmentFlag( caIndex );

			_renderPassConfiguration.colorAttachments[caIndex] = sourceCAUsage;
			_renderPassConfiguration.activeAttachmentsMask.setOrUnset( colorAttachmentBit, sourceCAUsage.valid() );
		}
	}

	void RenderPassDynamicState::_resetColorAttachmentUsages( uint32 pFirstIndex, uint32 pCount )
	{
		for( uint32 caIndex = pFirstIndex; ( caIndex < cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM ) && ( pCount != 0 ); ++caIndex, --pCount )
		{
			const auto colorAttachmentBit = cxdefs::makeRTAttachmentFlag( caIndex );

			_renderPassConfiguration.colorAttachments[caIndex].reset();
			_renderPassConfiguration.activeAttachmentsMask.unset( colorAttachmentBit );
		}
	}

}
