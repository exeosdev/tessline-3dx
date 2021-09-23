
#include "displayNative.h"
#include "internal/private/displayInternal.h"

namespace ts3::system
{

    DisplayManager::DisplayManager( SysContextHandle pSysContext )
    : SysObject( std::move( pSysContext ) )
    , mInternal( std::make_unique<ObjectInternalData>() )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    {
        _nativeCtor();

        mInternal->driverFactoryMap[EDisplayDriverType::Generic] = [this]() {
            return createSysObject<DisplayDriverGeneric>( this );
        };
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
        mInternal->driverFactoryMap[EDisplayDriverType::DXGI] = [this]() {
            return createSysObject<DisplayDriverDXGI>( this );
        };
    #endif
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
        mInternal->driverFactoryMap[EDisplayDriverType::SDL] = [this]() {
            return createSysObject<DisplayDriverSDL>( this );
        };
    #endif
    }

    DisplayManager::~DisplayManager() noexcept
    {
        _nativeDtor();
    }

    DisplayDriverHandle DisplayManager::createDisplayDriver( EDisplayDriverType pDriverID )
    {
        pDriverID = resolveDisplayDriverID( pDriverID );
        if( pDriverID == EDisplayDriverType::Unknown )
        {
            return nullptr;
        }

        auto & factoryCallback = mInternal->driverFactoryMap.at( pDriverID );
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

    bool DisplayManager::checkWindowGeometry( const WindowGeometry & pWindowGeometry ) const
    {
        const auto & windowPos = pWindowGeometry.position;
        const auto & windowSize = pWindowGeometry.size;

        auto screenSize = queryDefaultDisplaySize();
        auto minWindowSize = queryMinWindowSize();

        if( windowSize != cvWindowSizeMax )
        {
            if( ( windowSize.x == 0 ) || ( windowSize.y == 0 ) )
            {
                return false;
            }

            if( ( windowSize.x > screenSize.x ) || ( windowSize.y > screenSize.y ) )
            {
                return false;
            }

            if( ( windowSize.x < minWindowSize.x ) || ( windowSize.y < minWindowSize.y ) )
            {
                return false;
            }
        }

        if( windowPos != cvWindowPositionAuto )
        {
            if( ( windowPos.x < 0 ) || ( windowPos.y < 0 ) )
            {
                return false;
            }

            auto maxPosX = static_cast<int32>( screenSize.x - windowSize.x );
            auto maxPosY = static_cast<int32>( screenSize.y - windowSize.y );
            if ( ( windowPos.x > maxPosX ) || ( windowPos.y > maxPosY ) )
            {
                return false;
            }
        }

        return true;
    }

    WindowGeometry DisplayManager::validateWindowGeometry( const WindowGeometry & pWindowGeometry ) const
    {
        const auto & windowPos = pWindowGeometry.position;
        const auto & windowSize = pWindowGeometry.size;

        auto resultGeometry = pWindowGeometry;

        auto screenSize = queryDefaultDisplaySize();
        auto minWindowSize = queryMinWindowSize();

        if( windowSize == cvWindowSizeMax )
        {
            // Window size exceeds the screen size - clamp the size.
            resultGeometry.size.x = screenSize.x;
            resultGeometry.size.y = screenSize.y;
        }
        else
        {
            if ( ( windowSize.x == 0 ) || ( windowSize.y == 0 ) )
            {
                // Any dimension set to 0 means "use default size". By default,
                // we just use the ratio of the screen and 70% of its dimensions.
                resultGeometry.size.x = static_cast<uint32>( screenSize.x * 0.7 );
                resultGeometry.size.y = static_cast<uint32>( screenSize.y * 0.7 );
            }
            else
            {
                // Size of the window must be less than the size of the screen...
                resultGeometry.size.x = getMinOf( resultGeometry.size.x, screenSize.x );
                resultGeometry.size.y = getMinOf( resultGeometry.size.y, screenSize.y );

                // ... but at the same time bigger than the minimum allowed size (Win32-specific, really).
                resultGeometry.size.x = getMaxOf( resultGeometry.size.x, minWindowSize.x );
                resultGeometry.size.y = getMaxOf( resultGeometry.size.y, minWindowSize.y );
            }
        }

        if ( ( windowPos.x < 0 ) || ( windowPos.y < 0 ) )
        {
            resultGeometry.position.x = static_cast<int32>( ( screenSize.x - windowSize.x ) / 2 );
            resultGeometry.position.y = static_cast<int32>( ( screenSize.y - windowSize.y ) / 2 );
        }
        else
        {
            auto maxPosX = static_cast<int32>( screenSize.x - windowSize.x );
            auto maxPosY = static_cast<int32>( screenSize.y - windowSize.y );
            resultGeometry.position.x = getMinOf( resultGeometry.position.x, maxPosX );
            resultGeometry.position.y = getMinOf( resultGeometry.position.y, maxPosY );
        }

        return resultGeometry;
    }


    DisplayDriver::DisplayDriver( DisplayManager * pDisplayManager,
                                  EDisplayDriverType pDriverType )
    : SysObject( pDisplayManager->mSysContext )
    , mDisplayManager( pDisplayManager )
    , mDriverType( pDriverType )
    , mInternal( std::make_unique<ObjectInternalData>( this ) )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    {}

    DisplayDriver::~DisplayDriver()
    {
        reset();
    }

    void DisplayDriver::syncDisplayConfiguration()
    {
        if( !mInternal->adapterInternalStorage.empty() )
        {
            _resetDisplayConfiguration();
        }
        _initializeDisplayConfiguration();
    }

    void DisplayDriver::reset()
    {
        _resetDisplayConfiguration();
    }

    ColorFormat DisplayDriver::queryDefaultSystemColorFormat() const
    {
        return _drvQueryDefaultSystemColorFormat();
    }

    const DisplayAdapterList & DisplayDriver::getAdapterList() const
    {
        return mInternal->adapterList;
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
            return mInternal->primaryAdapter;
        }
        else
        {
            return mInternal->adapterList.at( pAdapterIndex );
        }
    }

