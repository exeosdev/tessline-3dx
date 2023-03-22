
#include "MTL_apiTranslationLayer.h"
#include <ts3/gpuapi/resources/textureCommon.h>
#include <ts3/gpuapi/state/samplerCommon.h>
#include <ts3/math/color.h>

namespace ts3::gpuapi
{

	MTLCompareFunction atl::translateMTLCompFunc( ECompFunc pCompFunc )
	{
		switch( pCompFunc )
		{
			ts3CaseReturn( ECompFunc::Never        , MTLCompareFunctionNever        );
			ts3CaseReturn( ECompFunc::Always       , MTLCompareFunctionAlways       );
			ts3CaseReturn( ECompFunc::Equal        , MTLCompareFunctionEqual        );
			ts3CaseReturn( ECompFunc::NotEqual     , MTLCompareFunctionNotEqual     );
			ts3CaseReturn( ECompFunc::Greater      , MTLCompareFunctionGreater      );
			ts3CaseReturn( ECompFunc::GreaterEqual , MTLCompareFunctionGreaterEqual );
			ts3CaseReturn( ECompFunc::Less         , MTLCompareFunctionLess         );
			ts3CaseReturn( ECompFunc::LessEqual    , MTLCompareFunctionLessEqual    );

			default:
				break;
		};

		ts3Throw( 0 );
	}

	MTLStencilOperation atl::translateMTLStencilOp( EStencilOp pStencilOp )
	{
		switch( pStencilOp )
		{
			ts3CaseReturn( EStencilOp::Zero      , MTLStencilOperationZero           );
			ts3CaseReturn( EStencilOp::Keep      , MTLStencilOperationKeep           );
			ts3CaseReturn( EStencilOp::Replace   , MTLStencilOperationReplace        );
			ts3CaseReturn( EStencilOp::IncrClamp , MTLStencilOperationIncrementClamp );
			ts3CaseReturn( EStencilOp::IncrWrap  , MTLStencilOperationIncrementWrap  );
			ts3CaseReturn( EStencilOp::DecrClamp , MTLStencilOperationDecrementClamp );
			ts3CaseReturn( EStencilOp::DecrWrap  , MTLStencilOperationDecrementWrap  );
			ts3CaseReturn( EStencilOp::Invert    , MTLStencilOperationInvert         );

			default:
				break;
		};

		ts3Throw( 0 );
	}

	MTLSamplerAddressMode atl::translateMTLTextureAddressMode( ETextureAddressMode pAddressMode )
	{
		switch( pAddressMode )
		{
			ts3CaseReturn( ETextureAddressMode::ClampToColor      , MTLSamplerAddressModeClampToBorderColor );
			ts3CaseReturn( ETextureAddressMode::ClampToEdge       , MTLSamplerAddressModeClampToEdge        );
			ts3CaseReturn( ETextureAddressMode::MirrorClampToEdge , MTLSamplerAddressModeMirrorClampToEdge  );
			ts3CaseReturn( ETextureAddressMode::MirrorRepeat      , MTLSamplerAddressModeMirrorRepeat       );
			ts3CaseReturn( ETextureAddressMode::Repeat            , MTLSamplerAddressModeRepeat             );

			default:
				break;
		};

		ts3Throw( 0 );
	}

	MTLSamplerBorderColor atl::translateMTLTextureBorderPredefinedColor( ETextureBorderPredefinedColor pColor )
	{
		switch( pColor )
		{
			ts3CaseReturn( ETextureBorderPredefinedColor::OpaqueBlack      , MTLSamplerBorderColorOpaqueBlack      );
			ts3CaseReturn( ETextureBorderPredefinedColor::OpaqueWhite      , MTLSamplerBorderColorOpaqueWhite      );
			ts3CaseReturn( ETextureBorderPredefinedColor::TransparentBlack , MTLSamplerBorderColorTransparentBlack );

			default:
				break;
		};

		ts3Throw( 0 );
	}


	MTLSamplerMinMagFilter atl::translateMTLTextureFilter( ETextureFilter pFilter )
	{
		switch( pFilter )
		{
			ts3CaseReturn( ETextureFilter::Point       , MTLSamplerMinMagFilterNearest );
			ts3CaseReturn( ETextureFilter::Linear      , MTLSamplerMinMagFilterLinear  );
			ts3CaseReturn( ETextureFilter::Anisotropic , MTLSamplerMinMagFilterLinear  );

			default:
				break;
		};

		ts3Throw( 0 );
	}

	MTLSamplerMipFilter atl::translateMTLTextureMipMode( ETextureMipMode pMipMode )
	{
		switch( pMipMode )
		{
			ts3CaseReturn( ETextureMipMode::Disable , MTLSamplerMipFilterNotMipmapped );
			ts3CaseReturn( ETextureMipMode::Nearest , MTLSamplerMipFilterNearest  );
			ts3CaseReturn( ETextureMipMode::Linear  , MTLSamplerMipFilterLinear  );

			default:
				break;
		};

		ts3Throw( 0 );
	}

	MTLSamplerBorderColor atl::selectMTLSamplerBorderColor(
			ETextureBorderPredefinedColor pPredefinedColor,
			const math::RGBAColorR32Norm & pCustomColor )
	{
		auto selectedColor = pPredefinedColor;

		if( selectedColor == ETextureBorderPredefinedColor::Undefined )
		{
			const auto u8Color = pCustomColor.toU8Color();

			if( u8Color == ts3::math::colors::CX_COLOR_BLACK_OPAQUE )
			{
				selectedColor = ETextureBorderPredefinedColor::OpaqueBlack;
			}
			else if( u8Color == ts3::math::colors::CX_COLOR_BLACK_TRANSPARENT )
			{
				selectedColor = ETextureBorderPredefinedColor::TransparentBlack;
			}
			else if( u8Color == ts3::math::colors::CX_COLOR_WHITE_OPAQUE )
			{
				selectedColor = ETextureBorderPredefinedColor::OpaqueWhite;
			}
		}

		if( selectedColor == ETextureBorderPredefinedColor::Undefined )
		{
			// Custom color which is not any of the supported ones.
			selectedColor = ETextureBorderPredefinedColor::OpaqueWhite;
		}

		return translateMTLTextureBorderPredefinedColor( selectedColor );
	}

}
