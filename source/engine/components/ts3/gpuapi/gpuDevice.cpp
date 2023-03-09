
#include <ts3/gpuapi/commandSystem.h>
#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/gpuapi/gpuDriver.h>
#include <ts3/gpuapi/presentationLayer.h>
#include <ts3/gpuapi/state/pipelineImmutableStateCache.h>

namespace ts3::gpuapi
{
	
	static const math::RGBAColorU8 sDefaultClearColorDriver0     { 0x11, 0x66, 0xCC, 0xFF };
	static const math::RGBAColorU8 sDefaultClearColorDriverDX11  { 0x77, 0xAA, 0x5F, 0xFF };
	static const math::RGBAColorU8 sDefaultClearColorDriverDX12  { 0x22, 0x88, 0x3F, 0xFF };
	static const math::RGBAColorU8 sDefaultClearColorDriverMTL1  { 0xFF, 0x99, 0x66, 0xFF };
	static const math::RGBAColorU8 sDefaultClearColorDriverGL4   { 0x55, 0x88, 0xAA, 0xFF };
	static const math::RGBAColorU8 sDefaultClearColorDriverGLES3 { 0x7A, 0x00, 0x4D, 0xFF };
	static const math::RGBAColorU8 sDefaultClearColorDriverVK1   { 0x8F, 0x0F, 0x1F, 0xFF };

	enum EGPUDeviceInternalStateFlags : uint32
	{
		E_GPU_DEVICE_INTERNAL_STATE_FLAG_DEBUG_DEVICE_BIT = 0x0001,
		E_GPU_DEVICE_INTERNAL_STATE_FLAG_MULTI_THREAD_ACCESS_BIT = 0x0002,
		E_GPU_DEVICE_INTERNAL_STATE_FLAG_ENABLE_RESOURCE_ACTIVE_REFS_TRACKING_BIT = 0x0008
	};

	
	GPUDevice::GPUDevice( GPUDriver & pDriver )
	: GPUDriverChildObject( pDriver )
	, mGPUDriverID( pDriver.queryGPUDriverID() )
	, mSysContext( pDriver.mSysContext )
	{
		if( pDriver.isDebugFunctionalityRequested() )
		{
			_internalStateFlags.set( E_GPU_DEVICE_INTERNAL_STATE_FLAG_DEBUG_DEVICE_BIT );
		}
	}

	GPUDevice::~GPUDevice() = default;

	bool GPUDevice::isDebugDevice() const noexcept
	{
		return _internalStateFlags.isSet( E_GPU_DEVICE_INTERNAL_STATE_FLAG_DEBUG_DEVICE_BIT );
	}

	bool GPUDevice::isMultiThreadAccessSupported() const noexcept
	{
		return _internalStateFlags.isSet( E_GPU_DEVICE_INTERNAL_STATE_FLAG_MULTI_THREAD_ACCESS_BIT );
	}

	bool GPUDevice::isResourceActiveRefsTrackingEnabled() const noexcept
	{
		return _internalStateFlags.isSet( E_GPU_DEVICE_INTERNAL_STATE_FLAG_ENABLE_RESOURCE_ACTIVE_REFS_TRACKING_BIT );
	}

	CommandSystem & GPUDevice::getCommandSystem() const noexcept
	{
		return *_commandSystem;
	}

	PipelineImmutableStateFactory & GPUDevice::getPipelineStateFactory() const noexcept
	{
		return *_immutableStateFactoryPtr;
	}

	PresentationLayer * GPUDevice::getPresentationLayer() const noexcept
	{
		return _presentationLayer.get();
	}

	const math::RGBAColorU8 & GPUDevice::getDefaultClearColor() const noexcept
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

	const RenderTargetAttachmentClearConfig & GPUDevice::getDefaultClearConfig() const noexcept
	{
		static const RenderTargetAttachmentClearConfig sDefaultClearConfig =
		{
			getDefaultClearColor(),
			1.0f,
			0
		};
		return sDefaultClearConfig;
	}

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

	RenderTargetTextureHandle GPUDevice::createRenderTargetTexture( const RenderTargetTextureCreateInfo & pCreateInfo )
	{
		return _drvCreateRenderTargetTexture( pCreateInfo );
	}

