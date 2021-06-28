
#pragma once

#ifndef __TS3_CORE_CORE_ENGINE_STATE_H__
#define __TS3_CORE_CORE_ENGINE_STATE_H__

#include "prerequisites.h"

namespace ts3
{

	struct CoreEngineState
	{
	};

	struct CoreEngineStateDeleter
    {
	    void operator()( CoreEngineState * pState ) const;
    };

	CoreEngineState * initializeCoreEngineState();

	void releaseCoreEngineState( CoreEngineState * pState );

}

#endif // __TS3_CORE_CORE_ENGINE_STATE_H__
