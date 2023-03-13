
#pragma once

#ifndef __TS3_GPUAPI_RENDER_TARGET_TEXTURE_H__
#define __TS3_GPUAPI_RENDER_TARGET_TEXTURE_H__

#include "gpuResource.h"
#include "textureReference.h"

namespace ts3::gpuapi
{

	/// @brief
	enum class ERenderTargetTextureType : enum_default_value_t
	{
		RTColor = 1,
		RTDepth = 2,
		RTDepthStencil = 3,
		Unknown = 0
	};

	/// @brief
	struct RenderTargetTextureCreateInfo
	{
		TextureReference targetTexture;
		ERenderTargetTextureType rttType;
		RenderTargetTextureLayout rttLayout;
		Bitmask<ETextureBindFlags> bindFlags;
	};

	/// @brief
	class RenderTargetTexture : public GPUResourceWrapper
	{
		friend class GPUDevice;
		friend class Texture;

	public:
		ERenderTargetTextureType const mRTTextureType;

		RenderTargetTextureLayout const mRTTextureLayout;

		Bitmask<resource_flags_value_t> const mInternalResourceFlags;

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
		TS3_ATTR_NO_DISCARD const TextureReference & getTargetTextureRef() const noexcept;

		///
		TS3_ATTR_NO_DISCARD GPUDeviceChildObject * getInternalRenderBuffer() const noexcept;

		///
		TS3_ATTR_NO_DISCARD bool empty() const noexcept;

		///
		TS3_ATTR_NO_DISCARD bool isDepthStencilTexture() const noexcept;

		///
		TS3_ATTR_NO_DISCARD bool isDepthStencilRenderBuffer() const noexcept;

	protected:
		void setTargetTexture( const TextureReference & pTargetTextureRef );

		void setInternalRenderBuffer( GpaHandle<GPUDeviceChildObject> pInternalRenderBuffer );

	private:
		/// Internal texture used by this RTT. Can be null, if this is a write-only depth-stencil
		/// RTT (created solely for the purpose of an off-screen depth/stencil testing).
		/// In particular, this is null always when isDepthStencilRenderBuffer() returns true.
		TextureReference _targetTexture;

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