	GraphicsPipelineStateObjectHandle GPUDevice::createGraphicsPipelineStateObject( const GraphicsPipelineStateObjectCreateInfo & pCreateInfo )
	{
		return _drvCreateGraphicsPipelineStateObject( pCreateInfo );
	}

	BlendImmutableStateHandle GPUDevice::createBlendImmutableState( const BlendConfig & pConfig )
	{
		ts3DebugAssert( _immutableStateFactoryPtr );
		return _immutableStateFactoryPtr->createBlendState( pConfig );
	}

	DepthStencilImmutableStateHandle GPUDevice::createDepthStencilImmutableState( const DepthStencilConfig & pConfig )
	{
		ts3DebugAssert( _immutableStateFactoryPtr );
		return _immutableStateFactoryPtr->createDepthStencilState( pConfig );
	}

	GraphicsShaderLinkageImmutableStateHandle GPUDevice::createGraphicsShaderLinkageImmutableState( const GraphicsShaderSet & pShaderSet )
	{
		ts3DebugAssert( _immutableStateFactoryPtr );
		return _immutableStateFactoryPtr->createGraphicsShaderLinkageState( pShaderSet );
	}

	IAInputLayoutImmutableStateHandle GPUDevice::createIAInputLayoutImmutableState( const IAInputLayoutDefinition & pDefinition )
	{
		ts3DebugAssert( _immutableStateFactoryPtr );
		return _immutableStateFactoryPtr->createIAInputLayoutState( pDefinition );
	}

	IAVertexStreamImmutableStateHandle GPUDevice::createIAVertexStreamImmutableState( const IAVertexStreamDefinition & pDefinition )
	{
		ts3DebugAssert( _immutableStateFactoryPtr );
		return _immutableStateFactoryPtr->createIAVertexStreamState( pDefinition );
	}

	RasterizerImmutableStateHandle GPUDevice::createRasterizerImmutableState( const RasterizerConfig & pConfig )
	{
		ts3DebugAssert( _immutableStateFactoryPtr );
		return _immutableStateFactoryPtr->createRasterizerState( pConfig );
	}

	RenderTargetBindingImmutableStateHandle GPUDevice::createRenderTargetBindingImmutableState( const RenderTargetBindingDefinition & pDefinition )
	{
		ts3DebugAssert( _immutableStateFactoryPtr );
		return _immutableStateFactoryPtr->createRenderTargetBindingState( pDefinition );
	}

	RenderPassConfigurationImmutableStateHandle GPUDevice::createRenderPassConfigurationImmutableState( const RenderPassConfiguration & pConfiguration )
	{
		ts3DebugAssert( _immutableStateFactoryPtr );
		return _immutableStateFactoryPtr->createRenderPassState( pConfiguration.getValidated() );
	}

	BlendImmutableStateHandle GPUDevice::createBlendImmutableStateCached(
			const UniqueGPUObjectName & pUniqueName,
			const BlendConfig & pConfig )
	{
		ts3DebugAssert( _immutableStateCachePtr );
		return _immutableStateCachePtr->createState<BlendImmutableState>( pUniqueName, pConfig );
	}

	DepthStencilImmutableStateHandle GPUDevice::createDepthStencilImmutableStateCached(
			const UniqueGPUObjectName & pUniqueName,
			const DepthStencilConfig & pConfig )
	{
		ts3DebugAssert( _immutableStateCachePtr );
		return _immutableStateCachePtr->createState<DepthStencilImmutableState>( pUniqueName, pConfig );
	}

	GraphicsShaderLinkageImmutableStateHandle GPUDevice::createGraphicsShaderLinkageImmutableStateCached(
			const UniqueGPUObjectName & pUniqueName,
			const GraphicsShaderSet & pShaderSet )
	{
		ts3DebugAssert( _immutableStateCachePtr );
		return _immutableStateCachePtr->createState<GraphicsShaderLinkageImmutableState>( pUniqueName, pShaderSet );
	}

