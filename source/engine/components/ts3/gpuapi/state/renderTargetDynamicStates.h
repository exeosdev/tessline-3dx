
#pragma once

#ifndef __TS3_GPUAPI_RENDER_TARGET_DYNAMIC_STATES_H__
#define __TS3_GPUAPI_RENDER_TARGET_DYNAMIC_STATES_H__

#include "renderPassCommon.h"

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

		TS3_ATTR_NO_DISCARD native_uint countActiveColorAttachments() const noexcept;

		TS3_ATTR_NO_DISCARD const RenderTargetBindingDefinition & getBindingDefinition() const noexcept;

		void assign( const RenderTargetBindingDefinition & pDefinition );

		RenderTargetAttachmentBinding & setColorAttachmentBinding( native_uint pIndex );

		void setColorAttachmentBinding( native_uint pIndex, const RenderTargetAttachmentBinding & pRPCAttachmentBinding );
		void setColorAttachmentBindings( const RenderTargetColorAttachmentBindingArray & pRPCAttachmentBindings );
		void setColorAttachmentBindings( native_uint pFirstIndex, native_uint pCount, const RenderTargetAttachmentBinding * pRPCAttachmentBindings );

		RenderTargetAttachmentBinding & setDepthStencilAttachmentBinding();

		void setDepthStencilAttachmentBinding( const RenderTargetAttachmentBinding & pRPDSAttachmentBinding );

		void resetColorAttachmentBinding( native_uint pIndex );
		void resetColorAttachmentBindings( native_uint pFirstIndex, native_uint pCount );
		void resetColorAttachmentBindings();

		void resetDepthStencilAttachmentBinding();

		void resetAllAttachmentBindings();

	private:
		void _setColorAttachmentBindings( native_uint pFirstIndex, native_uint pCount, const RenderTargetAttachmentBinding * pRPCAttachmentBindings );
		void _resetColorAttachmentBindings( native_uint pFirstIndex, native_uint pCount );

	private:
		RenderTargetBindingDefinition _renderTargetBindingDefinition;
	};

	class RenderPassConfigurationDynamicState
	{
	public:
		RenderPassConfigurationDynamicState( const RenderPassConfigurationDynamicState & ) = default;
		RenderPassConfigurationDynamicState & operator=( const RenderPassConfigurationDynamicState & ) = default;

		RenderPassConfigurationDynamicState();
		explicit RenderPassConfigurationDynamicState( const RenderPassConfiguration & pConfiguration );

		~RenderPassConfigurationDynamicState();

		TS3_ATTR_NO_DISCARD bool empty() const noexcept;

		TS3_ATTR_NO_DISCARD native_uint countActiveColorAttachments() const noexcept;

		TS3_ATTR_NO_DISCARD const RenderPassConfiguration & getRenderPassConfiguration() const noexcept;

		void assign( const RenderPassConfiguration & pConfiguration );

		RenderPassAttachmentConfig & setColorAttachmentUsage( native_uint pIndex );

		void setColorAttachmentUsage( native_uint pIndex, const RenderPassAttachmentConfig & pRPCAttachmentUsage );
		void setColorAttachmentUsages( const RenderPassColorAttachmentConfigArray & pRPCAttachmentUsages );
		void setColorAttachmentBindings( native_uint pFirstIndex, native_uint pCount, const RenderPassAttachmentConfig * pRPCAttachmentUsages );

		RenderPassAttachmentConfig & setDepthStencilAttachmentUsage();

		void setDepthStencilAttachmentUsage( const RenderPassAttachmentConfig & pRPDSAttachmentUsage );

		void resetColorAttachmentUsage( native_uint pIndex );
		void resetColorAttachmentUsages( native_uint pFirstIndex, native_uint pCount );
		void resetColorAttachmentUsages();

		void resetDepthStencilAttachmentUsage();

		void resetAllAttachmentUsages();

	private:
		void _setColorAttachmentUsages( native_uint pFirstIndex, native_uint pCount, const RenderPassAttachmentConfig * pRPCAttachmentUsages );
		void _resetColorAttachmentUsages( native_uint pFirstIndex, native_uint pCount );

	private:
		RenderPassConfiguration _renderPassConfiguration;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_RENDER_TARGET_DYNAMIC_STATES_H__
