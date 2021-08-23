
#ifndef __TS3_SYSTEM_DISPLAY_H__
#define __TS3_SYSTEM_DISPLAY_H__

#include "sysContext.h"
#include "sysObject.h"
#include "displayCommon.h"

namespace ts3
{
namespace system
{

    ts3SysDeclareHandle( DisplayAdapter );
    ts3SysDeclareHandle( DisplayOutput );
    ts3SysDeclareHandle( DisplayVideoMode );

    /// @brief
    class DisplayManager : public SysObject
    {
    public:
        explicit DisplayManager( SysContextHandle pSysContext );
        virtual ~DisplayManager();

        TS3_PCL_ATTR_NO_DISCARD DisplaySize queryDefaultDisplaySize() const;

        TS3_PCL_ATTR_NO_DISCARD DisplaySize queryMinWindowSize() const;

        TS3_PCL_ATTR_NO_DISCARD bool checkDriverSupport( EDisplayDriverType pDriverID ) const;

        TS3_PCL_ATTR_NO_DISCARD EDisplayDriverType resolveDisplayDriverID( EDisplayDriverType pDriverID ) const;

        TS3_PCL_ATTR_NO_DISCARD bool checkWindowGeometry( const math::Pos2i & pWindowPosition,
                                                          const math::Size2u & pWindowSize ) const;

        bool validateWindowGeometry( math::Pos2i & pWindowPosition,
                                     math::Size2u & pWindowSize ) const;

    private:
        virtual void _nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const = 0;
        virtual void _nativeQueryMinWindowSize( DisplaySize & pOutSize ) const = 0;
    };

    /// @brief
    class DisplayDriver : public SysObject
    {
    public:
        using AdapterList = std::vector<DisplayAdapterHandle>;
        using OutputList = std::vector<DisplayOutputHandle>;

    public:
        DisplayDriver( DisplayManagerHandle pDisplayManager );
        virtual ~DisplayDriver();

        void enumerateAdapters();

        void initializeDevicesTopology();

        TS3_PCL_ATTR_NO_DISCARD DisplayOutputHandle getDefaultAdapter() const;

        TS3_PCL_ATTR_NO_DISCARD const AdapterList & getAdapterList() const;

        TS3_PCL_ATTR_NO_DISCARD bool hasActiveAdapters() const;

    private:
        virtual void _nativeEnumerateAdapters() = 0;

    private:
        AdapterList _adapterList;
        DisplayAdapterHandle _primaryAdapter;
    };

    /// @brief
    class DisplayAdapter : public SysObject
    {
    public:
        using OutputList = DisplayDriver::OutputList;
        using VideoModeList = std::vector<DisplayVideoModeHandle>;

    public:
        DisplayDriverHandle const mDisplayDriver;
        DisplayAdapterDesc const mAdapterDesc;

    public:
        DisplayAdapter( DisplayDriverHandle pDisplayDriver, DisplayAdapterDesc pAdapterDesc );
        virtual ~DisplayAdapter();

        void enumerateOutputs();

        TS3_PCL_ATTR_NO_DISCARD uint32 checkVideoSettingsSupported( const DisplayVideoSettings & pVideoSettings ) const;

        TS3_PCL_ATTR_NO_DISCARD DisplayOutputHandle getDefaultOutput() const;

        TS3_PCL_ATTR_NO_DISCARD const OutputList & getOutputList() const;

        TS3_PCL_ATTR_NO_DISCARD bool hasActiveOutputs() const;

        TS3_PCL_ATTR_NO_DISCARD bool isActive() const;

        TS3_PCL_ATTR_NO_DISCARD bool isPrimaryAdapter() const;

    private:
        virtual void _nativeEnumerateOutputs() = 0;

    private:
        OutputList _outputList;
        DisplayOutputHandle _primaryOutput;
    };

    /// @brief
    class DisplayOutput : public SysObject
    {
    public:
        using VideoModeList = DisplayAdapter::VideoModeList;

    public:
        DisplayDriverHandle const mDisplayDriver;
        DisplayAdapterHandle const mParentAdapter;
        DisplayOutputDesc const mOutputDesc;

    public:
        DisplayOutput( DisplayAdapterHandle pDisplayAdapter, DisplayOutputDesc pOutputDesc );
        virtual ~DisplayOutput();

        TS3_PCL_ATTR_NO_DISCARD uint32 checkVideoSettingsSupported( const DisplayVideoSettings & pVideoSettings ) const;

        TS3_PCL_ATTR_NO_DISCARD const VideoModeList & getVideoModeList() const;

        TS3_PCL_ATTR_NO_DISCARD bool isActive() const;

        TS3_PCL_ATTR_NO_DISCARD bool isPrimaryOutput() const;
    };

    /// @brief
    class DisplayVideoMode : public SysObject
    {
    public:
        DisplayDriverHandle const mDisplayDriver;
        DisplayOutputHandle const mParentOutput;
        DisplayVideoModeDesc const mVideoModeDesc;

    public:
        DisplayVideoMode( DisplayOutputHandle pDisplayOutput, DisplayVideoModeDesc pVideoModeDesc );
        virtual ~DisplayVideoMode();

        TS3_PCL_ATTR_NO_DISCARD bool isActive() const;
    };

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_DISPLAY_H__
