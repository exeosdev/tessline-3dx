
#include <ts3/gpuapi/commandSystem.h>
#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/gpuapi/gpuDriver.h>
#include <ts3/gpuapi/presentationLayer.h>

namespace ts3::gpuapi
{

	static const math::RGBAColorU8 sDefaultClearColorDriver0     { 0x11, 0x66, 0xCC, 0xFF };
	static const math::RGBAColorU8 sDefaultClearColorDriverDX11  { 0x77, 0xAA, 0x5F, 0xFF };
	static const math::RGBAColorU8 sDefaultClearColorDriverDX12  { 0x22, 0x88, 0x3F, 0xFF };
	static const math::RGBAColorU8 sDefaultClearColorDriverMTL1  { 0xFF, 0x99, 0x66, 0xFF };
	static const math::RGBAColorU8 sDefaultClearColorDriverGL4   { 0x55, 0x88, 0xAA, 0xFF };
	static const math::RGBAColorU8 sDefaultClearColorDriverGLES3 { 0x7A, 0x00, 0x4D, 0xFF };
	static const math::RGBAColorU8 sDefaultClearColorDriverVK1   { 0x8F, 0x0F, 0x1F, 0xFF };

	GPUDevice::GPUDevice( GPUDriver & pDriver )
	: GPUDriverChildObject( pDriver )
	, mGPUDriverID( pDriver.queryGPUDriverID() )
	, mSysContext( pDriver.mSysContext )
	{
		if( pDriver.isDebugFunctionalityRequested() )
		{
			_internalStateFlags.set( E_INTERNAL_STATE_FLAG_DEBUG_DEVICE_BIT );
		}
	}

	GPUDevice::~GPUDevice() = default;

	GPUBufferHandle GPUDevice::createGPUBuffer( const GPUBufferCreateInfo & pCreateInfo )
	{
	    return _drvCreateGPUBuffer( pCreateInfo );
	}

	SamplerHandle GPUDevice::createSampler( const SamplerCreateInfo & pCreateInfo )
	{
	    return _drvCreateSampler( pCreateInfo );
	}

	ShaderHandle GPUDevice::createShader( const ShaderCreateInfo & pCreateInfo )
	{
	    return _drvCreateShader( pCreateInfo );
	}

	TextureHandle GPUDevice::createTexture( const TextureCreateInfo & pCreateInfo )
	{
	    return _drvCreateTexture( pCreateInfo );
	}

	const math::RGBAColorU8 & GPUDevice::getDefaultClearColor() const
	{
		switch( mGPUDriverID )
		{
			case EGPUDriverID::GDIDirectX11:
				return sDefaultClearColorDriverDX11;

			case EGPUDriverID::GDIDirectX12:
				return sDefaultClearColorDriverDX12;

			case EGPUDriverID::GDIMetal1:
				return sDefaultClearColorDriverMTL1;

			case EGPUDriverID::GDIOpenGLDesktop4:
				return sDefaultClearColorDriverGL4;

			case EGPUDriverID::GDIOpenGLES3:
				return sDefaultClearColorDriverGLES3;

			case EGPUDriverID::GDIVulkan10:
				return sDefaultClearColorDriverVK1;

			default:
				break;
		}
		return sDefaultClearColorDriver0;
	}

	const RenderTargetClearConfig & GPUDevice::getDefaultRenderTargetClearConfig() const
	{
		static const RenderTargetClearConfig defaultRenderTargetClearConfig =
		{
			getDefaultClearColor(),
			1.0f,
			0
		};
		return defaultRenderTargetClearConfig;
	}

	void GPUDevice::setPresentationLayer( PresentationLayerHandle pPresentationLayer )
	{
		if( !_drvOnSetPresentationLayer( pPresentationLayer ) )
		{
			return;
		}
		_presentationLayer = pPresentationLayer;
	}

	CommandSystem * GPUDevice::getCommandSystem() const
	{
		return _commandSystem.get();
	}

	PresentationLayer * GPUDevice::getPresentationLayer() const
	{
		return _presentationLayer.get();
	}

	bool GPUDevice::isDebugDevice() const
	{
		return _internalStateFlags.isSet( E_INTERNAL_STATE_FLAG_DEBUG_DEVICE_BIT );
	}

	bool GPUDevice::_drvOnSetPresentationLayer( PresentationLayerHandle pPresentationLayer )
	{
		return true;
	}

} // namespace ts3::gpuapi
