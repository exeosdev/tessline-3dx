
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

    protected:
        explicit DisplayAdapter( DisplayDriver & pDisplayDriver );

        uint32 validateOutputsConfiguration();

        DisplayAdapterDesc & getAdapterDescInternal();

        template <typename TpOutput, typename TpAdapter>
        Handle<TpOutput> createOutput( TpAdapter & pAdapter )
        {
            auto outputObject = createSysObject<TpOutput>( pAdapter );
            _registerOutput( outputObject );
            return outputObject;
        }

    private:
        void _registerOutput( DisplayOutputHandle pOutput );

    protected:
        struct DisplayAdapterPrivateData;
        std::unique_ptr<DisplayAdapterPrivateData> _privateData;
    };

    /// @brief
    class DisplayOutput : public SysObject
    {
        friend class DisplayAdapter;

    public:
        EDisplayDriverType const mDriverType;
        DisplayDriver * const mDisplayDriver;
        DisplayAdapter * const mParentAdapter;

    public:
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

    protected:
        explicit DisplayOutput( DisplayAdapter & pDisplayAdapter );

        uint32 validateVideoModesConfiguration( EColorFormat pColorFormat );

        DisplayOutputDesc & getOutputDescInternal();

        template <typename TpVideoMode, typename TpOutput>
        Handle<TpVideoMode> createVideoMode( TpOutput & pOutput, EColorFormat pColorFormat )
        {
            auto videoModeObject = createSysObject<TpVideoMode>( pOutput );
            _registerVideoMode( pColorFormat, videoModeObject );
            return videoModeObject;
        }

    private:
        void _registerVideoMode( EColorFormat pColorFormat, DisplayVideoModeHandle pVideoMode );

    protected:
        struct DisplayOutputPrivateData;
        std::unique_ptr<DisplayOutputPrivateData> _privateData;
    };

    /// @brief
    class DisplayVideoMode : public SysObject
    {
        friend class DisplayOutput;

    public:
        EDisplayDriverType const mDriverType;
        DisplayDriver * const mDisplayDriver;
        DisplayOutput * const mParentOutput;

    public:
        virtual ~DisplayVideoMode() noexcept;

        TS3_FUNC_NO_DISCARD const DisplayVideoModeDesc & getModeDesc() const;

    protected:
        explicit DisplayVideoMode( DisplayOutput & pDisplayOutput );

        DisplayVideoModeDesc & getModeDescInternal();

    protected:
        struct DisplayVideoModePrivateData;
        std::unique_ptr<DisplayVideoModePrivateData> _privateData;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_DISPLAY_CONFIGURATION_H__
