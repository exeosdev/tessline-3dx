
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
        return true;
    }

    bool DisplayAdapter::isPrimaryAdapter() const
    {
        return mPrivate->descPriv.flags.isSet( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );
    }

    bool DisplayAdapter::hasActiveOutputs() const
    {}


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
        if( pAdapterIndex == CX_DSM_INDEX_INVALID )
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
        DisplayOutputID outputIDGen;
        outputIDGen.outputID = pOutputID;

        auto * adapter = getAdapter( outputIDGen.uAdapterIndex );
        auto & output = adapter->mPrivate->outputStorage.at( outputIDGen.uOutputIndex );

        return &output;
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
    }

    void DisplayDriver::_enumAdapters()
    {
        _nativeEnumAdapters();

        if( !mPrivate->adapterStorage.empty() )
        {
            mPrivate->adapterList.reserve( mPrivate->adapterStorage.size() );
            for( auto & adapter : mPrivate->adapterStorage )
            {
                if( adapter.isPrimaryAdapter() )
                {
                    mPrivate->primaryAdapter = &adapter;
                }
                mPrivate->adapterList.push_back( &adapter );
            }
        }
    }

    void DisplayDriver::_enumOutputs( DisplayAdapter & pAdapter )
    {
        _nativeEnumOutputs( pAdapter );

        if( !pAdapter.mPrivate->outputStorage.empty() )
        {
            pAdapter.mPrivate->outputList.reserve( pAdapter.mPrivate->outputStorage.size() );
            for( auto & output : pAdapter.mPrivate->outputStorage )
            {
                if( output.isPrimaryOutput() )
                {
                    pAdapter.mPrivate->primaryOutput = &output;
                }
                pAdapter.mPrivate->outputList.push_back( &output );
            }
        }

    }

    void DisplayDriver::_enumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat )
    {
        _nativeEnumVideoModes( pOutput, pColorFormat );

        auto & colorFormatData = pOutput.mPrivate->colorFormatMap[pColorFormat];
        if( !colorFormatData.videoModeStorage.empty() )
        {
            colorFormatData.videoModeList.reserve( colorFormatData.videoModeStorage.size() );
            for( auto & videoMode : colorFormatData.videoModeStorage )
            {
                colorFormatData.videoModeList.push_back( &videoMode );
            }
        }
    }


}
