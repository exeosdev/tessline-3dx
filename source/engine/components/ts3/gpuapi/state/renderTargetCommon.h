
#pragma once

#ifndef __TS3_GPUAPI_RENDER_TARGET_COMMON_H__
#define __TS3_GPUAPI_RENDER_TARGET_COMMON_H__

#include "commonGPUStateDefs.h"
#include <ts3/gpuapi/resources/renderBuffer.h>

namespace ts3
{
namespace gpuapi
{

	ts3DeclareClassHandle( RenderBuffer );
	ts3DeclareClassHandle( RenderTargetStateObject );

	struct RenderTargetLayout;
	struct RenderTargetResourceBinding;

	struct RenderTargetStateObjectCreateInfo;

	/// @brief
	enum class ERenderTargetAttachmentID : enum_default_value_t
	{
		RTColor0,
		RTColor1,
		RTColor2,
		RTColor3,
		RTColor4,
		RTColor5,
		RTColor6,
		RTColor7,
		RTDepthStencil,
		RTUndefined
	};

	/// @brief
	enum class ERenderTargetResourceType : enum_default_value_t
	{
		RenderBuffer,
		Texture,
		Unknown
	};

	/// @brief A set of bit flags representing render target attachments.
	enum ERenderTargetAttachmentFlags : uint32
	{
		// Note: Implementation depends on all COLOR values having consecutive bit positions!

		E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_0_BIT    = 1 << 0,
		E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_1_BIT    = 1 << 1,
		E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_2_BIT    = 1 << 2,
		E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_3_BIT    = 1 << 3,
		E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_4_BIT    = 1 << 4,
		E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_5_BIT    = 1 << 5,
		E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_6_BIT    = 1 << 6,
		E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_7_BIT    = 1 << 7,
		E_RENDER_TARGET_ATTACHMENT_MASK_COLOR_ALL      = 0x7F,
		E_RENDER_TARGET_ATTACHMENT_FLAG_DEPTH_BIT      = 1 << 8,
		E_RENDER_TARGET_ATTACHMENT_FLAG_STENCIL_BIT    = 1 << 9,
		E_RENDER_TARGET_ATTACHMENT_FLAGS_DEPTH_STENCIL = E_RENDER_TARGET_ATTACHMENT_FLAG_DEPTH_BIT |
		                                                 E_RENDER_TARGET_ATTACHMENT_FLAG_STENCIL_BIT,
		E_RENDER_TARGET_ATTACHMENT_FLAGS_DEFAULT_C0DS  = E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_0_BIT |
		                                                 E_RENDER_TARGET_ATTACHMENT_FLAGS_DEPTH_STENCIL,
		E_RENDER_TARGET_ATTACHMENT_MASK_ALL            = E_RENDER_TARGET_ATTACHMENT_MASK_COLOR_ALL |
		                                                 E_RENDER_TARGET_ATTACHMENT_FLAGS_DEPTH_STENCIL,
	};

	struct RTARenderBufferRef
	{
		RenderBuffer * renderBuffer;
	};

	struct RTATextureRef
	{
		Texture * texture;
		TextureSubResource textureSubResource;
	};

	struct RenderTargetLayoutDesc
	{
		struct AttachmentLayoutDesc
		{
			ERenderTargetAttachmentID attachmentID = ERenderTargetAttachmentID::RTUndefined;
			ETextureFormat format;
			constexpr explicit operator bool() const
			{
				return attachmentID != ERenderTargetAttachmentID::RTUndefined;
			}
		};
		using AttachmentLayoutDescArray = std::array<AttachmentLayoutDesc, GPU_SYSTEM_METRIC_RT_MAX_COMBINED_ATTACHMENTS_NUM>;
		AttachmentLayoutDescArray attachmentLayoutDescArray;
	};

