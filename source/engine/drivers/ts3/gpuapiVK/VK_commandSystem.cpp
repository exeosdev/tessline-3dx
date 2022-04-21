
#include "VK_commandSystem.h"
#include "VK_commandList.h"
#include "VK_gpuDevice.h"
#include <ts3/gpuapi/commandContext.h>

namespace ts3::gpuapi
{

	VKCommandSystem::VKCommandSystem( VKGPUDevice & pVKGPUDevice )
	: CommandSystem( pVKGPUDevice )
	{ }

	VKCommandSystem::~VKCommandSystem() = default;

	std::unique_ptr<CommandContext> VKCommandSystem::acquireCommandContext( ECommandContextType pContextType )
	{
		std::unique_ptr<CommandContext> commandContext;

		auto contextExecutionMode = ecGetCommandContextExecutionMode( pContextType );
		// if( auto * commandList = acquireCommandList( contextExecutionMode ) )
		{
			if( contextExecutionMode == ECommandExecutionMode::Direct )
			{
				// commandContext.reset( new CommandContextDirectGraphics( *this, *commandList ) );
			}
			else
			{
				// commandContext.reset( new CommandContextDeferredGraphics( *this, *commandList ) );
			}
		}

		return commandContext;
	}

	CommandSync VKCommandSystem::submitContext( CommandContextDirect & pContext, const CommandContextSubmitInfo & pSubmitInfo )
	{
		CommandSync cmdSyncObject;

		if( pSubmitInfo.syncMode == ECommandSubmitSyncMode::Default )
		{
		}

		return cmdSyncObject;
	}

} // namespace ts3::gpuapi
