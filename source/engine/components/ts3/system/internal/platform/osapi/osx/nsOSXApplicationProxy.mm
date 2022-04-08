
#include "osxSysContext.h"
#include "nsOSXApplicationProxy.h"

@implementation NSOSXApplicationProxy

-( void ) terminate: ( id )pSender
{
}

-( void ) sendEvent: ( NSEvent * )pEvent
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

-( void ) setOSXSysContext: ( ts3::system::OSXSysContext * )pOSXSysContext
{
	self->mOSXSysContext = pOSXSysContext;
}

-( id ) init
{
	self = [super init];
	if( self )
	{
		NSNotificationCenter * nsNotifCenter = [NSNotificationCenter defaultCenter];

//		[nsNotifCenter addObserver:self
//		               selector:@selector(windowWillClose:)
//		               name:NSWindowWillCloseNotification
//		               object:nil];
//
//		[nsNotifCenter addObserver:self
//		               selector:@selector(focusSomeWindow:)
//		               name:NSApplicationDidBecomeActiveNotification
//		               object:nil];
//
//		[nsNotifCenter addObserver:self
//		               selector:@selector(localeDidChange:)
//		               name:NSCurrentLocaleDidChangeNotification
//		               object:nil];
	}

	return self;
}

-( void ) dealloc
{
	NSNotificationCenter * nsNotifCenter = [NSNotificationCenter defaultCenter];

	[nsNotifCenter removeObserver:self name:NSWindowWillCloseNotification object:nil];
	[nsNotifCenter removeObserver:self name:NSApplicationDidBecomeActiveNotification object:nil];
	[nsNotifCenter removeObserver:self name:NSCurrentLocaleDidChangeNotification object:nil];

	[super dealloc];
}

-( void ) windowWillClose: (NSNotification *)pNotification
{
}

-( void ) applicationWillFinishLaunching: (NSNotification *)pNotification
{
	auto & osxSharedData = ts3::system::platform::osxGetOSXSharedData( *mOSXSysContext );
	ts3DebugAssert( !osxSharedData.stateFlags.isSet( ts3::system::platform::E_OSX_COMMON_STATE_APP_FINISHED_LAUNCHING_BIT ) );

	if( [NSApp mainMenu] == nil )
	{
		if( !ts3::system::platform::osxNibLoadMenuNibFile() )
		{
			ts3::system::platform::osxNibCreateDefaultApplicationMenu();
		}
	}

	if( [NSApp mainMenu] )
	{
		[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
		[NSApp stop:nil];
	}
}

-( void ) applicationDidFinishLaunching: (NSNotification *)pNotification
{
	auto & osxSharedData = ts3::system::platform::osxGetOSXSharedData( *mOSXSysContext );
	osxSharedData.stateFlags.set( ts3::system::platform::E_OSX_COMMON_STATE_APP_FINISHED_LAUNCHING_BIT );
}

@end
