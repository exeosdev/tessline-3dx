
#pragma once

#ifndef __TS3_GPUAPI_RENDER_PASS_DESCRIPTORS_H__
#define __TS3_GPUAPI_RENDER_PASS_DESCRIPTORS_H__

#include "ts3/gpuapi/state/renderPassCommon.h"
#include "ts3/gpuapi/resources/textureReference.h"

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( RenderTargetTexture );

	/// @brief
	class RenderPassAttachmentBaseDescriptor
	{
	public:
		RenderPassAttachmentBaseDescriptor() = default;
		~RenderPassAttachmentBaseDescriptor() = default;

		TS3_ATTR_NO_DISCARD bool empty() const noexcept
		{
			return _attachmentTexture ? true : false;
		}

		TS3_ATTR_NO_DISCARD RenderTargetTexture * getAttachmentTexture() const noexcept;

		TS3_ATTR_NO_DISCARD Bitmask<ERenderPassActionFlags> getActionMask() const noexcept;

		TS3_ATTR_NO_DISCARD ERenderPassAttachmentLoadAction getLoadAction() const noexcept;

		TS3_ATTR_NO_DISCARD ERenderPassAttachmentStoreAction getStoreAction() const noexcept;

		bool configureLoadAction( ERenderPassAttachmentLoadAction pLoadAction );

		bool configureStoreAction( ERenderPassAttachmentStoreAction pStoreAction );

	protected:
		RenderTargetTextureHandle _attachmentTexture;
		Bitmask<ERenderPassActionFlags> _renderPassActionMask;
		ERenderPassAttachmentLoadAction _renderPassLoadAction;
		ERenderPassAttachmentStoreAction _renderPassStoreAction;
	};

	inline RenderTargetTexture * RenderPassAttachmentBaseDescriptor::getAttachmentTexture() const noexcept
	{
		return _attachmentTexture.get();
	}

	/// @brief
	class RenderPassColorAttachmentDescriptor : public RenderPassAttachmentBaseDescriptor
	{
	public:
		RenderPassColorAttachmentDescriptor() = default;
		~RenderPassColorAttachmentDescriptor() = default;

		RenderPassColorAttachmentDescriptor( const RenderPassColorAttachmentDescriptor & ) = default;
		RenderPassColorAttachmentDescriptor & operator=( const RenderPassColorAttachmentDescriptor & ) = default;

		TS3_ATTR_NO_DISCARD RenderTargetTexture * getResolveTargetTexture() const noexcept;

		bool configureResolve( RenderTargetTextureHandle pResolveTargetTexture );

	private:
		RenderTargetTextureHandle _resolveTargetTexture;
	};

	inline RenderTargetTexture * RenderPassColorAttachmentDescriptor::getResolveTargetTexture() const noexcept
	{
		return _resolveTargetTexture.get();
	}

	/// @brief
	class RenderPassDepthStencilAttachmentDescriptor : public RenderPassAttachmentBaseDescriptor
	{
	public:
		RenderPassDepthStencilAttachmentDescriptor() = default;
		~RenderPassDepthStencilAttachmentDescriptor() = default;

		RenderPassDepthStencilAttachmentDescriptor( const RenderPassDepthStencilAttachmentDescriptor & ) = default;
		RenderPassDepthStencilAttachmentDescriptor & operator=( const RenderPassDepthStencilAttachmentDescriptor & ) = default;
	};

	using RenderPassColorAttachmentDescriptorArray = std::array<RenderPassColorAttachmentDescriptor, cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM>;

	/// @brief
	struct RenderPassColorAttachmentDescriptorBindingDesc
	{
		/// RT color attachment this binding refers to.
		render_target_index_t attachmentIndex = cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM;

		/// Binding definition.
		RenderPassColorAttachmentDescriptor colorAttachmentDescriptor;
	};

	/// @brief
	class RenderPassAttachmentDescriptorSet
	{
	public:
		TS3_ATTR_NO_DISCARD const RenderPassColorAttachmentDescriptor & getColorAttachmentDescriptor( render_target_index_t pIndex ) const noexcept;

		TS3_ATTR_NO_DISCARD const RenderPassDepthStencilAttachmentDescriptor & getDepthStencilAttachmentDescriptor() const noexcept;

		TS3_ATTR_NO_DISCARD Bitmask<ERTAttachmentFlags> getActiveAttachmentsMask() const noexcept;

		TS3_ATTR_NO_DISCARD uint32 getActiveColorAttachmentDescriptorsNum() const noexcept;

		TS3_ATTR_NO_DISCARD bool checkColorAttachmentDescriptor( render_target_index_t pIndex ) const noexcept;

		TS3_ATTR_NO_DISCARD bool checkDepthStencilAttachmentDescriptor() const noexcept;

		void assign( const RenderPassAttachmentDescriptorSet & pOther );

		void setColorAttachment( render_target_index_t pIndex, const RenderPassColorAttachmentDescriptor & pDescriptor );

		void setColorAttachments( render_target_index_t pFirstIndex, const ArrayView<RenderPassColorAttachmentDescriptor> & pDescriptors );

		void setColorAttachment( const RenderPassColorAttachmentDescriptorBindingDesc & pBinding );

		void setColorAttachments( const ArrayView<RenderPassColorAttachmentDescriptorBindingDesc> & pBindings );

		void setColorAttachments( const RenderPassColorAttachmentDescriptorArray & pDescriptors );

		void setColorAttachments( const RenderPassColorAttachmentDescriptorArray & pDescriptors, uint32 pFirstIndex, uint32 pCount );

		void setDepthStencilAttachment( const RenderPassDepthStencilAttachmentDescriptor & pDescriptor );

		void resetColorAttachment( render_target_index_t pIndex );

		void resetColorAttachments( uint32 pFirstIndex, uint32 pCount);

		void resetColorAttachments();

		void resetDepthStencilAttachment();

		virtual void applyDescriptorBindings();

	private:
		void _updateInternal();

		void _setColorAttachmentDescriptor( render_target_index_t pIndex, const RenderPassColorAttachmentDescriptor & pDescriptor );

		void _setDepthStencilAttachmentDescriptor( const RenderPassDepthStencilAttachmentDescriptor & pDescriptor );

	protected:
		RenderPassColorAttachmentDescriptorArray _colorAttachmentsDescriptors;
		RenderPassDepthStencilAttachmentDescriptor _depthStencilAttachmentsDescriptor;
		Bitmask<ERTAttachmentFlags> _activeAttachmentsMask;
		uint32 _activeColorAttachmentDescriptorsNum;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_RENDER_PASS_DESCRIPTORS_H__
