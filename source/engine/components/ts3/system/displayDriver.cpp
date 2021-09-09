
#include "displayDriverNative.h"
#include "displayManager.h"

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


    DisplayAdapter::DisplayAdapter( DisplayDriver * pDriver )
    : mDisplayDriver( pDriver )
    , mDriverType( pDriver->mDriverType )
    , mPrivate( std::make_unique<ObjectPrivateData>( this ) )
    , mDesc( &( mPrivate->descPriv ) )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {
        dsmInitializeNativeData( &( mPrivate->nativeDataPriv ), mDisplayDriver->mDriverType );
    }

    DisplayAdapter::~DisplayAdapter()
    {
        dsmReleaseNativeData( &( mPrivate->nativeDataPriv ), mDisplayDriver->mDriverType );
    }

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
    {
        dsmInitializeNativeData( &( mPrivate->nativeDataPriv ), mDisplayDriver->mDriverType );
    }

    DisplayOutput::~DisplayOutput()
    {
        dsmReleaseNativeData( &( mPrivate->nativeDataPriv ), mDisplayDriver->mDriverType );
    }

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
    {
        dsmInitializeNativeData( &( mPrivate->nativeDataPriv ), mDisplayDriver->mDriverType );
    }

    DisplayVideoMode::~DisplayVideoMode()
    {
        dsmReleaseNativeData( &( mPrivate->nativeDataPriv ), mDisplayDriver->mDriverType );
    }


    DisplayDriver::DisplayDriver( DisplayManager * pDisplayManager,
                                  EDisplayDriverType pDriverType )
    : SysObject( pDisplayManager->mSysContext )
    , mDisplayManager( pDisplayManager )
    , mDriverType( pDriverType )
    , mPrivate( std::make_unique<ObjectPrivateData>( this ) )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {
        dsmInitializeNativeData( &( mPrivate->nativeDataPriv ), mDriverType );
    }

    DisplayDriver::~DisplayDriver()
    {
        dsmReleaseNativeData( &( mPrivate->nativeDataPriv ), mDriverType );
    }

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
        adapter.mPrivate->descPriv.driverType = mDriverType;
        adapter.mPrivate->descPriv.adapterIndex = static_cast<dsm_index_t>( adapterIndex );

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
        output.mPrivate->descPriv.driverType = mDriverType;
        output.mPrivate->descPriv.outputIndex = outputIDGen.uOutputIndex;
        output.mPrivate->descPriv.outputID = outputIDGen.outputID;

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
        videoMode.mPrivate->descPriv.driverType = mDriverType;
        videoMode.mPrivate->descPriv.videoModeIndex = videoModeIDGen.uModeIndex;
        videoMode.mPrivate->descPriv.videoModeID = videoModeIDGen.modeID;
        videoMode.mPrivate->descPriv.colorFormat = colorFormatData.colorFormat;

        // Video modes are not added to the helper list at this point.
        // This is done as a post-process step later in DisplayDriver::_enumVideoModes().
        // Assertion added to prevent problems in case of refactoring.
        ts3DebugAssert( colorFormatData.videoModeList.empty() );

        return &videoMode;
    }

    void DisplayDriver::_initializeDisplayConfiguration()
    {
        _enumAdapters();
        for( auto & adapter : mPrivate->adapterInternalStorage )
        {
            _enumOutputs( adapter );
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

    void DisplayDriver::_enumAdapters()
    {
        ts3DebugAssert( mPrivate->adapterInternalStorage.empty() );

        _nativeEnumAdapters();

        if( !mPrivate->adapterInternalStorage.empty() )
        {
            mPrivate->adapterList.reserve( mPrivate->adapterInternalStorage.size() );
            for( auto & adapter : mPrivate->adapterInternalStorage )
            {
                if( adapter.isActiveAdapter() )
                {
                    mPrivate->activeAdaptersNum += 1;
                }
                if( adapter.isPrimaryAdapter() )
                {
                    mPrivate->primaryAdapter = &adapter;
                }
                mPrivate->adapterList.push_back( &adapter );
            }

            if( !mPrivate->primaryAdapter )
            {
                auto & firstAdapter = mPrivate->adapterInternalStorage[0];
                firstAdapter.mPrivate->descPriv.flags.set( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );
                mPrivate->primaryAdapter = &firstAdapter;
            }
        }
    }

    void DisplayDriver::_enumOutputs( DisplayAdapter & pAdapter )
    {
        ts3DebugAssert( pAdapter.mPrivate->outputInternalStorage.empty() );

        _nativeEnumOutputs( pAdapter );

        if( !pAdapter.mPrivate->outputInternalStorage.empty() )
        {
            pAdapter.mPrivate->outputList.reserve( pAdapter.mPrivate->outputInternalStorage.size() );
            for( auto & output : pAdapter.mPrivate->outputInternalStorage )
            {
                if( output.isActiveOutput() )
                {
                    pAdapter.mPrivate->activeOutputsNum += 1;
                }
                if( output.isPrimaryOutput() )
                {
                    pAdapter.mPrivate->primaryOutput = &output;
                }
                pAdapter.mPrivate->outputList.push_back( &output );
            }

            if( !pAdapter.mPrivate->primaryOutput )
            {
                auto & firstOutput = pAdapter.mPrivate->outputInternalStorage[0];
                firstOutput.mPrivate->descPriv.flags.set( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT );
                pAdapter.mPrivate->primaryOutput = &firstOutput;
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


}
