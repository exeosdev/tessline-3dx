
#pragma once

#ifndef __TS3_GPUAPI_RENDER_PASS_COMMON_H__
#define __TS3_GPUAPI_RENDER_PASS_COMMON_H__

#include "renderTargetCommon.h"

namespace ts3::gpuapi
{

	/// @brief
	enum ERenderPassAttachmentActionFlags : uint32
	{
		E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_LOAD_CLEAR_BIT            = 0x01,
		E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_LOAD_DISCARD_BIT          = 0x02,
		E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_LOAD_FETCH_BIT            = 0x04,
		E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_LOAD_RESTRICT_ACCESS_BIT  = 0x08,
		E_RENDER_PASS_ATTACHMENT_ACTION_FLAGS_LOAD_ALL                 = 0x0F,
		E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_STORE_DISCARD_BIT         = 0x0100,
		E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_STORE_KEEP_BIT            = 0x0200,
		E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_STORE_RESOLVE_BIT         = 0x0400,
		E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_STORE_RESTRICT_ACCESS_BIT = 0x0800,
		E_RENDER_PASS_ATTACHMENT_ACTION_FLAGS_STORE_ALL                = 0x0F00,
	};

	/// @brief Specifies how the contents of render pass attachments are treated at the beginning of a render pass.
	enum class ERenderPassAttachmentLoadAction : uint32
	{
		Clear = E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_LOAD_CLEAR_BIT,
		/// Indicates that the pass does not have any dependency on the previous contents of the resource.
		/// The resource may have the previous data or some uninitialized one. No guarantees are given.
		Discard = E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_LOAD_DISCARD_BIT,
		Fetch = E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_LOAD_FETCH_BIT,
		RestrictAccess = E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_LOAD_RESTRICT_ACCESS_BIT,
		Undefined = 0
	};

	/// @brief Specifies how the contents of render pass attachments are treated at the end of a render pass.
	enum class ERenderPassAttachmentStoreAction : uint32
	{
		/// Indicates that the contents need not to be preserved after the pass is finished. The contents MAY
		/// (but not necessarily have to) be discarded by the driver. There should be no assumptions about it.
		Discard = E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_STORE_DISCARD_BIT,
		/// The contents of the resource is to be preserved and kept after the pass is done.
		/// This indicates there will be a dependency on the data in the future.
		Keep = E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_STORE_KEEP_BIT,
		KeepResolve = E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_STORE_KEEP_BIT | E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_STORE_RESOLVE_BIT,
		Resolve = E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_STORE_RESOLVE_BIT,
		RestrictAccess = E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_STORE_RESTRICT_ACCESS_BIT,
		Undefined = 0
	};

	struct RenderPassAttachmentConfig
	{
		RenderTargetAttachmentClearConfig clearConfig;
		Bitmask<ERenderTargetBufferFlags> clearMask;
		ERenderPassAttachmentLoadAction renderPassLoadAction = ERenderPassAttachmentLoadAction::Undefined;
		ERenderPassAttachmentStoreAction renderPassStoreAction = ERenderPassAttachmentStoreAction::Undefined;

		void reset()
		{
			renderPassLoadAction = ERenderPassAttachmentLoadAction::Undefined;
			renderPassStoreAction = ERenderPassAttachmentStoreAction::Undefined;
		}

		bool empty() const noexcept
		{
			return renderPassLoadAction == ERenderPassAttachmentLoadAction::Undefined ||
			       renderPassStoreAction == ERenderPassAttachmentStoreAction::Undefined;
		}

		explicit operator bool() const noexcept
		{
			return !empty();
		}
	};

	using RenderPassColorAttachmentConfigArray = RenderTargetColorAttachmentPropertyArray<RenderPassAttachmentConfig>;

	struct RenderPassConfiguration : public RenderTargetAttachmentConfigurationSet<RenderPassAttachmentConfig>
	{
		Bitmask<ERTAttachmentFlags> attachmentsAccessRestrictMask = 0;

		Bitmask<ERTAttachmentFlags> attachmentActionClearMask = 0;

		Bitmask<ERTAttachmentFlags> attachmentActionDiscardMask = 0;

		TS3_ATTR_NO_DISCARD uint32 countAttachmentsAccessRestrict() const noexcept
		{
			return popCount( attachmentsAccessRestrictMask & E_RT_ATTACHMENT_MASK_ALL );
		}

		TS3_ATTR_NO_DISCARD uint32 countAttachmentsActionClear() const noexcept
		{
			return popCount( attachmentActionClearMask & E_RT_ATTACHMENT_MASK_ALL );
		}

		TS3_ATTR_NO_DISCARD uint32 countAttachmentsActionDiscard() const noexcept
		{
			return popCount( attachmentActionDiscardMask & E_RT_ATTACHMENT_MASK_ALL );
		}

		TS3_ATTR_NO_DISCARD RenderPassConfiguration getValidated() const noexcept
		{
			RenderPassConfiguration validatedConfiguration = *this;
			validatedConfiguration.resetAttachmentsFlags();
			return validatedConfiguration;
		}

		TS3_GPUAPI_API void resetAttachmentsFlags() noexcept;
	};

	namespace smutil
	{

		TS3_GPUAPI_API_NO_DISCARD Bitmask<ERenderPassAttachmentActionFlags> getRenderPassAttachmentActionMask(
				const RenderPassAttachmentConfig & pAttachmentConfig );

		TS3_GPUAPI_API_NO_DISCARD Bitmask<ERTAttachmentFlags> getRenderPassAttachmentArbitraryActionMask(
				const RenderPassColorAttachmentConfigArray & pColorAttachments,
				const RenderPassAttachmentConfig pDepthStencilAttachment,
				Bitmask<ERTAttachmentFlags> pActiveAttachmentsMask,
				Bitmask<ERenderPassAttachmentActionFlags> pActionMask );

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_RENDER_PASS_COMMON_H__
