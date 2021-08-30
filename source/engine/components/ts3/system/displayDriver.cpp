
#include "displayDriverNative.h"
#include "displayManager.h"

namespace ts3::system
{

    DisplayDriver::DisplayDriver( DisplayManager * pDisplayManager, EDisplayDriverType pDriverType )
    : SysObject( pDisplayManager->mSysContext )
    , mDisplayManager( pDisplayManager )
    , mDriverType( pDriverType )
    , mPrivateData( std::make_unique<DriverPrivateData>() )
    , mNativeData( &( mPrivateData->nativeDataPriv ) )
    {
        dsmInitializeNativeData( &( mPrivateData->nativeDataPriv ), mDriverType );
    }

    DisplayDriver::~DisplayDriver()
    {
        dsmReleaseNativeData( &( mPrivateData->nativeDataPriv ), mDriverType );
    }

    void DisplayDriver::initialize()
    {
        _nativeInitialize();
    }

    void DisplayDriver::release()
    {
        _nativeRelease();
    }

    void DisplayDriver::resetDisplayConfiguration()
    {
        _nativeResetDisplayConfiguration();
    }

    void DisplayDriver::syncDisplayConfiguration()
    {
        _nativeSyncDisplayConfiguration();
    }

    const DisplayAdapterList & DisplayDriver::getAdapterList() const
    {
        return mPrivateData->adapterRefIndex;
    }

    const DisplayOutputList & DisplayDriver::getOutputList( dsm_index_t pAdapterIndex ) const
    {
        auto * adapter = getAdapter( pAdapterIndex );
        return adapter->getOutputList();
    }

    DisplayAdapter * DisplayDriver::getAdapter( dsm_index_t pAdapterIndex ) const
    {
        if( pAdapterIndex == CX_DSM_INDEX_INVALID )
        {
            return mPrivateData->primaryAdapter;
        }
        else
        {
            auto * adapter = mPrivateData->adapterRefIndex.at( pAdapterIndex );
            return adapter;
        }
    }

    DisplayAdapter * DisplayDriver::getDefaultAdapter() const
    {
        return mPrivateData->primaryAdapter;
    }

    DisplayOutput * DisplayDriver::getDefaultOutput( dsm_index_t pAdapterIndex ) const
    {
        auto * adapter = getAdapter( pAdapterIndex );
        return adapter->getDefaultOutput();
    }

    DisplayAdapter * DisplayDriver::registerAdapter()
    {
        const auto adapterIndex = mPrivateData->adapterList.size();

        auto adapter = createSysObject<DisplayAdapter>( this );
        mPrivateData->adapterList.push_back( adapter );
        adapter->mPrivateData->descPriv.driverType = mDriverType;
        adapter->mPrivateData->descPriv.adapterIndex = adapterIndex;

        mPrivateData->adapterRefIndex.push_back( adapter.get() );
        ts3DebugAssert( mPrivateData->adapterList.size() == mPrivateData->adapterRefIndex.size() );

        return adapter.get();
    }

    DisplayOutput * DisplayDriver::registerOutput( DisplayAdapter & pAdapter )
    {
        const auto outputIndex = pAdapter.mPrivateData->outputList.size();

        DisplayOutputID outputIDGen;
        outputIDGen.uAdapterIndex = pAdapter.mPrivateData->descPriv.adapterIndex;
        outputIDGen.uOutputIndex = static_cast<dsm_index_t>( outputIndex );

        auto output = createSysObject<DisplayOutput>( &pAdapter );
        pAdapter.mPrivateData->outputList.push_back( output );
        output->mPrivateData->descPriv.driverType = mDriverType;
        output->mPrivateData->descPriv.outputIndex = outputIndex;
        output->mPrivateData->descPriv.outputID = outputIDGen.outputID;

        pAdapter.mPrivateData->outputRefIndex.push_back( output.get() );
        ts3DebugAssert( pAdapter.mPrivateData->outputList.size() == pAdapter.mPrivateData->outputRefIndex.size() );

        return output.get();
    }

    DisplayVideoMode * DisplayDriver::registerVideoMode( DisplayOutput & pOutput, ColorFormat pColorFormat )
    {
        auto & colorFormatData = pOutput.mPrivateData->colorFormatMap[pColorFormat];

        const auto videoModeIndex = colorFormatData.videoModeList.size();

        DisplayVideoModeID videoModeIDGen;
        videoModeIDGen.uOutputID = pOutput.mPrivateData->descPriv.outputID;
        videoModeIDGen.uColorFormatIndex = static_cast<dsm_index_t>( pColorFormat );
        videoModeIDGen.uModeIndex = static_cast<dsm_index_t>( videoModeIndex );

        auto videoMode = createSysObject<DisplayVideoMode>( &pOutput );
        colorFormatData.videoModeList.push_back( videoMode );
        videoMode->mPrivateData->descPriv.driverType = mDriverType;
        videoMode->mPrivateData->descPriv.videoModeIndex = videoModeIndex;
        videoMode->mPrivateData->descPriv.videoModeID = videoModeIDGen.modeID;
        videoMode->mPrivateData->descPriv.colorFormat = pColorFormat;

        colorFormatData.videoModeRefIndex.push_back( videoMode.get() );
        ts3DebugAssert( colorFormatData.videoModeList.size() == colorFormatData.videoModeRefIndex.size() );

        return videoMode.get();
    }


