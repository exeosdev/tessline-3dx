
#ifndef __TS3_SYSTEM_DISPLAY_H__
#define __TS3_SYSTEM_DISPLAY_H__

#include "sysContext.h"
#include "sysObject.h"
#include "displayCommon.h"
#include "windowCommon.h"

namespace ts3::system
{

    struct DisplayManagerNativeData;
    struct DisplayDriverNativeData;
    struct DisplayAdapterNativeData;
    struct DisplayOutputNativeData;
    struct DisplayVideoModeNativeData;

    ts3SysDeclareHandle( DisplayManager );
    ts3SysDeclareHandle( DisplayDriver );
    ts3SysDeclareHandle( DisplayAdapter );
    ts3SysDeclareHandle( DisplayOutput );
    ts3SysDeclareHandle( DisplayVideoMode );

    /// @brief
    class DisplayManager : public SysObject
    {
    public:
        struct ObjectInternalData;
        std::unique_ptr<ObjectInternalData> const mInternal;
        const DisplayManagerNativeData * const mNativeData = nullptr;

    public:
        explicit DisplayManager( SysContextHandle pSysContext );
        virtual ~DisplayManager() noexcept;

        TS3_PCL_ATTR_NO_DISCARD DisplayDriverHandle createDisplayDriver( EDisplayDriverType pDriverID );

        TS3_PCL_ATTR_NO_DISCARD bool checkDriverSupport( EDisplayDriverType pDriverID ) const;

        TS3_PCL_ATTR_NO_DISCARD EDisplayDriverType resolveDisplayDriverID( EDisplayDriverType pDriverID ) const;

        TS3_PCL_ATTR_NO_DISCARD DisplaySize queryDefaultDisplaySize() const;

        TS3_PCL_ATTR_NO_DISCARD DisplaySize queryMinWindowSize() const;

        TS3_SYSTEM_API_NODISCARD bool checkWindowGeometry( const WindowGeometry & pWindowGeometry ) const;

        TS3_SYSTEM_API_NODISCARD WindowGeometry validateWindowGeometry( const WindowGeometry & pWindowGeometry ) const;

    private:
        void _nativeConstructor();
        void _nativeDestructor() noexcept;
        void _nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const;
        void _nativeQueryMinWindowSize( DisplaySize & pOutSize ) const;
    };

    /// @brief
    class DisplayDriver : public SysObject
    {
    public:
        struct ObjectInternalData;
        DisplayManager * const mDisplayManager;
        EDisplayDriverType const mDriverType;
        std::unique_ptr<ObjectInternalData> const mInternal;
        const DisplayDriverNativeData * const mNativeData = nullptr;

    protected:
        DisplayDriver( DisplayManager * pDisplayManager,
                       EDisplayDriverType pDriverType );

    public:
        virtual ~DisplayDriver();

        void syncDisplayConfiguration();

        void reset();

        EColorFormat queryDefaultSystemColorFormat() const;

        TS3_PCL_ATTR_NO_DISCARD const DisplayAdapterList & getAdapterList() const;
        TS3_PCL_ATTR_NO_DISCARD const DisplayOutputList & getOutputList( dsm_index_t pAdapterIndex ) const;

        TS3_PCL_ATTR_NO_DISCARD DisplayAdapter * getAdapter( dsm_index_t pAdapterIndex ) const;
        TS3_PCL_ATTR_NO_DISCARD DisplayAdapter * getDefaultAdapter() const;

        TS3_PCL_ATTR_NO_DISCARD DisplayOutput * getDefaultOutput( dsm_index_t pAdapterIndex = CX_DSM_INDEX_DEFAULT ) const;
        TS3_PCL_ATTR_NO_DISCARD DisplayOutput * getOutput( dsm_output_id_t pOutputID ) const;

        TS3_PCL_ATTR_NO_DISCARD bool hasActiveAdapters() const;
        TS3_PCL_ATTR_NO_DISCARD bool hasAnyAdapters() const;

        TS3_PCL_ATTR_NO_DISCARD bool hasValidConfiguration() const;

        TS3_PCL_ATTR_NO_DISCARD std::string generateConfigurationDump( const std::string & pLinePrefix = {} ) const;

    protected:
        /// @brief Adds a new adapter and initializes its common properties. Returns the pointer to the created object.
        /// Used by actual driver implementations. Returned pointer is persistent throughout the app lifetime.
        DisplayAdapter * addAdapter();

        /// @brief Adds a new output and initializes its common properties. Returns the pointer to the created object.
        /// Used by actual driver implementations. Returned pointer is persistent throughout the app lifetime.
        DisplayOutput * addOutput( DisplayAdapter & pAdapter );

