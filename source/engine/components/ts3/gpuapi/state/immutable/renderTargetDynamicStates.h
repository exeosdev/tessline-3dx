
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

		void assign( const RenderTargetBindingDefinition & pDefinition );

		RenderTargetColorAttachmentBinding & setColorAttachmentBinding( render_target_index_t pIndex );

		void setColorAttachmentBinding( render_target_index_t pIndex, const RenderTargetColorAttachmentBinding & pRPCAttachmentBinding );
		void setColorAttachmentBindings( render_target_index_t pFirstIndex, const ArrayView<RenderTargetColorAttachmentBinding> & pRPCAttachmentBindings );
		void setColorAttachmentBindings( const RenderTargetColorAttachmentBindingArray & pRPCAttachmentBindings );
		void setColorAttachmentBindings( const RenderTargetColorAttachmentBindingArray & pRPCAttachmentBindings, uint32 pFirstIndex, uint32 pCount );

		RenderTargetDepthStencilAttachmentBinding & setDepthStencilAttachmentBinding();

		void setDepthStencilAttachmentBinding( const RenderTargetDepthStencilAttachmentBinding & pRPDSAttachmentBindings );

		void resetColorAttachmentBinding( render_target_index_t pIndex );
		void resetColorAttachmentBindings( uint32 pFirstIndex, uint32 pCount);
		void resetColorAttachmentBindings();

		void resetDepthStencilAttachmentBinding();

		void resetAllBindings();

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

		void assign( const RenderPassConfiguration & pConfiguration );

		RenderPassColorAttachmentUsage & setColorAttachmentUsage( render_target_index_t pIndex );

		void setColorAttachmentUsage( render_target_index_t pIndex, const RenderPassColorAttachmentUsage & pRPCAttachmentUsage );
		void setColorAttachmentUsages( render_target_index_t pFirstIndex, const ArrayView<RenderPassColorAttachmentUsage> & pRPCAttachmentUsages );
		void setColorAttachmentUsages( const RenderPassColorAttachmentUsageArray & pRPCAttachmentUsages );
		void setColorAttachmentUsages( const RenderPassColorAttachmentUsageArray & pRPCAttachmentUsages, uint32 pFirstIndex, uint32 pCount );

		RenderPassDepthStencilAttachmentUsage & setDepthStencilAttachmentUsage();

		void setDepthStencilAttachmentUsage( const RenderPassDepthStencilAttachmentUsage & pRPDSAttachmentUsage );

		void resetColorAttachmentUsage( render_target_index_t pIndex );
		void resetColorAttachmentUsages( uint32 pFirstIndex, uint32 pCount);
		void resetColorAttachmentUsages();

		void resetDepthStencilAttachmentUsage();

		void resetAllUsages();

	private:
		RenderPassConfiguration _renderPassConfiguration;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_RENDER_TARGET_DYNAMIC_STATES_H__
