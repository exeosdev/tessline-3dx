
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
