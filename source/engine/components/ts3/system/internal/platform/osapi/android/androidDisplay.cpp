
#include <ts3/system/internal/internalDisplay.h>
#include <ts3/system/internal/internaltemContext.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3
{

	void DisplayManager::_sysInitialize()
	{
	}

	void DisplayManager::_sysRelease() noexcept
	{
	}

	void DisplayManager::_sysQueryDisplaySize( DisplaySize & pDisplaySize ) const
	{
	}

	void DisplayManager::_sysQueryMinWindowSize( DisplaySize & pMinWindowSize ) const
	{
	}

	void nativeDisplayManagerInitialize( DisplayManager & )
	{
	}

	void DisplayManagerImplProxy::nativeQueryDisplaySize( const DisplayManager & pDisplayManager, DisplaySize & pDisplaySize )
	{
        // Native window for the app is stored within the display manager.
        auto * androidNativeWindow = pDisplayManager.systemContext->nativeData->androidNativeWindow;

        // This function requires the window to already be available and throws if the window
        // pointer is still nullptr (ANativeWindow is not created with the activity itself!!)
        if( androidNativeWindow == nullptr )
        {
            throw 0;
        }

        int32_t windowWidth = ANativeWindow_getWidth( androidNativeWindow );
        int32_t windowHeight = ANativeWindow_getHeight( androidNativeWindow );

        if( ( windowWidth > 0 ) && ( windowHeight > 0 ) )
        {
            pDisplaySize.x = static_cast<uint32>( windowWidth );
            pDisplaySize.y = static_cast<uint32>( windowHeight );
        }
        else
        {
            pDisplaySize.x = 0u;
            pDisplaySize.y = 0u;
        }
	}

	void DisplayManagerImplProxy::nativeQueryMinWindowSize( const DisplayManager & pDisplayManager, DisplaySize & pMinWindowSize )
	{
        nativeQueryDisplaySize( pDisplayManager, pMinWindowSize );
	}

}
#endif
