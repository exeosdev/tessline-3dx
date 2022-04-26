
#include "osxSysContext.h"
#include "nsOSXApplicationProxy.h"
#import <AppKit/NSNibLoading.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_OSX )
namespace ts3::system
{

	namespace platform
	{

		OSXSharedData & osxGetOSXSharedData( SysContext & pSysContext )
		{
			auto * osxSysContext = pSysContext.queryInterface<OSXSysContext>();
			return osxSysContext->mNativeData.osxSharedData;
		}

		OSXSharedData & osxGetOSXSharedData( OSXSysContext & pSysContext )
		{
			return pSysContext.mNativeData.osxSharedData;
		}

		bool osxNibLoadMenuNibFile()
		{
		@autoreleasepool
		{
			const auto cvAppKitVersion = floor( NSAppKitVersionNumber );

			if( cvAppKitVersion >= NSAppKitVersionNumber10_8 )
			{
				NSBundle * mainBundle = [NSBundle mainBundle];
				NSDictionary * bundleInfoDict = [mainBundle infoDictionary];
				if( bundleInfoDict != nil )
				{
					NSString * mainNibFleNameStr = [bundleInfoDict valueForKey:@"NSMainNibFile"];

					if( mainNibFleNameStr != nil )
					{
						if( [mainBundle loadNibNamed:mainNibFleNameStr owner:[NSApplication sharedApplication] topLevelObjects:nil] )
						{
							return true;
						}
					}
					else
					{
						NSString * mainMenuNibResource = [mainBundle pathForResource:@"MainMenu" ofType:@"nib"];
						if( mainMenuNibResource != nil )
						{
							if( [mainBundle loadNibNamed:@"MainMenu" owner:[NSApplication sharedApplication] topLevelObjects:nil] )
							{
								return true;
							}
						}

					}
				}
			}

			return false;
		}
		}

		void osxNibCreateDefaultApplicationMenu()
		{
		@autoreleasepool
		{
			if( NSApp == nil )
			{
				return;
			}

			NSMenu * mainAppMenuBar = [[NSMenu alloc] init];
			[NSApp setMainMenu:mainAppMenuBar];

			auto * appMenuItem = [mainAppMenuBar addItemWithTitle:@"" action:nullptr keyEquivalent:@""];
			auto * appMenu = [[NSMenu alloc] init];
			[appMenuItem setSubmenu:appMenu];

			auto * servicesMenuItem = [appMenu addItemWithTitle:@"Services" action:nullptr keyEquivalent:@""];
			auto * servicesMenu = [[NSMenu alloc] init];
			[NSApp setServicesMenu:servicesMenu];
			[servicesMenuItem setSubmenu:servicesMenu];

			auto * windowMenuItem = [mainAppMenuBar addItemWithTitle:@"" action:nullptr keyEquivalent:@""];
			auto * windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
			[NSApp setWindowsMenu:windowMenu];
			[windowMenuItem setSubmenu:windowMenu];

			SEL setAppleMenuSelector = NSSelectorFromString(@"setAppleMenu:");
			[NSApp performSelector:setAppleMenuSelector withObject:appMenu];

			[mainAppMenuBar release];
		}
		}

		bool osxCheckAppKitFrameworkVersion( NSAppKitVersion pRequiredVersion )
		{
			const auto cvAppKitVersion = floor( NSAppKitVersionNumber );

			if( cvAppKitVersion >= pRequiredVersion )
			{
				return true;
			}

			return false;
		}

		const char * osxQueryCGErrorMessage( CGError pCGError )
		{
			switch( pCGError )
			{
			case kCGErrorSuccess           : return "kCGErrorSuccess"           ;
			case kCGErrorFailure           : return "kCGErrorFailure"           ;
			case kCGErrorIllegalArgument   : return "kCGErrorIllegalArgument"   ;
			case kCGErrorInvalidConnection : return "kCGErrorInvalidConnection" ;
			case kCGErrorInvalidContext    : return "kCGErrorInvalidContext"    ;
			case kCGErrorCannotComplete    : return "kCGErrorCannotComplete"    ;
			case kCGErrorNotImplemented    : return "kCGErrorNotImplemented"    ;
			case kCGErrorRangeCheck        : return "kCGErrorRangeCheck"        ;
			case kCGErrorTypeCheck         : return "kCGErrorTypeCheck"         ;
			case kCGErrorInvalidOperation  : return "kCGErrorInvalidOperation"  ;
			case kCGErrorNoneAvailable     : return "kCGErrorNoneAvailable"     ;
			}
			return "<UNKNOWN>";
		}

	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_OSX
