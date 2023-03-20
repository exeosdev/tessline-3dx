
#pragma once

#ifndef __TS3_GPUAPI_RENDER_TARGET_COMMON_H__
#define __TS3_GPUAPI_RENDER_TARGET_COMMON_H__

#include "commonGPUStateDefs.h"
#include "../resources/textureCommon.h"
#include <ts3/stdext/bitUtils.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( RenderTargetTexture );

	enum ERenderTargetBufferFlags : uint32
	{
		E_RENDER_TARGET_BUFFER_FLAG_COLOR_BIT = 0x01,
		E_RENDER_TARGET_BUFFER_FLAG_DEPTH_BIT = 0x02,
		E_RENDER_TARGET_BUFFER_FLAG_STENCIL_BIT = 0x04,

		E_RENDER_TARGET_BUFFER_MASK_DEPTH_STENCIL =
			E_RENDER_TARGET_BUFFER_FLAG_DEPTH_BIT |
			E_RENDER_TARGET_BUFFER_FLAG_STENCIL_BIT,

		E_RENDER_TARGET_BUFFER_MASK_ALL =
			E_RENDER_TARGET_BUFFER_FLAG_COLOR_BIT |
			E_RENDER_TARGET_BUFFER_MASK_DEPTH_STENCIL,
	};

	namespace cxdefs
	{

		/// @brief
		inline constexpr uint32 getRTAttachmentRequiredUsageFlag( native_uint pAttachmentIndex )
		{
			return
				( pAttachmentIndex < gpm::RT_MAX_COLOR_ATTACHMENTS_NUM ) ?
				E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_COLOR_BIT :
				E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_DEPTH_STENCIL_BIT;
		}

	}

	template <typename TAttachmentProperty>
	using RenderTargetColorAttachmentPropertyArray = std::array<TAttachmentProperty, gpm::RT_MAX_COLOR_ATTACHMENTS_NUM>;

	template <typename TAttachmentProperty>
	struct RenderTargetAttachmentPropertySet
	{
		using AttachmentPropertyArray = std::array<TAttachmentProperty, gpm::RT_MAX_COMBINED_ATTACHMENTS_NUM>;

		AttachmentPropertyArray attachments;

		Bitmask<ERTAttachmentFlags> activeAttachmentsMask = 0;

		ArrayView<TAttachmentProperty> const colorAttachments;

		TAttachmentProperty & depthStencilAttachment;

		RenderTargetAttachmentPropertySet()
		: colorAttachments( bindArrayView( attachments.data(), gpm::RT_MAX_COLOR_ATTACHMENTS_NUM ) )
		, depthStencilAttachment( attachments[E_RT_ATTACHMENT_INDEX_DEPTH_STENCIL] )
		{}

		RenderTargetAttachmentPropertySet( const RenderTargetAttachmentPropertySet<TAttachmentProperty> & pSource )
		: attachments( pSource.attachments )
		, activeAttachmentsMask( pSource.activeAttachmentsMask )
		, colorAttachments( bindArrayView( attachments.data(), gpm::RT_MAX_COLOR_ATTACHMENTS_NUM ) )
		, depthStencilAttachment( attachments[E_RT_ATTACHMENT_INDEX_DEPTH_STENCIL] )
		{}

		RenderTargetAttachmentPropertySet & operator=( const RenderTargetAttachmentPropertySet<TAttachmentProperty> & pRhs )
		{
			attachments = pRhs.attachments;
			activeAttachmentsMask = pRhs.activeAttachmentsMask;
			return *this;
		}

		TS3_ATTR_NO_DISCARD bool empty() const noexcept
		{
			return activeAttachmentsMask.empty();
		}

		TS3_ATTR_NO_DISCARD uint32 countActiveColorAttachments() const noexcept
		{
			return popCount( static_cast<uint32>( activeAttachmentsMask & E_RT_ATTACHMENT_MASK_COLOR_ALL ) );
		}

		TS3_ATTR_NO_DISCARD bool isColorAttachmentActive( uint32 pAttachmentIndex ) const noexcept
		{
			const auto attachmentBit = cxdefs::makeRTAttachmentFlag( pAttachmentIndex );
			return ( attachmentBit != 0 ) && activeAttachmentsMask.isSet( attachmentBit );
		}

		TS3_ATTR_NO_DISCARD bool isDepthStencilAttachmentActive() const noexcept
		{
			return activeAttachmentsMask.isSet( E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT );
		}

		TS3_ATTR_NO_DISCARD bool hasAnyColorAttachmentsActive() const noexcept
		{
			return ( activeAttachmentsMask & E_RT_ATTACHMENT_MASK_COLOR_ALL) != 0;
		}
	};

	template <typename TAttachmentProperty>
	struct RenderTargetAttachmentConfigurationSet : public RenderTargetAttachmentPropertySet<TAttachmentProperty>
	{
		Bitmask<ERTAttachmentFlags> attachmentsActionResolveMask = 0;

		TS3_ATTR_NO_DISCARD uint32 countAttachmentsActionResolve() const noexcept
		{
			return popCount( static_cast<uint32>( attachmentsActionResolveMask & E_RT_ATTACHMENT_MASK_COLOR_ALL ) );
		}
	};

	/// @brief
	struct RenderTargetAttachmentBinding
	{
		RenderTargetTextureHandle attachmentTexture;
		RenderTargetTextureHandle resolveTexture;

		void reset()
		{
			attachmentTexture.reset();
			resolveTexture.reset();
		}

		bool empty() const noexcept
		{
			return !attachmentTexture;
		}

		explicit operator bool() const noexcept
		{
			return !empty();
		}
	};

	using RenderTargetColorAttachmentBindingArray = RenderTargetColorAttachmentPropertyArray<RenderTargetAttachmentBinding>;

	struct RenderTargetBindingDefinition : public RenderTargetAttachmentConfigurationSet<RenderTargetAttachmentBinding>
	{
		TS3_ATTR_NO_DISCARD RenderTargetBindingDefinition getValidated() const noexcept
		{
			RenderTargetBindingDefinition validatedDefinition = *this;
			validatedDefinition.resetAttachmentsFlags();
			return validatedDefinition;
		}

		TS3_GPUAPI_API void resetAttachmentsFlags() noexcept;
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
	struct RenderTargetLayout : public RenderTargetAttachmentPropertySet<RenderTargetAttachmentLayout>
	{
		TextureSize2D sharedImageSize = cxdefs::TEXTURE_SIZE_2D_UNDEFINED;

		uint32 sharedMSAALevel = 0;
	};

	namespace smutil
	{

		TS3_GPUAPI_API_NO_DISCARD const RenderTargetAttachmentBinding * getRenderTargetBindingDefinitionFirstTarget(
				const RenderTargetBindingDefinition & pBindingDefinition );

		TS3_GPUAPI_API_NO_DISCARD bool validateRenderTargetBindingDefinition(
				const RenderTargetBindingDefinition & pBindingDefinition );

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

	template <typename TFunction>
	inline bool foreachRTAttachmentIndex( Bitmask<ERTAttachmentFlags> pActiveAttachmentsMask, TFunction pFunction )
	{
		// A local copy of the active attachments mask. Bits of already processed attachments
		// are removed, so when the value reaches 0, we can immediately stop further processing.
		auto activeAttachmentsMask = pActiveAttachmentsMask;

		for( // Iterate using RTA (Render Target Attachment) index value.
			native_uint attachmentIndex = 0;
			// Stop after reaching the limit or when there are no active attachments to process.
			cxdefs::isRTAttachmentIndexValid( attachmentIndex ) && !activeAttachmentsMask.empty();
			// This is rather self-descriptive, but it looked bad without this third comment here :)
			++attachmentIndex )
		{
			const auto attachmentBit = cxdefs::makeRTAttachmentFlag( attachmentIndex );
			// Check if the attachments mask has this bit set.
			if( activeAttachmentsMask.isSet( attachmentBit ) )
			{
				// The function returns false if there was some internal error condition
				// and the processing should be aborted.
				if( !pFunction( attachmentIndex, makeBitmaskEx<ERTAttachmentFlags>( attachmentBit ) ) )
				{
					return false;
				}

				// Update the control mask.
				activeAttachmentsMask.unset( attachmentBit );
			}
		}

		return true;
	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_RENDER_TARGET_COMMON_H__
