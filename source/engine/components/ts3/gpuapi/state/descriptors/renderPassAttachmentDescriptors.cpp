
#include "renderPassAttachmentDescriptors.h"

namespace ts3::gpuapi
{

	bool RenderPassAttachmentBaseDescriptor::configureLoadAction( ERenderPassAttachmentLoadAction pLoadAction )
	{
		const auto actionMask = makeBitmask<uint32>( pLoadAction );

		_renderPassActionMask.unset( E_RENDER_PASS_ACTION_MASK_LOAD_ALL_BITS );
		_renderPassActionMask.set( actionMask );
		_renderPassLoadAction = pLoadAction;

		return true;
	}

	bool RenderPassAttachmentBaseDescriptor::configureStoreAction( ERenderPassAttachmentStoreAction pStoreAction )
	{
		// Resolve can be enabled using configureResolve() for color attachments;
		if( ( pStoreAction == ERenderPassAttachmentStoreAction::Resolve ) || ( pStoreAction == ERenderPassAttachmentStoreAction::KeepResolve ) )
		{
			return false;
		}

		const auto actionMask = makeBitmask<uint32>( pStoreAction );

		_renderPassActionMask.unset( E_RENDER_PASS_ACTION_MASK_STORE_ALL_BITS );
		_renderPassActionMask.set( actionMask );
		_renderPassStoreAction = pStoreAction;

		return true;
	}


	bool RenderPassColorAttachmentDescriptor::configureResolve( RenderTargetTextureHandle pResolveTargetTexture )
	{
		if( pResolveTargetTexture )
		{
			if( _renderPassStoreAction == ERenderPassAttachmentStoreAction::Keep )
			{
				// Store action 'Keep' with additional resolve becomes 'KeepResolve'.
				_renderPassStoreAction = ERenderPassAttachmentStoreAction::KeepResolve;
				_renderPassActionMask.set( E_RENDER_PASS_ACTION_FLAG_STORE_RESOLVE_BIT );
			}
			else
			{
				// Any other action becomes simply 'Resolve' as resolving overwrites them.
				_renderPassStoreAction = ERenderPassAttachmentStoreAction::Resolve;
				_renderPassActionMask.set( E_RENDER_PASS_ACTION_FLAG_STORE_RESOLVE_BIT );
			}

			// Update the resolve target texture.
			_resolveTargetTexture = pResolveTargetTexture;
		}
		else
		{
			if( _renderPassStoreAction == ERenderPassAttachmentStoreAction::KeepResolve )
			{
				// Store action 'KeepResolve' with resolve action removed becomes 'Keep'.
				_renderPassActionMask.unset( E_RENDER_PASS_ACTION_FLAG_STORE_RESOLVE_BIT );
				_renderPassStoreAction = ERenderPassAttachmentStoreAction::Keep;
			}
			else if( _renderPassStoreAction == ERenderPassAttachmentStoreAction::Resolve )
			{
				// Store action 'Resolve' with resolve action removed becomes an undefined action.
				_renderPassActionMask.unset( E_RENDER_PASS_ACTION_FLAG_STORE_RESOLVE_BIT );
				_renderPassStoreAction = ERenderPassAttachmentStoreAction::Undefined;
			}

			// Reset the resolve target texture.
			_resolveTargetTexture = nullptr;
		}
	}


	void RenderPassAttachmentDescriptorSet::_updateInternal()
	{}

	void RenderPassAttachmentDescriptorSet::_setColorAttachmentDescriptor( render_target_index_t pIndex,
	                                                                       const RenderPassColorAttachmentDescriptor & pDescriptor )
	{
		auto & descriptorRef = _colorAttachmentsDescriptors[pIndex];

		const auto previousBindingStatus = !descriptorRef.empty();
		const auto colorAttachmentBindingFlag = cxdefs::makeRTAttachmentFlag( pIndex );

		descriptorRef = pDescriptor;

		if( !pDescriptor.empty() && !previousBindingStatus )
		{
			ts3DebugAssert( !_activeAttachmentsMask.isSet( colorAttachmentBindingFlag ) );
			_activeAttachmentsMask.set( colorAttachmentBindingFlag );
			_activeColorAttachmentDescriptorsNum += 1;
		}
		else if( pDescriptor.empty() && previousBindingStatus )
		{
			ts3DebugAssert( _activeAttachmentsMask.isSet( colorAttachmentBindingFlag ) && ( _activeColorAttachmentDescriptorsNum > 0 ) );
			_activeAttachmentsMask.unset( colorAttachmentBindingFlag );
			_activeColorAttachmentDescriptorsNum -= 1;
		}
	}

	void RenderPassAttachmentDescriptorSet::_setDepthStencilAttachmentDescriptor( const RenderPassDepthStencilAttachmentDescriptor & pDescriptor )
	{
		const auto previousBindingStatus = !_depthStencilAttachmentsDescriptor.empty();

		_depthStencilAttachmentsDescriptor = pDescriptor;

		if( !pDescriptor.empty() && !previousBindingStatus )
		{
			ts3DebugAssert( !_activeAttachmentsMask.isSet( E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT ) );
			_activeAttachmentsMask.set( E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT );
		}
		else if( pDescriptor.empty() && previousBindingStatus )
		{
			ts3DebugAssert( _activeAttachmentsMask.isSet( E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT ) );
			_activeAttachmentsMask.unset( E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT );
		}
	}

}
