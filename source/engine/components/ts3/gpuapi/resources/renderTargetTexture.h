
#pragma once

#ifndef __TS3_GPUAPI_RENDER_TARGET_TEXTURE_H__
#define __TS3_GPUAPI_RENDER_TARGET_TEXTURE_H__

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
	struct RenderTargetTextureLayout
	{
		TextureSize2D bufferSize;
		ETextureFormat internalDataFormat;
		uint32 msaaLevel;
	};

	/// @brief
	struct RenderTargetTextureCreateInfo
	{
		RenderTargetTextureLayout rttLayout;
		Bitmask<ETextureBindFlags> bindFlags;
	};

	/// @brief
	class RenderTargetTexture
	{
	public:
		ERenderTargetTextureType const mRTTextureType;

		RenderTargetTextureLayout const mRTTLayout;

	public:
		RenderTargetTexture(
			GPUDevice & pGPUDevice,
			ERenderTargetTextureType pRTTextureType,
			const RenderTargetTextureLayout & pRTTextureLayout );

		explicit RenderTargetTexture( TextureHandle pTargetTexture );

		virtual ~RenderTargetTexture();

		///
		TS3_ATTR_NO_DISCARD TextureHandle getTargetTexture() const;

		///
		TS3_ATTR_NO_DISCARD bool isDepthStencilRenderBuffer() const;

	protected:
		void setTargetTexture( const TextureReference & pTargetTextureRef );

	private:
		/// Internal texture used by this RTT. Can be null, if this is a write-only depth-stencil
		/// RTT (created solely for the purpose of an off-screen depth/stencil testing).
		/// In particular, this is null always when isDepthStencilRenderBuffer() returns true.
		TextureReference _targetTexture;
	};

	namespace ResUtils
	{

		TS3_GPUAPI_API_NO_DISCARD ERenderTargetTextureType queryRenderTargetTextureType( ETextureFormat pFormat );

		TS3_GPUAPI_API_NO_DISCARD bool validateRenderTextureLayout(
				TextureHandle pTexture,
				const RenderTargetTextureLayout & pRTTextureLayout );

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_RENDER_TARGET_TEXTURE_H__
