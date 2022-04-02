
#include "internal/displaySystemPrivate.h"

namespace ts3::system
{

    DisplayAdapter::DisplayAdapter( DisplayDriver & pDriver )
    : SysObject( pDriver.mSysContext )
    , mDriverType( pDriver.mDriverType )
    , mDisplayDriver( &pDriver )
    , _privateData( std::make_unique<DisplayAdapterPrivateData>() )
    {}

    DisplayAdapter::~DisplayAdapter() noexcept = default;

    DisplayOutput * DisplayAdapter::findOutput( DisplayOutputPredicate pPredicate ) const
    {
        for( auto & outputPtr : _privateData->outputInstanceList )
        {
            if( pPredicate( *outputPtr ) )
            {
                return outputPtr.get();
            }
        }
        return nullptr;
    }

    DisplayOutputList DisplayAdapter::findOutputs( DisplayOutputPredicate pPredicate ) const
    {
        DisplayOutputList outputList;
        for( auto & outputPtr : _privateData->outputInstanceList )
        {
            if( pPredicate( *outputPtr ) )
            {
                outputList.push_back( outputPtr.get() );
            }
        }
        return outputList;
    }

    DisplayOutput * DisplayAdapter::getOutput( dsm_index_t pOutputIndex ) const
    {
        if( pOutputIndex == CX_DSM_INDEX_DEFAULT )
        {
            return _privateData->primaryOutput;
        }
        else
        {
            return _privateData->outputList.at( pOutputIndex );
        }
    }

    DisplayOutput * DisplayAdapter::getDefaultOutput() const
    {
        return _privateData->primaryOutput;
    }

    const DisplayAdapterDesc & DisplayAdapter::getAdapterDesc() const
    {
        return _privateData->adapterDesc;
    }

    const DisplayOutputList & DisplayAdapter::getOutputList() const
    {
        return _privateData->outputList;
    }

    bool DisplayAdapter::isActiveAdapter() const
    {
        return _privateData->adapterDesc.flags.isSet( E_DISPLAY_ADAPTER_FLAG_ACTIVE_BIT );
    }

    bool DisplayAdapter::isPrimaryAdapter() const
    {
        return _privateData->adapterDesc.flags.isSet( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );
    }

    bool DisplayAdapter::hasActiveOutputs() const
    {
        return _privateData->activeOutputsNum > 0;
    }

    bool DisplayAdapter::hasAnyOutputs() const
    {
        return !_privateData->outputInstanceList.empty();
    }

    void DisplayAdapter::registerOutput( DisplayOutputHandle pOutput )
    {
        const auto outputIndex = _privateData->outputInstanceList.size();

        DisplayOutputIDGen outputIDGen;
        outputIDGen.uAdapterIndex = _privateData->adapterDesc.adapterIndex;
        outputIDGen.uOutputIndex = static_cast<dsm_index_t>( outputIndex );

        auto & outputDesc = pOutput->getOutputDescInternal();
        outputDesc.driverType = mDriverType;
        outputDesc.outputIndex = outputIDGen.uOutputIndex;
        outputDesc.outputID = outputIDGen.outputID;

        _privateData->outputInstanceList.push_back( std::move( pOutput ) );

        // Outputs are not added to the helper list at this point.
        // This is done as a post-process step later in DisplayDriver::_enumOutputs().
        // Assertion added to prevent problems in case of refactoring.
        ts3DebugAssert( _privateData->outputList.empty() );
    }

