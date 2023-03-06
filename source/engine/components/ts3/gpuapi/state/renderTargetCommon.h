
#pragma once

#ifndef __TS3_GPUAPI_RENDER_TARGET_COMMON_H__
#define __TS3_GPUAPI_RENDER_TARGET_COMMON_H__

#include "commonGPUStateDefs.h"
#include <ts3/gpuapi/resources/renderBuffer.h>
#include <ts3/stdext/bitUtils.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( RenderTargetTexture );

	/// @brief
	struct RenderTargetAttachmentBinding
	{
		RenderTargetTextureHandle attachmentTexture;
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
		ETextureFormat format;
	};

	/// @brief Typedef for ordered, fixed-size array of layout definitions for render target attachments.
	/// Entries are ordered according to ERTAttachmentIndex values.
	using RenderTargetColorAttachmentLayoutArray = std::array<RenderTargetAttachmentLayout, cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM>;

	/// @brief A definition of a vertex layout used to create a driver-specific RenderTargetLayout object.
	struct RenderTargetLayoutConfiguration
	{
		///
		Bitmask<ERTAttachmentFlags> activeAttachmentsMask;
		///
		RenderTargetColorAttachmentLayoutArray colorAttachments;
		///
		RenderTargetAttachmentLayout depthStencilAttachment;
	};

	namespace defaults
	{

		TS3_GPUAPI_OBJ const RenderTargetLayoutConfiguration cvRenderTargetLayoutConfigurationDefaultBGRA8;
		TS3_GPUAPI_OBJ const RenderTargetLayoutConfiguration cvRenderTargetLayoutConfigurationDefaultBGRA8D24S8;
		TS3_GPUAPI_OBJ const RenderTargetLayoutConfiguration cvRenderTargetLayoutConfigurationDefaultRGBA8;
		TS3_GPUAPI_OBJ const RenderTargetLayoutConfiguration cvRenderTargetLayoutConfigurationDefaultRGBA8D24S8;

	}



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
		using AttachmentLayoutDescArray = std::array<AttachmentLayoutDesc, cxdefs::GPU_SYSTEM_METRIC_RT_MAX_COMBINED_ATTACHMENTS_NUM>;
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
		using AttachmentResourceBindingDescArray = std::array<AttachmentResourceBindingDesc, cxdefs::GPU_SYSTEM_METRIC_RT_MAX_COMBINED_ATTACHMENTS_NUM>;
		AttachmentResourceBindingDescArray attachmentResourceBindingDescArray;
		uint32 activeBindingsNum = cxdefs::GPU_SYSTEM_METRIC_RT_MAX_COMBINED_ATTACHMENTS_NUM;
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

	template <typename TAttachmentInfo>
	struct RenderTargetAttachmentsConfig
	{
		Bitmask<ERenderTargetAttachmentFlags> attachmentMask = 0;
		uint32 colorAttachmentActiveCount = 0;
		TAttachmentInfo colorAttachmentArray[cxdefs::GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM];
		TAttachmentInfo depthStencilAttachment;

		TS3_ATTR_NO_DISCARD TAttachmentInfo & operator[]( ERenderTargetAttachmentIndex pIndex )
		{
			ts3DebugAssert( cxdefs::isRTAttachmentIndexValid( pIndex ) );
			return ( pIndex == E_RT_ATTACHMENT_INDEX_DEPTH_STENCIL ) ? depthStencilAttachment : colorAttachmentArray[pIndex];
		}

		TS3_ATTR_NO_DISCARD const TAttachmentInfo & operator[]( ERenderTargetAttachmentIndex pIndex ) const
		{
			ts3DebugAssert( cxdefs::isRTAttachmentIndexValid( pIndex ) );
			return ( pIndex == E_RT_ATTACHMENT_INDEX_DEPTH_STENCIL ) ? depthStencilAttachment : colorAttachmentArray[pIndex];
		}

		TS3_ATTR_NO_DISCARD bool depthStencilActive() const noexcept
		{
			return attachmentMask.isSetAnyOf( E_RT_ATTACHMENT_MASK_DEPTH_STENCIL );
		}

		TS3_ATTR_NO_DISCARD bool empty() const noexcept
		{
			return !attachmentMask.isSetAnyOf( E_RT_ATTACHMENT_MASK_COLOR_ALL ) || ( colorAttachmentActiveCount == 0 );
		}
	};

	struct RenderTargetLayout : public RenderTargetAttachmentsConfig<RenderTargetAttachmentLayout>
	{
	};

	struct RenderTargetResourceBinding : public RenderTargetAttachmentsConfig<RenderTargetAttachmentResourceBinding>
	{
		TextureSize2D commonBufferSize;
		uint32 commonMSAALevel = cxdefs::RT_ATTACHMENT_MSAA_LEVEL_INVALID;
	};

	namespace defaults
	{

		TS3_GPUAPI_OBJ const RenderTargetLayout cvRenderTargetLayoutDefaultBGRA8;
		TS3_GPUAPI_OBJ const RenderTargetLayout cvRenderTargetLayoutDefaultBGRA8D24S8;
		TS3_GPUAPI_OBJ const RenderTargetLayout cvRenderTargetLayoutDefaultRGBA8;
		TS3_GPUAPI_OBJ const RenderTargetLayout cvRenderTargetLayoutDefaultRGBA8D24S8;

	}

	namespace smutil
	{

		TS3_GPUAPI_API bool createRenderTargetLayout(
				const RenderTargetLayoutDesc & pRTLayoutDesc,
				RenderTargetLayout & pOutRTLayout );

		TS3_GPUAPI_API bool createRenderTargetLayoutAndResourceBinding(
				const RenderTargetResourceBindingDesc & pRTResourceBindingDesc,
				RenderTargetLayout & pOutRTLayout,
				RenderTargetResourceBinding & pOutRTResourceBinding );

		TS3_GPUAPI_API_NO_DISCARD bool checkRenderTargetLayoutCompatibility(
				const RenderTargetResourceBinding & pRTResourceBinding,
				const RenderTargetLayout & pRTLayout );

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_RENDER_TARGET_COMMON_H__
