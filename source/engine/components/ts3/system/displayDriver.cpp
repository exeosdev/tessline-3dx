
#include "displayDriverNative.h"
#include "displayManager.h"
#include <ts3/stdext/stringUtils.h>

namespace ts3::system
{

    static const ColorFormat sColorFormatArray[6] =
    {
        ColorFormat::B8G8R8,
        ColorFormat::B8G8R8A8,
        ColorFormat::B8G8R8A8SRGB,
        ColorFormat::R8G8B8A8,
        ColorFormat::R8G8B8A8SRGB,
        ColorFormat::R10G10B10A2,
    };

    EDisplayAdapterVendorID _queryAdapterVendorID( const DisplayAdapter & pAdapter );


    DisplayAdapter::DisplayAdapter( DisplayDriver * pDriver )
    : mDisplayDriver( pDriver )
    , mDriverType( pDriver->mDriverType )
    , mPrivate( std::make_unique<ObjectPrivateData>( this ) )
    , mDesc( &( mPrivate->descPriv ) )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {}

    DisplayAdapter::~DisplayAdapter() = default;

    const DisplayOutputList & DisplayAdapter::getOutputList() const
    {
        return mPrivate->outputList;
    }

    DisplayOutput * DisplayAdapter::getOutput( dsm_index_t pOutputIndex ) const
    {
        if( pOutputIndex == CX_DSM_INDEX_DEFAULT )
        {
            return mPrivate->primaryOutput;
        }
        else
        {
            return mPrivate->outputList.at( pOutputIndex );
        }
    }

    DisplayOutput * DisplayAdapter::getDefaultOutput() const
    {
        return mPrivate->primaryOutput;
    }

    bool DisplayAdapter::isActiveAdapter() const
    {
        return mPrivate->descPriv.flags.isSet( E_DISPLAY_ADAPTER_FLAG_ACTIVE_BIT );
    }

    bool DisplayAdapter::isPrimaryAdapter() const
    {
        return mPrivate->descPriv.flags.isSet( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );
    }

    bool DisplayAdapter::hasActiveOutputs() const
    {
        return mPrivate->activeOutputsNum > 0;
    }

    bool DisplayAdapter::hasAnyOutputs() const
    {
        return !mPrivate->outputInternalStorage.empty();
    }


    DisplayOutput::DisplayOutput( DisplayAdapter * pAdapter )
    : mDisplayDriver( pAdapter->mDisplayDriver )
    , mParentAdapter( pAdapter )
    , mDriverType( pAdapter->mDriverType )
    , mPrivate( std::make_unique<ObjectPrivateData>( this ) )
    , mDesc( &( mPrivate->descPriv ) )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {}

    DisplayOutput::~DisplayOutput() = default;

    const DisplayVideoModeList & DisplayOutput::getVideoModeList() const
    {
        return mPrivate->getVideoModeList( ColorFormat::SystemNative );
    }

    const DisplayVideoModeList & DisplayOutput::getVideoModeList( ColorFormat pColorFormat ) const
    {
        return mPrivate->getVideoModeList( pColorFormat );
    }

    bool DisplayOutput::isActiveOutput() const
    {
        return mPrivate->descPriv.flags.isSet( E_DISPLAY_OUTPUT_FLAG_ACTIVE_BIT );
    }

    bool DisplayOutput::isPrimaryOutput() const
    {
        return mPrivate->descPriv.flags.isSet( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT );
    }


    DisplayVideoMode::DisplayVideoMode( DisplayOutput * pOutput )
    : mDisplayDriver( pOutput->mDisplayDriver )
    , mParentOutput( pOutput )
    , mDriverType( pOutput->mDriverType )
    , mPrivate( std::make_unique<ObjectPrivateData>( this ) )
    , mDesc( &( mPrivate->descPriv ) )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {}

    DisplayVideoMode::~DisplayVideoMode() = default;


    DisplayDriver::DisplayDriver( DisplayManager * pDisplayManager,
                                  EDisplayDriverType pDriverType )
    : SysObject( pDisplayManager->mSysContext )
    , mDisplayManager( pDisplayManager )
    , mDriverType( pDriverType )
    , mPrivate( std::make_unique<ObjectPrivateData>( this ) )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {}

    DisplayDriver::~DisplayDriver() = default;

    void DisplayDriver::initializeDisplayConfiguration()
    {
        if( !mPrivate->adapterInternalStorage.empty() )
        {
            _resetDisplayConfiguration();
        }
        _initializeDisplayConfiguration();
    }

    void DisplayDriver::resetDisplayConfiguration()
    {
        _resetDisplayConfiguration();
    }

    const DisplayAdapterList & DisplayDriver::getAdapterList() const
    {
        return mPrivate->adapterList;
    }

