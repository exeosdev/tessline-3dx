
#include "osxWindowSystem.h"
#include "osxDisplaySystem.h"
#include "nsOSXWindow.h"
#include "nsOSXEventListener.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_OSX )
namespace ts3::system
{

	namespace platform
	{

		OSXFrameGeometry _osxCheckFrameGeometry( NSScreen * pNSScreen, const FrameGeometry & pFrameGeometry );

		OSXFrameGeometry _osxCheckFrameGeometryUpdate( NSWindow * pNSWindow,
		                                               const FrameGeometry & pFrameGeometry,
		                                               Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags );

	}

	OSXWindowManager::OSXWindowManager( OSXDisplayManagerHandle pDisplayManager )
	: OSXNativeObject( std::move( pDisplayManager ) )
	{}
	
	OSXWindowManager::~OSXWindowManager() noexcept
	{}
	
	WindowHandle OSXWindowManager::_nativeCreateWindow( WindowCreateInfo pCreateInfo )
	{
		auto osxDisplayManager = mDisplayManager->getHandle<OSXDisplayManager>();

		NSArray * nsScreenArray = [NSScreen screens];
		NSScreen * defaultScreen = [nsScreenArray objectAtIndex:0];

		auto windowObject = createSysObject<OSXWindow>( getHandle<OSXWindowManager>() );

		@try
		{
			platform::osxCreateWindow( windowObject->mNativeData, defaultScreen, pCreateInfo );

			platform::osxCreateWindowDefaultView( windowObject->mNativeData );

			platform::osxCreateEventListener( windowObject->mNativeData );

			auto * nsWindow = ( NSWindow * )windowObject->mNativeData.nsWindow;

			if( ![nsWindow isMiniaturized] )
			{
				[NSApp activateIgnoringOtherApps:YES];

				[nsWindow makeKeyAndOrderFront:nil];
				[nsWindow orderFrontRegardless];
			}
		}
		@catch( NSException * pException )
		{
			const auto message = [[pException reason] UTF8String];
			ts3DebugInterrupt();
		}

		return windowObject;
	}

	void OSXWindowManager::_nativeDestroyWindow( Window & pWindow )
	{
	}


	OSXWindow::OSXWindow( OSXWindowManagerHandle pWindowManager )
	: OSXNativeObject( std::move( pWindowManager ), &mNativeData )
	{}
	
	OSXWindow::~OSXWindow() noexcept = default;
	
