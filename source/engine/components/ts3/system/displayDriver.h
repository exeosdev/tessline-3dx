
#ifndef __TS3_SYSTEM_DISPLAY_DRIVER_H__
#define __TS3_SYSTEM_DISPLAY_DRIVER_H__

#include "displayCommon.h"
#include <functional>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidDisplayManager.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32DisplayDriverGeneric.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11DisplayDriverGeneric.h"
#endif

#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
#  include "internal/platform/shared/dxgi/dxgiDisplayDriver.h"
#endif

namespace ts3
{

	using SysDsmAdapterCondPred = std::function<bool( const SysDsmAdapter * )>;
	using SysDsmOutputCondPred = std::function<bool( const SysDsmOutput * )>;

	struct SysDisplayDriverNativeData
	{
		using GenericType = SysDisplayDriverNativeDataGeneric;
		SysDisplayDriverNativeDataGeneric * generic = nullptr;
	#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
		using DXGIType = SysDisplayDriverNativeDataDXGI;
		SysDisplayDriverNativeDataDXGI * dxgi = nullptr;
	#endif
	};

	struct SysDsmAdapterNativeData
	{
		using GenericType = SysDsmAdapterNativeDataGeneric;
		SysDsmAdapterNativeDataGeneric * generic = nullptr;
	#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
		using DXGIType = SysDsmAdapterNativeDataDXGI;
		SysDsmAdapterNativeDataDXGI * dxgi = nullptr;
	#endif
	};

	struct SysDsmOutputNativeData
	{
		using GenericType = SysDsmOutputNativeDataGeneric;
		SysDsmOutputNativeDataGeneric * generic = nullptr;
	#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
		using DXGIType = SysDsmOutputNativeDataDXGI;
		SysDsmOutputNativeDataDXGI * dxgi = nullptr;
	#endif
	};

	struct SysDsmVideoModeNativeData
	{
		using GenericType = SysDsmVideoModeNativeDataGeneric;
		SysDsmVideoModeNativeDataGeneric * generic = nullptr;
	#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
		using DXGIType = SysDsmVideoModeNativeDataDXGI;
		SysDsmVideoModeNativeDataDXGI * dxgi = nullptr;
	#endif
	};

    struct SysDsmVideoMode
    {
    public:
        ESysDisplayDriverType driverType = ESysDisplayDriverType::Unknown;
        SysDisplayDriver * driver = nullptr;
        SysDsmOutput * output = nullptr;
        SysDsmVideoModeNativeData nativeData;
	    SysDsmVideoModeDesc modeDesc;

    public:
	    SysDsmVideoMode( const SysDsmVideoMode & ) = delete;
	    SysDsmVideoMode & operator=( const SysDsmVideoMode & ) = delete;

	    explicit SysDsmVideoMode( SysDsmOutput & pOutput );
        ~SysDsmVideoMode();
    };

    struct SysDsmOutput
    {
    public:
	    using VideoModeList = std::vector<SysDsmVideoMode *>;
	    ESysDisplayDriverType driverType = ESysDisplayDriverType::Unknown;
        SysDisplayDriver * driver = nullptr;
        SysDsmAdapter * adapter = nullptr;
        SysDsmOutputNativeData nativeData;
	    SysDsmOutputDesc outputDesc;
	    std::unordered_map<SysColorFormat, VideoModeList> videoModeMap;

    public:
	    SysDsmOutput( const SysDsmOutput & ) = delete;
	    SysDsmOutput & operator=( const SysDsmOutput & ) = delete;

	    explicit SysDsmOutput( SysDsmAdapter & pAdapter );
        ~SysDsmOutput();
    };

	struct SysDsmAdapter
	{
	public:
		ESysDisplayDriverType driverType = ESysDisplayDriverType::Unknown;
		SysDisplayDriver * driver = nullptr;
		SysDsmAdapterNativeData nativeData;
        SysDsmAdapterDesc adapterDesc;
		std::vector<SysDsmOutput *> outputList;
		SysDsmOutput * defaultOutput = nullptr;

	public:
		SysDsmAdapter( const SysDsmAdapter & ) = delete;
		SysDsmAdapter & operator=( const SysDsmAdapter & ) = delete;

		explicit SysDsmAdapter( SysDisplayDriver & pDisplayDriver );
		~SysDsmAdapter();
	};

	class SysDisplayDriver : public SysBaseObject
	{
		friend class SysDisplayManager;

	public:
		SysDisplayDriverNativeData mNativeData;
		SysDisplayManagerHandle const mDisplayManager;
		ESysDisplayDriverType const mDriverType;

	public:
		SysDisplayDriver( SysDisplayManagerHandle pDisplayManager, ESysDisplayDriverType pDriverType ) noexcept;
		virtual ~SysDisplayDriver() noexcept;

		virtual ESysDisplayDriverType queryDriverID() const noexcept = 0;

		void resetInternalState();

		std::vector<SysDsmAdapterDesc *> enumAdapterList();

		std::vector<SysDsmOutputDesc *> enumOutputList( sys_dsm_index_t pAdapterIndex );

