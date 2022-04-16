
#include "osxWindowSystem.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_OSX )
namespace ts3::system
{

    namespace platform
    {

        void osxCreateWindow( OSXWindowNativeData & pWindowNativeData, const WindowCreateInfo & pCreateInfo )
        {
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

    }

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_OSX
