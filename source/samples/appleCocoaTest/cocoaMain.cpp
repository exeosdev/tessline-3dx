
#include <ts3/system/windowSystem.h>
#include <ts3/system/eventCore.h>
#include <ts3/system/openGLDriver.h>
#include <ts3/system/sysContext.h>

using namespace ts3;
using namespace ts3::system;

int main( int argc, const char ** argv )
{
	auto sysContext = platform::createSysContext( {} );
	auto ev = sysContext->createEventController();
	auto dm = sysContext->createDisplayManager();
	auto wm = sysContext->createWindowManager( dm );

	auto gd = sysContext->createOpenGLSystemDriver( dm );
	auto gv = gd->initializePlatform();

	OpenGLDisplaySurfaceCreateInfo surfaceCreateInfo;
	surfaceCreateInfo.frameGeometry.size = {800, 600 };
	surfaceCreateInfo.frameGeometry.style = EFrameStyle::Caption;
	surfaceCreateInfo.visualConfig = vsxGetDefaultVisualConfigForSysWindow();
	surfaceCreateInfo.targetAPIClass = EOpenGLAPIClass::OpenGLDesktop;
	surfaceCreateInfo.minimumAPIVersion = gv.apiVersion;
	//surfaceCreateInfo.flags.set( E_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT );
	auto ds = gd->createDisplaySurface( surfaceCreateInfo );

	OpenGLRenderContextCreateInfo renderContextCreateInfo;
	renderContextCreateInfo.requestedAPIVersion = gv.apiVersion;
	renderContextCreateInfo.contextAPIProfile = EOpenGLAPIProfile::Core;
	renderContextCreateInfo.shareContext = nullptr;
	auto rc = gd->createRenderContext( *ds, renderContextCreateInfo );

	// WindowCreateInfo wci{};
	// wci.frameGeometry.position = {100,100};
	// wci.frameGeometry.size = {500,500};
	// wci.frameGeometry.style = EFrameStyle::Fixed;
	// wci.title = "SampleTitle";
	// auto cwin = wm->createWindow( wci );

	auto ed = ev->createEventDispatcher();
	ed->setEventSystemConfigFlags( E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_AUTO_QUIT_ON_PRIMARY_SOURCE_DESTROY_BIT );
	ev->setActiveEventDispatcher( *ed );

	bool runApp = true;

	ed->setEventHandler(
			EEventCodeIndex::AppActivityQuit,
			[&runApp](const EventObject & pEvt) -> bool {
				runApp = false;
				return true;
			});

	ev->registerPrimaryEventSource( *ds );
	rc->bindForCurrentThread( *ds );

	while( runApp )
	{
		ev->dispatchPendingEventsPeek();
		ds->clearColorBuffer();
		ds->swapBuffers();
	}

	return 0;
}
