
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
    class DisplayAdapter
    {
    public:
        struct ObjectPrivateData;
        DisplayDriver * const mDisplayDriver;
        std::unique_ptr<ObjectPrivateData> const mPrivate;
        const DisplayAdapterDesc * const mDesc = nullptr;
        const DisplayAdapterNativeData * const mNativeData = nullptr;

    public:
        explicit DisplayAdapter( DisplayDriver * pDisplayDriver );
        virtual ~DisplayAdapter();

        TS3_PCL_ATTR_NO_DISCARD const DisplayOutputList & getOutputList() const;

        TS3_PCL_ATTR_NO_DISCARD DisplayOutput * getOutput( dsm_index_t pOutputIndex ) const;
        TS3_PCL_ATTR_NO_DISCARD DisplayOutput * getDefaultOutput() const;

        TS3_PCL_ATTR_NO_DISCARD bool isActive() const;
        TS3_PCL_ATTR_NO_DISCARD bool isPrimaryAdapter() const;
        TS3_PCL_ATTR_NO_DISCARD bool hasActiveOutputs() const;
    };

    /// @brief
    class DisplayOutput
    {
    public:
        struct ObjectPrivateData;
        DisplayDriver * const mDisplayDriver;
        DisplayAdapter * const mParentAdapter;
        std::unique_ptr<ObjectPrivateData> const mPrivate;
        const DisplayOutputDesc * const mDesc = nullptr;
        const DisplayOutputNativeData * const mNativeData = nullptr;

    public:
        explicit DisplayOutput( DisplayAdapter * pDisplayAdapter );
        virtual ~DisplayOutput();

        TS3_PCL_ATTR_NO_DISCARD bool checkVideoSettingsSupported( const DisplayVideoSettings & pVideoSettings ) const;

        TS3_PCL_ATTR_NO_DISCARD bool checkVideoSettingsSupported( const DisplayVideoSettings & pVideoSettings,
                                                                  ColorFormat pColorFormat ) const;

        TS3_PCL_ATTR_NO_DISCARD const DisplayVideoModeList & getVideoModeList() const;

        TS3_PCL_ATTR_NO_DISCARD const DisplayVideoModeList & getVideoModeList( ColorFormat pColorFormat ) const;

        TS3_PCL_ATTR_NO_DISCARD bool isActive() const;
        TS3_PCL_ATTR_NO_DISCARD bool isPrimaryOutput() const;
    };

    /// @brief
    class DisplayVideoMode
    {
    public:
        struct ObjectPrivateData;
        DisplayDriver * const mDisplayDriver;
        DisplayOutput * const mParentOutput;
        std::unique_ptr<ObjectPrivateData> const mPrivate;
        const DisplayVideoModeDesc * const mDesc = nullptr;
        const DisplayVideoModeNativeData * const mNativeData = nullptr;

    public:
        explicit DisplayVideoMode( DisplayOutput * pDisplayOutput );
        virtual ~DisplayVideoMode();
    };

    /// @brief
    class DisplayDriver : public SysObject
    {
    public:
        struct ObjectPrivateData;
        DisplayManager * const mDisplayManager;
        EDisplayDriverType const mDriverType;
        std::unique_ptr<ObjectPrivateData> const mPrivate;
        const DisplayDriverNativeData * const mNativeData = nullptr;

    protected:
        DisplayDriver( DisplayManager * pDisplayManager,
                       EDisplayDriverType pDriverType );

    public:
        virtual ~DisplayDriver();

        void initializeDisplayConfiguration();
        void resetDisplayConfiguration();

        void enumVideoModes( dsm_output_id_t pOutputID, ColorFormat pColorFormat );

        TS3_PCL_ATTR_NO_DISCARD const DisplayAdapterList & getAdapterList() const;
        TS3_PCL_ATTR_NO_DISCARD const DisplayOutputList & getOutputList( dsm_index_t pAdapterIndex ) const;

        TS3_PCL_ATTR_NO_DISCARD DisplayAdapter * getAdapter( dsm_index_t pAdapterIndex ) const;
        TS3_PCL_ATTR_NO_DISCARD DisplayAdapter * getDefaultAdapter() const;
        TS3_PCL_ATTR_NO_DISCARD DisplayOutput * getDefaultOutput( dsm_index_t pAdapterIndex = CX_DSM_INDEX_INVALID ) const;
        TS3_PCL_ATTR_NO_DISCARD DisplayOutput * getOutput( dsm_output_id_t pOutputID ) const;

        TS3_PCL_ATTR_NO_DISCARD static dsm_output_id_t queryOutputID( dsm_index_t pAdapterIndex, dsm_index_t pOutputIndex );

        TS3_PCL_ATTR_NO_DISCARD static ColorFormat resolveSystemColorFormat( ColorFormat pColorFormat );

    protected:
        DisplayAdapter * addAdapter();
        DisplayOutput * addOutput( DisplayAdapter & pAdapter );
        DisplayVideoMode * addVideoMode( DisplayOutput & pOutput, ColorFormat pColorFormat );

    private:
        void _initializeDisplayConfiguration();
        void _resetDisplayConfiguration();
        void _resetVideoModeData( DisplayOutput & pOutput, ColorFormat pColorFormat );

        void _enumAdapters();
        void _enumOutputs( DisplayAdapter & pAdapter );
        void _enumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat );

        DisplayAdapter * _getAdapter( dsm_index_t pAdapterIndex ) const;
        DisplayOutput * _getOutput( dsm_output_id_t pOutputID ) const;

        virtual void _nativeEnumAdapters() = 0;
        virtual void _nativeEnumOutputs( DisplayAdapter & pAdapter ) = 0;
        virtual void _nativeEnumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat ) = 0;

        virtual void _nativeDestroyAdapter( DisplayAdapter & pAdapter ) = 0;
        virtual void _nativeDestroyOutput( DisplayOutput & pOutput ) = 0;
        virtual void _nativeDestroyVideoMode( DisplayVideoMode & pVideoMode ) = 0;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_DISPLAY_DRIVER_H__
