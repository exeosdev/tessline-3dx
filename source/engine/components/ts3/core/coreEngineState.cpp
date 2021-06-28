
#include "coreEngineState.h"

namespace ts3
{

    void CoreEngineStateDeleter::operator()( CoreEngineState * pState ) const
    {
        releaseCoreEngineState( pState );
    }

	CoreEngineState * initializeCoreEngineState()
	{
		return new CoreEngineState();
	}

	void releaseCoreEngineState( CoreEngineState * pState )
	{
		delete pState;
	}

}
