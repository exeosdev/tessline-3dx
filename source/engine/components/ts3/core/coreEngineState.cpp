
#include "coreEngineState.h"

namespace ts3
{

	CoreEngineState * initializeCoreEngineState()
	{
		return new CoreEngineState();
	}

	void releaseCoreEngineState( CoreEngineState * pState )
	{
		delete pState;
	}

}
