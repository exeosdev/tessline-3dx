
#ifndef __TS3_SYSTEM_DISPLAY_DRIVER_INTERNAL_H__
#define __TS3_SYSTEM_DISPLAY_DRIVER_INTERNAL_H__

#include "displayDriver.h"
#include "systemContextNative.h"

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

	class SysDisplayDriverGeneric : public SysDisplayDriver
	{
		friend class SysDisplayDriver;
		friend class SysDisplayDriverGenericImplProxy;
		friend class SysDisplayManager;

	private:
		SysDisplayDriverGeneric( SysDisplayManager * pDisplayManager );

	public:
		virtual ~SysDisplayDriverGeneric();

		virtual ESysDisplayDriverType queryDriverID() const override final;

	private:
		virtual void _drvOnResetInternalState() override final;
		virtual void _drvEnumAdapterList() override final;
		virtual void _drvEnumOutputList( SysDsmAdapter & pAdapterInfo ) override final;
		virtual void _drvEnumVideoModeList( SysDsmOutput & pOutputInfo, SysColorFormat pFormat ) override final;

		static SysDisplayDriver * create( SysDisplayManager * pDisplayManager );
	};

	class SysDisplayDriverGenericImplProxy
	{
	public:
		static void nativeInitializeDriver( SysDisplayDriverGeneric & pDisplayDriver );
		static void nativeReleaseDriver( SysDisplayDriverGeneric & pDisplayDriver ) noexcept;
		static void nativeOnResetInternalState( SysDisplayDriverGeneric & pDisplayDriver ) noexcept;
		static void nativeEnumAdapterList( SysDisplayDriverGeneric & pDisplayDriver );
		static void nativeEnumOutputList( SysDisplayDriverGeneric & pDisplayDriver, SysDsmAdapter & pAdapter );
		static void nativeEnumVideoModeList( SysDisplayDriverGeneric & pDisplayDriver, SysDsmOutput & pOutput, SysColorFormat pFormat );
	};

	template <typename TpNativeData>
	inline TpNativeData * sysCreateDsmNativeData( ESysDisplayDriverType pDriverID )
	{
		TpNativeData * nativeData = nullptr;

		try
		{
			switch ( pDriverID )
			{
				case ESysDisplayDriverType::Generic:
				{
					nativeData = new TpNativeData();
					nativeData->generic = new typename TpNativeData::GenericType();
					break;
				}
			#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
				case ESysDisplayDriverType::DXGI:
				{
					nativeData = new TpNativeData();
					nativeData->dxgi = new typename TpNativeData::DXGIType();
					break;
				}
			#endif
			#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_VULKAN )
				case ESysDisplayDriverType::Vulkan:
				{
					nativeData = new TpNativeData();
					nativeData->vulkan = new typename TpNativeData::VulkanType();
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

		assert( nativeData != nullptr );
		return nativeData;
	}

	template <typename TpNativeData>
	inline void sysDestroyDsmNativeData( TpNativeData * pNativeData, ESysDisplayDriverType pDriverType )
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
			#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_VULKAN )
				case ESysDisplayDriverType::Vulkan:
				{
					if( pNativeData->vulkan != nullptr )
					{
                        delete pNativeData->vulkan;
                        pNativeData->vulkan = nullptr;
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

#endif // __TS3_SYSTEM_DISPLAY_DRIVER_INTERNAL_H__
