
#ifndef __TS3_SYSTEM_DISPLAY_INTERNAL_H__
#define __TS3_SYSTEM_DISPLAY_INTERNAL_H__

#include "../../displayCommon.h"

namespace ts3::system
{

    // The default array with all ColorFormats supported by the library.
    // Used by the common driver layer to iterate over support CFs.
    inline constexpr EColorFormat cvColorFormatArray[8] =
    {
        EColorFormat::B8G8R8,
        EColorFormat::B8G8R8A8,
        EColorFormat::B8G8R8A8SRGB,
        EColorFormat::B8G8R8X8,
        EColorFormat::R8G8B8A8,
        EColorFormat::R8G8B8A8SRGB,
        EColorFormat::R8G8B8X8,
        EColorFormat::R10G10B10A2,
    };

    static_assert( staticArraySize( cvColorFormatArray ) == CX_ENUM_COLOR_FORMAT_COUNT );

    union DisplayOutputIDGen
    {
        struct
        {
            dsm_index_t uAdapterIndex;
            dsm_index_t uOutputIndex;
        };

        dsm_output_id_t outputID = 0;
    };

    union DisplayVideoModeIDGen
    {
        struct
        {
            dsm_output_id_t uOutputID;
            dsm_index_t uColorFormatIndex;
            dsm_index_t uModeIndex;
        };

        dsm_video_mode_id_t modeID = 0u;
    };

    union DisplayVideoSettingsHashGen
    {
        struct
        {
            uint16 uResWidth;
            uint16 uResHeight;
            uint16 uRefreshRate;
            uint8 uFlags;
            uint8 uColorFormatIndex;
        };

        dsm_video_settings_hash_t hashValue = 0u;
    };

    inline dsm_index_t dsmExtractOutputIDAdapterIndex( dsm_output_id_t pOutputID )
    {
        DisplayOutputIDGen outputIDGen;
        outputIDGen.outputID = pOutputID;
        return outputIDGen.uAdapterIndex;
    }

    inline dsm_index_t dsmExtractOutputIDOutputIndex( dsm_output_id_t pOutputID )
    {
        DisplayOutputIDGen outputIDGen;
        outputIDGen.outputID = pOutputID;
        return outputIDGen.uOutputIndex;
    }

} // namespace ts3::system

#endif // __TS3_SYSTEM_DISPLAY_INTERNAL_H__
