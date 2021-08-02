
#ifndef __TS3_SYSTEM_DISPLAY_MANAGER_H__
#define __TS3_SYSTEM_DISPLAY_MANAGER_H__

#include "displayCommon.h"

namespace ts3
{
namespace system
{

    ts3DeclareHandle( DsmDisplayManager );

    DsmDisplayManager sysDsmInitializeDisplayManager;

	class DisplayManager : public BaseObject
	{
	public:
		DisplayManagerNativeData mNativeData;

		explicit DisplayManager( ContextHandle pContext ) noexcept;
		virtual ~DisplayManager() noexcept;

		static DisplayManagerHandle create( ContextHandle pContext );

	#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
		DisplayDriverHandle createDisplayDriverDXGI( const DisplayDriverCreateInfoDXGI & pCreateInfo );
	#endif

		DisplayDriverHandle createDisplayDriverGeneric();

		bool validateWindowGeometry( WindowGeometry & pWindowGeometry ) const;

		TS3_PCL_ATTR_NO_DISCARD DisplaySize queryDisplaySize() const;
		TS3_PCL_ATTR_NO_DISCARD DisplaySize queryMinWindowSize() const;

		static bool checkDisplayDriverSupport( EDsmDisplayDriverType pDriverID );

		static EDsmDisplayDriverType resolveDisplayDriverID( EDsmDisplayDriverType pDriverID );

	private:
		void _sysInitialize();
		void _sysRelease() noexcept;
		void _sysQueryDisplaySize( DisplaySize & pDisplaySize ) const;
		void _sysQueryMinWindowSize( DisplaySize & pMinWindowSize ) const;
	};

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_DISPLAY_MANAGER_H__