    ColorFormat DisplayDriver::resolveColorFormat( ColorFormat pColorFormat ) const
    {
        if( pColorFormat == ColorFormat::SystemNative )
        {
            return _nativeGetSystemDefaultColorFormat();
        }
        else if( pColorFormat == ColorFormat::Unknown )
        {
            return ColorFormat::Unknown;
        }
        else
        {
            return pColorFormat;
        }
    }

    const DisplayOutputList & DisplayDriver::getOutputList( dsm_index_t pAdapterIndex ) const
    {
        auto * adapter = getAdapter( pAdapterIndex );
        return adapter->getOutputList();
    }

    DisplayAdapter * DisplayDriver::getAdapter( dsm_index_t pAdapterIndex ) const
    {
        if( pAdapterIndex == CX_DSM_INDEX_DEFAULT )
        {
            return mPrivate->primaryAdapter;
        }
        else
        {
            return mPrivate->adapterList.at( pAdapterIndex );
        }
    }

    DisplayAdapter * DisplayDriver::getDefaultAdapter() const
    {
        return mPrivate->primaryAdapter;
    }

    DisplayOutput * DisplayDriver::getDefaultOutput( dsm_index_t pAdapterIndex ) const
    {
        auto * adapter = getAdapter( pAdapterIndex );
        return adapter->getDefaultOutput();
    }

    DisplayOutput * DisplayDriver::getOutput( dsm_output_id_t pOutputID ) const
    {
        return _getOutput( pOutputID );
    }

    ColorFormat DisplayDriver::getSystemDefaultColorFormat() const
    {
        return _nativeGetSystemDefaultColorFormat();
    }

    ArrayView<const ColorFormat> DisplayDriver::getSupportedColorFormatList() const
    {
        return _nativeGetSupportedColorFormatList();
    }

    bool DisplayDriver::hasActiveAdapters() const
    {
        return mPrivate->activeAdaptersNum > 0;
    }

    bool DisplayDriver::hasAnyAdapters() const
    {
        return !mPrivate->adapterInternalStorage.empty();
    }

    std::string DisplayDriver::generateConfigurationDump( const std::string & pLinePrefix ) const
    {
        const auto adaptersNum = mPrivate->adapterInternalStorage.size();
        const auto displayDevicesNum = adaptersNum * 2;
        const auto averageDisplayModesNum = 16;
        const auto averageLineLength = 40 + pLinePrefix.length();
        const auto strReserveSize = ( displayDevicesNum * averageDisplayModesNum + displayDevicesNum ) * averageLineLength;

        std::string result;
        result.reserve( strReserveSize );

        if( !hasAnyAdapters() )
        {
            result.append( pLinePrefix );
            result.append( "<empty>" );
        }
        else
        {
            const auto & adapterList = getAdapterList();
            for( const auto * adapter : adapterList )
            {
                if( adapter->mDesc->adapterIndex > 0 )
                {
                    result.append( 1, '\n' );
                }
                result.append( pLinePrefix );
                result.append( adapter->mDesc->toString() );

                if( !adapter->hasAnyOutputs() )
                {
                    result.append( 1, '\n' );
                    result.append( pLinePrefix );
                    result.append( 1, '\t' );
                    result.append( "<empty>" );
                }
                else
                {
                    const auto & outputList = adapter->getOutputList();
                    for( const auto * output : outputList )
                    {
                        result.append( 1, '\n' );
                        result.append( pLinePrefix );
                        result.append( 1, '\t' );
                        result.append( output->mDesc->toString() );

                        auto colorFormatList = getSupportedColorFormatList();
                        for( auto colorFormat : colorFormatList )
                        {
                            auto colorFormatStr = vsxQueryColorFormatStr( colorFormat );
                            result.append( 1, '\n' );
                            result.append( pLinePrefix );
                            result.append( 2, '\t' );
                            result.append( colorFormatStr );

                            const auto & displayModeList = output->getVideoModeList( colorFormat );
                            if( displayModeList.empty() )
                            {
                                result.append( 1, '\n' );
                                result.append( pLinePrefix );
                                result.append( 3, '\t' );
                                result.append( "<empty>" );
                            }
                            else
                            {
                                for( const auto * displayMode : displayModeList )
                                {
                                    result.append( 1, '\n' );
                                    result.append( pLinePrefix );
                                    result.append( 3, '\t' );
                                    result.append( displayMode->mDesc->toString() );
                                }
                            }
                        }

                    }
                }
            }
        }

        return result;
    }

    DisplayAdapter * DisplayDriver::addAdapter()
    {
        const auto adapterIndex = mPrivate->adapterInternalStorage.size();

        auto & adapter = mPrivate->adapterInternalStorage.emplace_back( this );
        auto & adapterDesc = dsmGetObjectDesc( adapter );
        adapterDesc.driverType = mDriverType;
        adapterDesc.adapterIndex = static_cast<dsm_index_t>( adapterIndex );

        // Adapters are not added to the helper list at this point.
        // This is done as a post-process step later in DisplayDriver::_enumAdapters().
        // Assertion added to prevent problems in case of refactoring.
        ts3DebugAssert( mPrivate->adapterList.empty() );

        return &adapter;
    }

