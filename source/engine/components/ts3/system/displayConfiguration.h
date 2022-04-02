
#ifndef __TS3_SYSTEM_DISPLAY_CONFIGURATION_H__
#define __TS3_SYSTEM_DISPLAY_CONFIGURATION_H__

#include "displayCommon.h"
#include "sysContext.h"
#include "sysObject.h"

namespace ts3::system
{

    ts3SysDeclareHandle( DisplayAdapter );
    ts3SysDeclareHandle( DisplayOutput );
    ts3SysDeclareHandle( DisplayVideoMode );

    /// @brief
    class DisplayAdapter : public SysObject
    {
        friend class DisplayDriver;

    public:
        EDisplayDriverType const mDriverType;
        DisplayDriver * const mDisplayDriver;

    public:
        explicit DisplayAdapter( DisplayDriver & pDisplayDriver );

        virtual ~DisplayAdapter() noexcept;

        TS3_FUNC_NO_DISCARD DisplayOutput * findOutput( DisplayOutputPredicate pPredicate ) const;
        TS3_FUNC_NO_DISCARD DisplayOutputList findOutputs( DisplayOutputPredicate pPredicate ) const;

        TS3_FUNC_NO_DISCARD DisplayOutput * getOutput( dsm_index_t pOutputIndex ) const;
        TS3_FUNC_NO_DISCARD DisplayOutput * getDefaultOutput() const;

        TS3_FUNC_NO_DISCARD const DisplayAdapterDesc & getAdapterDesc() const;
        TS3_FUNC_NO_DISCARD const DisplayOutputList & getOutputList() const;

        TS3_FUNC_NO_DISCARD bool isActiveAdapter() const;
        TS3_FUNC_NO_DISCARD bool isPrimaryAdapter() const;
        TS3_FUNC_NO_DISCARD bool hasActiveOutputs() const;
        TS3_FUNC_NO_DISCARD bool hasAnyOutputs() const;

    private:
        // Registers an output of this adapter. Adds it to the list and fills internal info.
        // Called by the display driver when the display configuration is initialized.
        void registerOutput( DisplayOutputHandle pOutput );

        // Validates the list of outputs, checks and updates flags, fills the additional pointer-based list.
        // Internally it also calls validateVideoModesConfiguration() for each output and supported color format.
        uint32 validateOutputsConfiguration();

        // Returns a writable adapter desc. Used by the driver to fill the info about this adapter.
        DisplayAdapterDesc & getAdapterDescInternal();

    protected:
        struct DisplayAdapterPrivateData;
        std::unique_ptr<DisplayAdapterPrivateData> _privateData;
    };

    /// @brief
    class DisplayOutput : public SysObject
    {
        friend class DisplayDriver;
        friend class DisplayAdapter;

    public:
        EDisplayDriverType const mDriverType;
        DisplayDriver * const mDisplayDriver;
        DisplayAdapter * const mParentAdapter;

    public:
        explicit DisplayOutput( DisplayAdapter & pDisplayAdapter );
        virtual ~DisplayOutput() noexcept;

        TS3_FUNC_NO_DISCARD ArrayView<const EColorFormat> getSupportedColorFormatList() const;

        TS3_FUNC_NO_DISCARD bool checkVideoSettingsSupport( const DisplayVideoSettings & pVideoSettings,
                                                                EColorFormat pColorFormat = EColorFormat::Unknown ) const;

        TS3_FUNC_NO_DISCARD DisplayVideoMode * findVideoMode( EColorFormat pColorFormat, DisplayVideoModePredicate pPredicate ) const;
        TS3_FUNC_NO_DISCARD DisplayVideoModeList findVideoModes( EColorFormat pColorFormat, DisplayVideoModePredicate pPredicate ) const;

        TS3_FUNC_NO_DISCARD const DisplayOutputDesc & getOutputDesc() const;
        TS3_FUNC_NO_DISCARD const DisplayVideoModeList & getVideoModeList() const;
        TS3_FUNC_NO_DISCARD const DisplayVideoModeList & getVideoModeList( EColorFormat pColorFormat ) const;

        TS3_FUNC_NO_DISCARD bool isActiveOutput() const;
        TS3_FUNC_NO_DISCARD bool isPrimaryOutput() const;

        TS3_FUNC_NO_DISCARD bool isColorFormatSupported( EColorFormat pColorFormat ) const;

    private:
        // Registers a supported video mode for this output. Adds it to the list and fills internal info.
        // Called by the display driver when the display configuration is initialized.
        void registerVideoMode( EColorFormat pColorFormat, DisplayVideoModeHandle pVideoMode );

        uint32 validateVideoModesConfiguration( EColorFormat pColorFormat );

        // Returns a writable output desc. Used by the driver and parent adapter to fill the info about this output.
        DisplayOutputDesc & getOutputDescInternal();

    protected:
        struct DisplayOutputPrivateData;
        std::unique_ptr<DisplayOutputPrivateData> _privateData;
    };

    /// @brief
    class DisplayVideoMode : public SysObject
    {
        friend class DisplayDriver;
        friend class DisplayOutput;

    public:
        EDisplayDriverType const mDriverType;
        DisplayDriver * const mDisplayDriver;
        DisplayOutput * const mParentOutput;

    public:
        explicit DisplayVideoMode( DisplayOutput & pDisplayOutput );
        virtual ~DisplayVideoMode() noexcept;

        TS3_FUNC_NO_DISCARD const DisplayVideoModeDesc & getModeDesc() const;

    private:
        // Returns a writable video mode desc. Used by the driver and parent output to fill the info about this video mode.
        DisplayVideoModeDesc & getModeDescInternal();

    protected:
        struct DisplayVideoModePrivateData;
        std::unique_ptr<DisplayVideoModePrivateData> _privateData;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_DISPLAY_CONFIGURATION_H__