    DisplayAdapter::DisplayAdapter( DisplayDriver * pDriver )
    : SysObject( pDriver->mSysContext )
    , mDisplayDriver( pDriver )
    , mPrivateData( std::make_unique<AdapterPrivateData>() )
    , mDesc( &( mPrivateData->descPriv ) )
    , mNativeData( &( mPrivateData->nativeDataPriv ) )
    {
        dsmInitializeNativeData( &( mPrivateData->nativeDataPriv ), mDisplayDriver->mDriverType );
    }

    DisplayAdapter::~DisplayAdapter()
    {
        dsmReleaseNativeData( &( mPrivateData->nativeDataPriv ), mDisplayDriver->mDriverType );
    }


    DisplayOutput::DisplayOutput( DisplayAdapter * pAdapter )
    : SysObject( pAdapter->mSysContext )
    , mDisplayDriver( pAdapter->mDisplayDriver )
    , mParentAdapter( pAdapter )
    , mPrivateData( std::make_unique<OutputPrivateData>() )
    , mDesc( &( mPrivateData->descPriv ) )
    , mNativeData( &( mPrivateData->nativeDataPriv ) )
    {
        dsmInitializeNativeData( &mNativeData, mDisplayDriver->mDriverType );
    }

    DisplayOutput::~DisplayOutput()
    {
        dsmReleaseNativeData( &mNativeData, mDisplayDriver->mDriverType );
    }


    DisplayVideoMode::DisplayVideoMode( DisplayOutput * pOutput )
    : SysObject( pOutput->mSysContext )
    , mDisplayDriver( pOutput->mDisplayDriver )
    , mParentOutput( pOutput )
    , mPrivateData( std::make_unique<VideoModePrivateData>() )
    , mDesc( &( mPrivateData->descPriv ) )
    , mNativeData( &( mPrivateData->nativeDataPriv ) )
    {
        dsmInitializeNativeData( &mNativeData, mDisplayDriver->mDriverType );
    }

    DisplayVideoMode::~DisplayVideoMode()
    {
        dsmReleaseNativeData( &mNativeData, mDisplayDriver->mDriverType );
    }


    DisplayDriverNativeImpl::DisplayDriverNativeImpl( DisplayManager * pDisplayManager, EDisplayDriverType pDriverType )
    : DisplayDriver( pDisplayManager, pDriverType )
    {}

    DisplayDriverNativeImpl::~DisplayDriverNativeImpl()
    {}

    void DisplayDriverNativeImpl::_nativeInitialize()
    void DisplayDriverNativeImpl::_nativeRelease()
    void DisplayDriverNativeImpl::_nativeResetDisplayConfiguration()
    void DisplayDriverNativeImpl::_nativeSyncDisplayConfiguration()



    DisplayDriverNativeImpl::DisplayDriverNativeImpl( DisplayManager * pDisplayManager, EDisplayDriverType pDriverType )
    : DisplayDriver( pDisplayManager, pDriverType )
    {
    }

    DisplayDriver::~DisplayDriver()
    {
        dsmReleaseNativeData( &mNativeData, mDriverType );
    }

    void DisplayDriver::_nativeInitialize()
    {
        _drvInitialize();
    }

    void DisplayDriver::_nativeRelease()
    {
        _drvRelease();
    }

    void DisplayDriver::_nativeResetDisplayConfiguration()
    {
        mAdapterStorage.clear();
        mAdapterList.clear();
    }

    void DisplayDriver::_nativeSyncDisplayConfiguration()
    {
        ts3DebugAssert( mAdapterStorage.empty() && mAdapterList.empty() );

        _drvEnumAdapters();

        if( mAdapterList.empty() )
        {
            // Report
            return;
        }

        for( auto & adapter : mAdapterStorage )
        {
            _drvEnumOutputs( adapter );

            if( adapter.isPrimaryAdapter() )
            {
                mPrimaryAdapter = &adapter;
            }

            if( adapter.mOutputList.empty() )
            {
                // Warn: no active outputs found for 'adapter'
                continue;
            }

            for( auto & output : adapter.mOutputStorage )
            {
                _drvEnumVideoModes( output, ColorFormat::SystemNative );

                if( output.isPrimaryOutput() )
                {
                    adapter.mPrimaryOutput = &output;
                }

                if( output.mColorFormatMap[ColorFormat::SystemNative].videoModeList.empty() )
                {
                    // Warn: no supported display modes found for 'adapter'
                    continue;
                }
            }
        }
    }

    const DisplayAdapterList & DisplayDriver::_nativeGetAdapterList() const
    {
        return mAdapterList;
    }

    const DisplayOutputList & DisplayDriver::_nativeGetOutputList( dsm_index_t pAdapterIndex ) const
    {
        const auto & adapter = mAdapterStorage.at( pAdapterIndex );
        return adapter.mOutputList;
    }

    const DisplayVideoModeList & DisplayDriver::_nativeGetVideoModeList( dsm_output_id_t pOutputID, ColorFormat pColorFormat ) const
    {
        DisplayOutputID outputIDGen;
        outputIDGen.outputID = pOutputID;

        const auto & adapter = mAdapterStorage.at( outputIDGen.uAdapterIndex );
        const auto & output = adapter.mOutputStorage.at( outputIDGen.uOutputIndex );
        const auto & colorFormatData = output.mColorFormatMap.at( pColorFormat );

        return colorFormatData.videoModeList;
    }


}