	void OSXWindow::_nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode )
	{}
	
	void OSXWindow::_nativeSetFullscreenMode( bool pEnable )
	{}
	
	void OSXWindow::_nativeSetTitle( const std::string & pTitle )
	{}
	
	void OSXWindow::_nativeUpdateGeometry( const FrameGeometry & pFrameGeometry, Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags )
	{}

	FrameSize OSXWindow::_nativeGetSize( EFrameSizeMode pSizeMode ) const
	{
		return {};
	}
	

	namespace platform
	{

		void osxCreateWindow( OSXWindowNativeData & pWindowNativeData, NSScreen * pTargetScreen, const WindowCreateInfo & pCreateInfo )
		{
		@autoreleasepool
		{
			NSOSXWindow * nsWindow = nil;

			@try
			{
				const auto frameGeometry = _osxCheckFrameGeometry( pTargetScreen, pCreateInfo.frameGeometry );

				nsWindow = [[NSOSXWindow alloc] initWithContentRect:frameGeometry.frameRect
				                                styleMask:static_cast<NSWindowStyleMask>( frameGeometry.style )
				                                backing:NSBackingStoreBuffered
				                                defer:NO
				                                screen:pTargetScreen];

				if( [nsWindow respondsToSelector:@selector( setTabbingMode: )] )
				{
					[nsWindow setTabbingMode:NSWindowTabbingModeDisallowed];
				}

				pWindowNativeData.nsWindow = nsWindow;
				pWindowNativeData.nsTargetScreen = pTargetScreen;
			}
			@catch( NSException * pException )
			{
				const auto message = [[pException reason] UTF8String];
				ts3DebugInterrupt();
			}
		}
		}

		void osxCreateWindowDefaultView( OSXWindowNativeData & pWindowNativeData )
		{
		@autoreleasepool
		{
			if( ![( id )pWindowNativeData.nsWindow isKindOfClass:[NSOSXWindow class]] )
			{
				return;
			}

			auto * nsWindow = static_cast<NSOSXWindow *>( pWindowNativeData.nsWindow );

			@try
			{
				NSOSXWindowView * nsWindowView = [[NSOSXWindowView alloc] initForWindow:nsWindow];
				pWindowNativeData.nsView = nsWindowView;
			}
			@catch( NSException * pException )
			{
				const auto message = [[pException reason] UTF8String];
				ts3DebugInterrupt();
			}
		}
		}

		void osxSetFrameTitle( NSWindow * pNSWindow, const std::string & pTitle )
		{
		@autoreleasepool
		{
			const char * newTitle = "";
			if( !pTitle.empty() )
			{
				newTitle = pTitle.c_str();
			}

			NSString * nsTitle = [[NSString alloc] initWithUTF8String:newTitle];
			[pNSWindow setTitle:nsTitle];

			[nsTitle release];
		}
		}

		void osxUpdateFrameGeometry( NSWindow * pNSWindow, const FrameGeometry & pFrameGeometry, Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags )
		{
		@autoreleasepool
		{
			const auto frameGeometry = _osxCheckFrameGeometryUpdate( pNSWindow, pFrameGeometry, pUpdateFlags );

			if( pUpdateFlags.isSet( E_FRAME_GEOMETRY_UPDATE_FLAG_STYLE_BIT ) )
			{
				[pNSWindow setStyleMask:static_cast<NSWindowStyleMask>( frameGeometry.style )];
			}

			if( pUpdateFlags.isSet( E_FRAME_GEOMETRY_UPDATE_FLAG_POSITION_BIT ) )
			{
				[pNSWindow setFrameOrigin:frameGeometry.frameRect.origin];
			}

			if( pUpdateFlags.isSet( E_FRAME_GEOMETRY_UPDATE_FLAG_SIZE_BIT ) )
			{
				[pNSWindow setFrame:[pNSWindow frameRectForContentRect:frameGeometry.frameRect] display:YES];
			}
		}
		}

		FrameSize osxGetFrameSize( NSWindow * pNSWindow, EFrameSizeMode pSizeMode )
		{
		@autoreleasepool
		{
			NSRect resultRect;
			if( pSizeMode == EFrameSizeMode::OuterRect )
			{
				resultRect = [pNSWindow frame];
			}
			else
			{
				const auto frameRect = [pNSWindow frame];
				resultRect = [pNSWindow contentRectForFrameRect:frameRect];
			}

			FrameSize frameSize{};
			frameSize.x = static_cast<uint32>( resultRect.size.width );
			frameSize.y = static_cast<uint32>( resultRect.size.height );

			return frameSize;
		}
		}

		NSUInteger osxTranslateFrameStyle( EFrameStyle pStyle )
		{
			//
			constexpr NSUInteger cvCaptionFrameStyle = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable;
			//
			constexpr NSUInteger cvFixedFrameStyle = cvCaptionFrameStyle | NSWindowStyleMaskMiniaturizable;
			//
			constexpr NSUInteger cvOverlayFrameStyle = NSWindowStyleMaskBorderless;
			//
			constexpr NSUInteger cvResizeableFrameStyle = cvFixedFrameStyle | NSWindowStyleMaskResizable;

			NSUInteger resultStyle = Limits<NSUInteger>::maxValue;

			switch ( pStyle )
			{
				case EFrameStyle::Caption:
				{
					resultStyle = cvCaptionFrameStyle;
					break;
				}
				case EFrameStyle::Fixed:
				{
					resultStyle = cvFixedFrameStyle;
					break;
				}
				case EFrameStyle::Overlay:
				{
					resultStyle = cvOverlayFrameStyle;
					break;
				}
				case EFrameStyle::Resizeable:
				{
					resultStyle = cvResizeableFrameStyle;
					break;
				}
				default:
				{
					resultStyle = 0u;
					break;
				}
			}

			return resultStyle;
		}

		OSXFrameGeometry _osxCheckFrameGeometry( NSScreen * pNSScreen, const FrameGeometry & pFrameGeometry )
		{
			// Alternatively: CGDisplayPixelsHigh( kCGDirectMainDisplay ) ??
			const auto screenRect = [pNSScreen frame];

			auto xOrigin = static_cast<CGFloat>( pFrameGeometry.position.x );

			NSRect frameRect{};
			frameRect.origin.x = static_cast<CGFloat>( pFrameGeometry.position.x );
			frameRect.origin.y = static_cast<CGFloat>( pFrameGeometry.position.y );
			frameRect.size.width = static_cast<CGFloat>( pFrameGeometry.size.x );
			frameRect.size.height = static_cast<CGFloat>( pFrameGeometry.size.y );

			frameRect.origin.x = getMaxOf( frameRect.origin.x, 0.0f );
			frameRect.origin.y = screenRect.size.height - frameRect.origin.y - frameRect.size.height;

			OSXFrameGeometry osxGeometry{};
			osxGeometry.frameRect = frameRect;
			osxGeometry.style = osxTranslateFrameStyle( pFrameGeometry.style );

			return osxGeometry;
		}

		OSXFrameGeometry _osxCheckFrameGeometryUpdate( NSWindow * pNSWindow,
		                                               const FrameGeometry & pFrameGeometry,
		                                               Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags )
		{
			OSXFrameGeometry osxGeometry{};

			if( pUpdateFlags.isSet( E_FRAME_GEOMETRY_UPDATE_FLAG_STYLE_BIT ) )
			{
				osxGeometry.style = osxTranslateFrameStyle( pFrameGeometry.style );
			}
			else
			{
				osxGeometry.style = [pNSWindow styleMask];
			}

			if( pUpdateFlags.isSetAnyOf( E_FRAME_GEOMETRY_UPDATE_FLAG_POSITION_BIT | E_FRAME_GEOMETRY_UPDATE_FLAG_SIZE_BIT ) )
			{
				const auto screenRect = [[pNSWindow screen] frame];

				NSRect frameRect{};
				frameRect.origin.x = static_cast<CGFloat>( pFrameGeometry.position.x );
				frameRect.origin.y = static_cast<CGFloat>( pFrameGeometry.position.y );
				frameRect.size.width = static_cast<CGFloat>( pFrameGeometry.size.x );
				frameRect.size.height = static_cast<CGFloat>( pFrameGeometry.size.y );

				frameRect.origin.x = getMaxOf( frameRect.origin.x, 0.0f );
				frameRect.origin.y = screenRect.size.height - frameRect.origin.y - frameRect.size.height;

				osxGeometry.frameRect = frameRect;
			}
			else
			{
				osxGeometry.frameRect = [pNSWindow frame];
			}

			return osxGeometry;
		}

	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_OSX
