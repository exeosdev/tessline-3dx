
#include "displayDriverNative.h"

namespace ts3::system
{

    DisplayDriverNativeData::DisplayDriverNativeData( EDisplayDriverType pDriverType )
    : driverType( pDriverType )
    {
        dsmInitializeNativeData( this, pDriverType );
    }

    DisplayDriverNativeData::~DisplayDriverNativeData()
    {
        dsmReleaseNativeData( this, driverType );
    }


    DisplayAdapterNativeData::DisplayAdapterNativeData( EDisplayDriverType pDriverType )
    : driverType( pDriverType )
    {
        dsmInitializeNativeData( this, pDriverType );
    }

    DisplayAdapterNativeData::~DisplayAdapterNativeData()
    {
        dsmReleaseNativeData( this, driverType );
    }


    DisplayOutputNativeData::DisplayOutputNativeData( EDisplayDriverType pDriverType )
    : driverType( pDriverType )
    {
        dsmInitializeNativeData( this, pDriverType );
    }

    DisplayOutputNativeData::~DisplayOutputNativeData()
    {
        dsmReleaseNativeData( this, driverType );
    }


    DisplayVideoModeNativeData::DisplayVideoModeNativeData( EDisplayDriverType pDriverType )
    : driverType( pDriverType )
    {
        dsmInitializeNativeData( this, pDriverType );
    }

    DisplayVideoModeNativeData::~DisplayVideoModeNativeData()
    {
        dsmReleaseNativeData( this, driverType );
    }


    DisplayDriver::ObjectPrivateData::ObjectPrivateData( DisplayDriver * pDriver )
    : parentDriver( pDriver )
    , nativeDataPriv( pDriver->mDriverType )
    {}


    DisplayAdapter::ObjectPrivateData::ObjectPrivateData( DisplayAdapter * pAdapter )
    : parentAdapter( pAdapter )
    , nativeDataPriv( pAdapter->mDriverType )
    {}


    DisplayOutput::ObjectPrivateData::ObjectPrivateData( DisplayOutput * pOutput )
    : parentOutput( pOutput )
    , nativeDataPriv( pOutput->mDriverType )
    {}


    DisplayVideoMode::ObjectPrivateData::ObjectPrivateData( DisplayVideoMode * pVideoMode )
    : parentVideoMode( pVideoMode )
    , nativeDataPriv( pVideoMode->mDriverType )
    {}

}
