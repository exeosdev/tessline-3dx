
#pragma once

#ifndef __TS3_ENGINE_RES_RESOURCE_LOADER_H__
#define __TS3_ENGINE_RES_RESOURCE_LOADER_H__

#include "../prerequisites.h"

namespace ts3
{

	class ResourceLoader
	{
	protected:
		gpuapi::CommandContext & getGPUCommandContext() const
		{
			if( !_gpuCommandContext )
			{
				throw 0;
			}
			return *_gpuCommandContext;
		}

	private:
		gpuapi::CommandContext * _gpuCommandContext;
	};

} // namespace ts3

#endif // __TS3_ENGINE_RES_RESOURCE_LOADER_H__
