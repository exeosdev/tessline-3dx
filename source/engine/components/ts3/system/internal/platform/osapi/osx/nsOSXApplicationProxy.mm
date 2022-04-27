
#include "osxSysContext.h"
#include "nsOSXApplicationProxy.h"

@implementation NSOSXApplicationProxy

-( void ) terminate:( id )pSender
{
}

-( void ) sendEvent:( NSEvent * )pEvent
{
	[super sendEvent:pEvent];
}

+( void ) registerUserDefaults
{
@autoreleasepool
{

	NSDictionary * appDefaults = [[NSDictionary alloc] initWithObjectsAndKeys:
	                              [NSNumber numberWithBool:NO], @"AppleMomentumScrollSupported",
	                              [NSNumber numberWithBool:NO], @"ApplePressAndHoldEnabled",
	                              [NSNumber numberWithBool:YES], @"ApplePersistenceIgnoreState",
	                              nil];

	[[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];

	[appDefaults release];
}
}

@end


@implementation NSOSXApplicationDelegate

-( id ) initWithSysContext: ( OSXSysContext * )pSysContext
{
	if( [super init] )
	{
		mSysContext = pSysContext;

		NSNotificationCenter * defaultNotificationCenter = [NSNotificationCenter defaultCenter];

		[defaultNotificationCenter addObserver:self
		                           selector:@selector( applicationDidBecomeActiveNotificationHandler: )
		                           name:NSApplicationDidBecomeActiveNotification
		                           object:nil];

		[defaultNotificationCenter addObserver:self
		                           selector:@selector( currentLocaleDidChangeNotificationHandler: )
		                           name:NSCurrentLocaleDidChangeNotification
		                           object:nil];

		[defaultNotificationCenter addObserver:self
		                           selector:@selector( windowWillCloseNotificationHandler: )
		                           name:NSWindowWillCloseNotification
		                           object:nil];
	}

	return self;
}

-( void ) dealloc
{
	NSNotificationCenter * defaultNotificationCenter = [NSNotificationCenter defaultCenter];

	[defaultNotificationCenter removeObserver:self
	                           name:NSWindowWillCloseNotification
	                           object:nil];

	[defaultNotificationCenter removeObserver:self
	                           name:NSApplicationDidBecomeActiveNotification
	                           object:nil];

	[defaultNotificationCenter removeObserver:self
	                           name:NSCurrentLocaleDidChangeNotification
	                           object:nil];

	[super dealloc];
}

-( void ) applicationDidBecomeActiveNotificationHandler:( NSNotification * )pNotification
{
	// TODO: Implement window restoration/raising the window.
	// One exception is when app's key window is not ours.

	if( NSWindow * nsAppKeyWindow = [NSApp keyWindow] )
	{
		// Idea:
		// auto * ts3Window = mSysContext->findWindowByNSWindow( nsAppKeyWindow );
		// if( !ts3Window )
		// {
		//   return;
		// }
	}
}

-( void ) currentLocaleDidChangeNotificationHandler:( NSNotification * )pNotification
{
	// TODO: This will be useful to enable system-level locale change notification (currently a design proposal).
	return;
}

-( void ) windowWillCloseNotificationHandler:( NSNotification * )pNotification
{
	// This handler solves a problem with events when the key window is closed - we have narrowed down
	// few issues to be caused by this problem. Simple solution: iterate over app's windows and make
	// a certain window the key window.

	if( ![pNotification object] )
	{
		// Not sure if this can happen, but events without
		// sources are definitely not the right ones to handle.
		// TODO: Maybe a diagnostic message? Looks like an edge case.
		return;
	}

	// Get the window that will be closed
	auto * nsWindowToClose = ( NSWindow * )[pNotification object];

	if( ![nsWindowToClose isKeyWindow] )
	{
		// If the window is not a key one, there is nothing to do here.
		return;
	}

	for( NSWindow * nsWindow in [NSApp orderedWindows] )
	{
		// Skip the window which is about to be closed. Also, ignore all windows which cannot become key windows.
		if( ( nsWindow == nsWindowToClose ) || ![nsWindow canBecomeKeyWindow] )
		{
			continue;
		}

		if( [nsWindow isOnActiveSpace] )
		{
			[nsWindow makeKeyAndOrderFront:self];
			return;
		}
	}

	// No window has been found, enumerate windows using [windowNumbersWithOptions]. Quote from the docs:
	// "If no options are specified, only visible windows belonging to the calling application and on the active space are included".
	// That seems to be exactly what we want to get here.

	NSArray<NSNumber *> * nsVisibleActiveAppWindowNumbers = [NSWindow windowNumbersWithOptions:0];

	if( !nsVisibleActiveAppWindowNumbers or ( [nsVisibleActiveAppWindowNumbers count] == 0 ) )
	{
		return;
	}

	for( NSNumber * nsWindowNumber in nsVisibleActiveAppWindowNumbers )
	{
		NSWindow * nsWindow = [NSApp windowWithWindowNumber:[nsWindowNumber integerValue]];

		if( ( nsWindow != nsWindowToClose ) && [nsWindow canBecomeKeyWindow] )
		{
			[nsWindow makeKeyAndOrderFront:self];
			return;
		}
	}
}

-( void ) applicationWillFinishLaunching: (NSNotification *)pNotification
{
	auto & osxSharedData = ts3::system::platform::osxGetOSXSharedData( *mSysContext );
	ts3DebugAssert( !osxSharedData.stateFlags.isSet( ts3::system::platform::E_OSX_COMMON_STATE_APP_FINISHED_LAUNCHING_BIT ) );

	if( ![NSApp mainMenu] )
	{
		if( !ts3::system::platform::osxNibLoadMenuNibFile() )
		{
			ts3::system::platform::osxNibCreateDefaultApplicationMenu();
		}
	}
}

-( void ) applicationDidFinishLaunching:( NSNotification * )pNotification
{
	auto & osxSharedData = ts3::system::platform::osxGetOSXSharedData( *mSysContext );

	[NSOSXApplicationProxy registerUserDefaults];

	// Note: [setActivationPolicy] has to be called exactly in this place:
	// - after the app finished its launching process AND
	// - after the menu has been created (does not count if the menu is absent) AND
	// - before [activateIgnoringOtherApps] gets called.
	// Any other combination makes the menu "inactive" (cannot be clicked at all) until the app loses focus
	// and is re-focused again. Seems to be confirmed by other low-level libraries implementations (GLFW, SDL).

	if( [NSApp mainMenu] )
	{
		// > NSApplicationActivationPolicyRegular:
		// > "The application is an ordinary app that appears in the Dock and may have a user interface."
		//
		// We use that in case app has the menu configured and successfully created.
		[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
	}
	else
	{
		// > NSApplicationActivationPolicyAccessory:
		// > "The application does not appear in the Dock and does not have a menu bar,
		// > but it may be activated programmatically or by clicking on one of its windows".
		//
		// Setting NSApplicationActivationPolicyRegular in this case tend to render weird results,
		// so it seems like a good solution for a missing menu cases (or a fallback option).
		[NSApp setActivationPolicy:NSApplicationActivationPolicyAccessory];
	}

	// This has to go after the app's activation policy is properly set.
	[NSApp activateIgnoringOtherApps:YES];

	osxSharedData.stateFlags.set( ts3::system::platform::E_OSX_COMMON_STATE_APP_FINISHED_LAUNCHING_BIT );
}

-( BOOL ) applicationShouldTerminateAfterLastWindowClosed:( NSApplication * )pApplication
{
	// According to the docs, returning YES causes the system to invoke applicationShouldTerminate() handler of the
	// application's event responder after the last window is closed. Regardless of whether we implement this using an
	// internal window list, it's useful to have it as a reference point.
	//
	// Source:
	// https://developer.apple.com/documentation/appkit/nsapplicationdelegate/1428381-applicationshouldterminateafterl?language=objc

	return YES;
}

-( NSApplicationTerminateReply ) applicationShouldTerminate:( NSApplication * )pSender
{
	return NSTerminateLater;
}

-( void ) applicationWillTerminate:( NSNotification * )pNotification
{
	// Whether this gets called, depends on applicationShouldTerminateAfterLastWindowClosed() above.
	// Right now we keep it active, it proved to be pretty handy in analysing different app lifetime
	// issues and multi-window cases.
}

@end
