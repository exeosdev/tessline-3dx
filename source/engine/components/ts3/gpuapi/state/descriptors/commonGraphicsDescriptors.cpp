
#include "commonGraphicsDescriptors.h"

namespace ts3::gpuapi
{

	BlendDescriptor::BlendDescriptor( const BlendConfig & pConfig )
	{
		setBlendConfig( pConfig );
	}

	BlendDescriptor & BlendDescriptor::operator=( const BlendConfig & pConfig )
	{
		setBlendConfig( pConfig );
		return *this;
	}

	const BlendConfig & BlendDescriptor::getBlendConfig() const noexcept
	{
		return _blendConfig;
	}

	bool BlendDescriptor::isAttachmentBlendActive( uint32 pRTAIndex ) const noexcept
	{
		const auto attachmentBit = cxdefs::makeRTAttachmentFlag( pRTAIndex );
		return ( attachmentBit != 0 ) && _blendConfig.attachmentsMask.isSet( attachmentBit );
	}

	RTAttachmentBlendSettings & BlendDescriptor::configureAttachmentBlendActive( uint32 pRTAIndex, bool pActive )
	{
		ts3DebugAssert( cxdefs::isRTAttachmentIndexValid( pRTAIndex ) );

		const auto attachmentBit = cxdefs::makeRTAttachmentFlag( pRTAIndex );
		_blendConfig.attachmentsMask.setOrUnset( attachmentBit, pActive );

		auto & attachmentBlendSettings = _blendConfig.attachments[pRTAIndex];
		return attachmentBlendSettings;
	}

	void BlendDescriptor::configureAttachmentMask( Bitmask<ERTAttachmentFlags> pAttachmentMask )
	{
		const auto validatedAttachmentMask = ( pAttachmentMask & E_RT_ATTACHMENT_MASK_COLOR_ALL );
		_blendConfig.attachmentsMask = validatedAttachmentMask;
	}

	void BlendDescriptor::configureAttachmentBlendSettings( uint32 pRTAIndex, const RTAttachmentBlendSettings & pSettings )
	{
		ts3DebugAssert( cxdefs::isRTAttachmentIndexValid( pRTAIndex ) );

		auto & attachmentBlendSettings = _blendConfig.attachments[pRTAIndex];
		attachmentBlendSettings = pSettings;
	}

	void BlendDescriptor::configureAttachmentBlendSettingsCopy( uint32 pTargetRTAIndex, uint32 pSourceRTAIndex )
	{
		ts3DebugAssert( cxdefs::isRTAttachmentIndexValid( pTargetRTAIndex ) );
		ts3DebugAssert( cxdefs::isRTAttachmentIndexValid( pSourceRTAIndex ) );

		auto & targetSettings = _blendConfig.attachments[pTargetRTAIndex];
		auto & sourceSettings = _blendConfig.attachments[pSourceRTAIndex];

		targetSettings = sourceSettings;
	}

	void BlendDescriptor::enableAlphaToCoverage( bool pEnable )
	{
		_blendConfig.flags.setOrUnset( E_BLEND_CONFIG_FLAG_ENABLE_ALPHA_TO_COVERAGE_BIT, pEnable );
	}

	void BlendDescriptor::enableIndependentBlending( bool pEnable )
	{
		_blendConfig.flags.setOrUnset( E_BLEND_CONFIG_FLAG_ENABLE_MRT_INDEPENDENT_BLENDING_BIT, pEnable );
	}

	void BlendDescriptor::setBlendConfig( const BlendConfig & pConfig )
	{
		_blendConfig.attachmentsMask = pConfig.attachmentsMask;
		_blendConfig.flags = pConfig.flags;
		memCopy( _blendConfig.attachments, pConfig.attachments );
	}

	void BlendDescriptor::resetAttachmentBlendSettings( uint32 pRTAIndex )
	{
		if( cxdefs::isRTAttachmentIndexValid( pRTAIndex ) )
		{
			_blendConfig.attachments[pRTAIndex] = defaults::cvCommonRTAttachmentBlendSettingsDefault;
		}
	}

