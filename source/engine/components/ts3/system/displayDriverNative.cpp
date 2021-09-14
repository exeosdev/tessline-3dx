
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

//    DisplayOutput::ObjectPrivateData::ColorFormatData & DisplayOutput::ObjectPrivateData::getOrCreateColorFormatData( ColorFormat pColorFormat )
//    {
//        auto resolvedColorFormat = dsmResolveColorFormat( pColorFormat );
//        auto colorFormatDataRef = _colorFormatMap.find( resolvedColorFormat );
//        if( colorFormatDataRef == _colorFormatMap.end() )
//        {
//            auto insertRes = _colorFormatMap.insert( {resolvedColorFormat, ColorFormatData{}} );
//            colorFormatDataRef = insertRes.first;
//            colorFormatDataRef->second.colorFormat = resolvedColorFormat;
//        }
//        return colorFormatDataRef->second;
//    }
//
//    DisplayOutput::ObjectPrivateData::ColorFormatData & DisplayOutput::ObjectPrivateData::getColorFormatData( ColorFormat pColorFormat )
//    {
//        auto resolvedColorFormat = dsmResolveColorFormat( pColorFormat );
//        return _colorFormatMap.at( resolvedColorFormat );
//    }
//
//    const DisplayOutput::ObjectPrivateData::ColorFormatData & DisplayOutput::ObjectPrivateData::getColorFormatData( ColorFormat pColorFormat ) const
//    {
//        auto resolvedColorFormat = dsmResolveColorFormat( pColorFormat );
//        return _colorFormatMap.at( resolvedColorFormat );
//    }
//
//    bool DisplayOutput::ObjectPrivateData::hasColorFormatData( ColorFormat pColorFormat ) const
//    {
//        auto resolvedColorFormat = dsmResolveColorFormat( pColorFormat );
//        auto colorFormatDataRef = _colorFormatMap.find( resolvedColorFormat );
//        return ( colorFormatDataRef != _colorFormatMap.end() ) && !colorFormatDataRef->second.videoModeInternalStorage.empty();
//    }


    DisplayVideoMode::ObjectPrivateData::ObjectPrivateData( DisplayVideoMode * pVideoMode )
    : parentVideoMode( pVideoMode )
    , nativeDataPriv( pVideoMode->mDriverType )
    {}

}
