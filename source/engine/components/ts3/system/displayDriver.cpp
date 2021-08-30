
#include "displayDriverNative.h"
#include "displayManager.h"

namespace ts3::system
{


    DisplayAdapter::DisplayAdapter( DisplayDriver * pDriver )
    : mDisplayDriver( pDriver )
    , mPrivate( std::make_unique<ObjectPrivateData>() )
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

    bool DisplayAdapter::isActive() const
    {
        return !mPrivate->outputStorage.empty();
    }

    bool DisplayAdapter::isPrimaryAdapter() const
    {
        return mPrivate->descPriv.flags.isSet( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );
    }

    bool DisplayAdapter::hasActiveOutputs() const
    {
        return mPrivate->activeOutputsNum > 0;
    }


    DisplayOutput::DisplayOutput( DisplayAdapter * pAdapter )
    : mDisplayDriver( pAdapter->mDisplayDriver )
    , mParentAdapter( pAdapter )
    , mPrivate( std::make_unique<ObjectPrivateData>() )
    , mDesc( &( mPrivate->descPriv ) )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {
        dsmInitializeNativeData( &mNativeData, mDisplayDriver->mDriverType );
    }

    DisplayOutput::~DisplayOutput()
    {
        dsmReleaseNativeData( &mNativeData, mDisplayDriver->mDriverType );
    }

    const DisplayVideoModeList & DisplayOutput::getVideoModeList( ColorFormat pColorFormat ) const
    {
        auto colorFormat = dsmResolveSystemColorFormat( pColorFormat );
        auto & colorFormatData = mPrivate->colorFormatMap.at( colorFormat );
        return colorFormatData.videoModeList;
    }

    bool DisplayOutput::isActive() const
    {
        auto nativeColorFormat = dsmResolveSystemColorFormat( ColorFormat::SystemNative );
        auto & colorFormatData = mPrivate->colorFormatMap.at( nativeColorFormat );
        return !colorFormatData.videoModeStorage.empty();
    }

    bool DisplayOutput::isPrimaryOutput() const
    {
        return mPrivate->descPriv.flags.isSet( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT );
    }


    DisplayVideoMode::DisplayVideoMode( DisplayOutput * pOutput )
    : mDisplayDriver( pOutput->mDisplayDriver )
    , mParentOutput( pOutput )
    , mPrivate( std::make_unique<ObjectPrivateData>() )
    , mDesc( &( mPrivate->descPriv ) )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {
        dsmInitializeNativeData( &mNativeData, mDisplayDriver->mDriverType );
    }

    DisplayVideoMode::~DisplayVideoMode()
    {
        dsmReleaseNativeData( &mNativeData, mDisplayDriver->mDriverType );
    }


    DisplayDriver::DisplayDriver( DisplayManager * pDisplayManager,
                                  EDisplayDriverType pDriverType )
    : SysObject( pDisplayManager->mSysContext )
    , mDisplayManager( pDisplayManager )
    , mDriverType( pDriverType )
    , mPrivate( std::make_unique<ObjectPrivateData>() )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {
        dsmInitializeNativeData( &( mPrivate->nativeDataPriv ), mDriverType );
    }

    DisplayDriver::~DisplayDriver()
    {
        dsmReleaseNativeData( &mNativeData, mDriverType );
    }

    void DisplayDriver::initializeDisplayConfiguration()
    {
        if( !mPrivate->adapterStorage.empty() )
        {
            _resetDisplayConfiguration();
        }
        _initializeDisplayConfiguration();
    }

    void DisplayDriver::resetDisplayConfiguration()
    {
        _resetDisplayConfiguration();
    }

    void DisplayDriver::enumVideoModes( dsm_output_id_t pOutputID, ColorFormat pColorFormat )
    {
        auto * output = _getOutput( pOutputID );
        _resetVideoModeData( *output, pColorFormat );
        _enumVideoModes( *output, pColorFormat );
    }

