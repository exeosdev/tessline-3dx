
#include <ts3/system/internal/internalDisplayManager.h>
#include <ts3/system/internal/internalSystemContext.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3
{

	void SysDisplayManager::_sysInitialize()
	{
	}

	void SysDisplayManager::_sysRelease() noexcept
	{
	}

	void SysDisplayManager::_sysQueryDisplaySize( SysDisplaySize & pDisplaySize ) const
	{
	}

	void SysDisplayManager::_sysQueryMinWindowSize( SysDisplaySize & pMinWindowSize ) const
	{
	}

	void nativeDisplayManagerInitialize( SysDisplayManager & )
	{
	}

	void SysDisplayManagerImplProxy::nativeQueryDisplaySize( const SysDisplayManager & pDisplayManager, SysDisplaySize & pDisplaySize )
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

	void SysDisplayManagerImplProxy::nativeQueryMinWindowSize( const SysDisplayManager & pDisplayManager, SysDisplaySize & pMinWindowSize )
	{
        nativeQueryDisplaySize( pDisplayManager, pMinWindowSize );
	}

}
#endif
