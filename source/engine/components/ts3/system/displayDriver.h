
#ifndef __TS3_SYSTEM_DISPLAY_DRIVER_H__
#define __TS3_SYSTEM_DISPLAY_DRIVER_H__

#include "sysContext.h"
#include "sysObject.h"
#include "displayCommon.h"

namespace ts3
{
namespace system
{

    struct DisplayDriverNativeData;
    struct DisplayAdapterNativeData;
    struct DisplayOutputNativeData;
    struct DisplayVideoModeNativeData;

    ts3SysDeclareHandle( DisplayAdapter );
    ts3SysDeclareHandle( DisplayOutput );
    ts3SysDeclareHandle( DisplayVideoMode );

    /// @brief
    class DisplayDriver : public SysObject
    {
    public:
        DisplayManager * const mDisplayManager;
        EDisplayDriverType const mDriverType;

    public:
        DisplayDriver( DisplayManager * pDisplayManager,
                       EDisplayDriverType pDriverType );

        virtual ~DisplayDriver();

        void refreshDisplayConfiguration();

        TS3_PCL_ATTR_NO_DISCARD const DisplayAdapterList & getAdapterList() const;
        TS3_PCL_ATTR_NO_DISCARD const DisplayOutputList & getOutputList( dsm_index_t pAdapterIndex ) const;

        TS3_PCL_ATTR_NO_DISCARD DisplayAdapter * getAdapter( dsm_index_t pAdapterIndex ) const;
        TS3_PCL_ATTR_NO_DISCARD DisplayAdapter * getDefaultAdapter() const;
        TS3_PCL_ATTR_NO_DISCARD DisplayOutput * getDefaultOutput( dsm_index_t pAdapterIndex = CX_DSM_INDEX_INVALID ) const;

        TS3_PCL_ATTR_NO_DISCARD bool checkVideoModeSupport( dsm_output_id_t pOutputID,
                                                            const DisplayVideoSettings & pSettings ) const;

        TS3_PCL_ATTR_NO_DISCARD bool checkVideoModeSupport( dsm_index_t pAdapterIndex,
                                                            dsm_index_t pOutputIndex,
                                                            const DisplayVideoSettings & pSettings ) const;

        TS3_PCL_ATTR_NO_DISCARD static dsm_output_id_t queryOutputID( dsm_index_t pAdapterIndex, dsm_index_t pOutputIndex );

        TS3_PCL_ATTR_NO_DISCARD static ColorFormat resolveSystemColorFormat( ColorFormat pColorFormat );

    private:
        virtual void _nativeSyncDisplayTopology() = 0;
    };

    /// @brief
    class DisplayAdapter : public SysObject
    {
    public:
        DisplayDriver * const mDisplayDriver;
        const DisplayAdapterDesc * const mDesc;

    public:
        DisplayAdapter( DisplayDriver * pDisplayDriver,
                        const DisplayAdapterDesc & pAdapterDesc );

        virtual ~DisplayAdapter();

        void enumerateOutputs();

        TS3_PCL_ATTR_NO_DISCARD const DisplayOutputList & getOutputList() const;

        TS3_PCL_ATTR_NO_DISCARD DisplayOutput * getOutput( dsm_index_t pOutputIndex ) const;
        TS3_PCL_ATTR_NO_DISCARD DisplayOutput * getDefaultOutput() const;

        TS3_PCL_ATTR_NO_DISCARD bool isActive() const;
        TS3_PCL_ATTR_NO_DISCARD bool isPrimaryAdapter() const;
        TS3_PCL_ATTR_NO_DISCARD bool hasActiveOutputs() const;
    };

    /// @brief
    class DisplayOutput : public SysObject
    {
    public:
        DisplayDriver * const mDisplayDriver;
        DisplayAdapter * const mParentAdapter;
        const DisplayOutputDesc * const mDesc;

    public:
        DisplayOutput( DisplayAdapter * pDisplayAdapter,
                       const DisplayOutputDesc & pOutputDesc );

        virtual ~DisplayOutput();

        TS3_PCL_ATTR_NO_DISCARD bool checkVideoSettingsSupported( const DisplayVideoSettings & pVideoSettings,
                                                                  ColorFormat pColorFormat = ColorFormat::SystemNative ) const;

        TS3_PCL_ATTR_NO_DISCARD const DisplayVideoModeList & getVideoModeList( ColorFormat pColorFormat = ColorFormat::SystemNative) const;

        TS3_PCL_ATTR_NO_DISCARD bool isActive() const;
        TS3_PCL_ATTR_NO_DISCARD bool isPrimaryOutput() const;
    };

    /// @brief
    class DisplayVideoMode : public SysObject
    {
    public:
        DisplayDriver * const mDisplayDriver;
        DisplayOutput * const mParentOutput;
        const DisplayVideoModeDesc * const mDesc;

    public:
        DisplayVideoMode( DisplayOutput * pDisplayOutput,
                          const DisplayVideoModeDesc & pVideoModeDesc );

        virtual ~DisplayVideoMode();
    };

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_DISPLAY_DRIVER_H__