    const DisplayAdapterList & DisplayDriver::getAdapterList() const
    {
        return mPrivate->adapterList;
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

    DisplayAdapter * DisplayDriver::addAdapter()
    {
        const auto adapterIndex = mPrivate->adapterStorage.size();

        auto & adapter = mPrivate->adapterStorage.emplace_back( this );
        adapter.mPrivate->descPriv.driverType = mDriverType;
        adapter.mPrivate->descPriv.adapterIndex = adapterIndex;

        ts3DebugAssert( mPrivate->adapterList.empty() );

        return &adapter;
    }

    DisplayOutput * DisplayDriver::addOutput( DisplayAdapter & pAdapter )
    {
        const auto outputIndex = pAdapter.mPrivate->outputStorage.size();

        DisplayOutputID outputIDGen;
        outputIDGen.uAdapterIndex = pAdapter.mPrivate->descPriv.adapterIndex;
        outputIDGen.uOutputIndex = static_cast<dsm_index_t>( outputIndex );

        auto & output = pAdapter.mPrivate->outputStorage.emplace_back( &pAdapter );
        output.mPrivate->descPriv.driverType = mDriverType;
        output.mPrivate->descPriv.outputIndex = outputIndex;
        output.mPrivate->descPriv.outputID = outputIDGen.outputID;

        ts3DebugAssert( pAdapter.mPrivate->outputList.empty() );

        return &output;
    }

    DisplayVideoMode * DisplayDriver::addVideoMode( DisplayOutput & pOutput, ColorFormat pColorFormat )
    {
        auto & colorFormatData = pOutput.mPrivate->colorFormatMap[pColorFormat];

        const auto videoModeIndex = colorFormatData.videoModeStorage.size();

        DisplayVideoModeID videoModeIDGen;
        videoModeIDGen.uOutputID = pOutput.mPrivate->descPriv.outputID;
        videoModeIDGen.uColorFormatIndex = static_cast<dsm_index_t>( pColorFormat );
        videoModeIDGen.uModeIndex = static_cast<dsm_index_t>( videoModeIndex );

        auto & videoMode = colorFormatData.videoModeStorage.emplace_back( &pOutput );
        videoMode.mPrivate->descPriv.driverType = mDriverType;
        videoMode.mPrivate->descPriv.videoModeIndex = videoModeIndex;
        videoMode.mPrivate->descPriv.videoModeID = videoModeIDGen.modeID;
        videoMode.mPrivate->descPriv.colorFormat = pColorFormat;

        ts3DebugAssert( colorFormatData.videoModeList.empty() );

        return &videoMode;
    }

    void DisplayDriver::_initializeDisplayConfiguration()
    {
        _enumAdapters();
        for( auto & adapter : mPrivate->adapterStorage )
        {
            _enumOutputs( adapter );
            for( auto & output : adapter.mPrivate->outputStorage )
            {
                _enumVideoModes( output, ColorFormat::SystemNative );
            }
        }
    }

    void DisplayDriver::_resetDisplayConfiguration()
    {
        for( auto & adapter : mPrivate->adapterStorage )
        {
            for( auto & output : adapter.mPrivate->outputStorage )
            {
                for( auto & colorFormatData : output.mPrivate->colorFormatMap )
                {
                    for( auto & videoMode : colorFormatData.second.videoModeStorage )
                    {
                        _nativeDestroyVideoMode( videoMode );
                    }
                    colorFormatData.second.videoModeStorage.clear();
                    colorFormatData.second.videoModeList.clear();
                }
                output.mPrivate->colorFormatMap.clear();
            }
            adapter.mPrivate->outputStorage.clear();
        }
        mPrivate->adapterStorage.clear();
    }

    void DisplayDriver::_resetVideoModeData( DisplayOutput & pOutput, ColorFormat pColorFormat )
    {
        auto colorFormat = dsmResolveSystemColorFormat( pColorFormat );
        auto & colorFormatData = pOutput.mPrivate->colorFormatMap[colorFormat];

        for( auto & videoMode : colorFormatData.videoModeStorage )
        {
            _nativeDestroyVideoMode( videoMode );
        }

        colorFormatData.videoModeList.clear();
        colorFormatData.videoModeStorage.clear();
    }

    void DisplayDriver::_enumAdapters()
    {
        ts3DebugAssert( mPrivate->adapterStorage.empty() );

        _nativeEnumAdapters();

        if( !mPrivate->adapterStorage.empty() )
        {
            mPrivate->adapterList.reserve( mPrivate->adapterStorage.size() );
            for( auto & adapter : mPrivate->adapterStorage )
            {
                if( adapter.isActive() )
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
                auto & firstAdapter = mPrivate->adapterStorage[0];
                firstAdapter.mPrivate->descPriv.flags.set( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );
                mPrivate->primaryAdapter = &firstAdapter;
            }
        }
    }

    void DisplayDriver::_enumOutputs( DisplayAdapter & pAdapter )
    {
        ts3DebugAssert( pAdapter.mPrivate->outputStorage.empty() );

        _nativeEnumOutputs( pAdapter );

        if( !pAdapter.mPrivate->outputStorage.empty() )
        {
            pAdapter.mPrivate->outputList.reserve( pAdapter.mPrivate->outputStorage.size() );
            for( auto & output : pAdapter.mPrivate->outputStorage )
            {
                if( output.isActive() )
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
                auto & firstOutput = pAdapter.mPrivate->outputStorage[0];
                firstOutput.mPrivate->descPriv.flags.set( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT );
                pAdapter.mPrivate->primaryOutput = &firstOutput;
            }
        }

    }

    void DisplayDriver::_enumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat )
    {
        auto & colorFormatData = pOutput.mPrivate->colorFormatMap[pColorFormat];
        ts3DebugAssert( colorFormatData.videoModeStorage.empty() );

        _nativeEnumVideoModes( pOutput, pColorFormat );

        if( !colorFormatData.videoModeStorage.empty() )
        {
            colorFormatData.videoModeList.reserve( colorFormatData.videoModeStorage.size() );
            for( auto & videoMode : colorFormatData.videoModeStorage )
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
        auto outputIndex = dsmExtractOutputIDOutputrIndex( pOutputID );

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
