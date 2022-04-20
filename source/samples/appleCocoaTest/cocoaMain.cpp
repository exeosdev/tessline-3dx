
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

	while( true )
	{
		ev->dispatchPendingEventsPeek();
	}

	return 0;
}
