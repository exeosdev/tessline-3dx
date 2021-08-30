
#ifndef __TS3_SYSTEM_DISPLAY_DRIVER_H__
#define __TS3_SYSTEM_DISPLAY_DRIVER_H__

#include "sysContext.h"
#include "sysObject.h"
#include "displayCommon.h"

namespace ts3::system
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
        struct DriverPrivateData;
        DisplayManager * const mDisplayManager;
        EDisplayDriverType const mDriverType;
        std::unique_ptr<DriverPrivateData> const mPrivateData;
        const DisplayDriverNativeData * const mNativeData;

    public:
        DisplayDriver( DisplayManager * pDisplayManager, EDisplayDriverType pDriverType );
        virtual ~DisplayDriver();

        void initialize();
        void release();

        void resetDisplayConfiguration();
        void syncDisplayConfiguration();

        TS3_PCL_ATTR_NO_DISCARD const DisplayAdapterList & getAdapterList() const;
        TS3_PCL_ATTR_NO_DISCARD const DisplayOutputList & getOutputList( dsm_index_t pAdapterIndex ) const;

        TS3_PCL_ATTR_NO_DISCARD DisplayAdapter * getAdapter( dsm_index_t pAdapterIndex ) const;
        TS3_PCL_ATTR_NO_DISCARD DisplayAdapter * getDefaultAdapter() const;
        TS3_PCL_ATTR_NO_DISCARD DisplayOutput * getDefaultOutput( dsm_index_t pAdapterIndex = CX_DSM_INDEX_INVALID ) const;

        TS3_PCL_ATTR_NO_DISCARD bool isInitialized() const;

        TS3_PCL_ATTR_NO_DISCARD static dsm_output_id_t queryOutputID( dsm_index_t pAdapterIndex, dsm_index_t pOutputIndex );

        TS3_PCL_ATTR_NO_DISCARD static ColorFormat resolveSystemColorFormat( ColorFormat pColorFormat );

    protected:
        DisplayAdapter * registerAdapter();
        DisplayOutput * registerOutput( DisplayAdapter & pAdapter );
        DisplayVideoMode * registerVideoMode( DisplayOutput & pOutput, ColorFormat pColorFormat );

    private:
        virtual void _nativeInitialize() = 0;
        virtual void _nativeRelease() = 0;
        virtual void _nativeResetDisplayConfiguration() = 0;
        virtual void _nativeSyncDisplayConfiguration() = 0;
    };

    /// @brief
    class DisplayAdapter : public SysObject
    {
    public:
        struct AdapterPrivateData;
        DisplayDriver * const mDisplayDriver;
        std::unique_ptr<AdapterPrivateData> const mPrivateData;
        const DisplayAdapterDesc * const mDesc;
        const DisplayAdapterNativeData * const mNativeData;

    public:
        explicit DisplayAdapter( DisplayDriver * pDisplayDriver );
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
        struct OutputPrivateData;
        DisplayDriver * const mDisplayDriver;
        DisplayAdapter * const mParentAdapter;
        std::unique_ptr<OutputPrivateData> const mPrivateData;
        const DisplayOutputDesc * const mDesc;
        const DisplayOutputNativeData * const mNativeData;

    public:
        explicit DisplayOutput( DisplayAdapter * pDisplayAdapter );
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
        struct VideoModePrivateData;
        DisplayDriver * const mDisplayDriver;
        DisplayOutput * const mParentOutput;
        std::unique_ptr<VideoModePrivateData> const mPrivateData;
        const DisplayVideoModeDesc * const mDesc;
        const DisplayVideoModeNativeData * const mNativeData;

    public:
        explicit DisplayVideoMode( DisplayOutput * pDisplayOutput );
        virtual ~DisplayVideoMode();
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_DISPLAY_DRIVER_H__
