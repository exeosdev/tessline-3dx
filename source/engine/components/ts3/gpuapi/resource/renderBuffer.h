
#pragma once

#ifndef __TS3_GPUAPI_RENDER_BUFFER_H__
#define __TS3_GPUAPI_RENDER_BUFFER_H__

#include "textureCommon.h"

namespace ts3
{
namespace gpuapi
{

	/// @brief
	enum class ERenderBufferType : enum_default_value_t
	{
		RBColor = 1,
		RBDepthStencil = 2,
	};

	struct RenderBufferLayout
	{
		TextureSize2D bufferSize;
		ETextureFormat internalDataFormat;
		uint32 msaaLevel;
	};

	/// @brief
	struct RenderBufferCreateInfo
	{
		ERenderBufferType bufferType;
		RenderBufferLayout layout;
	};

	/// @brief
	class TS3_GPUAPI_CLASS RenderBuffer : public GPUBaseObject
	{
	public:
		ERenderBufferType const mERenderBufferType;
		RenderBufferLayout const mRenderBufferLayout;

	public:
		RenderBuffer( GPUDevice & pGPUDevice, ERenderBufferType pRenderBufferType, const RenderBufferLayout & pRenderBufferLayout );
		virtual ~RenderBuffer();

		virtual bool isNull() const = 0;

	protected:
		static bool validateCreateInfo( const RenderBufferCreateInfo & pCreateInfo );
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3_GPUAPI_RENDER_BUFFER_H__
