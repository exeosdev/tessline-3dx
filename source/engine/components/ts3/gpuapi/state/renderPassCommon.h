
#pragma once

#ifndef __TS3_GPUAPI_RENDER_PASS_COMMON_H__
#define __TS3_GPUAPI_RENDER_PASS_COMMON_H__

#include "renderTargetCommon.h"
#include "../resources/shaderCommon.h"

namespace ts3::GpuAPI
{

	/// @brief Specifies how the contents of render pass attachments are treated at the beginning of a render pass.
	enum class ERenderPassAttachmentLoadOp : uint32
	{
		/// Indicates that the pass does not have any dependency on the previous contents of the resource.
		/// The resource may have the previous data or some uninitialized one. No guarantees are given.
		Discard,

		///
		Keep,

		///
		Clear,

		///
		NoAccess
	};

	/// @brief Specifies how the contents of render pass attachments are treated at the end of a render pass.
	enum class ERenderPassAttachmentStoreOp : uint32
	{

		/// Indicates that the contents need not to be preserved after the pass is finished. The contents MAY
		/// (but not necessarily have to) be discarded by the driver. There should be no assumptions about it.
		Discard,

		/// The contents of the resource is to be preserved and kept after the pass is done.
		/// This indicates there will be a dependency on the data in the future.
		Keep,

		///
		Resolve,

		///
		NoAccess,
	};

	///
	struct RenderSubPassAttachmentDesc
	{
		ERenderTargetAttachmentID attachmentID = ERenderTargetAttachmentID::RTUndefined;
		ERenderPassAttachmentLoadOp loadOp;
		ERenderPassAttachmentStoreOp storeOp;
	};


} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_RENDER_PASS_COMMON_H__
