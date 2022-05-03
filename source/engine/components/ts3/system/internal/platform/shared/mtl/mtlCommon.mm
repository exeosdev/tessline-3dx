
#include "mtlCommon.h"

namespace ts3::system
{

    namespace platform
    {

        MTLPixelFormat mtlChoosePixelFormatForVisualConfig( const VisualConfig & pVisualConfig )
        {
            switch( pVisualConfig.colorFormat )
            {
                ts3CaseReturn( EColorFormat::B8G8R8,       MTLPixelFormatBGRA8Unorm );
                ts3CaseReturn( EColorFormat::B8G8R8A8,     MTLPixelFormatBGRA8Unorm );
                ts3CaseReturn( EColorFormat::B8G8R8A8SRGB, MTLPixelFormatBGRA8Unorm_sRGB );
                ts3CaseReturn( EColorFormat::B8G8R8X8,     MTLPixelFormatBGRA8Unorm );
                ts3CaseReturn( EColorFormat::R8G8B8A8,     MTLPixelFormatBGRA8Unorm );
                ts3CaseReturn( EColorFormat::R8G8B8A8SRGB, MTLPixelFormatBGRA8Unorm_sRGB );
                ts3CaseReturn( EColorFormat::R8G8B8X8,     MTLPixelFormatBGRA8Unorm );
                ts3CaseReturn( EColorFormat::R10G10B10A2,  MTLPixelFormatRGB10A2Unorm );

                default:
                {
                    break;
                }
            }

            return MTLPixelFormatBGRA8Unorm;
        }

    }

}