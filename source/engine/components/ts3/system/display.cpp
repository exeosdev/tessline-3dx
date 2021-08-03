
#include "displayNative.h"

namespace ts3
{
namespace system
{

    void _nativeDsmDisplayManagerInitialize( DsmDisplayManager & pDisplayManager );
    void _nativeDsmDisplayManagerRelease( DsmDisplayManager & pDisplayManager );
    void _nativeDsmDisplayManagerQueryDisplaySize( DsmDisplayManager & pDisplayManager, DisplaySize & pOutDisplaySize );
    void _nativeDsmDisplayManagerQueryMinWindowSize( DsmDisplayManager & pDisplayManager, DisplaySize & pOutMinWindowSize );
    
    DsmDisplayManagerHandle dsmInitializeDisplayManager( CoreSessionContextHandle pCSContext )
    {
        auto * displayManager = new DsmDisplayManager();
        _nativeDsmDisplayManagerInitialize( *displayManager );
        return displayManager;
    }

    void dsmDestroyDisplayManager( DsmDisplayManagerHandle pDisplayManager )
    {
        if( !pDisplayManager )
        {
            return;
        }

        _nativeDsmDisplayManagerRelease( *pDisplayManager );
        delete pDisplayManager;
    }

    DisplaySize dsmDisplayManagerQueryDisplaySize( DsmDisplayManagerHandle pDisplayManager )
    {
        DisplaySize result;
        _nativeDsmDisplayManagerQueryDisplaySize( *pDisplayManager, result );
        return result;
    }

    DisplaySize dsmDisplayManagerQueryMinWindowSize( DsmDisplayManagerHandle pDisplayManager )
    {
        DisplaySize result;
        _nativeDsmDisplayManagerQueryMinWindowSize( *pDisplayManager, result );
        return result;
    }

    bool dsmDisplayManagerCheckDriverSupport( EDsmDisplayDriverType pDriverID )
    {
        return true;
    }

    EDsmDisplayDriverType dsmDisplayManagerResolveDisplayDriverID( EDsmDisplayDriverType pDriverID )
    {
        EDsmDisplayDriverType resolvedDriverID = EDsmDisplayDriverType::Unknown;

        if( pDriverID == EDsmDisplayDriverType::Generic )
        {
            resolvedDriverID = EDsmDisplayDriverType::Generic;
        }
        else if( pDriverID == EDsmDisplayDriverType::Default )
        {
            // TODO
        }
        else if( pDriverID == EDsmDisplayDriverType::DXGI )
        {
            // TODO
        }

        return resolvedDriverID;
    }

    bool dsmDisplayManagerValidateWindowGeometry( DsmDisplayManagerHandle pDisplayManager,
                                                  const WindowGeometry & pWindowGeometry )
    {
        const auto & framePos = pWindowGeometry.position;
        const auto & frameSize = pWindowGeometry.size;

        auto screenSize = dsmDisplayManagerQueryDisplaySize( pDisplayManager );
        auto minWindowSize = dsmDisplayManagerQueryMinWindowSize( pDisplayManager );

        if ( frameSize == cvWindowSizeMax )
        {
            return false;
        }
        else if ( ( frameSize.x == 0 ) || ( frameSize.y == 0 ) )
        {
            return false;
        }
        else if ( ( frameSize.x > screenSize.x ) || ( frameSize.y > screenSize.y ) )
        {
            return false;
        }
        else if ( ( frameSize.x < minWindowSize.x ) || ( frameSize.y < minWindowSize.y ) )
        {
            return false;
        }

        if ( ( framePos.x < 0 ) || ( framePos.y < 0 ) )
        {
            return false;
        }
        else
        {
            int32 maxPosX = screenSize.x - frameSize.x;
            int32 maxPosY = screenSize.y - frameSize.y;
            if ( ( framePos.x > maxPosX ) || ( framePos.y > maxPosY ) )
            {
                return false;
            }
        }

        return true;
    }

    bool dsmDisplayManagerValidateWindowGeometry( DsmDisplayManagerHandle pDisplayManager,
                                                  WindowGeometry & pWindowGeometry )
    {
        auto originalPos = pWindowGeometry.position;
        auto originalSize = pWindowGeometry.size;
        auto & framePos = pWindowGeometry.position;
        auto & frameSize = pWindowGeometry.size;

        auto screenSize = queryDisplaySize();
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
            frameSize.x = static_cast<uint32>( screenSize.x * 0.7f );
            frameSize.y = static_cast<uint32>( screenSize.y * 0.7f );
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
            framePos.x = ( screenSize.x - frameSize.x ) / 2;
            framePos.y = ( screenSize.y - frameSize.y ) / 2;
        }
        else
        {
            int32 maxPosX = screenSize.x - frameSize.x;
            int32 maxPosY = screenSize.y - frameSize.y;
            framePos.x = getMinOf( framePos.x, maxPosX );
            framePos.y = getMinOf( framePos.y, maxPosY );
        }

        return ( framePos != originalPos ) || ( frameSize != originalSize );
    }

} // namespace system
} // namespace ts3
