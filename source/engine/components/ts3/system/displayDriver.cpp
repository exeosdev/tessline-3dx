
#include "displayDriver.h"
#include "displayManager.h"

namespace ts3
{


	SysDsmAdapter::SysDsmAdapter( SysDisplayDriver & pDisplayDriver )
	: driver( &pDisplayDriver )
	, driverType( pDisplayDriver.mDriverType )
	{
		sysDsmInitializeNativeData( &nativeData, driverType );
	}

	SysDsmAdapter::~SysDsmAdapter()
	{
		sysDsmReleaseNativeData( &nativeData, driverType );
	}


	SysDsmOutput::SysDsmOutput( SysDsmAdapter & pAdapter )
	: driver( pAdapter.driver )
	, driverType( pAdapter.driverType )
	, adapter( &pAdapter )
	{
		sysDsmInitializeNativeData( &nativeData, driverType );
	}

	SysDsmOutput::~SysDsmOutput()
	{
		sysDsmReleaseNativeData( &nativeData, driverType );
	}


	SysDsmVideoMode::SysDsmVideoMode( SysDsmOutput & pOutput )
	: driver( pOutput.driver )
	, driverType( pOutput.driverType )
	, output( &pOutput )
	{
		sysDsmInitializeNativeData( &nativeData, driverType );
	}

	SysDsmVideoMode::~SysDsmVideoMode()
	{
		sysDsmReleaseNativeData( &nativeData, driverType );
	}


	SysDisplayDriver::SysDisplayDriver( SysDisplayManagerHandle pDisplayManager, ESysDisplayDriverType pDriverType ) noexcept
	: SysBaseObject( pDisplayManager->mSysContext )
	, mDisplayManager( pDisplayManager )
	, mDriverType( pDriverType )
	, _defaultAdapter( nullptr )
	{
		sysDsmInitializeNativeData( &mNativeData, mDriverType );
	}

	SysDisplayDriver::~SysDisplayDriver() noexcept
	{
		sysDsmReleaseNativeData( &mNativeData, mDriverType );
	}

	void SysDisplayDriver::resetInternalState()
	{
		for( auto * adapter : _adapterList )
		{
			for( auto * output : adapter->outputList )
			{
				for( auto & videoModeList : output->videoModeMap )
				{
					videoModeList.second.clear();
				}
				output->videoModeMap.clear();
			}
			adapter->outputList.clear();
		}
		_adapterList.clear();
		_internalOutputMap.clear();
		_internalVideoModeMap.clear();

		// Driver-specific method for releasing additional cached state (e.g. within driver's native data).
		_sysResetInternalState();
	}

	std::vector<SysDsmAdapterDesc *> SysDisplayDriver::enumAdapterList()
	{
		if( _adapterList.empty() )
		{
			// Enumerate adapters, driver-specific.
			// Adds all available adapters to _adapterList.
			_sysEnumAdapterList();

			for( auto * adapter : _adapterList )
			{
				if( adapter->adapterDesc.flags.isSet( E_SYS_DSM_ADAPTER_FLAG_PRIMARY_BIT ) )
				{
					_defaultAdapter = adapter;
					break;
				}
			}

			if( ( _defaultAdapter == nullptr ) && !_adapterList.empty() )
			{
				_defaultAdapter = _adapterList[0];
			}
		}

		std::vector<SysDsmAdapterDesc *> result;
		result.reserve( _adapterList.size() );
		for( auto * adapter : _adapterList )
		{
			result.push_back( &( adapter->adapterDesc ) );
		}

		return result;
	}

	std::vector<SysDsmOutputDesc *> SysDisplayDriver::enumOutputList( sys_dsm_index_t pAdapterIndex )
	{
		auto * adapter = _adapterList.at( pAdapterIndex );
		if( adapter->outputList.empty() )
		{
			// Enumerate outputs, driver-specific.
			// Adds all available outputs to adapter->outputList.
			_sysEnumOutputList( *adapter );

			for( auto * output : adapter->outputList )
			{
				if( output->outputDesc.flags.isSet( E_SYS_DSM_OUTPUT_FLAG_PRIMARY_BIT ) )
				{
					adapter->defaultOutput = output;
				}
				_internalOutputMap[output->outputDesc.id] = output;
			}

			if( ( adapter->defaultOutput == nullptr ) && !adapter->outputList.empty() )
			{
				adapter->defaultOutput = adapter->outputList[0];
			}
		}

		std::vector<SysDsmOutputDesc *> result;
		result.reserve( adapter->outputList.size() );
		for( auto * output : adapter->outputList )
		{
			result.push_back( &( output->outputDesc ) );
		}

		return result;
	}

	std::vector<SysDsmVideoModeDesc *> SysDisplayDriver::enumVideoModeList( sys_dsm_output_id_t pOutputID, SysColorFormat pFormat )
	{
		auto * output = _internalOutputMap.at( pOutputID );
		auto & videoModeList = output->videoModeMap[pFormat];
		if( videoModeList.empty() )
		{
			// Enumerate video modes, driver-specific.
			// Adds all available video modes to videoModeList.
			_sysEnumVideoModeList( *output, pFormat );

			for( auto * videoMode : videoModeList )
			{
				_internalVideoModeMap[videoMode->modeDesc.id] = videoMode;
			}
		}

		std::vector<SysDsmVideoModeDesc *> result;
		result.reserve( videoModeList.size() );
		for( auto * videoModeState : videoModeList )
		{
			result.push_back( &( videoModeState->modeDesc ) );
		}

		return result;
	}