    uint32 DisplayAdapter::validateOutputsConfiguration()
    {
        if( !_privateData->outputInstanceList.empty() )
        {
            // Reserve space for the list of pointers/handles for outputs.
            _privateData->outputList.reserve( _privateData->outputInstanceList.size() );

            for( auto & outputPtr : _privateData->outputInstanceList )
            {
                if( outputPtr->isPrimaryOutput() && !_privateData->primaryOutput )
                {
                    // Similar to the default/primary system adapter, we select default
                    // output of an adapter if the driver has not set it during enumeration.
                    _privateData->primaryOutput = outputPtr.get();
                }

                if( outputPtr->isActiveOutput() )
                {
                    _privateData->activeOutputsNum += 1;
                }

                for( auto colorFormat : cvColorFormatArray )
                {
                    outputPtr->validateVideoModesConfiguration( colorFormat );
                }

                _privateData->outputList.push_back( outputPtr.get() );
            }

            // Validate if the default output for this adapter has been properly set.
            if( _privateData->primaryOutput )
            {
                auto & outputDesc = _privateData->primaryOutput->getOutputDescInternal();
                if( !outputDesc.flags.isSet( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT ) )
                {
                    ts3DebugOutputFmt(
                        "Primary/Default output of [%s] selected by the driver does not have "\
                        "E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT set. Is that intentional?",
                        _privateData->adapterDesc.name.c_str() );
                }
            }
            else
            {
                auto & firstOutput = _privateData->outputInstanceList.front();
                auto & firstOutputDesc = firstOutput->getOutputDescInternal();
                firstOutputDesc.flags.set( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT );
                _privateData->primaryOutput = firstOutput.get();
            }
        }

        return _privateData->activeOutputsNum;
    }

    DisplayAdapterDesc & DisplayAdapter::getAdapterDescInternal()
    {
        return _privateData->adapterDesc;
    }


    DisplayOutput::DisplayOutput( DisplayAdapter & pAdapter )
    : SysObject( pAdapter.mSysContext )
    , mDriverType( pAdapter.mDriverType )
    , mDisplayDriver( pAdapter.mDisplayDriver )
    , mParentAdapter( &pAdapter )
    , _privateData( std::make_unique<DisplayOutputPrivateData>() )
    {}

    DisplayOutput::~DisplayOutput() noexcept = default;

    ArrayView<const EColorFormat> DisplayOutput::getSupportedColorFormatList() const
    {
        return bindArrayView( _privateData->supportedColorFormatList.data(),
                              _privateData->supportedColorFormatList.size() );
    }

    bool DisplayOutput::checkVideoSettingsSupport( const DisplayVideoSettings & pVideoSettings, EColorFormat pColorFormat ) const
    {
        const auto & colorFormatData = _privateData->colorFormatMap.at( pColorFormat );
        for( const auto & videoMode : colorFormatData.videoModeInstanceList )
        {
            const auto & videoModeDesc = videoMode->getModeDesc();
            if( videoModeDesc.settings.matches( pVideoSettings ) )
            {
                return true;
            }
        }
        return false;
    }

    DisplayVideoMode * DisplayOutput::findVideoMode( EColorFormat pColorFormat, DisplayVideoModePredicate pPredicate ) const
    {
        for( auto & colorFormatData : _privateData->colorFormatMap )
        {
            if( ( colorFormatData.first == pColorFormat ) || ( pColorFormat == EColorFormat::Unknown ) )
            {
                for( auto & videoModePtr : colorFormatData.second.videoModeInstanceList )
                {
                    if( pPredicate( *videoModePtr ) )
                    {
                        return videoModePtr.get();
                    }
                }
            }
        }
        return nullptr;
    }

    DisplayVideoModeList DisplayOutput::findVideoModes( EColorFormat pColorFormat, DisplayVideoModePredicate pPredicate ) const
    {
        DisplayVideoModeList videoModeList;
        for( auto & colorFormatData : _privateData->colorFormatMap )
        {
            if( ( colorFormatData.first == pColorFormat ) || ( pColorFormat == EColorFormat::Unknown ) )
            {
                for( auto & videoModePtr : colorFormatData.second.videoModeInstanceList )
                {
                    if( pPredicate( *videoModePtr ) )
                    {
                        videoModeList.push_back( videoModePtr.get() );
                    }
                }
            }
        }
        return videoModeList;
    }

    const DisplayOutputDesc & DisplayOutput::getOutputDesc() const
    {
        return _privateData->outputDesc;
    }