    DisplayAdapter * DisplayDriver::getDefaultAdapter() const
    {
        return mInternal->primaryAdapter;
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

    bool DisplayDriver::hasActiveAdapters() const
    {
        return mInternal->activeAdaptersNum > 0;
    }

    bool DisplayDriver::hasAnyAdapters() const
    {
        return !mInternal->adapterInternalStorage.empty();
    }

    bool DisplayDriver::hasValidConfiguration() const
    {
        return !mInternal->adapterInternalStorage.empty() && ( mInternal->combinedActiveOutputsNum > 0 );
    }

    std::string DisplayDriver::generateConfigurationDump( const std::string & pLinePrefix ) const
    {
        const auto adaptersNum = mInternal->adapterInternalStorage.size();
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

                        auto colorFormats = output->getSupportedColorFormatList();
                        for( auto colorFormat : colorFormats )
                        {
                            auto colorFormatStr = vsxQueryColorFormatStr( colorFormat );
                            result.append( 1, '\n' );
                            result.append( pLinePrefix );
                            result.append( 2, '\t' );
                            result.append( colorFormatStr );

                            const auto & videoModeList = output->getVideoModeList( colorFormat );
                            if( videoModeList.empty() )
                            {
                                result.append( 1, '\n' );
                                result.append( pLinePrefix );
                                result.append( 3, '\t' );
                                result.append( "<empty>" );
                            }
                            else
                            {
                                for( const auto * videoMode : videoModeList )
                                {
                                    result.append( 1, '\n' );
                                    result.append( pLinePrefix );
                                    result.append( 3, '\t' );
                                    result.append( videoMode->mDesc->toString() );
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
        const auto adapterIndex = mInternal->adapterInternalStorage.size();

        auto & adapter = mInternal->adapterInternalStorage.emplace_back( this );
        auto & adapterDesc = dsmGetObjectDesc( adapter );
        adapterDesc.driverType = mDriverType;
        adapterDesc.adapterIndex = static_cast<dsm_index_t>( adapterIndex );

        // Adapters are not added to the helper list at this point.
        // This is done as a post-process step later in DisplayDriver::_enumAdapters().
        // Assertion added to prevent problems in case of refactoring.
        ts3DebugAssert( mInternal->adapterList.empty() );

        return &adapter;
    }

    DisplayOutput * DisplayDriver::addOutput( DisplayAdapter & pAdapter )
    {
        const auto outputIndex = pAdapter.mInternal->outputInternalStorage.size();

        DisplayOutputIDGen outputIDGen;
        outputIDGen.uAdapterIndex = pAdapter.mInternal->descPriv.adapterIndex;
        outputIDGen.uOutputIndex = static_cast<dsm_index_t>( outputIndex );

        auto & output = pAdapter.mInternal->outputInternalStorage.emplace_back( &pAdapter );
        auto & outputDesc = dsmGetObjectDesc( output );
        outputDesc.driverType = mDriverType;
        outputDesc.outputIndex = outputIDGen.uOutputIndex;
        outputDesc.outputID = outputIDGen.outputID;

        // Outputs are not added to the helper list at this point.
        // This is done as a post-process step later in DisplayDriver::_enumOutputs().
        // Assertion added to prevent problems in case of refactoring.
        ts3DebugAssert( pAdapter.mInternal->outputList.empty() );

        return &output;
    }

    DisplayVideoMode * DisplayDriver::addVideoMode( DisplayOutput & pOutput, ColorFormat pColorFormat )
    {
        auto & colorFormatData = pOutput.mInternal->colorFormatMap[pColorFormat];

        if( colorFormatData.colorFormat == ColorFormat::Unknown )
        {
            colorFormatData.colorFormat = pColorFormat;
        }

        const auto videoModeIndex = colorFormatData.videoModeInternalStorage.size();

        DisplayVideoModeIDGen videoModeIDGen;
        videoModeIDGen.uOutputID = pOutput.mInternal->descPriv.outputID;
        videoModeIDGen.uColorFormatIndex = static_cast<dsm_index_t>( colorFormatData.colorFormat );
        videoModeIDGen.uModeIndex = static_cast<dsm_index_t>( videoModeIndex );

        auto & videoMode = colorFormatData.videoModeInternalStorage.emplace_back( &pOutput );
        auto & videoModeDesc = dsmGetObjectDesc( videoMode );
        videoModeDesc.driverType = mDriverType;
        videoModeDesc.videoModeIndex = videoModeIDGen.uModeIndex;
        videoModeDesc.videoModeID = videoModeIDGen.modeID;
        videoModeDesc.colorFormat = pColorFormat;

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

        _enumVideoModes();
    }

    void DisplayDriver::_resetDisplayConfiguration()
    {
        mInternal->adapterInternalStorage.clear();
        mInternal->adapterList.clear();
        mInternal->primaryAdapter = nullptr;
        mInternal->activeAdaptersNum = 0u;
        mInternal->combinedActiveOutputsNum = 0u;
    }

    void DisplayDriver::_enumDisplayDevices()
    {
        _drvEnumDisplayDevices();

        // "Post-processing" part - in order to avoid duplicate
        // common aspects of the enumeration process, some steps
        // are done here, after the driver finishes enumeration.

        if( !mInternal->adapterInternalStorage.empty() )
        {
            // Reserve space for the list of pointers/handles for adapters.
            mInternal->adapterList.reserve( mInternal->adapterInternalStorage.size() );

            for( auto & adapter : mInternal->adapterInternalStorage )
            {
                // Update the non-driver-specific part of the adapter info
                auto & adapterDesc = dsmGetObjectDesc( adapter );

                if( adapterDesc.vendorID == EDisplayAdapterVendorID::Unknown )
                {
                    // Driver can set the vendor ID internally, but in case it is missing,
                    // this function tries to resolve the ID by looking at the adapter desc.
                    adapterDesc.vendorID = dsmResolveAdapterVendorID( adapterDesc.name );
                }
                if( adapter.isPrimaryAdapter() && !mInternal->primaryAdapter )
                {
                    // Driver can also explicitly set the primary system adapter.
                    // If it has not been set, we use the first adapter with proper flag set.
                    mInternal->primaryAdapter = &adapter;
                }
                if( adapter.isActiveAdapter() )
                {
                    mInternal->activeAdaptersNum += 1;
                }

                mInternal->adapterList.push_back( &adapter );

                // If the current adapter has any outputs listed, go through them as well
                // and update the common part of their info just like with adapters above.
                if( !adapter.mInternal->outputInternalStorage.empty() )
                {
                    // Reserve space for the list of pointers/handles for outputs.
                    adapter.mInternal->outputList.reserve( adapter.mInternal->outputInternalStorage.size() );

                    for( auto & output : adapter.mInternal->outputInternalStorage )
                    {
                        if( output.isPrimaryOutput() && adapter.mInternal->primaryOutput )
                        {
                            // Similar to the default/primary system adapter, we select default
                            // output of an adapter if the driver has not set it during enumeration.
                            adapter.mInternal->primaryOutput = &output;
                        }
                        if( output.isActiveOutput() )
                        {
                            adapter.mInternal->activeOutputsNum += 1;
                            mInternal->combinedActiveOutputsNum += 1;
                        }

                        adapter.mInternal->outputList.push_back( &output );
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
        if( mInternal->primaryAdapter )
        {
            // Default./primary adapter will usually have the proper bit set (all drivers should do that).
            // In case the adapter has been set, but this bit is missing, emit a warning. It may be an
            // intentional choice, but also an error or missing driver-specific init code.

            auto & adapterDesc = dsmGetObjectDesc( *( mInternal->primaryAdapter ) );
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

            auto & firstAdapter = mInternal->adapterInternalStorage.front();
            firstAdapter.mInternal->descPriv.flags.set( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );
            mInternal->primaryAdapter = &firstAdapter;
        }
    }

    void DisplayDriver::_enumDisplayDevicesCheckDefaultOutput( DisplayAdapter & pAdapter )
    {
        if( pAdapter.mInternal->primaryOutput )
        {
            // Just like the above check for default adapter, we check the state of the default output.

            auto & outputDesc = dsmGetObjectDesc( *( pAdapter.mInternal->primaryOutput ) );
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

            auto & firstOutput = pAdapter.mInternal->outputInternalStorage.front();
            firstOutput.mInternal->descPriv.flags.set( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT );
            pAdapter.mInternal->primaryOutput = &firstOutput;
        }
    }

    void DisplayDriver::_enumVideoModes()
    {
        for( auto & adapter : mInternal->adapterInternalStorage )
        {
            for( auto & output : adapter.mInternal->outputInternalStorage )
            {
                output.mInternal->supportedColorFormatList.reserve( staticArraySize( cvColorFormatArray ) );

                for( auto colorFormat : cvColorFormatArray )
                {
                    _drvEnumVideoModes( output, colorFormat );

                    try
                    {
                        auto & colorFormatData = output.mInternal->colorFormatMap.at( colorFormat );
                        ts3DebugAssert( !colorFormatData.videoModeInternalStorage.empty() );
                        output.mInternal->supportedColorFormatList.push_back( colorFormat );
                        colorFormatData.videoModeList.reserve( colorFormatData.videoModeInternalStorage.size() );
                        for( auto & videoMode : colorFormatData.videoModeInternalStorage )
                        {
                            colorFormatData.videoModeList.push_back( &videoMode );
                        }
                    }
                    catch( ... )
                    {
                        continue;
                    }
                }
            }
        }
    }

    DisplayAdapter * DisplayDriver::_getAdapter( dsm_index_t pAdapterIndex ) const
    {
        if( pAdapterIndex == CX_DSM_INDEX_DEFAULT )
        {
            return mInternal->primaryAdapter;
        }
        else
        {
            return mInternal->adapterList.at( pAdapterIndex );
        }
    }

    DisplayOutput * DisplayDriver::_getOutput( dsm_output_id_t pOutputID ) const
    {
        auto adapterIndex = dsmExtractOutputIDAdapterIndex( pOutputID );
        auto outputIndex = dsmExtractOutputIDOutputIndex( pOutputID );

        auto * adapter = _getAdapter( adapterIndex );

        if( outputIndex == CX_DSM_INDEX_DEFAULT )
        {
            return adapter->mInternal->primaryOutput;
        }
        else
        {
            return adapter->mInternal->outputList.at( outputIndex );
        }
    }


    DisplayAdapter::DisplayAdapter( DisplayDriver * pDriver )
    : mDisplayDriver( pDriver )
    , mDriverType( pDriver->mDriverType )
    , mInternal( std::make_unique<ObjectInternalData>( this ) )
    , mDesc( &( mInternal->descPriv ) )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    {}

    DisplayAdapter::~DisplayAdapter() = default;

    const DisplayOutputList & DisplayAdapter::getOutputList() const
    {
        return mInternal->outputList;
    }

    DisplayOutput * DisplayAdapter::getOutput( dsm_index_t pOutputIndex ) const
    {
        if( pOutputIndex == CX_DSM_INDEX_DEFAULT )
        {
            return mInternal->primaryOutput;
        }
        else
        {
            return mInternal->outputList.at( pOutputIndex );
        }
    }

    DisplayOutput * DisplayAdapter::getDefaultOutput() const
    {
        return mInternal->primaryOutput;
    }

    bool DisplayAdapter::isActiveAdapter() const
    {
        return mInternal->descPriv.flags.isSet( E_DISPLAY_ADAPTER_FLAG_ACTIVE_BIT );
    }

    bool DisplayAdapter::isPrimaryAdapter() const
    {
        return mInternal->descPriv.flags.isSet( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );
    }

    bool DisplayAdapter::hasActiveOutputs() const
    {
        return mInternal->activeOutputsNum > 0;
    }

    bool DisplayAdapter::hasAnyOutputs() const
    {
        return !mInternal->outputInternalStorage.empty();
    }


    DisplayOutput::DisplayOutput( DisplayAdapter * pAdapter )
    : mDisplayDriver( pAdapter->mDisplayDriver )
    , mParentAdapter( pAdapter )
    , mDriverType( pAdapter->mDriverType )
    , mInternal( std::make_unique<ObjectInternalData>( this ) )
    , mDesc( &( mInternal->descPriv ) )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    {}

    DisplayOutput::~DisplayOutput() = default;

    ArrayView<const ColorFormat> DisplayOutput::getSupportedColorFormatList() const
    {
        return bindArrayView( mInternal->supportedColorFormatList.data(), mInternal->supportedColorFormatList.size() );
    }

    bool DisplayOutput::checkVideoSettingsSupport( const DisplayVideoSettings & pVideoSettings ) const
    {
        auto colorFormat = mDisplayDriver->queryDefaultSystemColorFormat();
        return checkVideoSettingsSupport( pVideoSettings, colorFormat );
    }

    bool DisplayOutput::checkVideoSettingsSupport( const DisplayVideoSettings & pVideoSettings, ColorFormat pColorFormat ) const
    {
        const auto & colorFormatData = mInternal->colorFormatMap.at( pColorFormat );
        for( const auto & displayMode : colorFormatData.videoModeInternalStorage )
        {
            if( displayMode.mDesc->settings.matches( pVideoSettings ) )
            {
                return true;
            }
        }
        return false;
    }

    const DisplayVideoModeList & DisplayOutput::getVideoModeList() const
    {
        auto colorFormat = mDisplayDriver->queryDefaultSystemColorFormat();
        return getVideoModeList( colorFormat );
    }

    const DisplayVideoModeList & DisplayOutput::getVideoModeList( ColorFormat pColorFormat ) const
    {
        const auto & colorFormatData = mInternal->colorFormatMap.at( pColorFormat );
        return colorFormatData.videoModeList;
    }

    bool DisplayOutput::isActiveOutput() const
    {
        return mInternal->descPriv.flags.isSet( E_DISPLAY_OUTPUT_FLAG_ACTIVE_BIT );
    }

    bool DisplayOutput::isPrimaryOutput() const
    {
        return mInternal->descPriv.flags.isSet( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT );
    }

    bool DisplayOutput::isColorFormatSupported( ColorFormat pColorFormat ) const
    {
        const auto & colorFormatData = mInternal->colorFormatMap.at( pColorFormat );
        return !colorFormatData.videoModeInternalStorage.empty();
    }


    DisplayVideoMode::DisplayVideoMode( DisplayOutput * pOutput )
    : mDisplayDriver( pOutput->mDisplayDriver )
    , mParentOutput( pOutput )
    , mDriverType( pOutput->mDriverType )
    , mInternal( std::make_unique<ObjectInternalData>( this ) )
    , mDesc( &( mInternal->descPriv ) )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    {}

    DisplayVideoMode::~DisplayVideoMode() = default;

}
