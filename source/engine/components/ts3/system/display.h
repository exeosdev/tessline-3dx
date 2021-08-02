
#ifndef __TS3_SYSTEM_DISPLAY_MANAGER_H__
#define __TS3_SYSTEM_DISPLAY_MANAGER_H__

#include "displayCommon.h"

namespace ts3
{

    ts3DeclareSysHandle( SysDsmDisplayManager );

    SysDsmDisplayManagerHandle sysDsmInitializeDisplayManager;

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

		static bool checkDisplayDriverSupport( ESysDsmDisplayDriverType pDriverID );

		static ESysDsmDisplayDriverType resolveDisplayDriverID( ESysDsmDisplayDriverType pDriverID );

	private:
		void _sysInitialize();
		void _sysRelease() noexcept;
		void _sysQueryDisplaySize( SysDisplaySize & pDisplaySize ) const;
		void _sysQueryMinWindowSize( SysDisplaySize & pMinWindowSize ) const;
	};

}

#endif // __TS3_SYSTEM_DISPLAY_MANAGER_H__
