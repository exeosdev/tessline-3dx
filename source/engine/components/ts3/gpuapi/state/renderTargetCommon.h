
#pragma once

#ifndef __TS3_GPUAPI_RENDER_TARGET_COMMON_H__
#define __TS3_GPUAPI_RENDER_TARGET_COMMON_H__

#include "commonGPUStateDefs.h"
#include <ts3/gpuapi/resources/renderBuffer.h>

namespace ts3::gpuapi
{

	struct RTAttachmentDesc
	{
		ERenderTargetAttachmentID attachmentID = ERenderTargetAttachmentID::RTUndefined;
		ETextureFormat format;
	};

	ts3DeclareClassHandle( RenderBuffer );
	ts3DeclareClassHandle( RenderTargetStateObject );

	struct RenderTargetLayout;
	struct RenderTargetResourceBinding;

	struct RenderTargetStateObjectCreateInfo;

	/// @brief
	enum class ERenderTargetResourceType : enum_default_value_t
	{
		RenderBuffer,
		Texture,
		Unknown
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
		using AttachmentLayoutDescArray = std::array<AttachmentLayoutDesc, E_GPU_SYSTEM_METRIC_RT_MAX_COMBINED_ATTACHMENTS_NUM>;
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
		using AttachmentResourceBindingDescArray = std::array<AttachmentResourceBindingDesc, E_GPU_SYSTEM_METRIC_RT_MAX_COMBINED_ATTACHMENTS_NUM>;
		AttachmentResourceBindingDescArray attachmentResourceBindingDescArray;
		uint32 activeBindingsNum = E_GPU_SYSTEM_METRIC_RT_MAX_COMBINED_ATTACHMENTS_NUM;
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
		RenderTargetAttachmentLayout colorAttachmentArray[E_GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM];
		RenderTargetAttachmentLayout depthStencilAttachment;
	};

	inline constexpr uint32 CX_RT_BUFFER_MSAA_LEVEL_INVALID = Limits<uint32>::maxValue;

	struct RenderTargetResourceBinding
	{
		RenderTargetAttachmentResourceBinding colorAttachmentArray[E_GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM];
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

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_RENDER_TARGET_COMMON_H__
