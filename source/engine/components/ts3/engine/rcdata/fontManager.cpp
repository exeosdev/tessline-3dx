
#include "fontManager.h"

namespace ts3
{

	FontManager::FontManager() = default;

	FontHandle FontManager::createFont( const FontCreateInfo & pFontCreateInfo )
	{
		auto fontLoader = getFontLoader( pFontCreateInfo.fontType );
		if( !fontLoader )
		{
			return nullptr;
		}

		return fontLoader->createFont( pFontCreateInfo );
	}

	void FontManager::registerFontLoader( font_type_t pFontType, FontLoaderFactoryCallback pFactoryCallback )
	{
		_loaderFactoryCallbackMap[pFontType] = std::move( pFactoryCallback );
	}

	FontLoaderPtr FontManager::getFontLoader( font_type_t pFontType )
	{
		auto loaderFactoryCallbackIter = _loaderFactoryCallbackMap.find( pFontType );
		if( loaderFactoryCallbackIter != _loaderFactoryCallbackMap.end() )
		{
			auto fontLoader = loaderFactoryCallbackIter->second();
			return fontLoader;
		}
		return nullptr;
	}

}
