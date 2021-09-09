
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
        // Enum all display devices (adapters and outputs). This process is combined,
        // because some drivers (like GDI, for example) may perform better if this is
        // done atomically. After this is done, lists of adapters and their outputs
        // are populated and all configuration/properties are valid and available.
        _enumDisplayDevices();

        // Now, enumerate supported display modes for all outputs.
        for( auto & adapter : mPrivate->adapterInternalStorage )
        {
            for( auto & output : adapter.mPrivate->outputInternalStorage )
            {
                // Each driver can provide a list of supported color formats.
                // The default set (sColorFormatArray) may not be available
                // on every platform (especially on mobile).
                auto colorFormatList = getSupportedColorFormatList();

                for( auto colorFormat : colorFormatList )
                {
                    // Enum all supported display modes for the current output and color format
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

        // "Post-processing" part - in order to avoid duplicate
        // common aspects of the enumeration process, some steps
        // are done here, after the driver finishes enumeration.

        if( !mPrivate->adapterInternalStorage.empty() )
        {
            // Reserve space for the list of pointers/handles for adapters.
            mPrivate->adapterList.reserve( mPrivate->adapterInternalStorage.size() );

            for( auto & adapter : mPrivate->adapterInternalStorage )
            {
                // Update the non-driver-specific part of the adapter info
                auto & adapterDesc = dsmGetObjectDesc( adapter );

                if( adapterDesc.vendorID == EDisplayAdapterVendorID::Unknown )
                {
                    // Driver can set the vendor ID internally, but in case it is missing,
                    // this function tries to resolve the ID by looking at the adapter desc.
                    adapterDesc.vendorID = dsmResolveAdapterVendorID( adapterDesc.name );
                }
                if( adapter.isPrimaryAdapter() && !mPrivate->primaryAdapter )
                {
                    // Driver can also explicitly set the primary system adapter.
                    // If it has not been set, we use the first adapter with proper flag set.
                    mPrivate->primaryAdapter = &adapter;
                }
                if( adapter.isActiveAdapter() )
                {
                    mPrivate->activeAdaptersNum += 1;
                }

                mPrivate->adapterList.push_back( &adapter );

                // If the current adapter has any outputs listed, go through them as well
                // and update the common part of their info just like with adapters above.
                if( !adapter.mPrivate->outputInternalStorage.empty() )
                {
                    // Reserve space for the list of pointers/handles for outputs.
                    adapter.mPrivate->outputList.reserve( adapter.mPrivate->outputInternalStorage.size() );

                    for( auto & output : adapter.mPrivate->outputInternalStorage )
                    {
                        // Update the non-driver-specific part of the output info
                        auto & outputDesc = dsmGetObjectDesc( output );

                        if( output.isPrimaryOutput() && adapter.mPrivate->primaryOutput )
                        {
                            // Similar to the default/primary system adapter, we select default
                            // output of an adapter if the driver has not set it during enumeration.
                            adapter.mPrivate->primaryOutput = &output;
                        }
                        if( output.isActiveOutput() )
                        {
                            adapter.mPrivate->activeOutputsNum += 1;
                        }

                        adapter.mPrivate->outputList.push_back( &output );
                    }

                    // Validate if the default output for this adapter has been properly set.
                    _enumDisplayDevicesCheckDefaultOutput( adapter );
                }
            }
            // Validate if the default system adapter has been properly set.
            _enumDisplayDevicesCheckDefaultAdapter();
        }
    }

    void DisplayDriver::_enumDisplayDevicesCheckDefaultAdapter()
    {
        if( mPrivate->primaryAdapter )
        {
            // Default./primary adapter will usually have the proper bit set (all drivers should do that).
            // In case the adapter has been set, but this bit is missing, emit a warning. It may be an
            // intentional choice, but also an error or missing driver-specific init code.

            auto & adapterDesc = dsmGetObjectDesc( *( mPrivate->primaryAdapter ) );
            if( !adapterDesc.flags.isSet( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT ) )
            {
                ts3DebugOutput(
                    "Primary/Default adapter selected by the driver does not have "\
                    "E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT set. Is that intentional?" );
            }
        }
        else
        {
            // If there is no default adapter set, it means it has not been done by the driver AND
            // there has not been any adapter marked as PRIMARY. In this case, just select the first
            // one, update its state and set as the default one.

            auto & firstAdapter = mPrivate->adapterInternalStorage.front();
            firstAdapter.mPrivate->descPriv.flags.set( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );
            mPrivate->primaryAdapter = &firstAdapter;
        }
    }

    void DisplayDriver::_enumDisplayDevicesCheckDefaultOutput( DisplayAdapter & pAdapter )
    {
        if( pAdapter.mPrivate->primaryOutput )
        {
            // Just like the above check for default adapter, we check the state of the default output.

            auto & outputDesc = dsmGetObjectDesc( *( pAdapter.mPrivate->primaryOutput ) );
            if( !outputDesc.flags.isSet( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT ) )
            {
                auto & adapterDesc = dsmGetObjectDesc( pAdapter );
                ts3DebugOutputFmt(
                        "Primary/Default output of [%s] selected by the driver does not have "\
                        "E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT set. Is that intentional?",
                        adapterDesc.name.c_str() );
            }
        }
        else
        {
            // Same here. If nothing has been set, pick the first output and make it the default one.

            auto & firstOutput = pAdapter.mPrivate->outputInternalStorage.front();
            firstOutput.mPrivate->descPriv.flags.set( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT );
            pAdapter.mPrivate->primaryOutput = &firstOutput;
        }
    }

    void DisplayDriver::_enumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat )
    {
        // There is no "update" option for the display topology. Each time the configuration changes,
        // we do a full reset of the whole cached state and re-initialize/enumerate everything. Thus,
        // the list of display modes should be always empty.

        auto & colorFormatData = pOutput.mPrivate->getColorFormatData( pColorFormat );
        ts3DebugAssert( colorFormatData.videoModeInternalStorage.empty() );

        // Driver-specific enumeration.
        _nativeEnumVideoModes( pOutput, colorFormatData.colorFormat );

        if( !colorFormatData.videoModeInternalStorage.empty() )
        {
            // Reserve the space for list of handles/pointers to available video modes.
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
        auto adapterIndex = dsmExtractOutputIDAdapterIndex( pOutputID );
        auto outputIndex = dsmExtractOutputIDOutputIndex( pOutputID );

        auto * adapter = _getAdapter( adapterIndex );

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
