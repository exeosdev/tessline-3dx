
#pragma once

#ifndef __TS3_GPUAPI_RENDER_TARGET_TEXTURE_H__
#define __TS3_GPUAPI_RENDER_TARGET_TEXTURE_H__

#include "gpuResource.h"
#include "textureReference.h"
#include "../state/renderTargetCommon.h"

namespace ts3::gpuapi
{

	/// @brief
	struct RenderTargetTextureCreateInfo
	{
		TextureReference targetTexture;
		RenderTargetTextureLayout rtTextureLayout;
		Bitmask<ETextureBindFlags> bindFlags;
	};

	/// @brief
	class RenderTargetTexture : public GPUResourceView
	{
		friend class GPUDevice;
		friend class Texture;

	public:
		ERenderTargetTextureType const mRTTextureType;

		Bitmask<ERenderTargetBufferFlags> const mRTBufferMask;

		RenderTargetTextureLayout const mRTTextureLayout;

		/// Internal texture used by this RTT. Can be null, if this is a write-only depth-stencil
		/// RTT (created solely for the purpose of an off-screen depth/stencil testing).
		/// In particular, this is null always when isDepthStencilRenderBuffer() returns true.
		TextureReference const mTargetTexture;

	public:
		RenderTargetTexture(
			GPUDevice & pGPUDevice,
			ERenderTargetTextureType pRTTextureType,
			const RenderTargetTextureLayout & pRTTextureLayout,
			TextureReference pTargetTexture );

		RenderTargetTexture(
			GPUDevice & pGPUDevice,
			ERenderTargetTextureType pRTTextureType,
			const RenderTargetTextureLayout & pRTTextureLayout,
			GpaHandle<GPUDeviceChildObject> pInternalRenderBuffer,
			Bitmask<resource_flags_value_t> pRenderBufferFlags );

		virtual ~RenderTargetTexture();

		///
		TS3_ATTR_NO_DISCARD bool empty() const noexcept;

		///
		TS3_ATTR_NO_DISCARD bool isDepthStencilTexture() const noexcept;

		///
		TS3_ATTR_NO_DISCARD bool isDepthStencilRenderBuffer() const noexcept;

		///
		template <typename TRenderBufferType>
		TS3_ATTR_NO_DISCARD TRenderBufferType * getInternalRenderBuffer() const noexcept
		{
			return _internalRenderBuffer ? _internalRenderBuffer->queryInterface<TRenderBufferType>() : nullptr;
		}

	private:
		///
		GpaHandle<GPUDeviceChildObject> _internalRenderBuffer;
	};

	namespace rcutil
	{

		TS3_GPUAPI_API_NO_DISCARD ERenderTargetTextureType queryRenderTargetTextureType( ETextureFormat pFormat );

		TS3_GPUAPI_API_NO_DISCARD RenderTargetTextureLayout queryRenderTargetTextureLayout( const TextureLayout & pTextureLayout );

		TS3_GPUAPI_API_NO_DISCARD bool validateRenderTargetTextureLayout(
				TextureHandle pTargetTexture,
				const RenderTargetTextureLayout & pRTTextureLayout );

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_RENDER_TARGET_TEXTURE_H__
