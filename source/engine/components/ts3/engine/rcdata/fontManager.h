
#pragma once

#ifndef __TS3_ENGINE_FONT_MANAGER_H__
#define __TS3_ENGINE_FONT_MANAGER_H__

#include "font.h"
#include "fontTexture.h"

namespace ts3
{

	class FontManager
	{
	public:
		FontManager();

		FontHandle createFont( const FontCreateInfo & pFontCreateInfo );

		FontHandle findFont( font_guid_t pFontGUID ) const;

		uint32 getFontsNum( font_type_t pFontType ) const;

		uint32 getFontsNum() const;

		void registerFontLoader( font_type_t pFontType, FontLoaderFactoryCallback pFactoryCallback );

		template <typename TpLoader, typename... TpArgs>
		void registerFontLoader( font_type_t pFontType, gpuapi::CommandContext & pGPUCommandContext, TpArgs && ...pArgs )
		{
			registerFontLoader(
					pFontType,
					[&pGPUCommandContext]() {
						return std::make_unique<TpLoader>( pGPUCommandContext );
					} );
		}

	private:
		FontLoaderPtr getFontLoader( font_type_t pFontType );

	private:
		using FontLoaderFactoryMap = std::unordered_map<font_type_t, FontLoaderFactoryCallback>;
		FontLoaderFactoryMap _loaderFactoryCallbackMap;
	};

}

#endif // __TS3_ENGINE_FONT_MANAGER_H__