	struct RenderTargetResourceBindingDesc
	{
		struct AttachmentResourceBindingDesc
		{
			ERenderTargetAttachmentID attachmentID = ERenderTargetAttachmentID::RTUndefined;
			ERenderTargetResourceType attachmentResourceType = ERenderTargetResourceType::Unknown;
			RTARenderBufferRef renderBufferRef;
			RTATextureRef textureRef;
			constexpr explicit operator bool() const
			{
				return attachmentID != ERenderTargetAttachmentID::RTUndefined;
			}
		};
		using AttachmentResourceBindingDescArray = std::array<AttachmentResourceBindingDesc, GPU_SYSTEM_METRIC_RT_MAX_COMBINED_ATTACHMENTS_NUM>;
		AttachmentResourceBindingDescArray attachmentResourceBindingDescArray;
		uint32 activeBindingsNum = GPU_SYSTEM_METRIC_RT_MAX_COMBINED_ATTACHMENTS_NUM;
		uint32 sharedMSAALevel = 0;
	};

	struct RenderTargetAttachmentLayout
	{
		ETextureFormat format = ETextureFormat::UNKNOWN;

		constexpr explicit operator bool() const
		{
			return format != ETextureFormat::UNKNOWN;
		}
	};

	struct RenderTargetAttachmentResourceBinding
	{
		union
		{
			RTARenderBufferRef uRenderBufferRef;
			RTATextureRef uTextureRef;
		};

		ERenderTargetResourceType attachmentResourceType = ERenderTargetResourceType::Unknown;
		ETextureFormat format = ETextureFormat::UNKNOWN;

		constexpr explicit operator bool() const
		{
			return attachmentResourceType != ERenderTargetResourceType::Unknown;
		}
	};

	struct RenderTargetLayout
	{
		Bitmask<ERenderTargetAttachmentFlags> attachmentMask = 0;
		uint32 colorAttachmentActiveCount = 0;
		uint32 depthStencilAttachmentState = 0;
		RenderTargetAttachmentLayout colorAttachmentArray[GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM];
		RenderTargetAttachmentLayout depthStencilAttachment;
	};

	inline constexpr uint32 CX_RT_BUFFER_MSAA_LEVEL_INVALID = Limits<uint32>::maxValue;

	struct RenderTargetResourceBinding
	{
		RenderTargetAttachmentResourceBinding colorAttachmentArray[GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM];
		RenderTargetAttachmentResourceBinding depthStencilAttachment;
		TextureSize2D commonBufferSize;
		uint32 commonMSAALevel = CX_RT_BUFFER_MSAA_LEVEL_INVALID;
	};

	TS3_GPUAPI_OBJ const RenderTargetLayoutDesc cvRenderTargetLayoutDescDefaultBGRA8;
	TS3_GPUAPI_OBJ const RenderTargetLayoutDesc cvRenderTargetLayoutDescDefaultBGRA8D24S8;
	TS3_GPUAPI_OBJ const RenderTargetLayoutDesc cvRenderTargetLayoutDescDefaultRGBA8;
	TS3_GPUAPI_OBJ const RenderTargetLayoutDesc cvRenderTargetLayoutDescDefaultRGBA8D24S8;

	TS3_GPUAPI_API bool createRenderTargetLayout( const RenderTargetLayoutDesc & pRTLayoutDesc,
	                                              RenderTargetLayout & pOutRTLayout );

	TS3_GPUAPI_API bool createRenderTargetLayoutAndResourceBinding( const RenderTargetResourceBindingDesc & pRTResourceBindingDesc,
	                                                                RenderTargetLayout & pOutRTLayout,
	                                                                RenderTargetResourceBinding & pOutRTResourceBinding );

	TS3_GPUAPI_API bool checkRenderTargetLayoutCompatibility( const RenderTargetResourceBinding & pRTResourceBinding,
	                                                          const RenderTargetLayout & pRTLayout );

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3_GPUAPI_RENDER_TARGET_COMMON_H__