        /// @brief Adds a new video mode and initializes its common properties. Returns the pointer to the created object.
        /// Used by actual driver implementations. Returned pointer is persistent throughout the app lifetime.
        DisplayVideoMode * addVideoMode( DisplayOutput & pOutput, EColorFormat pColorFormat );

    private:
        void _initializeDisplayConfiguration();
        void _resetDisplayConfiguration();

        void _enumDisplayDevices();
        void _enumDisplayDevicesCheckDefaultAdapter();
        void _enumDisplayDevicesCheckDefaultOutput( DisplayAdapter & pAdapter );
        void _enumVideoModes();

        DisplayAdapter * _getAdapter( dsm_index_t pAdapterIndex ) const;
        DisplayOutput * _getOutput( dsm_output_id_t pOutputID ) const;

        virtual void _drvEnumDisplayDevices() = 0;
        virtual void _drvEnumVideoModes( DisplayOutput & pOutput, EColorFormat pColorFormat ) = 0;
        virtual EColorFormat _drvQueryDefaultSystemColorFormat() const = 0;
    };

    /// @brief
    class DisplayAdapter
    {
    public:
        struct ObjectInternalData;
        DisplayDriver * const mDisplayDriver;
        EDisplayDriverType const mDriverType;
        std::unique_ptr<ObjectInternalData> const mInternal;
        const DisplayAdapterDesc * const mDesc = nullptr;
        const DisplayAdapterNativeData * const mNativeData = nullptr;

    public:
        explicit DisplayAdapter( DisplayDriver * pDisplayDriver );
        virtual ~DisplayAdapter();

        TS3_PCL_ATTR_NO_DISCARD const DisplayOutputList & getOutputList() const;

        TS3_PCL_ATTR_NO_DISCARD DisplayOutput * getOutput( dsm_index_t pOutputIndex ) const;
        TS3_PCL_ATTR_NO_DISCARD DisplayOutput * getDefaultOutput() const;

        TS3_PCL_ATTR_NO_DISCARD bool isActiveAdapter() const;
        TS3_PCL_ATTR_NO_DISCARD bool isPrimaryAdapter() const;
        TS3_PCL_ATTR_NO_DISCARD bool hasActiveOutputs() const;
        TS3_PCL_ATTR_NO_DISCARD bool hasAnyOutputs() const;
    };

    /// @brief
    class DisplayOutput
    {
    public:
        struct ObjectInternalData;
        DisplayDriver * const mDisplayDriver;
        DisplayAdapter * const mParentAdapter;
        EDisplayDriverType const mDriverType;
        std::unique_ptr<ObjectInternalData> const mInternal;
        const DisplayOutputDesc * const mDesc = nullptr;
        const DisplayOutputNativeData * const mNativeData = nullptr;

    public:
        explicit DisplayOutput( DisplayAdapter * pDisplayAdapter );
        virtual ~DisplayOutput();

        TS3_PCL_ATTR_NO_DISCARD ArrayView<const EColorFormat> getSupportedColorFormatList() const;

        TS3_PCL_ATTR_NO_DISCARD bool checkVideoSettingsSupport( const DisplayVideoSettings & pVideoSettings ) const;
        TS3_PCL_ATTR_NO_DISCARD bool checkVideoSettingsSupport( const DisplayVideoSettings & pVideoSettings, EColorFormat pColorFormat ) const;

        TS3_PCL_ATTR_NO_DISCARD const DisplayVideoModeList & getVideoModeList() const;
        TS3_PCL_ATTR_NO_DISCARD const DisplayVideoModeList & getVideoModeList( EColorFormat pColorFormat ) const;

        TS3_PCL_ATTR_NO_DISCARD bool isActiveOutput() const;
        TS3_PCL_ATTR_NO_DISCARD bool isPrimaryOutput() const;

        TS3_PCL_ATTR_NO_DISCARD bool isColorFormatSupported( EColorFormat pColorFormat ) const;
    };

    /// @brief
    class DisplayVideoMode
    {
    public:
        struct ObjectInternalData;
        DisplayDriver * const mDisplayDriver;
        DisplayOutput * const mParentOutput;
        EDisplayDriverType const mDriverType;
        std::unique_ptr<ObjectInternalData> const mInternal;
        const DisplayVideoModeDesc * const mDesc = nullptr;
        const DisplayVideoModeNativeData * const mNativeData = nullptr;

    public:
        explicit DisplayVideoMode( DisplayOutput * pDisplayOutput );
        virtual ~DisplayVideoMode();
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_DISPLAY_H__
