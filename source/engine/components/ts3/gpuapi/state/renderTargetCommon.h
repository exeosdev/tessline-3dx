
#pragma once

#ifndef __TS3_GPUAPI_RENDER_TARGET_COMMON_H__
#define __TS3_GPUAPI_RENDER_TARGET_COMMON_H__

#include "commonGPUStateDefs.h"
#include <ts3/gpuapi/resources/renderBuffer.h>

namespace ts3::GpuAPI
{

	/// @brief
	enum class ERenderTargetAttachmentID : uint16
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

	namespace CxDefs
	{

		///
		constexpr auto RT_MAX_COLOR_ATTACHMENTS_NUM = static_cast<render_target_index_t>( E_GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM );

		constexpr auto RT_ATTACHMENT_MSAA_LEVEL_INVALID = Limits<uint32>::maxValue;

		/// @brief
		inline constexpr bool isRTAttachmentIndexValid( render_target_index_t pIndex )
		{
			return pIndex < CxDefs::RT_MAX_COLOR_ATTACHMENTS_NUM;
		}

	}

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

	template <typename TAttachmentInfo>
	struct RenderTargetAttachmentsConfig
	{
		Bitmask<ERenderTargetAttachmentFlags> attachmentMask = 0;
		uint32 colorAttachmentActiveCount = 0;
		TAttachmentInfo colorAttachmentArray[E_GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM];
		TAttachmentInfo depthStencilAttachment;

		TS3_ATTR_NO_DISCARD TAttachmentInfo & operator[]( ERenderTargetAttachmentIndex pIndex )
		{
			ts3DebugAssert( CxDefs::isRTAttachmentIndexValid( pIndex ) );
			return ( pIndex == E_RT_ATTACHMENT_INDEX_DEPTH_STENCIL ) ? depthStencilAttachment : colorAttachmentArray[pIndex];
		}

		TS3_ATTR_NO_DISCARD const TAttachmentInfo & operator[]( ERenderTargetAttachmentIndex pIndex ) const
		{
			ts3DebugAssert( CxDefs::isRTAttachmentIndexValid( pIndex ) );
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
		uint32 commonMSAALevel = CxDefs::RT_ATTACHMENT_MSAA_LEVEL_INVALID;
	};

	namespace Defaults
	{

		TS3_GPUAPI_OBJ const RenderTargetLayout cvRenderTargetLayoutDefaultBGRA8;
		TS3_GPUAPI_OBJ const RenderTargetLayout cvRenderTargetLayoutDefaultBGRA8D24S8;
		TS3_GPUAPI_OBJ const RenderTargetLayout cvRenderTargetLayoutDefaultRGBA8;
		TS3_GPUAPI_OBJ const RenderTargetLayout cvRenderTargetLayoutDefaultRGBA8D24S8;

	}

	namespace StateMgmt
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

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_RENDER_TARGET_COMMON_H__
