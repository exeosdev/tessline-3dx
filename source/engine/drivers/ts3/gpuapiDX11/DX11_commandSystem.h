
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_COMMAND_SYSTEM_H__
#define __TS3DRIVER_GPUAPI_DX11_COMMAND_SYSTEM_H__

#include "DX11_prerequisites.h"
#include <ts3/gpuapi/commandSystem.h>
#include <mutex>

namespace ts3
{
namespace gpuapi
{

	struct DX11CommandSyncData
	{
		ID3D11DeviceContext1 * d3d11DeviceContext1 = nullptr;
		ID3D11Query * d3d11SyncQuery = nullptr;
	};

	TS3GX_DX11_API void releaseDX11CommandSyncData( void * pSyncData );

	/// @brief
	class TS3GX_DX11_CLASS DX11CommandSystem final : public CommandSystem
	{
	public:
		ComPtr<ID3D11Device1> const mD3D11Device1 = nullptr;

		explicit DX11CommandSystem( DX11GPUDevice & pDX11GPUDevice );
		virtual ~DX11CommandSystem();

		virtual std::unique_ptr<CommandContext> acquireCommandContext( ECommandContextType pContextType ) override;

		virtual CommandSync submitContext( CommandContextDirect & pContext, const CommandContextSubmitInfo & pSubmitInfo ) override;

	private:
		DX11CommandList * acquireCommandList( ECommandExecutionMode pCommandExecutionMode );
		bool initializeDirectCommandList();
		bool createDeferredCommandList();

	private:
		DX11CommandListHandle _directCommandList;
		using DX11CommandListStorage = std::unordered_map<DX11CommandList *, DX11CommandListHandle>;
		DX11CommandListStorage _deferredCommandListStorage;
		using DX11CommandListList = std::list<DX11CommandList *>;
		DX11CommandListList _availableList;
		std::mutex _availableListLock;
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_DX11_COMMAND_SYSTEM_H__