	IAInputLayoutImmutableStateHandle GPUDevice::createIAInputLayoutImmutableStateCached(
			const UniqueGPUObjectName & pUniqueName,
			const IAInputLayoutDefinition & pDefinition )
	{
		ts3DebugAssert( _immutableStateCachePtr );
		return _immutableStateCachePtr->createState<IAInputLayoutImmutableState>( pUniqueName, pDefinition );
	}

	IAVertexStreamImmutableStateHandle GPUDevice::createIAVertexStreamImmutableStateCached(
			const UniqueGPUObjectName & pUniqueName,
			const IAVertexStreamDefinition & pDefinition )
	{
		ts3DebugAssert( _immutableStateCachePtr );
		return _immutableStateCachePtr->createState<IAVertexStreamImmutableState>( pUniqueName, pDefinition );
	}

	RasterizerImmutableStateHandle GPUDevice::createRasterizerImmutableStateCached(
			const UniqueGPUObjectName & pUniqueName,
			const RasterizerConfig & pConfig )
	{
		ts3DebugAssert( _immutableStateCachePtr );
		return _immutableStateCachePtr->createState<RasterizerImmutableState>( pUniqueName, pConfig );
	}

	RenderTargetBindingImmutableStateHandle GPUDevice::createRenderTargetBindingImmutableStateCached(
			const UniqueGPUObjectName & pUniqueName,
			const RenderTargetBindingDefinition & pDefinition )
	{
		ts3DebugAssert( _immutableStateCachePtr );
		return _immutableStateCachePtr->createState<RenderTargetBindingImmutableState>( pUniqueName, pDefinition );
	}

	RenderPassConfigurationImmutableStateHandle GPUDevice::createRenderPassConfigurationImmutableStateCached(
			const UniqueGPUObjectName & pUniqueName,
			const RenderPassConfiguration & pConfiguration )
	{
		ts3DebugAssert( _immutableStateCachePtr );
		return _immutableStateCachePtr->createState<RenderPassConfigurationImmutableState>( pUniqueName, pConfiguration );
	}

	void GPUDevice::resetImmutableStateCache( Bitmask<EPipelineImmutableStateTypeFlags> pResetMask )
	{
		ts3DebugAssert( _immutableStateCachePtr );
		_immutableStateCachePtr->reset( pResetMask );
	}

	void GPUDevice::setPresentationLayer( PresentationLayerHandle pPresentationLayer )
	{
		if( !_drvOnSetPresentationLayer( pPresentationLayer ) )
		{
			return;
		}
		_presentationLayer = pPresentationLayer;
	}

	bool GPUDevice::onGPUResourceActiveRefsZero( GPUResource & pGPUResource )
	{
		return true;
	}

	void GPUDevice::setImmutableStateCache( PipelineImmutableStateCache & pStateCache )
	{
		_immutableStateCachePtr = &pStateCache;
		_immutableStateFactoryPtr = &( pStateCache.mStateFactory );
	}

	bool GPUDevice::_drvOnSetPresentationLayer( PresentationLayerHandle pPresentationLayer )
	{
		return true;
	}

	GPUBufferHandle GPUDevice::_drvCreateGPUBuffer( const GPUBufferCreateInfo & pCreateInfo )
	{
		return nullptr;
	}

	SamplerHandle GPUDevice::_drvCreateSampler( const SamplerCreateInfo & pCreateInfo )
	{
		return nullptr;
	}

	ShaderHandle GPUDevice::_drvCreateShader( const ShaderCreateInfo & pCreateInfo )
	{
		return nullptr;
	}

	TextureHandle GPUDevice::_drvCreateTexture( const TextureCreateInfo & pCreateInfo )
	{
		return nullptr;
	}

	RenderTargetTextureHandle GPUDevice::_drvCreateRenderTargetTexture( const RenderTargetTextureCreateInfo & pCreateInfo )
	{
		return nullptr;
	}

	GraphicsPipelineStateObjectHandle GPUDevice::_drvCreateGraphicsPipelineStateObject( const GraphicsPipelineStateObjectCreateInfo & pCreateInfo )
	{
		return nullptr;
	}

} // namespace ts3::gpuapi