		std::vector<SysDsmVideoModeDesc *> enumVideoModeList( sys_dsm_output_id_t pOutputID, SysColorFormat pFormat );

		TS3_PCL_ATTR_NO_DISCARD SysDsmAdapterDesc * getDefaultAdapter() const;

		TS3_PCL_ATTR_NO_DISCARD SysDsmOutputDesc * getDefaultOutput( sys_dsm_index_t pAdapterIndex ) const;

		TS3_PCL_ATTR_NO_DISCARD SysDsmAdapterDesc * findAdapterIf( const SysDsmAdapterCondPred & pCondPred ) const;

		TS3_PCL_ATTR_NO_DISCARD SysDsmOutputDesc * findOutputIf( sys_dsm_index_t pAdapterIndex, const SysDsmOutputCondPred & pCondPred ) const;

	protected:
		SysDsmAdapter * registerAdapter();
		SysDsmOutput * registerOutput( SysDsmAdapter & pAdapter );
		SysDsmVideoMode * registerVideoMode( SysDsmOutput & pOutputInfo, SysColorFormat pFormat );

	private:
		virtual void _sysResetInternalState() = 0;
		virtual void _sysEnumAdapterList() = 0;
		virtual void _sysEnumOutputList( SysDsmAdapter & pAdapterInfo ) = 0;
		virtual void _sysEnumVideoModeList( SysDsmOutput & pOutputInfo, SysColorFormat pFormat ) = 0;

	protected:
		std::vector<SysDsmAdapter *> _adapterList;
		SysDsmAdapter * _defaultAdapter = nullptr;
		std::unordered_map<sys_dsm_output_id_t, SysDsmOutput *> _internalOutputMap;
		std::unordered_map<sys_dsm_video_mode_id_t , SysDsmVideoMode *> _internalVideoModeMap;
	};

	class SysDisplayDriverGeneric : public SysDisplayDriver
	{
	public:
		explicit SysDisplayDriverGeneric( SysDisplayManagerHandle pDisplayManager ) noexcept;
		virtual ~SysDisplayDriverGeneric() noexcept;

		virtual ESysDisplayDriverType queryDriverID() const noexcept override final;

		static SysDisplayDriverHandle create( SysDisplayManagerHandle pDisplayManager );

	private:
		virtual void _sysResetInternalState() override final;
		virtual void _sysEnumAdapterList() override final;
		virtual void _sysEnumOutputList( SysDsmAdapter & pAdapterInfo ) override final;
		virtual void _sysEnumVideoModeList( SysDsmOutput & pOutputInfo, SysColorFormat pFormat ) override final;
	};

#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
	class SysDisplayDriverDXGI : public SysDisplayDriver
	{
	public:
		explicit SysDisplayDriverDXGI( SysDisplayManagerHandle pDisplayManager ) noexcept;
		virtual ~SysDisplayDriverDXGI() noexcept;

		virtual ESysDisplayDriverType queryDriverID() const noexcept override final;

		static SysDisplayDriverHandle create( SysDisplayManagerHandle pDisplayManager, const SysDisplayDriverCreateInfoDXGI & pCreateInfo );

	private:
		void initialize( const SysDisplayDriverCreateInfoDXGI & pCreateInfo );

		virtual void _sysResetInternalState() override final;
		virtual void _sysEnumAdapterList() override final;
		virtual void _sysEnumOutputList( SysDsmAdapter & pAdapterInfo ) override final;
		virtual void _sysEnumVideoModeList( SysDsmOutput & pOutputInfo, SysColorFormat pFormat ) override final;
	};
#endif

	template <typename TpNativeData>
	inline void sysDsmInitializeNativeData( TpNativeData * pNativeData, ESysDisplayDriverType pDriverID )
	{
		try
		{
			switch ( pDriverID )
			{
				case ESysDisplayDriverType::Generic:
				{
					pNativeData->generic = new typename TpNativeData::GenericType();
					break;
				}
			#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
				case ESysDisplayDriverType::DXGI:
				{
					pNativeData->dxgi = new typename TpNativeData::DXGIType();
					break;
				}
			#endif
				default:
				{
					throw 0;
				}
			}
		}
		catch( ... )
		{
		}
	}

	template <typename TpNativeData>
	inline void sysDsmReleaseNativeData( TpNativeData * pNativeData, ESysDisplayDriverType pDriverType )
	{
		try
		{
			switch( pDriverType )
			{
				case ESysDisplayDriverType::Generic:
				{
					if( pNativeData->generic != nullptr )
					{
						delete pNativeData->generic;
						pNativeData->generic = nullptr;
					}
					break;
				}
			#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
				case ESysDisplayDriverType::DXGI:
				{
					if( pNativeData->dxgi != nullptr )
					{
						delete pNativeData->dxgi;
						pNativeData->dxgi = nullptr;
					}
					break;
				}
			#endif
			}
		}
		catch( ... )
		{
		}
	}

}

#endif // __TS3_SYSTEM_DISPLAY_DRIVER_H__
