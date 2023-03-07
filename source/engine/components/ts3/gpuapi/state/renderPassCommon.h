
#pragma once

#ifndef __TS3_GPUAPI_RENDER_PASS_COMMON_H__
#define __TS3_GPUAPI_RENDER_PASS_COMMON_H__

#include "renderTargetCommon.h"

namespace ts3::gpuapi
{

	/// @brief
	enum ERenderPassActionFlags : uint32
	{
		E_RENDER_PASS_ACTION_FLAG_LOAD_CLEAR_BIT            = 0x01,
		E_RENDER_PASS_ACTION_FLAG_LOAD_DISCARD_BIT          = 0x02,
		E_RENDER_PASS_ACTION_FLAG_LOAD_FETCH_BIT            = 0x04,
		E_RENDER_PASS_ACTION_FLAG_LOAD_RESTRICT_ACCESS_BIT  = 0x08,
		E_RENDER_PASS_ACTION_MASK_LOAD_ALL_BITS             = 0x0F,
		E_RENDER_PASS_ACTION_FLAG_STORE_DISCARD_BIT         = 0x0100,
		E_RENDER_PASS_ACTION_FLAG_STORE_KEEP_BIT            = 0x0200,
		E_RENDER_PASS_ACTION_FLAG_STORE_RESOLVE_BIT         = 0x0400,
		E_RENDER_PASS_ACTION_FLAG_STORE_RESTRICT_ACCESS_BIT = 0x0800,
		E_RENDER_PASS_ACTION_MASK_STORE_ALL_BITS            = 0x0F00,
	};

	/// @brief Specifies how the contents of render pass attachments are treated at the beginning of a render pass.
	enum class ERenderPassAttachmentLoadAction : uint32
	{
		///
		Clear = E_RENDER_PASS_ACTION_FLAG_LOAD_CLEAR_BIT,

		/// Indicates that the pass does not have any dependency on the previous contents of the resource.
		/// The resource may have the previous data or some uninitialized one. No guarantees are given.
		Discard = E_RENDER_PASS_ACTION_FLAG_LOAD_DISCARD_BIT,

		///
		Fetch = E_RENDER_PASS_ACTION_FLAG_LOAD_FETCH_BIT,

		///
		RestrictAccess = E_RENDER_PASS_ACTION_FLAG_LOAD_RESTRICT_ACCESS_BIT,

		///
		Undefined = 0
	};

	/// @brief Specifies how the contents of render pass attachments are treated at the end of a render pass.
	enum class ERenderPassAttachmentStoreAction : uint32
	{
		/// Indicates that the contents need not to be preserved after the pass is finished. The contents MAY
		/// (but not necessarily have to) be discarded by the driver. There should be no assumptions about it.
		Discard = E_RENDER_PASS_ACTION_FLAG_STORE_DISCARD_BIT,

		/// The contents of the resource is to be preserved and kept after the pass is done.
		/// This indicates there will be a dependency on the data in the future.
		Keep = E_RENDER_PASS_ACTION_FLAG_STORE_KEEP_BIT,

		///
		KeepResolve = E_RENDER_PASS_ACTION_FLAG_STORE_KEEP_BIT | E_RENDER_PASS_ACTION_FLAG_STORE_RESOLVE_BIT,

		///
		Resolve = E_RENDER_PASS_ACTION_FLAG_STORE_RESOLVE_BIT,

		///
		RestrictAccess = E_RENDER_PASS_ACTION_FLAG_STORE_RESTRICT_ACCESS_BIT,

		///
		Undefined = 0
	};

	struct RenderPassAttachmentUsage
	{
		RenderTargetAttachmentClearConfig clearConfig;
		ERenderPassAttachmentLoadAction renderPassLoadAction = ERenderPassAttachmentLoadAction::Undefined;
		ERenderPassAttachmentStoreAction renderPassStoreAction = ERenderPassAttachmentStoreAction::Undefined;

		void reset()
		{
			renderPassLoadAction = ERenderPassAttachmentLoadAction::Undefined;
			renderPassStoreAction = ERenderPassAttachmentStoreAction::Undefined;
		}

		bool valid() const noexcept
		{
			return renderPassLoadAction != ERenderPassAttachmentLoadAction::Undefined &&
			       renderPassStoreAction != ERenderPassAttachmentStoreAction::Undefined;
		}

		explicit operator bool() const noexcept
		{
			return valid();
		}
	};

	struct RenderPassColorAttachmentUsage : public RenderPassAttachmentUsage
	{
	};

	struct RenderPassDepthStencilAttachmentUsage : public RenderPassAttachmentUsage
	{
	};

	using RenderPassColorAttachmentUsageArray = std::array<RenderPassColorAttachmentUsage, cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM>;

	struct RenderPassConfiguration
	{
		RenderPassColorAttachmentUsageArray colorAttachments;

		RenderPassDepthStencilAttachmentUsage depthStencilAttachment;

		Bitmask<ERTAttachmentFlags> activeAttachmentsMask;
	};

	namespace smutil
	{

		TS3_GPUAPI_API_NO_DISCARD Bitmask<ERenderPassActionFlags> getRenderPassAttachmentActionMask(
				const RenderPassAttachmentUsage & pAttachmentUsage );

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_RENDER_PASS_COMMON_H__
