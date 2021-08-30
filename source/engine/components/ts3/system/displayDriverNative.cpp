
#include "displayDriverNative.h"

namespace ts3::system
{

    DisplayVideoModeNativeImpl::DisplayVideoModeNativeImpl( DisplayOutputNativeImpl * pOutput )
    : DisplayVideoMode( pOutput, mDescPriv )
    {
        dsmInitializeNativeData( &mNativeData, mDisplayDriver->mDriverType );
    }

    DisplayVideoModeNativeImpl::~DisplayVideoModeNativeImpl()
    {
        dsmReleaseNativeData( &mNativeData, mDisplayDriver->mDriverType );
    }


    DisplayOutputNativeImpl::DisplayOutputNativeImpl( DisplayAdapterNativeImpl * pAdapter )
    : DisplayOutput( pAdapter, mDescPriv )
    {
        dsmInitializeNativeData( &mNativeData, mDisplayDriver->mDriverType );
    }

    DisplayOutputNativeImpl::~DisplayOutputNativeImpl()
    {
        dsmReleaseNativeData( &mNativeData, mDisplayDriver->mDriverType );
    }


    DisplayAdapterNativeImpl::DisplayAdapterNativeImpl( DisplayDriverNativeImpl * pDriver )
    : DisplayAdapter( pDriver, mDescPriv )
    {
        dsmInitializeNativeData( &mNativeData, mDisplayDriver->mDriverType );
    }

    DisplayAdapterNativeImpl::~DisplayAdapterNativeImpl()
    {
        dsmReleaseNativeData( &mNativeData, mDisplayDriver->mDriverType );
    }


    DisplayDriverNativeImpl::DisplayDriverNativeImpl( DisplayManager * pDisplayManager,
                                                      EDisplayDriverType pDriverType )
    : DisplayDriver( pDisplayManager, pDriverType )
    {
        dsmInitializeNativeData( &mNativeData, mDriverType );
    }

    DisplayDriverNativeImpl::~DisplayDriverNativeImpl()
    {
        dsmReleaseNativeData( &mNativeData, mDriverType );
    }

    void DisplayDriverNativeImpl::_nativeInitialize()
    {
        _drvInitialize();
    }
    void DisplayDriverNativeImpl::_nativeRelease()
    {
        _drvRelease();
    }

    void DisplayDriverNativeImpl::_nativeResetDisplayConfiguration()
    {
        mAdapterStorage.clear();
        mAdapterList.clear();
    }

    void DisplayDriverNativeImpl::_nativeSyncDisplayConfiguration()
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

    const DisplayAdapterList & DisplayDriverNativeImpl::_nativeGetAdapterList() const
    {
        return mAdapterList;
    }

    const DisplayOutputList & DisplayDriverNativeImpl::_nativeGetOutputList( dsm_index_t pAdapterIndex ) const
    {
        const auto & adapter = mAdapterStorage.at( pAdapterIndex );
        return adapter.mOutputList;
    }

    const DisplayVideoModeList & DisplayDriverNativeImpl::_nativeGetVideoModeList( dsm_output_id_t pOutputID, ColorFormat pColorFormat ) const
    {
        DisplayOutputID outputIDGen;
        outputIDGen.outputID = pOutputID;

        const auto & adapter = mAdapterStorage.at( outputIDGen.uAdapterIndex );
        const auto & output = adapter.mOutputStorage.at( outputIDGen.uOutputIndex );
        const auto & colorFormatData = output.mColorFormatMap.at( pColorFormat );

        return colorFormatData.videoModeList;
    }

    DisplayAdapterNativeImpl * DisplayDriverNativeImpl::addAdapter()
    {
        const auto adapterIndex = mAdapterStorage.size();

        auto & adapter = mAdapterStorage.emplace_back( this );
        adapter.mDescPriv.driverType = mDriverType;
        adapter.mDescPriv.adapterIndex = adapterIndex;

        mAdapterList.push_back( &adapter );
        ts3DebugAssert( mAdapterStorage.size() == mAdapterList.size() );

        return &adapter;
    }

    DisplayOutputNativeImpl * DisplayDriverNativeImpl::addOutput( DisplayAdapterNativeImpl & pAdapter )
    {
        const auto outputIndex = pAdapter.mOutputStorage.size();

        DisplayOutputID outputIDGen;
        outputIDGen.uAdapterIndex = pAdapter.mDesc.adapterIndex;
        outputIDGen.uOutputIndex = static_cast<dsm_index_t>( outputIndex );

        auto & output = pAdapter.mOutputStorage.emplace_back( this );
        output.mDescPriv.driverType = mDriverType;
        output.mDescPriv.outputIndex = outputIndex;
        output.mDescPriv.outputID = outputIDGen.outputID;

        pAdapter.mOutputList.push_back( &output );
        ts3DebugAssert( pAdapter.mOutputStorage.size() == pAdapter.mOutputList.size() );

        return &output;
    }

    DisplayVideoModeNativeImpl * DisplayDriverNativeImpl::addVideoMode( DisplayOutputNativeImpl & pOutput,
                                                                        ColorFormat pColorFormat )
    {
        auto & colorFormatData = pOutput.mColorFormatMap[pColorFormat];

        const auto videoModeIndex = colorFormatData.videoModeStorage.size();

        DisplayVideoModeID videoModeIDGen;
        videoModeIDGen.uOutputID = pOutput.mDesc.outputID;
        videoModeIDGen.uColorFormatIndex = static_cast<dsm_index_t>( pColorFormat );
        videoModeIDGen.uModeIndex = static_cast<dsm_index_t>( videoModeIndex );

        auto & videoMode = colorFormatData.videoModeStorage.emplace_back( this );
        videoMode.mDescPriv.driverType = mDriverType;
        videoMode.mDescPriv.videoModeIndex = videoModeIndex;
        videoMode.mDescPriv.videoModeID = videoModeIDGen.modeID;
        videoMode.mDescPriv.colorFormat = pColorFormat;

        colorFormatData.videoModeList.push_back( &videoMode );
        ts3DebugAssert( colorFormatData.videoModeStorage.size() == colorFormatData.videoModeList.size() );

        return &videoMode;
    }

} // namespace ts3::system
