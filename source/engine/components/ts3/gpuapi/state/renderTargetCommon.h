
#pragma once

#ifndef __TS3_GPUAPI_RENDER_TARGET_COMMON_H__
#define __TS3_GPUAPI_RENDER_TARGET_COMMON_H__

#include "commonGPUStateDefs.h"
#include <ts3/stdext/bitUtils.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( RenderTargetTexture );

	/// @brief
	struct RenderTargetAttachmentBinding
	{
		RenderTargetTextureHandle attachmentTexture;

		void reset()
		{
			attachmentTexture.reset();
		}

		bool empty() const noexcept
		{
			return !attachmentTexture;
		}

		explicit operator bool() const noexcept
		{
			return ( bool )attachmentTexture;
		}
	};

	/// @brief
	struct RenderTargetColorAttachmentBinding : public RenderTargetAttachmentBinding
	{
		RenderTargetTextureHandle resolveTargetTexture;
	};

	/// @brief
	struct RenderTargetDepthStencilAttachmentBinding : public RenderTargetAttachmentBinding
	{
	};

	using RenderTargetColorAttachmentBindingArray = std::array<RenderTargetColorAttachmentBinding, cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM>;

	/// @brief
	struct RenderTargetBindingDefinition
	{
		///
		Bitmask<ERTAttachmentFlags> activeAttachmentsMask;
		///
		RenderTargetColorAttachmentBindingArray colorAttachments;
		///
		RenderTargetDepthStencilAttachmentBinding depthStencilAttachment;
	};

	/// @brief
	struct RenderTargetAttachmentLayout
	{
		ETextureFormat format = ETextureFormat::UNKNOWN;

		void reset()
		{
			format = ETextureFormat::UNKNOWN;
		}

		bool valid() const noexcept
		{
			return format != ETextureFormat::UNKNOWN;
		}

		explicit operator bool() const noexcept
		{
			return valid();
		}
	};

	/// @brief A definition of a vertex layout used to create a driver-specific RenderTargetLayout object.
	struct RenderTargetLayout
	{
		/// Typedef for ordered, fixed-size array of layout definitions for render target attachments.
		/// Entries are ordered according to ERTAttachmentIndex values.
		using ColorAttachmentLayoutArray = std::array<RenderTargetAttachmentLayout, cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM>;
		///
		Bitmask<ERTAttachmentFlags> activeAttachmentsMask;
		///
		ColorAttachmentLayoutArray colorAttachments;
		///
		RenderTargetAttachmentLayout depthStencilAttachment;

		explicit operator bool() const noexcept
		{
			return valid();
		}

		TS3_ATTR_NO_DISCARD bool valid() const noexcept
		{
			return activeAttachmentsMask.isSetAnyOf( E_RT_ATTACHMENT_MASK_COLOR_ALL );
		}

		TS3_ATTR_NO_DISCARD uint32 countActiveColorAttachments() const noexcept
		{
			return popCount( activeAttachmentsMask & E_RT_ATTACHMENT_MASK_COLOR_ALL );
		}

		TS3_ATTR_NO_DISCARD uint32 hasDepthStencilAttachment() const noexcept
		{
			return activeAttachmentsMask.isSet( E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT );
		}
	};

	namespace smutil
	{

		TS3_GPUAPI_API_NO_DISCARD RenderTargetLayout getRenderTargetLayoutForBindingDefinition(
				const RenderTargetBindingDefinition & pBindingDefinition );

	}

	namespace defaults
	{

		TS3_GPUAPI_OBJ const RenderTargetLayout cvRenderTargetLayoutDefaultBGRA8;
		TS3_GPUAPI_OBJ const RenderTargetLayout cvRenderTargetLayoutDefaultBGRA8D24S8;
		TS3_GPUAPI_OBJ const RenderTargetLayout cvRenderTargetLayoutDefaultRGBA8;
		TS3_GPUAPI_OBJ const RenderTargetLayout cvRenderTargetLayoutDefaultRGBA8D24S8;

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_RENDER_TARGET_COMMON_H__