	void BlendDescriptor::reset()
	{
		_blendConfig.attachmentsMask = 0;
		_blendConfig.flags = 0;
		_blendConfig.attachments[0] = defaults::cvCommonRTAttachmentBlendSettingsDefault;
	}


	DepthStencilDescriptor::DepthStencilDescriptor( const DepthStencilConfig & pConfig )
	{
		setDepthStencilConfig( pConfig );
	}

	DepthStencilDescriptor & DepthStencilDescriptor::operator=( const DepthStencilConfig & pConfig )
	{
		setDepthStencilConfig( pConfig );
		return *this;
	}

	const DepthStencilConfig & DepthStencilDescriptor::getDepthStencilConfig() const noexcept
	{
		return _depthStencilConfig;
	}

	DepthTestSettings & DepthStencilDescriptor::configureDepthTestActive( bool pActive )
	{
		_depthStencilConfig.depthTestActiveState = pActive ? EActiveState::Enabled : EActiveState::Disabled;
		return _depthStencilConfig.depthTestSettings;
	}

	void DepthStencilDescriptor::configureDepthTestSettings( const DepthTestSettings & pSettings )
	{
		_depthStencilConfig.depthTestSettings = pSettings;
	}

	StencilTestSettings & DepthStencilDescriptor::configureStencilTestActive( bool pActive )
	{
		_depthStencilConfig.depthTestActiveState = pActive ? EActiveState::Enabled : EActiveState::Disabled;
		return _depthStencilConfig.stencilTestSettings;
	}

	void DepthStencilDescriptor::configureStencilTestSettings( const StencilTestSettings & pSettings )
	{
		_depthStencilConfig.stencilTestSettings = pSettings;
	}

	void DepthStencilDescriptor::setDepthStencilConfig( const DepthStencilConfig & pConfig )
	{
		_depthStencilConfig = pConfig;
	}

	void DepthStencilDescriptor::resetDepthConfig()
	{
		_depthStencilConfig.depthTestActiveState = EActiveState::Disabled;
		_depthStencilConfig.depthTestSettings = defaults::cvCommonDepthTestSettingsDefault;
	}

	void DepthStencilDescriptor::resetStencilConfig()
	{
		_depthStencilConfig.stencilTestActiveState = EActiveState::Disabled;
		_depthStencilConfig.stencilTestSettings = defaults::cvCommonStencilTestSettingsDefault;
	}

	void DepthStencilDescriptor::reset()
	{
		resetDepthConfig();
		resetStencilConfig();
	}


	RasterizerDescriptor::RasterizerDescriptor( const RasterizerConfig & pConfig )
	{
		setRasterizerConfig( pConfig );
	}

	const RasterizerConfig & RasterizerDescriptor::getRasterizerConfig() const noexcept
	{
		return _rasterizerConfig;
	}

	void RasterizerDescriptor::configureCullMode( ECullMode pCullMode )
	{
		_rasterizerConfig.cullMode = pCullMode;
	}

	void RasterizerDescriptor::configurePrimitiveFillMode( EPrimitiveFillMode pFillMode )
	{
		_rasterizerConfig.primitiveFillMode = pFillMode;
	}

	void RasterizerDescriptor::configureFrontFaceVerticesOrder( ETriangleVerticesOrder pVerticesOrder )
	{
		_rasterizerConfig.frontFaceVerticesOrder = pVerticesOrder;
	}

	void RasterizerDescriptor::configureScissorTestActive( bool pActive )
	{
		_rasterizerConfig.scissorTestActive = pActive ? EActiveState::Enabled : EActiveState::Disabled;
	}

	void RasterizerDescriptor::setRasterizerConfig( const RasterizerConfig & pConfig )
	{
		_rasterizerConfig = pConfig;
	}

	void RasterizerDescriptor::reset()
	{
		_rasterizerConfig = defaults::cvPipelineRasterizerConfigDefault;
	}

}
