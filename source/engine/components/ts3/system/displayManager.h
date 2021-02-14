
#ifndef __TS3_SYSTEM_DISPLAY_MANAGER_H__
#define __TS3_SYSTEM_DISPLAY_MANAGER_H__

#include "displayCommon.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidDisplayManager.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32DisplayManager.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11DisplayManager.h"
#endif

namespace ts3
{

#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
	struct SysDisplayDriverCreateInfoDXGI;
#endif

	class SysDisplayManager : public SysBaseObject
	{
	public:
		SysDisplayManagerNativeData mNativeData;

		explicit SysDisplayManager( SysContextHandle pSysContext ) noexcept;
		virtual ~SysDisplayManager() noexcept;

		static SysDisplayManagerHandle create( SysContextHandle pSysContext );

	#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
		SysDisplayDriverHandle createDisplayDriverDXGI( const SysDisplayDriverCreateInfoDXGI & pCreateInfo );
	#endif

		SysDisplayDriverHandle createDisplayDriverGeneric();

		bool validateWindowGeometry( SysWindowGeometry & pWindowGeometry ) const;

		TS3_PCL_ATTR_NO_DISCARD SysDisplaySize queryDisplaySize() const;
		TS3_PCL_ATTR_NO_DISCARD SysDisplaySize queryMinWindowSize() const;

		static bool checkDisplayDriverSupport( ESysDisplayDriverType pDriverID );

		static ESysDisplayDriverType resolveDisplayDriverID( ESysDisplayDriverType pDriverID );

	private:
		void _sysInitialize();
		void _sysRelease() noexcept;
		void _sysQueryDisplaySize( SysDisplaySize & pDisplaySize ) const;
		void _sysQueryMinWindowSize( SysDisplaySize & pMinWindowSize ) const;
	};

}

#endif // __TS3_SYSTEM_DISPLAY_MANAGER_H__
