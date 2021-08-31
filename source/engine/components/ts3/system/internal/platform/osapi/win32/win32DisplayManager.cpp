
#include <ts3/system/displayManagerNative.h>

namespace ts3::system
{

    void DisplayManager::_nativeQueryMinWindowSize( DisplaySize & pOutSize ) const
    {
        auto cxMin = ::GetSystemMetrics( SM_CXMIN );
        auto cyMin = ::GetSystemMetrics( SM_CYMIN );
        pOutSize.x = static_cast<uint32>( cxMin );
        pOutSize.y = static_cast<uint32>( cyMin );
    }

    void DisplayManager::_nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const
    {
        auto cxScreen = ::GetSystemMetrics( SM_CXSCREEN );
        auto cyScreen = ::GetSystemMetrics( SM_CYSCREEN );
        pOutSize.x = static_cast<uint32>( cxScreen );
        pOutSize.y = static_cast<uint32>( cyScreen );
    }

}
