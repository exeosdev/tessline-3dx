
#include "systemDisplayManager.h"
#include <ts3/system/displayDriver.h>
#include <ts3/system/displaySystem.h>

namespace ts3::gpuapi
{

	SystemDisplayManager::SystemDisplayManager( system::DisplayDriver * pExfDisplayDriver )
	: DisplayManager()
	, mExfDisplayDriver( pExfDisplayDriver )
	{}

	SystemDisplayManager::~SystemDisplayManager() = default;

	void SystemDisplayManager::reset()
	{
	}

	AdapterList SystemDisplayManager::enumAdapterList() const
	{
		AdapterList adapterList;

		try
		{
			auto sysAdapterList = mExfDisplayDriver->enumAdapterList();
			for( auto * sysAdapterDesc : sysAdapterList )
			{
				auto & adapterDesc = adapterList.emplace_back();
				adapterDesc.id = sysAdapterDesc->index;
				adapterDesc.name = sysAdapterDesc->name;
				adapterDesc.flags = translateSysAdapterFlags( sysAdapterDesc->flags );
			}
		}
		catch ( ... )
		{
			ts3DebugInterrupt();
		}

		return adapterList;
	}

	AdapterDesc SystemDisplayManager::getDefaultAdapter() const
	{
		AdapterDesc adapterDesc;

		try
		{
			auto * sysDefaultAdapterDesc = mExfDisplayDriver->getDefaultAdapter();
			if( sysDefaultAdapterDesc != nullptr )
			{
				adapterDesc.id = sysDefaultAdapterDesc->index;
				adapterDesc.name = sysDefaultAdapterDesc->name;
				adapterDesc.flags = translateSysAdapterFlags( sysDefaultAdapterDesc->flags );
			}
		}
		catch ( ... )
		{
			ts3DebugInterrupt();
		}

		return adapterDesc;
	}

	OutputList SystemDisplayManager::enumOutputList( display_system_id_t pAdapterID ) const
	{
		OutputList outputList;

		try
		{
			auto sysAdapterIndex = static_cast<ts3::sys_dsm_index_t>( pAdapterID );
			auto sysOutputList = mExfDisplayDriver->enumOutputList( sysAdapterIndex );
			for( auto * sysOutputDesc : sysOutputList )
			{
				auto & outputDesc = outputList.emplace_back();
				outputDesc.id = sysOutputDesc->id;
				outputDesc.name = sysOutputDesc->name;
				outputDesc.screenRect = sysOutputDesc->screenRect;
				outputDesc.flags = translateSysOutputFlags( sysOutputDesc->flags );
			}
		}
		catch ( ... )
		{
			ts3DebugInterrupt();
		}

		return outputList;
	}

	OutputDesc SystemDisplayManager::getDefaultOutput( display_system_id_t pAdapterID ) const
	{
		OutputDesc outputDesc;

		try
		{
			auto sysAdapterIndex = static_cast<ts3::sys_dsm_index_t>( pAdapterID );
			auto * sysDefaultOutputDesc = mExfDisplayDriver->getDefaultOutput( sysAdapterIndex );
			if( sysDefaultOutputDesc != nullptr )
			{
				outputDesc.id = sysDefaultOutputDesc->id;
				outputDesc.name = sysDefaultOutputDesc->name;
				outputDesc.screenRect = sysDefaultOutputDesc->screenRect;
				outputDesc.flags = translateSysOutputFlags( sysDefaultOutputDesc->flags );
			}
		}
		catch ( ... )
		{
			ts3DebugInterrupt();
		}

		return outputDesc;
	}

	VideoModeList SystemDisplayManager::enumVideoModeList( display_system_id_t pOutputID, system::EColorFormat pFormat ) const
	{
		VideoModeList videoModeList;

		try
		{
			auto sysOutputID = static_cast<ts3::sys_dsm_output_id_t>( pOutputID );
			auto sysVideoModeList = mExfDisplayDriver->enumVideoModeList( sysOutputID, pFormat );
			for( auto * sysVideoModeDesc : sysVideoModeList )
			{
				auto & outputDesc = videoModeList.emplace_back();
				outputDesc.id = sysVideoModeDesc->id;
				outputDesc.settings.resolution = sysVideoModeDesc->settings.resolution;
				outputDesc.settings.refreshRate = sysVideoModeDesc->settings.refreshRate;
				outputDesc.settings.flags = translateSysVideoSettingsFlags( sysVideoModeDesc->settings.flags );
				outputDesc.format = pFormat;
			}
		}
		catch ( ... )
		{
			ts3DebugInterrupt();
		}

		return videoModeList;
	}

}
