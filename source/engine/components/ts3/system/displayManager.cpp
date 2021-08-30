
#include "displayManagerNative.h"
#include "windowCommon.h"

namespace ts3::system
{

    DisplayManager::DisplayManager( SysContextHandle pSysContext )
    : SysObject( std::move( pSysContext ) )
    , mPrivate( std::make_unique<ObjectPrivateData>() )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {}

    DisplayManager::~DisplayManager()
    {}

    DisplaySize DisplayManager::queryDefaultDisplaySize() const
    {
        DisplaySize result;
        _nativeQueryDefaultDisplaySize( result );
        return result;
    }

    DisplaySize DisplayManager::queryMinWindowSize() const
    {
        DisplaySize result;
        _nativeQueryMinWindowSize( result );
        return result;
    }

    bool DisplayManager::checkDriverSupport( EDisplayDriverType pDriverID ) const
    {
        // The default driver is always supported. If no driver is available on
        // a given system, DisplayManager will fail to create in the first place,
        if( pDriverID == EDisplayDriverType::Default )
        {
            return true;
        }

        // Resolve the driver ID. This will return EDisplayDriverType::Unknown
        // if the specified driver is not supported on the current system.
        auto resolvedDriverID = resolveDisplayDriverID( pDriverID );

        return resolvedDriverID != EDisplayDriverType::Unknown;
    }

    EDisplayDriverType DisplayManager::resolveDisplayDriverID( EDisplayDriverType pDriverID ) const
    {
        EDisplayDriverType resolvedDriverID = EDisplayDriverType::Unknown;

        if( pDriverID == EDisplayDriverType::Generic )
        {
            resolvedDriverID = EDisplayDriverType::Generic;
        }
        else if( pDriverID == EDisplayDriverType::Default )
        {
        #if( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_FLAG_OS_WINFAMILY )
            resolvedDriverID = EDisplayDriverType::DXGI;
        #else
            resolvedDriverID = EDisplayDriverType::Generic;
        #endif
        }
        #if( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_FLAG_OS_WINFAMILY )
        else if( pDriverID == EDisplayDriverType::DXGI )
        {
            resolvedDriverID = EDisplayDriverType::DXGI;
        }
        #endif

        return resolvedDriverID;
    }

    bool DisplayManager::checkWindowGeometry( const math::Pos2i & pWindowPosition,
                                              const math::Size2u & pWindowSize ) const
    {
        const auto & framePos = pWindowPosition;
        const auto & frameSize = pWindowSize;

        auto screenSize = queryDefaultDisplaySize();
        auto minWindowSize = queryMinWindowSize();

        if ( frameSize == cvWindowSizeMax )
        {
            return false;
        }
        if ( ( frameSize.x == 0 ) || ( frameSize.y == 0 ) )
        {
            return false;
        }
        if ( ( frameSize.x > screenSize.x ) || ( frameSize.y > screenSize.y ) )
        {
            return false;
        }
        if ( ( frameSize.x < minWindowSize.x ) || ( frameSize.y < minWindowSize.y ) )
        {
            return false;
        }

        if ( ( framePos.x < 0 ) || ( framePos.y < 0 ) )
        {
            return false;
        }
        else
        {
            auto maxPosX = static_cast<int32>( screenSize.x - frameSize.x );
            auto maxPosY = static_cast<int32>( screenSize.y - frameSize.y );
            if ( ( framePos.x > maxPosX ) || ( framePos.y > maxPosY ) )
            {
                return false;
            }
        }

        return true;
    }

    bool DisplayManager::validateWindowGeometry( math::Pos2i & pWindowPosition,
                                                 math::Size2u & pWindowSize ) const
    {
        auto originalPos = pWindowPosition;
        auto originalSize = pWindowSize;
        auto & framePos = pWindowPosition;
        auto & frameSize = pWindowSize;

        auto screenSize = queryDefaultDisplaySize();
        auto minWindowSize = queryMinWindowSize();

        if ( frameSize == cvWindowSizeMax )
        {
            // Window size exceeds the screen size - clamp the size.
            frameSize.x = screenSize.x;
            frameSize.y = screenSize.y;
        }
        else if ( ( frameSize.x == 0 ) || ( frameSize.y == 0 ) )
        {
            // Any dimension set to 0 means "use default size". By default,
            // we just use the ratio of the screen and 70% of its dimensions.
            frameSize.x = static_cast<uint32>( screenSize.x * 0.7 );
            frameSize.y = static_cast<uint32>( screenSize.y * 0.7 );
        }
        else
        {
            // Size of the window must be less than the size of the screen...
            frameSize.x = getMinOf( frameSize.x, screenSize.x );
            frameSize.y = getMinOf( frameSize.y, screenSize.y );

            // ... but at the same time bigger than the minimum allowed size (Win32-specific, really).
            frameSize.x = getMaxOf( frameSize.x, minWindowSize.x );
            frameSize.y = getMaxOf( frameSize.y, minWindowSize.y );
        }

        if ( ( framePos.x < 0 ) || ( framePos.y < 0 ) )
        {
            framePos.x = static_cast<int32>( ( screenSize.x - frameSize.x ) / 2 );
            framePos.y = static_cast<int32>( ( screenSize.y - frameSize.y ) / 2 );
        }
        else
        {
            auto maxPosX = static_cast<int32>( screenSize.x - frameSize.x );
            auto maxPosY = static_cast<int32>( screenSize.y - frameSize.y );
            framePos.x = getMinOf( framePos.x, maxPosX );
            framePos.y = getMinOf( framePos.y, maxPosY );
        }

        return ( framePos != originalPos ) || ( frameSize != originalSize );
    }

} // namespace ts3::system
