
#ifndef __TS3_SYSTEM_DXGI_DISPLAY_SYSTEM_H__
#define __TS3_SYSTEM_DXGI_DISPLAY_SYSTEM_H__

#include <ts3/system/displayConfiguration.h>
#include <ts3/system/displaySystem.h>
#include "dxgiCommon.h"

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
namespace ts3::system
{

    class DisplayDriverDXGI;
    class DisplayAdapterDXGI;
    class DisplayOutputDXGI;
    class DisplayVideoModeDXGI;

	namespace platform
	{

	    struct DisplayDriverNativeDataDXGI
        {
	        ComPtr<IDXGIFactory1> dxgiFactory;
        };

	    struct DisplayAdapterNativeDataDXGI
        {
	        ComPtr<IDXGIAdapter1> dxgiAdapter;
	        DXGI_ADAPTER_DESC1  dxgiAdapterDesc;
        };

	    struct DisplayOutputNativeDataDXGI
        {
	        ComPtr<IDXGIOutput1> dxgiOutput;
	        DXGI_OUTPUT_DESC  dxgiOutputDesc;
        };

	    struct DisplayVideoModeNativeDataDXGI
        {
	        DXGI_MODE_DESC dxgiModeDesc;
        };

	}
	
	/// @brief
	class DisplayAdapterDXGI : public DisplayAdapter, public NativeObject<platform::DisplayAdapterNativeDataDXGI>
    {
	    friend class DisplayDriverDXGI;

    public:
        explicit DisplayAdapterDXGI( DisplayDriverDXGI & pDisplayDriver );
        virtual ~DisplayAdapterDXGI() noexcept;
    };

	/// @brief
	class DisplayOutputDXGI : public DisplayOutput, public NativeObject<platform::DisplayOutputNativeDataDXGI>
    {
	    friend class DisplayDriverDXGI;

    public:
        explicit DisplayOutputDXGI( DisplayAdapterDXGI & pDisplayAdapter );
        virtual ~DisplayOutputDXGI() noexcept;
    };

	/// @brief
	class DisplayVideoModeDXGI : public DisplayVideoMode, public NativeObject<platform::DisplayVideoModeNativeDataDXGI>
    {
	    friend class DisplayDriverDXGI;

    public:
        explicit DisplayVideoModeDXGI( DisplayOutputDXGI & pDisplayOutput );
        virtual ~DisplayVideoModeDXGI() noexcept;
    };

	/// @brief
	class DisplayDriverDXGI : public DisplayDriver, public NativeObject<platform::DisplayDriverNativeDataDXGI>
    {
    public:
        explicit DisplayDriverDXGI( DisplayManagerHandle pDisplayManager );
        virtual ~DisplayDriverDXGI() noexcept;

    private:
        void _initializeDXGIDriverState();
        void _releaseDXGIDriverState() noexcept;

        void _enumAdapterOutputs( DisplayAdapter & pAdapter );

        virtual void _nativeEnumDisplayDevices() override final;

        virtual void _nativeEnumVideoModes( DisplayOutput & pOutput, EColorFormat pColorFormat ) override final;

        virtual EColorFormat _nativeQueryDefaultSystemColorFormat() const override final;
    };

} // namespace ts3::system
#endif // TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI

#endif // __TS3_SYSTEM_DXGI_DISPLAY_SYSTEM_H__
