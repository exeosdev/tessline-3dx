
#ifndef __TS3_SYSTEM_DISPLAY_SYSTEM_H__
#define __TS3_SYSTEM_DISPLAY_SYSTEM_H__

#include "sysContext.h"
#include "sysObject.h"
#include "displayCommon.h"
#include "windowCommon.h"

namespace ts3::system
{

	ts3SysDeclareHandle( DisplayAdapter );
	ts3SysDeclareHandle( DisplayOutput );
	ts3SysDeclareHandle( DisplayVideoMode );

	/// @brief
	class DisplayManager : public SysObject
	{
	public:
		explicit DisplayManager( SysContextHandle pSysContext );
		virtual ~DisplayManager() noexcept;

		TS3_FUNC_NO_DISCARD DisplayDriverHandle createDisplayDriver( EDisplayDriverType pDriverID );

		TS3_FUNC_NO_DISCARD DisplayDriverHandle createDisplayDriver();

		TS3_FUNC_NO_DISCARD bool checkDriverSupport( EDisplayDriverType pDriverID ) const;

		TS3_FUNC_NO_DISCARD EDisplayDriverType resolveDisplayDriverID( EDisplayDriverType pDriverID ) const;

		TS3_FUNC_NO_DISCARD DisplayOffset queryDefaultDisplayOffset() const;

		TS3_FUNC_NO_DISCARD DisplaySize queryDefaultDisplaySize() const;

		TS3_FUNC_NO_DISCARD DisplaySize queryMinWindowSize() const;

		TS3_FUNC_NO_DISCARD bool checkFrameGeometry( const FrameGeometry & pFrameGeometry ) const;

		TS3_FUNC_NO_DISCARD FrameGeometry validateFrameGeometry( const FrameGeometry & pFrameGeometry ) const;

	private:
		virtual DisplayDriverHandle _nativeCreateDisplayDriver() = 0;

		virtual void _nativeQueryDefaultDisplayOffset( DisplayOffset & pOutOffset ) const = 0;

		virtual void _nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const = 0;

		virtual void _nativeQueryMinWindowSize( DisplaySize & pOutSize ) const = 0;

	protected:
		struct DisplayManagerPrivateData;
		std::unique_ptr<DisplayManagerPrivateData> _privateData;
	};

	/// @brief
	class DisplayDriver : public SysObject
	{
        friend class DisplayManager;

	public:
		DisplayManagerHandle const mDisplayManager;
		EDisplayDriverType const mDriverType;

	public:
		virtual ~DisplayDriver();

		explicit operator bool() const;

		void syncDisplayConfiguration();

		void reset();

		EColorFormat queryDefaultSystemColorFormat() const;

		TS3_FUNC_NO_DISCARD DisplayAdapter * findAdapter( DisplayAdapterPredicate pPredicate ) const;
		TS3_FUNC_NO_DISCARD DisplayAdapterList findAdapters( DisplayAdapterPredicate pPredicate ) const;

		TS3_FUNC_NO_DISCARD const DisplayAdapterList & getAdapterList() const;
		TS3_FUNC_NO_DISCARD const DisplayOutputList & getOutputList( dsm_index_t pAdapterIndex ) const;

		TS3_FUNC_NO_DISCARD DisplayAdapter * getAdapter( dsm_index_t pAdapterIndex ) const;
		TS3_FUNC_NO_DISCARD DisplayAdapter * getDefaultAdapter() const;

		TS3_FUNC_NO_DISCARD DisplayOutput * getDefaultOutput( dsm_index_t pAdapterIndex = CX_DSM_INDEX_DEFAULT ) const;
		TS3_FUNC_NO_DISCARD DisplayOutput * getOutput( dsm_output_id_t pOutputID ) const;

		TS3_FUNC_NO_DISCARD bool hasActiveAdapters() const;
		TS3_FUNC_NO_DISCARD bool hasAnyAdapters() const;

		TS3_FUNC_NO_DISCARD bool hasValidConfiguration() const;

		TS3_FUNC_NO_DISCARD std::string generateConfigurationDump( const std::string & pLinePrefix = {} ) const;

	protected:
		DisplayDriver( DisplayManagerHandle pDisplayManager, EDisplayDriverType pDriverType );

        static DisplayAdapterDesc & getAdapterDescInternal( DisplayAdapter & pAdapter );

        static DisplayOutputDesc & getOutputDescInternal( DisplayOutput & pOutput );

        static DisplayVideoModeDesc & getVideoModeDescInternal( DisplayVideoMode & pVideoMode );

		template <typename TpAdapter, typename TpDriver>
		Handle<TpAdapter> createAdapter( TpDriver & pDriver)
		{
			auto adapterHandle = createSysObject<TpAdapter>( pDriver );
			_registerAdapter( adapterHandle );
			return adapterHandle;
		}

        template <typename TpOutput, typename TpAdapter>
        Handle<TpOutput> createOutput( TpAdapter & pAdapter )
        {
            auto outputHandle = createSysObject<TpOutput>( pAdapter );
            _registerOutput( pAdapter, outputHandle );
            return outputHandle;
        }

        template <typename TpVideoMode, typename TpOutput>
        Handle<TpVideoMode> createVideoMode( TpOutput & pOutput, EColorFormat pColorFormat )
        {
            auto videoModeHandle = createSysObject<TpVideoMode>( pOutput );
            _registerVideoMode( pOutput, pColorFormat, videoModeHandle );
            return videoModeHandle;
        }

	private:
		/// Platform-level function: enumerates display devices in the system (adapters and their outputs).
		virtual void _nativeEnumDisplayDevices() = 0;

        /// Platform-level function: enumerates supported video modes for a given output and color format.
		virtual void _nativeEnumVideoModes( DisplayOutput & pOutput, EColorFormat pColorFormat ) = 0;

        /// Platform-level function: returns the default color format (OS-/driver-specific).
		virtual EColorFormat _nativeQueryDefaultSystemColorFormat() const = 0;

		void _initializeDisplayConfiguration();
		void _resetDisplayConfiguration();
		void _enumDisplayDevices();
		void _enumVideoModes();
		void _registerAdapter( DisplayAdapterHandle pAdapter );
        void _registerOutput( DisplayAdapter & pAdapter, DisplayOutputHandle pOutput );
        void _registerVideoMode( DisplayOutput & pOutput, EColorFormat pColorFormat, DisplayVideoModeHandle pVideoMode );
		void _validateAdaptersConfiguration();

	protected:
		struct DisplayDriverPrivateData;
		std::unique_ptr<DisplayDriverPrivateData> _privateData;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_DISPLAY_SYSTEM_H__
