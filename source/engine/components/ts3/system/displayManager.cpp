
#include "displayManagerNative.h"
#include "displayDriverNative.h"
#include "windowCommon.h"

namespace ts3::system
{

    DisplayManager::DisplayManager( SysContextHandle pSysContext )
    : SysObject( std::move( pSysContext ) )
    , mPrivate( std::make_unique<ObjectPrivateData>() )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {
        mPrivate->driverFactoryMap[EDisplayDriverType::Generic] = [this]() {
            return createSysObject<DisplayDriverGeneric>( this );
        };
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
        mPrivate->driverFactoryMap[EDisplayDriverType::DXGI] = [this]() {
            return createSysObject<DisplayDriverDXGI>( this );
        };
    #endif
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
        mPrivate->driverFactoryMap[EDisplayDriverType::SDL] = [this]() {
            return createSysObject<DisplayDriverSDL>( this );
        };
    #endif
    }

    DisplayManager::~DisplayManager() noexcept = default;

    DisplayDriverHandle DisplayManager::createDisplayDriver( EDisplayDriverType pDriverID )
    {
        pDriverID = resolveDisplayDriverID( pDriverID );
        if( pDriverID == EDisplayDriverType::Unknown )
        {
            return nullptr;
        }

        auto & factoryCallback = mPrivate->driverFactoryMap.at( pDriverID );
        auto displayDriver = factoryCallback();

        return displayDriver;
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

} // namespace ts3::system