	SysDsmAdapterDesc * SysDisplayDriver::getDefaultAdapter() const
	{
		return ( _defaultAdapter != nullptr ) ? &( _defaultAdapter->adapterDesc ) : nullptr;
	}

	SysDsmOutputDesc * SysDisplayDriver::getDefaultOutput( sys_dsm_index_t pAdapterIndex ) const
	{
		auto * adapter = _adapterList.at( pAdapterIndex );
		return &( adapter->defaultOutput->outputDesc );
	}

	SysDsmAdapterDesc * SysDisplayDriver::findAdapterIf( const SysDsmAdapterCondPred & pCondPred ) const
	{
		SysDsmAdapter * resultAdapter = nullptr;
		for( auto * adapter : _adapterList )
		{
			if( pCondPred && pCondPred( adapter ) )
			{
				resultAdapter = adapter;
				break;
			}
		}

		return ( resultAdapter != nullptr ) ? &( resultAdapter->adapterDesc ) : nullptr;
	}

	SysDsmOutputDesc * SysDisplayDriver::findOutputIf( sys_dsm_index_t pAdapterIndex, const SysDsmOutputCondPred & pCondPred ) const
	{
		SysDsmOutput * resultOutput = nullptr;
		auto * adapter = _adapterList.at( pAdapterIndex );
		for( auto * output : adapter->outputList )
		{
			if( pCondPred && pCondPred( output ) )
			{
				resultOutput = output;
				break;
			}
		}

		return ( resultOutput != nullptr ) ? &( resultOutput->outputDesc ) : nullptr;
	}

	SysDsmAdapter * SysDisplayDriver::registerAdapter()
	{
		auto thisDriverID = queryDriverID();
		auto adapterIndex = _adapterList.size();

		auto * adapter = _adapterList.emplace_back( new SysDsmAdapter( *this ) );
		adapter->driverType = thisDriverID;
		adapter->adapterDesc.driverType = thisDriverID;
		adapter->adapterDesc.index = static_cast<sys_dsm_index_t>( adapterIndex );
		adapter->adapterDesc.flags = 0u;
		adapter->adapterDesc.vendorID = ESysDsmAdapterVendorID::Unknown;
		adapter->adapterDesc.driverReserved = adapter;

		return adapter;
	}

	SysDsmOutput * SysDisplayDriver::registerOutput( SysDsmAdapter & pAdapter )
	{
		auto thisDriverID = queryDriverID();
		ts3DebugAssert( thisDriverID == pAdapter.driverType );
		auto outputIndex = pAdapter.outputList.size();

		SysDsmOutputID outputID;
		outputID.uAdapterIndex = pAdapter.adapterDesc.index;
		outputID.uOutputIndex = static_cast<sys_dsm_index_t>( outputIndex );

		auto * output = pAdapter.outputList.emplace_back( new SysDsmOutput( pAdapter ) );
		output->driverType = thisDriverID;
		output->outputDesc.driverType = thisDriverID;
		output->outputDesc.index = outputID.uOutputIndex;
		output->outputDesc.id = outputID.outputID;
		output->outputDesc.flags = 0u;
		output->outputDesc.driverReserved = output;

		return output;
	}

	SysDsmVideoMode * SysDisplayDriver::registerVideoMode( SysDsmOutput & pOutput, SysColorFormat pFormat )
	{
		auto thisDriverID = queryDriverID();
		ts3DebugAssert( thisDriverID == pOutput.driverType );
		auto & videoModeList = pOutput.videoModeMap.at( pFormat );
		auto videoModeIndex = videoModeList.size();

		SysDsmVideoModeID videoModeID;
		videoModeID.uOutputID = pOutput.outputDesc.id;
		videoModeID.uColorFormatIndex = static_cast<sys_dsm_index_t>( pFormat );
		videoModeID.uModeIndex = static_cast<sys_dsm_index_t>( videoModeIndex );

		auto * videoMode = videoModeList.emplace_back( new SysDsmVideoMode( pOutput ) );
		videoMode->driverType = thisDriverID;
		videoMode->modeDesc.driverType = thisDriverID;
		videoMode->modeDesc.index = videoModeID.uModeIndex;
		videoMode->modeDesc.format = pFormat;
		videoMode->modeDesc.id = videoModeID.modeID;
		videoMode->modeDesc.driverReserved = videoMode;

		return videoMode;
	}


	SysDisplayDriverGeneric::SysDisplayDriverGeneric( SysDisplayManagerHandle pDisplayManager ) noexcept
	: SysDisplayDriver( pDisplayManager, ESysDisplayDriverType::Generic )
	{}

	SysDisplayDriverGeneric::~SysDisplayDriverGeneric() noexcept = default;

	ESysDisplayDriverType SysDisplayDriverGeneric::queryDriverID() const noexcept
	{
		return ESysDisplayDriverType::Generic;
	}

	SysDisplayDriverHandle SysDisplayDriverGeneric::create( SysDisplayManagerHandle pDisplayManager )
	{
		auto displayDriver = sysCreateObject<SysDisplayDriverGeneric>( pDisplayManager );
		return displayDriver;
	}
	
}
