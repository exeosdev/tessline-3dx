
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_DISPLAY_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_DISPLAY_H__

#include "androidCommon.h"
#include <ts3/system/displayConfiguration.h>
#include <ts3/system/displaySystem.h>

namespace ts3::system
{

	ts3SysDeclareHandle( AndroidDisplayManager );
	ts3SysDeclareHandle( AndroidDisplayDriver );

	namespace platform
	{

		using AndroidDisplayManagerNativeData = AndroidNativeDataCommon;
		using AndroidDisplayDriverNativeData = AndroidNativeDataCommon;
		using AndroidDisplayAdapterNativeData = AndroidNativeDataCommon;
		using AndroidDisplayOutputNativeData = AndroidNativeDataCommon;
		using AndroidDisplayVideoModeNativeData = AndroidNativeDataCommon;

	}

	/// @brief
	class AndroidDisplayAdapter : public AndroidNativeObject<DisplayAdapter, platform::AndroidDisplayAdapterNativeData>
	{
		friend class AndroidDisplayDriver;

	public:
		explicit AndroidDisplayAdapter( AndroidDisplayDriver & pDisplayDriver );
		virtual ~AndroidDisplayAdapter() noexcept;
	};

	/// @brief
	class AndroidDisplayOutput : public AndroidNativeObject<DisplayOutput, platform::AndroidDisplayOutputNativeData>
	{
		friend class AndroidDisplayDriver;

	public:
		explicit AndroidDisplayOutput( AndroidDisplayAdapter & pDisplayAdapter );
		virtual ~AndroidDisplayOutput() noexcept;
	};

	/// @brief
	class AndroidDisplayVideoMode : public AndroidNativeObject<DisplayVideoMode, platform::AndroidDisplayVideoModeNativeData>
	{
		friend class AndroidDisplayDriver;

	public:
		explicit AndroidDisplayVideoMode( AndroidDisplayOutput & pDisplayOutput );
		virtual ~AndroidDisplayVideoMode() noexcept;
	};

	/// @brief
	class AndroidDisplayManager : public AndroidNativeObject<DisplayManager, platform::AndroidDisplayManagerNativeData>
	{
	public:
		explicit AndroidDisplayManager( SysContextHandle pSysContext );
		virtual ~AndroidDisplayManager() noexcept;

	private:
		virtual DisplayDriverHandle _nativeCreateDisplayDriver() override final;

		virtual void _nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const override final;

		virtual void _nativeQueryMinWindowSize( DisplaySize & pOutSize ) const override final;
	};

	/// @brief
	class AndroidDisplayDriver : public AndroidNativeObject<DisplayDriver, platform::AndroidDisplayDriverNativeData>
	{
	public:
		explicit AndroidDisplayDriver( AndroidDisplayManagerHandle pDisplayManager );
		virtual ~AndroidDisplayDriver() noexcept;

	private:
		virtual void _nativeEnumDisplayDevices() override final;

		virtual void _nativeEnumVideoModes( DisplayOutput & pOutput, EColorFormat pColorFormat ) override final;

		virtual EColorFormat _nativeQueryDefaultSystemColorFormat() const override final;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_DISPLAY_H__