    const DisplayVideoModeList & DisplayOutput::getVideoModeList() const
    {
        auto colorFormat = mDisplayDriver->queryDefaultSystemColorFormat();
        return getVideoModeList( colorFormat );
    }

    const DisplayVideoModeList & DisplayOutput::getVideoModeList( EColorFormat pColorFormat ) const
    {
        const auto & colorFormatData = _privateData->colorFormatMap.at( pColorFormat );
        return colorFormatData.videoModeList;
    }

    bool DisplayOutput::isActiveOutput() const
    {
        return _privateData->outputDesc.flags.isSet( E_DISPLAY_OUTPUT_FLAG_ACTIVE_BIT );
    }

    bool DisplayOutput::isPrimaryOutput() const
    {
        return _privateData->outputDesc.flags.isSet( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT );
    }

    bool DisplayOutput::isColorFormatSupported( EColorFormat pColorFormat ) const
    {
        const auto & colorFormatData = _privateData->colorFormatMap.at( pColorFormat );
        return !colorFormatData.videoModeInstanceList.empty();
    }

    void DisplayOutput::registerVideoMode( EColorFormat pColorFormat, DisplayVideoModeHandle pVideoMode )
    {
        auto & colorFormatData = _privateData->colorFormatMap[pColorFormat];

        if( colorFormatData.colorFormat == EColorFormat::Unknown )
        {
            colorFormatData.colorFormat = pColorFormat;
            _privateData->supportedColorFormatList.push_back( pColorFormat );
        }

        const auto videoModeIndex = colorFormatData.videoModeInstanceList.size();

        DisplayVideoModeIDGen videoModeIDGen;
        videoModeIDGen.uOutputID = _privateData->outputDesc.outputID;
        videoModeIDGen.uColorFormatIndex = static_cast<dsm_index_t>( colorFormatData.colorFormat );
        videoModeIDGen.uModeIndex = static_cast<dsm_index_t>( videoModeIndex );

        auto & videoModeDesc = pVideoMode->getModeDescInternal();
        videoModeDesc.driverType = mDriverType;
        videoModeDesc.videoModeIndex = videoModeIDGen.uModeIndex;
        videoModeDesc.videoModeID = videoModeIDGen.modeID;
        videoModeDesc.colorFormat = pColorFormat;

        colorFormatData.videoModeInstanceList.push_back( pVideoMode );

        // Video modes are not added to the helper list at this point.
        // This is done as a post-process step later in DisplayDriver::_enumVideoModes().
        // Assertion added to prevent problems in case of refactoring.
        ts3DebugAssert( colorFormatData.videoModeList.empty() );
    }

    uint32 DisplayOutput::validateVideoModesConfiguration( EColorFormat pColorFormat )
    {
        auto & colorFormatData = _privateData->colorFormatMap.at( pColorFormat );
        auto videoModesNum = colorFormatData.videoModeInstanceList.size();

        if( videoModesNum > 0 )
        {
            colorFormatData.videoModeList.reserve( videoModesNum );

            for( auto & videoModePtr : colorFormatData.videoModeInstanceList )
            {
                colorFormatData.videoModeList.push_back( videoModePtr.get() );
            }
        }

        return static_cast<uint32>( videoModesNum );
    }

    DisplayOutputDesc & DisplayOutput::getOutputDescInternal()
    {
        return _privateData->outputDesc;
    }


    DisplayVideoMode::DisplayVideoMode( DisplayOutput & pOutput )
    : SysObject( pOutput.mSysContext )
    , mDriverType( pOutput.mDriverType )
    , mDisplayDriver( pOutput.mDisplayDriver )
    , mParentOutput( &pOutput )
    , _privateData( std::make_unique<DisplayVideoModePrivateData>() )
    {}

    DisplayVideoMode::~DisplayVideoMode() noexcept = default;

    const DisplayVideoModeDesc & DisplayVideoMode::getModeDesc() const
    {
        return _privateData->modeDesc;
    }

    DisplayVideoModeDesc & DisplayVideoMode::getModeDescInternal()
    {
        return _privateData->modeDesc;
    }

} // namespace ts3::system