    DisplayOutput * DisplayDriver::addOutput( DisplayAdapter & pAdapter )
    {
        const auto outputIndex = pAdapter.mPrivate->outputInternalStorage.size();

        DisplayOutputIDGen outputIDGen;
        outputIDGen.uAdapterIndex = pAdapter.mPrivate->descPriv.adapterIndex;
        outputIDGen.uOutputIndex = static_cast<dsm_index_t>( outputIndex );

        auto & output = pAdapter.mPrivate->outputInternalStorage.emplace_back( &pAdapter );
        auto & outputDesc = dsmGetObjectDesc( output );
        outputDesc.driverType = mDriverType;
        outputDesc.outputIndex = outputIDGen.uOutputIndex;
        outputDesc.outputID = outputIDGen.outputID;

        // Outputs are not added to the helper list at this point.
        // This is done as a post-process step later in DisplayDriver::_enumOutputs().
        // Assertion added to prevent problems in case of refactoring.
        ts3DebugAssert( pAdapter.mPrivate->outputList.empty() );

        return &output;
    }

    DisplayVideoMode * DisplayDriver::addVideoMode( DisplayOutput & pOutput, ColorFormat pColorFormat )
    {
        auto & colorFormatData = pOutput.mPrivate->getColorFormatData( pColorFormat );

        const auto videoModeIndex = colorFormatData.videoModeInternalStorage.size();

        DisplayVideoModeIDGen videoModeIDGen;
        videoModeIDGen.uOutputID = pOutput.mPrivate->descPriv.outputID;
        videoModeIDGen.uColorFormatIndex = static_cast<dsm_index_t>( pColorFormat );
        videoModeIDGen.uModeIndex = static_cast<dsm_index_t>( videoModeIndex );

        auto & videoMode = colorFormatData.videoModeInternalStorage.emplace_back( &pOutput );
        auto & videoModeDesc = dsmGetObjectDesc( videoMode );
        videoModeDesc.driverType = mDriverType;
        videoModeDesc.videoModeIndex = videoModeIDGen.uModeIndex;
        videoModeDesc.videoModeID = videoModeIDGen.modeID;
        videoModeDesc.colorFormat = colorFormatData.colorFormat;

        // Video modes are not added to the helper list at this point.
        // This is done as a post-process step later in DisplayDriver::_enumVideoModes().
        // Assertion added to prevent problems in case of refactoring.
        ts3DebugAssert( colorFormatData.videoModeList.empty() );

        return &videoMode;
    }

    void DisplayDriver::_initializeDisplayConfiguration()
    {
        _enumDisplayDevices();

        for( auto & adapter : mPrivate->adapterInternalStorage )
        {
            for( auto & output : adapter.mPrivate->outputInternalStorage )
            {
                auto colorFormatList = getSupportedColorFormatList();
                for( auto colorFormat : colorFormatList )
                {
                    _enumVideoModes( output, colorFormat );
                }
            }
        }
    }

    void DisplayDriver::_resetDisplayConfiguration()
    {
        for( auto & adapter : mPrivate->adapterInternalStorage )
        {
            for( auto & output : adapter.mPrivate->outputInternalStorage )
            {
                auto colorFormatList = getSupportedColorFormatList();
                for( auto colorFormat : colorFormatList )
                {
                    auto & colorFormatData = output.mPrivate->getColorFormatData( colorFormat );
                    for( auto & videoMode : colorFormatData.videoModeInternalStorage )
                    {
                        _nativeDestroyVideoMode( videoMode );
                    }
                    colorFormatData.videoModeInternalStorage.clear();
                    colorFormatData.videoModeList.clear();
                }
                output.mPrivate->resetColorFormatMap();
                _nativeDestroyOutput( output );
            }
            adapter.mPrivate->outputInternalStorage.clear();
            _nativeDestroyAdapter( adapter );
        }
        mPrivate->adapterInternalStorage.clear();
    }

