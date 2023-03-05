
#pragma once

#ifndef __TS3_GPUAPI_COMMON_GRAPHICS_DESCRIPTORS_H__
#define __TS3_GPUAPI_COMMON_GRAPHICS_DESCRIPTORS_H__

#include "../commonGraphicsConfig.h"

namespace ts3::gpuapi
{

	/// @brief
	class BlendDescriptor
	{
	public:
		BlendDescriptor( const BlendDescriptor & ) = default;
		BlendDescriptor & operator=( const BlendDescriptor & ) = default;

		BlendDescriptor() = default;

		BlendDescriptor( const BlendConfig & pConfig );

		BlendDescriptor & operator=( const BlendConfig & pConfig );

		TS3_ATTR_NO_DISCARD const BlendConfig & getBlendConfig() const noexcept;

		TS3_ATTR_NO_DISCARD bool isAttachmentBlendActive( uint32 pRTAIndex ) const noexcept;

		TS3_ATTR_NO_DISCARD bool isIndependentBlendingConfigured() const noexcept;

		void configureAttachmentMask( Bitmask<ERTAttachmentFlags> pAttachmentMask );

		RTAttachmentBlendSettings & configureAttachmentBlendActive( uint32 pRTAIndex, bool pActive );

		TS3_ATTR_NO_DISCARD RTAttachmentBlendSettings & configureAttachmentBlendSettings( uint32 pRTAIndex );

		void configureAttachmentBlendSettings( uint32 pRTAIndex, const RTAttachmentBlendSettings & pSettings );

		void configureAttachmentBlendSettingsCopy( uint32 pTargetRTAIndex, uint32 pSourceRTAIndex );

		void enableAlphaToCoverage( bool pEnable );

		void enableIndependentBlending( bool pEnable );

		void setBlendConfig( const BlendConfig & pConfig );

		void resetAttachmentBlendSettings( uint32 pRTAIndex );

		void reset();

	private:
		BlendConfig _blendConfig;
	};

	/// @brief
	class DepthStencilDescriptor
	{
	public:
		DepthStencilDescriptor( const DepthStencilDescriptor & ) = default;
		DepthStencilDescriptor & operator=( const DepthStencilDescriptor & ) = default;

		DepthStencilDescriptor() = default;

		DepthStencilDescriptor( const DepthStencilConfig & pConfig );

		TS3_ATTR_NO_DISCARD DepthStencilDescriptor & operator=( const DepthStencilConfig & pConfig );

		TS3_ATTR_NO_DISCARD const DepthStencilConfig & getDepthStencilConfig() const noexcept;

		TS3_ATTR_NO_DISCARD bool isDepthTestEnabled() const noexcept;

		TS3_ATTR_NO_DISCARD bool isStencilTestEnabled() const noexcept;

		DepthTestSettings & configureDepthTestActive( bool pActive );

		TS3_ATTR_NO_DISCARD DepthTestSettings & configureDepthTestSettings();

		void configureDepthTestSettings( const DepthTestSettings & pSettings );

		StencilTestSettings & configureStencilTestActive( bool pActive );

		TS3_ATTR_NO_DISCARD StencilTestSettings & configureStencilTestSettings();

		void configureStencilTestSettings( const StencilTestSettings & pSettings );

		void setDepthStencilConfig( const DepthStencilConfig & pConfig );

		void resetDepthConfig();

		void resetStencilConfig();

		void reset();

	private:
		DepthStencilConfig _depthStencilConfig;
	};

	/// @brief
	class RasterizerDescriptor
	{
	public:
		RasterizerDescriptor( const RasterizerDescriptor & ) = default;
		RasterizerDescriptor & operator=( const RasterizerDescriptor & ) = default;

		RasterizerDescriptor() = default;

		RasterizerDescriptor( const RasterizerConfig & pConfig );

		TS3_ATTR_NO_DISCARD const RasterizerConfig & getRasterizerConfig() const noexcept;

		TS3_ATTR_NO_DISCARD bool isScissorTestEnabled() const noexcept;

		void configureCullMode( ECullMode pCullMode );

		void configurePrimitiveFillMode( EPrimitiveFillMode pFillMode );

		void configureFrontFaceVerticesOrder( ETriangleVerticesOrder pVerticesOrder );

		void configureScissorTestActive( bool pActive );

		void setRasterizerConfig( const RasterizerConfig & pConfig );

		void reset();

	private:
		RasterizerConfig _rasterizerConfig;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_COMMON_GRAPHICS_DESCRIPTORS_H__
