
#include <ts3/system/windowSystem.h>
#include <ts3/system/eventCore.h>
#include <ts3/system/sysContext.h>

using namespace ts3;
using namespace ts3::system;

int main( int argc, const char ** argv )
{
	auto sysContext = platform::createSysContext( {} );
	auto ev = sysContext->createEventController();
	auto dm = sysContext->createDisplayManager();
	auto wm = sysContext->createWindowManager( dm );

	WindowCreateInfo wci{};
	wci.frameGeometry.position = {100,100};
	wci.frameGeometry.size = {500,500};
	wci.frameGeometry.style = EFrameStyle::Fixed;
	wci.title = "SampleTitle";
	auto cwin = wm->createWindow( wci );

	auto ed = ev->createEventDispatcher();
	ev->setActiveEventDispatcher( *ed );

	bool runApp = true;

	ed->setEventHandler(
			EEventCodeIndex::AppActivityQuit,
			[&runApp](const EventObject & pEvt) -> bool {
				runApp = false;
				return true;
			});

	ev->registerPrimaryEventSource( *cwin );
	ev->setEventSystemConfigFlags( E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_AUTO_QUIT_ON_PRIMARY_SOURCE_DESTROY_BIT );

	while( runApp )
	{
		ev->dispatchPendingEventsPeek();
	}

	return 0;
}