    void DisplayDriver::_enumDisplayDevices()
    {
        _nativeEnumDisplayDevices();

        if( !mPrivate->adapterInternalStorage.empty() )
        {
            // Reserve space for the list of pointers/handles for adapters.
            mPrivate->adapterList.reserve( mPrivate->adapterInternalStorage.size() );

            for( auto & adapter : mPrivate->adapterInternalStorage )
            {
                if( adapter.isActiveAdapter() )
                {
                    mPrivate->activeAdaptersNum += 1;
                }
                if( adapter.isPrimaryAdapter() )
                {
                    // Primary adapter is set here and should not be assigned by the driver itself.
                    // We pick it 
                    ts3DebugAssert( !mPrivate->primaryAdapter );
                    mPrivate->primaryAdapter = &adapter;
                }

                adapter.mPrivate->descPriv.vendorID = _queryAdapterVendorID( adapter );

                mPrivate->adapterList.push_back( &adapter );

                if( !adapter.mPrivate->outputInternalStorage.empty() )
                {
                    adapter.mPrivate->outputList.reserve( adapter.mPrivate->outputInternalStorage.size() );
                    for( auto & output : adapter.mPrivate->outputInternalStorage )
                    {
                        if( output.isActiveOutput() )
                        {
                            adapter.mPrivate->activeOutputsNum += 1;
                        }
                        if( output.isPrimaryOutput() )
                        {
                            ts3DebugAssert( !adapter.mPrivate->primaryOutput );
                            adapter.mPrivate->primaryOutput = &output;
                        }

                        adapter.mPrivate->outputList.push_back( &output );
                    }
                    if( !adapter.mPrivate->primaryOutput && !adapter.mPrivate->outputInternalStorage.empty() )
                    {
                        auto & firstOutput = adapter.mPrivate->outputInternalStorage.front();
                        firstOutput.mPrivate->descPriv.flags.set( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT );
                        adapter.mPrivate->primaryOutput = &firstOutput;
                    }
                }
            }
            if( !mPrivate->primaryAdapter && !mPrivate->adapterInternalStorage.empty() )
            {
                auto & firstAdapter = mPrivate->adapterInternalStorage.front();
                firstAdapter.mPrivate->descPriv.flags.set( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );
                mPrivate->primaryAdapter = &firstAdapter;
            }
        }
    }

    void DisplayDriver::_enumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat )
    {
        auto & colorFormatData = pOutput.mPrivate->getColorFormatData( pColorFormat );
        ts3DebugAssert( colorFormatData.videoModeInternalStorage.empty() );

        _nativeEnumVideoModes( pOutput, colorFormatData.colorFormat );

        if( !colorFormatData.videoModeInternalStorage.empty() )
        {
            colorFormatData.videoModeList.reserve( colorFormatData.videoModeInternalStorage.size() );
            for( auto & videoMode : colorFormatData.videoModeInternalStorage )
            {
                colorFormatData.videoModeList.push_back( &videoMode );
            }
        }
    }

    DisplayAdapter * DisplayDriver::_getAdapter( dsm_index_t pAdapterIndex ) const
    {
        if( pAdapterIndex == CX_DSM_INDEX_DEFAULT )
        {
            return mPrivate->primaryAdapter;
        }
        else
        {
            return mPrivate->adapterList.at( pAdapterIndex );
        }
    }

    DisplayOutput * DisplayDriver::_getOutput( dsm_output_id_t pOutputID ) const
    {
        auto * adapter = _getAdapter( dsmExtractOutputIDAdapterIndex( pOutputID ) );
        auto outputIndex = dsmExtractOutputIDOutputIndex( pOutputID );

        if( outputIndex == CX_DSM_INDEX_DEFAULT )
        {
            return adapter->mPrivate->primaryOutput;
        }
        else
        {
            return adapter->mPrivate->outputList.at( outputIndex );
        }
    }


    EDisplayAdapterVendorID _queryAdapterVendorID( const DisplayAdapter & pAdapter )
    {
        auto adapterVendorID = EDisplayAdapterVendorID::Unknown;
        auto adapterString = strUtils::makeUpper( pAdapter.mPrivate->descPriv.name );

        if( ( adapterString.find( "AMD" ) != std::string::npos ) || ( adapterString.find( "ATI" ) != std::string::npos ) )
        {
            adapterVendorID = EDisplayAdapterVendorID::AMD;
        }
        else if( adapterString.find( "ARM" ) != std::string::npos )
        {
            adapterVendorID = EDisplayAdapterVendorID::ARM;
        }
        else if( adapterString.find( "GOOGLE" ) != std::string::npos )
        {
            adapterVendorID = EDisplayAdapterVendorID::Google;
        }
        else if( adapterString.find( "INTEL" ) != std::string::npos )
        {
            adapterVendorID = EDisplayAdapterVendorID::Intel;
        }
        else if( adapterString.find( "MICROSOFT" ) != std::string::npos )
        {
            adapterVendorID = EDisplayAdapterVendorID::Microsoft;
        }
        else if( adapterString.find( "NVIDIA" ) != std::string::npos )
        {
            adapterVendorID = EDisplayAdapterVendorID::Nvidia;
        }
        else if( adapterString.find( "QUALCOMM" ) != std::string::npos )
        {
            adapterVendorID = EDisplayAdapterVendorID::Qualcomm;
        }

        return adapterVendorID;
    }

}
