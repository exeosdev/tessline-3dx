
#pragma once

#ifndef __TS3_GPUAPI_RENDER_TARGET_DYNAMIC_STATES_H__
#define __TS3_GPUAPI_RENDER_TARGET_DYNAMIC_STATES_H__

#include "../renderPassCommon.h"

namespace ts3::gpuapi
{

	class RenderTargetBindingDynamicState
	{
	public:
		RenderTargetBindingDynamicState( const RenderTargetBindingDynamicState & ) = default;
		RenderTargetBindingDynamicState & operator=( const RenderTargetBindingDynamicState & ) = default;

		RenderTargetBindingDynamicState();
		explicit RenderTargetBindingDynamicState( const RenderTargetBindingDefinition & pDefinition );

		~RenderTargetBindingDynamicState();

		TS3_ATTR_NO_DISCARD bool empty() const noexcept;

		TS3_ATTR_NO_DISCARD uint32 countActiveColorAttachments() const noexcept;

		TS3_ATTR_NO_DISCARD const RenderTargetBindingDefinition & getBindingDefinition() const noexcept;

		void assign( const RenderTargetBindingDefinition & pDefinition );

		RenderTargetColorAttachmentBinding & setColorAttachmentBinding( render_target_index_t pIndex );

		void setColorAttachmentBinding( render_target_index_t pIndex, const RenderTargetColorAttachmentBinding & pRPCAttachmentBinding );
		void setColorAttachmentBindings( const RenderTargetColorAttachmentBindingArray & pRPCAttachmentBindings );
		void setColorAttachmentBindings( uint32 pFirstIndex, uint32 pCount, const RenderTargetColorAttachmentBinding * pRPCAttachmentBindings );

		RenderTargetDepthStencilAttachmentBinding & setDepthStencilAttachmentBinding();

		void setDepthStencilAttachmentBinding( const RenderTargetDepthStencilAttachmentBinding & pRPDSAttachmentBinding );

		void resetColorAttachmentBinding( render_target_index_t pIndex );
		void resetColorAttachmentBindings( uint32 pFirstIndex, uint32 pCount );
		void resetColorAttachmentBindings();

		void resetDepthStencilAttachmentBinding();

		void resetAllAttachmentBindings();

	private:
		void _setColorAttachmentBindings( uint32 pFirstIndex, uint32 pCount, const RenderTargetColorAttachmentBinding * pRPCAttachmentBindings );
		void _resetColorAttachmentBindings( uint32 pFirstIndex, uint32 pCount );

	private:
		RenderTargetBindingDefinition _renderTargetBindingDefinition;
	};

	class RenderPassDynamicState
	{
	public:
		RenderPassDynamicState( const RenderPassDynamicState & ) = default;
		RenderPassDynamicState & operator=( const RenderPassDynamicState & ) = default;

		RenderPassDynamicState();
		explicit RenderPassDynamicState( const RenderPassConfiguration & pConfiguration );

		~RenderPassDynamicState();

		TS3_ATTR_NO_DISCARD bool empty() const noexcept;

		TS3_ATTR_NO_DISCARD uint32 countActiveColorAttachments() const noexcept;

		TS3_ATTR_NO_DISCARD const RenderPassConfiguration & getRenderPassConfiguration() const noexcept;

		void assign( const RenderPassConfiguration & pConfiguration );

		RenderPassColorAttachmentUsage & setColorAttachmentUsage( render_target_index_t pIndex );

		void setColorAttachmentUsage( render_target_index_t pIndex, const RenderPassColorAttachmentUsage & pRPCAttachmentUsage );
		void setColorAttachmentUsages( const RenderPassColorAttachmentUsageArray & pRPCAttachmentUsages );
		void setColorAttachmentBindings( uint32 pFirstIndex, uint32 pCount, const RenderPassColorAttachmentUsage * pRPCAttachmentUsages );

		RenderPassDepthStencilAttachmentUsage & setDepthStencilAttachmentUsage();

		void setDepthStencilAttachmentUsage( const RenderPassDepthStencilAttachmentUsage & pRPDSAttachmentUsage );

		void resetColorAttachmentUsage( render_target_index_t pIndex );
		void resetColorAttachmentUsages( uint32 pFirstIndex, uint32 pCount );
		void resetColorAttachmentUsages();

		void resetDepthStencilAttachmentUsage();

		void resetAllAttachmentUsages();

	private:
		void _setColorAttachmentUsages( uint32 pFirstIndex, uint32 pCount, const RenderPassColorAttachmentUsage * pRPCAttachmentUsages );
		void _resetColorAttachmentUsages( uint32 pFirstIndex, uint32 pCount );

	private:
		RenderPassConfiguration _renderPassConfiguration;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_RENDER_TARGET_DYNAMIC_STATES_H__
