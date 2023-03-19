
#include <ts3/system/displaySystem.h>
#include <ts3/system/sysContext.h>
#include <ts3/system/eventCore.h>
#include <ts3/system/eventObject.h>
#include <ts3/system/openGLDriver.h>
#include <ts3/system/assetSystemNative.h>
#include <ts3/system/perfCounter.h>
#include <ts3/system/sysContextNative.h>

#include <ts3/core/coreEngineState.h>
#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/gpuapi/gpuDriver.h>
#include <ts3/gpuapi/gpuDriverAPI.h>
#include <ts3/gpuapi/commandContext.h>
#include <ts3/gpuapi/commandSystem.h>
#include <ts3/gpuapi/presentationLayer.h>
#include <ts3/gpuapi/gpuUtils.h>
#include <ts3/gpuapi/resources/gpuBuffer.h>
#include <ts3/gpuapi/state/sampler.h>
#include <ts3/gpuapi/resources/texture.h>
#include <ts3/gpuapi/resources/renderTargetTexture.h>
#include <ts3/gpuapi/state/shaderInputSignature.h>
#include <ts3/gpuapi/state/pipelineStateObject.h>
#include <ts3/gpuapi/state/renderTargetDynamicStates.h>
#include <ts3/gpuapi/state/renderTargetImmutableStates.h>
#include <ts3/gpuapi/state/inputAssemblerDynamicStates.h>
#include <ts3/gpuapi/state/inputAssemblerImmutableStates.h>

#include <ts3/engine/camera/cameraController.h>
#include <ts3/engine/res/image/bitmapCommon.h>

#include <ts3/engine/renderer/shaderLibrary.h>
#include <ts3/engine/renderer/effects/shadowRenderer.h>

#include <chrono>
#include <thread>

#include "gpuapi01-meshDefs.h"

using namespace ts3;
using namespace ts3::gpuapi;
using namespace ts3::system;
namespace math = ts3::math;

std::string sGxDriverName;

struct GraphicsDriverState
{
    std::string driverID;
    std::unique_ptr<GPUDriverInterface> driverInterface;
    GPUDriverHandle driver;
    GPUDeviceHandle device;
    PresentationLayerHandle  presentationLayer;
    std::unique_ptr<CommandContextDirectGraphics> cmdContext;
    bool pauseAnimation = false;
};

struct AppSharedResources
{
    ImageData txROG512Data;
};

void initializeGraphicsDriver( SysContextHandle pSysContext, GraphicsDriverState & pGxDriverState );

struct CB0Data
{
    math::Mat4x4f modelMatrix;
    math::Mat4x4f viewMatrix;
    math::Mat4x4f projectionMatrix;
};

DynamicMemoryBuffer loadShaderSourceDefault( AssetLoader & pAssetLoader, const std::string & pShaderFile )
{
	auto psAsset = pAssetLoader.openSubAsset(
			"shaders/" + sGxDriverName + "/" + pShaderFile,
			E_ASSET_OPEN_FLAG_NO_EXTENSION_BIT );

	DynamicMemoryBuffer resultBuffer;
	const auto sourceLength = psAsset->readAll( resultBuffer, 1 );

	resultBuffer[sourceLength] = 0;

	return resultBuffer;
}

std::function<DynamicMemoryBuffer()> bindShaderSourceLoadCallbackDefault( AssetLoader & pAssetLoader, const std::string & pShaderFile )
{
	return std::bind( loadShaderSourceDefault, std::ref( pAssetLoader ), std::ref( pShaderFile ) );
}

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )

#include <ts3/gpuapiGLES3/GLES3_gpuDriverAPI.h>
#include "../../engine/components/ts3/gpuapi/gpuDriver.h"

int ts3AndroidAppMain( int argc, char ** argv, AndroidAppState * pAppState )
{
    sGxDriverName = "GLES3";

    SysContextCreateInfo sysContextCreateInfo {};
    sysContextCreateInfo.nativeParams.aCommonAppState = pAppState;
    auto sysContext = createSysContext( sysContextCreateInfo );

    AssetLoaderCreateInfo aslCreateInfo;
    auto assetLoader = sysContext->createAssetLoader( aslCreateInfo );

    GraphicsDriverState gxDriverState;
    gxDriverState.driverID = "GLES3";
    gxDriverState.driverInterface = std::make_unique<GLES3GPUDriverInterface>();

#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )

#define ENABLE_TS3DRV_GL4 1
#define ENABLE_TS3DRV_D3D11 0

#if( ENABLE_TS3DRV_GL4 )
# include <ts3/gpuapiGL4/GL4_gpuDriverAPI.h>
#endif
#if( ENABLE_TS3DRV_D3D11 )
#  include <ts3/gpuapiDX11/DX11_gpuDriverAPI.h>
#endif

int main( int pArgc, const char ** pArgv )
{
    sGxDriverName = "GL4";

	SysContextCreateInfo sysContextCreateInfo;
	platform::SysContextCreateInfoNativeParams sysContextCreateInfoNP;
	sysContextCreateInfoNP.appExecModuleHandle = ::GetModuleHandleA( nullptr );
	sysContextCreateInfo.nativeParams = &sysContextCreateInfoNP;
	auto sysContext = platform::createSysContext( sysContextCreateInfo );

	platform::AssetLoaderCreateInfoNativeParams aslCreateInfoNP;
	aslCreateInfoNP.relativeAssetRootDir = "assets";
	AssetLoaderCreateInfo aslCreateInfo;
	aslCreateInfo.nativeParams = &aslCreateInfoNP;
	auto assetLoader = sysContext->createAssetLoader( aslCreateInfo );

    GraphicsDriverState gxDriverState;
    gxDriverState.driverID = sGxDriverName;

#if( ENABLE_TS3DRV_GL4 )
    if( sGxDriverName == "GL4" )
    {
        gxDriverState.driverInterface = std::make_unique<GL4GPUDriverInterface>();
    }
#endif
#if( ENABLE_TS3DRV_D3D11 )
    if( sGxDriverName == "DX11" )
    {
        gxDriverState.driverInterface = std::make_unique<DX11GPUDriverInterface>();
    }
#endif

    ts3DebugAssert( gxDriverState.driverInterface );

#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )

#include <ts3/gpuapiGL4/GL4_gpuDriverAPI.h>

int main( int pArgc, const char ** pArgv )
{
    sGxDriverName = "GL4";

    SysContextCreateInfo sysContextCreateInfo;
    auto sysContext = platform::createSysContext( sysContextCreateInfo );

    platform::AssetLoaderCreateInfoNativeParams aslCreateInfoNP;
    aslCreateInfoNP.relativeAssetRootDir = "assets";
    AssetLoaderCreateInfo aslCreateInfo;
    aslCreateInfo.nativeParams = &aslCreateInfoNP;
    auto assetLoader = sysContext->createAssetLoader( aslCreateInfo );

    GraphicsDriverState gxDriverState;
    gxDriverState.driverID = "GL4";
    gxDriverState.driverInterface = std::make_unique<GL4GPUDriverInterface>();

#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_OSX )

#include <ts3/gpuapiGL4/GL4_gpuDriverAPI.h>

int main( int pArgc, const char ** pArgv )
{
    sGxDriverName = "MTL";

    SysContextCreateInfo sysContextCreateInfo;
    auto sysContext = platform::createSysContext( sysContextCreateInfo );

    platform::AssetLoaderCreateInfoNativeParams aslCreateInfoNP;
    aslCreateInfoNP.relativeAssetRootDir = "../../../../tessline-3dx/assets";
    AssetLoaderCreateInfo aslCreateInfo;
    aslCreateInfo.nativeParams = &aslCreateInfoNP;
    auto assetLoader = sysContext->createAssetLoader( aslCreateInfo );

    GraphicsDriverState gxDriverState;
    gxDriverState.driverID = "GL4";
    gxDriverState.driverInterface = std::make_unique<GL4GPUDriverInterface>();

#endif

    srand( time( nullptr ) );

    bool runApp = true;

    auto evtController = sysContext->createEventController();
    auto evtDispatcher = evtController->createEventDispatcher();
    evtController->setActiveEventDispatcher( *evtDispatcher );

#if( TS3_PCL_TARGET_OS == TS3_PCL_TARGET_OS_ANDROID )
    bool waitForDisplay = true;

    evtDispatcher->setEventHandler(
            EEventCodeIndex::AppActivityDisplayInit,
            [&waitForDisplay,&sysContext](const EventObject & pEvt) -> bool {
                waitForDisplay = false;
                return true;
            });

    while( waitForDisplay )
    {
        evtController->dispatchPendingEventsAuto();
    }

//    evtDispatcher->setEventHandler(
//            EEventCodeIndex::AppActivityDisplayInit,
//            [&gfxState](const EventObject & pEvt) -> bool {
//                initializeGraphicsGL( gfxState );
//                gfxState.glContext->bindForCurrentThread( *(gfxState.glSurface) );
//                gfxState.pauseAnimation = false;
//                return true;
//            });
//
//    evtDispatcher->setEventHandler(
//            EEventCodeIndex::AppActivityDisplayTerm,
//            [&gfxState](const EventObject & pEvt) -> bool {
//                //gfxState.glSystemDriver->invalidate();
//                gfxState.glContext = nullptr;
//                gfxState.glSurface = nullptr;
//                gfxState.glSystemDriver = nullptr;
//                gfxState.pauseAnimation = true;
//                return true;
//            });
#endif

	bool isFullscreen = true;

    evtDispatcher->setEventHandler(
            EEventCodeIndex::AppActivityQuit,
            [&runApp](const EventObject & pEvt) -> bool {
                runApp = false;
                return true;
            });
    evtDispatcher->setEventHandler(
            EEventCodeIndex::WindowUpdateDestroy,
            [evtDispatcher,&gxDriverState](const EventObject & pEvt) -> bool {
                if( pEvt.eWindowUpdateDestroy.checkEventSource( gxDriverState.presentationLayer->getInternalSystemEventSource() ) )
                {
                    evtDispatcher->postEventAppQuit();
                }
                return true;
            });
    evtDispatcher->setEventHandler(
            EEventCodeIndex::InputKeyboard,
            [evtDispatcher,&gxDriverState,&isFullscreen](const EventObject & pEvt) -> bool {
            	auto & keyMap = pEvt.eInputKeyboard.inputKeyboardState->keyStateMap;
                if( pEvt.eInputKeyboard.keyCode == EKeyCode::Escape )
                {
                    evtDispatcher->postEventAppQuit();
                }
                else if( keyMap[EKeyCode::AltLeft] && keyMap[EKeyCode::Enter] )
                {
                	isFullscreen = !isFullscreen;
                	gxDriverState.presentationLayer->setFullscreenMode( isFullscreen );
                }
                return true;
            });

	initializeGraphicsDriver( sysContext, gxDriverState );

	const auto rtSize = gxDriverState.presentationLayer->queryRenderTargetSize();

	auto shaderLibrary = std::make_unique<ShaderLibrary>( gxDriverState.device );
	shaderLibrary->loadShaders( {
		{ "SID_DEFAULT_PASSTHROUGH_VS", gpuapi::EShaderType::GSVertex, bindShaderSourceLoadCallbackDefault( *assetLoader, "default_passthrough_vs" ) },
		{ "SID_DEFAULT_PASSTHROUGH_PS", gpuapi::EShaderType::GSPixel, bindShaderSourceLoadCallbackDefault( *assetLoader, "default_passthrough_ps" ) },
		{ "SID_SHADOW_0_PASS1_LIGHT_VS", gpuapi::EShaderType::GSVertex, bindShaderSourceLoadCallbackDefault( *assetLoader, "shadow_0_pass1_light_vs" ) },
		{ "SID_SHADOW_0_PASS1_LIGHT_PS", gpuapi::EShaderType::GSPixel, bindShaderSourceLoadCallbackDefault( *assetLoader, "shadow_0_pass1_light_ps" ) },
		{ "SID_SHADOW_0_PASS2_SHADOW_VS", gpuapi::EShaderType::GSVertex, bindShaderSourceLoadCallbackDefault( *assetLoader, "shadow_0_pass2_shadow_vs" ) },
		{ "SID_SHADOW_0_PASS2_SHADOW_PS", gpuapi::EShaderType::GSPixel, bindShaderSourceLoadCallbackDefault( *assetLoader, "shadow_0_pass2_shadow_ps" ) },
	} );

	ts3::ShadowConfig shadowConfig;
	shadowConfig.screenSize.width = rtSize.x;
	shadowConfig.screenSize.height = rtSize.y;
	shadowConfig.shadowMapSize.width = rtSize.x;
	shadowConfig.shadowMapSize.height = rtSize.y;
	auto shadowRenderer = std::make_unique<ts3::ShadowRenderer>( *shaderLibrary, shadowConfig );

    AppSharedResources appResources;
    {
        auto txROG512Asset = assetLoader->openSubAsset( "bitmaps/rog-logo1-512", E_ASSET_OPEN_FLAG_NO_EXTENSION_BIT );

        DynamicMemoryBuffer tmpBuffer;
        txROG512Asset->readAll( tmpBuffer );
        appResources.txROG512Data = loadBitmapFromMemory( tmpBuffer.data(), tmpBuffer.size() );
    }

    if( auto * eventSource = gxDriverState.presentationLayer->getInternalSystemEventSource() )
    {
        evtController->registerEventSource( *eventSource );
    }

	auto * gpuDevicePtr = gxDriverState.device.get();

    auto vertexShader = shaderLibrary->getShader( "SID_DEFAULT_PASSTHROUGH_VS" );
    auto pixelShader = shaderLibrary->getShader( "SID_DEFAULT_PASSTHROUGH_PS" );

    ts3::gpuapi::RenderPassConfiguration rednerPassConfig;

    ts3::gpuapi::GPUBufferHandle cbuffer0;
    {
        ts3::gpuapi::GPUBufferCreateInfo cbci;
        cbci.memoryFlags = ts3::gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
        cbci.resourceFlags = ts3::gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_STATIC_BIT;
        cbci.initialTarget = ts3::gpuapi::EGPUBufferTarget::ConstantBuffer;
        cbci.bufferSize = sizeof( CB0Data );
        cbuffer0 = gxDriverState.device->createGPUBuffer( cbci );
    }
    ts3::gpuapi::GPUBufferHandle vbuffer;
    {
		const auto vertexData = generateVertexPNT0Data();
        ts3::gpuapi::GPUBufferCreateInfo vbci;
        vbci.memoryFlags = ts3::gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
        vbci.resourceFlags = ts3::gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT;
        vbci.resourceFlags = ts3::gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_STATIC_BIT;
        vbci.initialTarget = ts3::gpuapi::EGPUBufferTarget::VertexBuffer;
        vbci.initDataDesc.pointer = vertexData.data();
        vbci.initDataDesc.size    = vertexData.size() * sizeof( VertexPNT0 );
        vbuffer = gxDriverState.device->createGPUBuffer( vbci );
    }
    ts3::gpuapi::GPUBufferHandle ibuffer;
    {
	    const auto indexData = generateIndexPNT0Data();
        ts3::gpuapi::GPUBufferCreateInfo ibci;
        ibci.memoryFlags = ts3::gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
        ibci.resourceFlags = ts3::gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT;
        ibci.resourceFlags = ts3::gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_STATIC_BIT;
        ibci.initialTarget = ts3::gpuapi::EGPUBufferTarget::IndexBuffer;
	    ibci.initDataDesc.pointer = indexData.data();
	    ibci.initDataDesc.size    = indexData.size() * sizeof( uint32 );
        ibuffer = gxDriverState.device->createGPUBuffer( ibci );
    }

    ts3::gpuapi::TextureHandle tex0;
    {
        ts3::gpuapi::TextureCreateInfo txci;
        txci.texClass = ts3::gpuapi::ETextureClass::T2D;
        txci.dimensions.width = appResources.txROG512Data.formatInfo.dimensions.x;
        txci.dimensions.height = appResources.txROG512Data.formatInfo.dimensions.y;
        txci.memoryFlags = ts3::gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
        txci.resourceFlags = ts3::gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT;
        txci.pixelFormat = ts3::gpuapi::ETextureFormat::RGBA8UN;
        txci.initialTarget = ts3::gpuapi::ETextureTarget::ShaderInputSampledImage;
        txci.initDataDesc.initialize( txci.dimensions );
        txci.initDataDesc.subTextureInitDataBasePtr[0].mipLevelInitDataArray[0].pointer = appResources.txROG512Data.pixelBuffer.data();
        txci.initDataDesc.subTextureInitDataBasePtr[0].mipLevelInitDataArray[0].size = appResources.txROG512Data.sizeInBytes;
        tex0 = gpuDevicePtr->createTexture( txci );
    }

    ts3::gpuapi::TextureHandle texRTColor0;
    ts3::gpuapi::RenderTargetTextureHandle texRTColor0RT;
    {
        ts3::gpuapi::TextureCreateInfo texRTColor0CI;
        texRTColor0CI.texClass = ts3::gpuapi::ETextureClass::T2D;
        texRTColor0CI.dimensions.width = 1920;
        texRTColor0CI.dimensions.height = 1080;
        texRTColor0CI.memoryFlags = ts3::gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
        texRTColor0CI.resourceFlags =
                ts3::gpuapi::E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_COLOR_BIT |
                ts3::gpuapi::E_GPU_RESOURCE_USAGE_FLAG_SHADER_INPUT_BIT;
        texRTColor0CI.pixelFormat = ts3::gpuapi::ETextureFormat::RGBA8UN;
        texRTColor0CI.initialTarget = ts3::gpuapi::ETextureTarget::RenderTargetColorAttachment;
        texRTColor0 = gpuDevicePtr->createTexture( texRTColor0CI );

		ts3::gpuapi::RenderTargetTextureCreateInfo texRTColor0RTCI;
		texRTColor0RTCI.targetTexture = texRTColor0;
		texRTColor0RT = gpuDevicePtr->createRenderTargetTexture( texRTColor0RTCI );
    }

    ts3::gpuapi::TextureHandle texRTDepthStencil;
	ts3::gpuapi::RenderTargetTextureHandle texRTDepthStencilRT;
    {
        ts3::gpuapi::TextureCreateInfo texRTDepthStencilCI;
        texRTDepthStencilCI.texClass = ts3::gpuapi::ETextureClass::T2D;
        texRTDepthStencilCI.dimensions.width = 1920;
        texRTDepthStencilCI.dimensions.height = 1080;
        texRTDepthStencilCI.memoryFlags = ts3::gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
        texRTDepthStencilCI.resourceFlags = ts3::gpuapi::E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_DEPTH_STENCIL_BIT;
        texRTDepthStencilCI.pixelFormat = ts3::gpuapi::ETextureFormat::D24UNS8U;
        texRTDepthStencilCI.initialTarget = ts3::gpuapi::ETextureTarget::RenderTargetDepthStencilAttachment;
        texRTDepthStencil = gpuDevicePtr->createTexture( texRTDepthStencilCI );

		ts3::gpuapi::RenderTargetTextureCreateInfo texRTDepthStencilRTCI;
		texRTDepthStencilRTCI.targetTexture = texRTDepthStencil;
		texRTDepthStencilRT = gpuDevicePtr->createRenderTargetTexture( texRTDepthStencilRTCI );
    }

    ts3::gpuapi::RenderPassConfigurationImmutableStateHandle fboRenderPassState;
    ts3::gpuapi::RenderPassConfigurationImmutableStateHandle scrRenderPassState;
    {
        RenderPassConfiguration rpConfig;
        rpConfig.activeAttachmentsMask = E_RT_ATTACHMENT_MASK_DEFAULT_C0_DS;
        rpConfig.attachmentsActionClearMask = E_RT_ATTACHMENT_MASK_DEFAULT_C0_DS;
        rpConfig.colorAttachments[0].renderPassLoadAction = ERenderPassAttachmentLoadAction::Clear;
        rpConfig.colorAttachments[0].renderPassStoreAction = ERenderPassAttachmentStoreAction::Keep;
        rpConfig.colorAttachments[0].clearConfig.colorValue = { 0.12f, 0.36f, 0.88f, 1.0f };
        rpConfig.depthStencilAttachment.renderPassLoadAction = ERenderPassAttachmentLoadAction::Clear;
        rpConfig.depthStencilAttachment.renderPassStoreAction = ERenderPassAttachmentStoreAction::Keep;
        rpConfig.depthStencilAttachment.clearConfig.depthValue = 1.0f;
        rpConfig.depthStencilAttachment.clearConfig.stencilValue = 0xFF;
		fboRenderPassState = gpuDevicePtr->createRenderPassConfigurationImmutableState( rpConfig );

		rpConfig.colorAttachments[0].clearConfig.colorValue = { 0.68f, 0.92f, 0.78f, 1.0f };
		scrRenderPassState = gpuDevicePtr->createRenderPassConfigurationImmutableState( rpConfig );
    }
    
    ts3::gpuapi::SamplerCreateInfo samplerCreateInfo;
    samplerCreateInfo.samplerConfig = ts3::gpuapi::cvSamplerConfigDefault;
    
    auto defaultSampler = gxDriverState.device->createSampler( samplerCreateInfo );

	GraphicsPipelineStateObjectHandle mainPSO;
    {
		RenderTargetLayout rtLayout;
		rtLayout.activeAttachmentsMask = E_RT_ATTACHMENT_MASK_DEFAULT_C0_DS;
		rtLayout.colorAttachments[0].format = ETextureFormat::BGRA8UN;
		rtLayout.depthStencilAttachment.format = ETextureFormat::D24UNS8U;

		GraphicsPipelineStateObjectCreateInfo psoci;
		psoci.renderTargetLayout.activeAttachmentsMask = E_RT_ATTACHMENT_MASK_DEFAULT_C0_DS;
		psoci.renderTargetLayout.colorAttachments[0].format = ETextureFormat::BGRA8UN;
		psoci.renderTargetLayout.depthStencilAttachment.format = ETextureFormat::D24UNS8U;
		psoci.blendConfig = defaults::cvPipelineBlendConfigDefault;
		psoci.depthStencilConfig = defaults::cvPipelineDepthStencilConfigEnableDepthTest;
		psoci.rasterizerConfig = defaults::cvPipelineRasterizerConfigDefault;
		psoci.rasterizerConfig.cullMode = ts3::gpuapi::ECullMode::Back;
		psoci.rasterizerConfig.primitiveFillMode = ts3::gpuapi::EPrimitiveFillMode::Solid;
		psoci.rasterizerConfig.frontFaceVerticesOrder = ts3::gpuapi::ETriangleVerticesOrder::CounterClockwise;
		psoci.renderTargetLayout = rtLayout;
		psoci.inputLayoutDefinition.activeAttributesMask =
			E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_0_BIT | E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_1_BIT | E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_2_BIT | E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_3_BIT;
		psoci.inputLayoutDefinition.primitiveTopology = EPrimitiveTopology::TriangleList;
		psoci.inputLayoutDefinition.attributeArray[0] = { 0, "POSITION", 0, ts3::gpuapi::EVertexAttribFormat::Vec3F32, 0 };
		psoci.inputLayoutDefinition.attributeArray[1] = { 0, "COLOR", 0, ts3::gpuapi::EVertexAttribFormat::Vec4F32, ts3::gpuapi::cxdefs::VERTEX_ATTRIBUTE_OFFSET_APPEND };
		psoci.inputLayoutDefinition.attributeArray[2] = { 0, "NORMAL", 0, ts3::gpuapi::EVertexAttribFormat::Vec3F32, ts3::gpuapi::cxdefs::VERTEX_ATTRIBUTE_OFFSET_APPEND };
		psoci.inputLayoutDefinition.attributeArray[3] = { 0, "TEXCOORD", 0, ts3::gpuapi::EVertexAttribFormat::Vec2F32, ts3::gpuapi::cxdefs::VERTEX_ATTRIBUTE_OFFSET_APPEND };
		psoci.shaderSet.addShader( vertexShader );
		psoci.shaderSet.addShader( pixelShader );
		psoci.shaderInputSignatureDesc.activeShaderStagesMask = ts3::gpuapi::E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT | ts3::gpuapi::E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT;
		psoci.shaderInputSignatureDesc.descriptorSetsNum = 2;
		psoci.shaderInputSignatureDesc.descriptorSetArray[0].descriptorType = ts3::gpuapi::EShaderInputDescriptorType::Resource;
		psoci.shaderInputSignatureDesc.descriptorSetArray[0].descriptorsNum = 2;
		psoci.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[0] = {0, ts3::gpuapi::EShaderInputDescriptorType::Resource, ts3::gpuapi::E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT };
		psoci.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[0].uResourceDesc = { ts3::gpuapi::EShaderInputResourceType::CBVConstantBuffer, 0, 1 };
		psoci.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[1] = { 1, ts3::gpuapi::EShaderInputDescriptorType::Resource, ts3::gpuapi::E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT };
		psoci.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[1].uResourceDesc = { ts3::gpuapi::EShaderInputResourceType::SRVTextureImage, 0, 1 };
		psoci.shaderInputSignatureDesc.descriptorSetArray[1].descriptorType = ts3::gpuapi::EShaderInputDescriptorType::Sampler;
		psoci.shaderInputSignatureDesc.descriptorSetArray[1].descriptorsNum = 1;
		psoci.shaderInputSignatureDesc.descriptorSetArray[1].descriptorList[0] = { 10, ts3::gpuapi::EShaderInputDescriptorType::Sampler, ts3::gpuapi::E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT };
		psoci.shaderInputSignatureDesc.descriptorSetArray[1].descriptorList[0].uSamplerConfig = { 0 };

		mainPSO = gxDriverState.device->createGraphicsPipelineStateObject( psoci );
    }

	RenderTargetBindingDynamicState rtds;
	auto & c0 = rtds.setColorAttachmentBinding( 0 );
	c0.attachmentTexture = texRTColor0RT;
	auto & ds = rtds.setDepthStencilAttachmentBinding();
	ds.attachmentTexture = texRTDepthStencilRT;

	IAVertexStreamDynamicState vsds;
	auto & vb0 = vsds.setVertexBufferRef( 0 );
	vb0.sourceBuffer = vbuffer;
	vb0.relativeOffset = 0;
	vb0.vertexStride = sizeof( VertexPNT0 );
	auto & ib = vsds.setIndexBufferRef();
	ib.sourceBuffer = ibuffer;
	ib.relativeOffset = 0;
	ib.indexFormat = EIndexDataFormat::Uint32;

    math::Vec3f cameraOriginPoint{ 0.0f, 1.0f,  -2.0f };
    math::Vec3f cameraTargetPoint{ 0.0f, 0.0f,  4.0f };

    ts3::CameraController cameraController;
    cameraController.initialize( cameraOriginPoint, cameraTargetPoint, 60.0f );
	
	ts3::gpuapi::ViewportDesc vpDescScreen{};
	vpDescScreen.origin.x = 0;
	vpDescScreen.origin.y = 0;
	vpDescScreen.size.x = rtSize.x;
	vpDescScreen.size.y = rtSize.y;
	vpDescScreen.depthRange.zNear = 0.0f;
	vpDescScreen.depthRange.zFar = 1.0f;

	ts3::gpuapi::ViewportDesc vpDescTexture{};
	vpDescTexture.origin.x = 0;
	vpDescTexture.origin.y = 0;
	vpDescTexture.size.x = 1920;
	vpDescTexture.size.y = 1080;
	vpDescTexture.depthRange.zNear = 0.0f;
	vpDescTexture.depthRange.zFar = 1.0f;

	auto ts3ViewTexture = lookAtLH(
		math::Vec3f{ 0.0f, 3.0f,  -1.0f },
		math::Vec3f{ 0.0f, 0.0f,  5.0f },
		math::Vec3f{ 0.0f, 1.0f,  0.0f } );

	auto ts3ProjectionTexture = math::perspectiveAspectLH<float>(
		ts3::math::constants::cxFloatRad60Degree, ( float )rtSize.x / ( float )rtSize.y, 0.1f, 1000.0f );
	
	CB0Data cb0DataBase =
	{
	    math::identity4<float>(),
	    math::identity4<float>(),
	    math::identity4<float>(),
	};

	const uint32 VNUM = 36;

	ts3::system::perf_counter_value_t u1ts = ts3::system::PerfCounter::queryCurrentStamp();
	ts3::system::perf_counter_value_t u2ts = ts3::system::PerfCounter::queryCurrentStamp();

	const float update1ts = 3.0f;
	const float update2ts = 10.0f;

	float u1angle = 0.0f;
	float u2angle = 0.0f;

	GPUBufferDataUploadDesc cb0DataUploadDesc;
	cb0DataUploadDesc.inputDataDesc.pointer = &cb0DataBase;
	cb0DataUploadDesc.inputDataDesc.size = sizeof( CB0Data );

	    bool rotate = false;

	    evtDispatcher->setEventHandler(
            ts3::system::EEventCodeIndex::InputMouseButton,
            [&rotate]( const ts3::system::EventObject & pEvt ) -> bool {
                const auto & eButton = pEvt.eInputMouseButton;
                if( eButton.buttonAction == EMouseButtonActionType::Click )
                {
                    rotate = true;
                }
                else if( eButton.buttonAction == EMouseButtonActionType::Release )
                {
                    rotate = false;
                }
                return true;
            });
    evtDispatcher->setEventHandler(
            ts3::system::EEventCodeIndex::InputMouseMove,
            [&cameraController,&rotate]( const ts3::system::EventObject & pEvt ) -> bool {
                if( rotate )
                {
                    const auto & emove = pEvt.eInputMouseMove;
                    cameraController.rotateAroundTarget( emove.movementDelta.x, emove.movementDelta.y );
                    if( emove.buttonStateMask.isSet( ts3::system::E_MOUSE_BUTTON_FLAG_LEFT_BIT ) )
                    {
                        cameraController.rotateAroundOrigin( emove.movementDelta.x, emove.movementDelta.y );
                    }
                    else if( emove.buttonStateMask.isSet( ts3::system::E_MOUSE_BUTTON_FLAG_RIGHT_BIT ) )
                    {
                        cameraController.rotateAroundTarget( emove.movementDelta.x, emove.movementDelta.y );
                    }
                }
                return true;
            });

	math::Vec3f lightPosition{ -2.0f, 3.0f, -2.0f };
	math::Vec3f lightTarget{ 2.0f, 0.0f,  5.0f };

	auto ts3ViewLight = lookAtLH(
		lightPosition,
		lightTarget,
		math::Vec3f{ 0.0f, 1.0f,  0.0f } );

	shadowRenderer->createRendererResources();
	shadowRenderer->setCSLightPosition( lightPosition );
	shadowRenderer->setCSLightTarget( lightTarget );
	shadowRenderer->setCSProjectionMatrixLightPerspectiveDefault();

    while( runApp )
    {
        if( gxDriverState.pauseAnimation )
        {
            continue;
        }

        try
        {
			auto pcstamp = PerfCounter::queryCurrentStamp();
			if( PerfCounter::convertToDuration<ts3::EDurationPeriod::Millisecond>( pcstamp - u1ts ) >= update1ts )
			{
				u1angle += math::constants::cxFloatRad1Degree * 10 * ( 1.0f / update1ts );
				u1ts = pcstamp;
			}
			if( PerfCounter::convertToDuration<ts3::EDurationPeriod::Millisecond>( pcstamp - u2ts ) >= update2ts )
			{
				u2angle += math::constants::cxFloatRad1Degree * 10 * ( 1.0f / update2ts );
				u2ts = pcstamp;
			}

            evtController->dispatchPendingEventsAuto();

			gxDriverState.cmdContext->beginCommandSequence();

			if( true )
			{
				const auto cameraViewMatrix = cameraController.computeViewMatrixLH();
				const auto cameraProjectionMatrix = math::perspectiveAspectLH<float>(
					cameraController.getPerspectiveFOVAngle(), ( float )rtSize.x / ( float )rtSize.y, 1.0f, 1000.0f );

				const auto modelMatrix1 = math::translation<float>( 0.0f, 0, 2.0f );
				const auto modelMatrix2 = math::translation<float>( 1.0f, 0, 4.0f );
				const auto modelMatrix3 = math::translation<float>( 2.0f, 0, 6.0f );

				CB0Data cb0Data;

				gxDriverState.cmdContext->setIAVertexStreamState( vsds );

				shadowRenderer->beginRenderPass1Light( *gxDriverState.cmdContext );
				{
					gxDriverState.cmdContext->cmdSetShaderConstantBuffer( 10, *cbuffer0 );

					shadowRenderer->updateMatricesForLightPass();

					const auto & lightViewMatrix = shadowRenderer->getLightViewMatrix();
					const auto & lightProjectionMatrix = shadowRenderer->getLightProjectionMatrix();

					cb0Data.viewMatrix = lightViewMatrix;
					cb0Data.projectionMatrix = lightProjectionMatrix;

					cb0Data.modelMatrix = math::identity4<float>();
					gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0Data );
					gxDriverState.cmdContext->cmdDrawDirectIndexed( 6, 42 );

					cb0Data.modelMatrix = modelMatrix1;
					gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0Data );
					gxDriverState.cmdContext->cmdDrawDirectIndexed( VNUM, 0 );

					cb0Data.modelMatrix = modelMatrix2;
					gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0Data );
					gxDriverState.cmdContext->cmdDrawDirectIndexed( VNUM, 0 );

					cb0Data.modelMatrix = modelMatrix3;
					gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0Data );
					gxDriverState.cmdContext->cmdDrawDirectIndexed( VNUM, 0 );
				}
				shadowRenderer->endRenderPass( *gxDriverState.cmdContext );

				gxDriverState.presentationLayer->bindRenderTarget( gxDriverState.cmdContext.get() );

				shadowRenderer->beginRenderPass2Shadow( *gxDriverState.cmdContext );
				{
					gxDriverState.cmdContext->cmdSetViewport( vpDescScreen );
					gxDriverState.cmdContext->cmdSetShaderConstantBuffer( 10, *cbuffer0 );
					gxDriverState.cmdContext->cmdSetShaderTextureSampler( 70, *defaultSampler );
					gxDriverState.cmdContext->cmdSetShaderTextureImage( 20, *tex0 );
					{
						cb0Data.viewMatrix = cameraViewMatrix;
						cb0Data.projectionMatrix = cameraProjectionMatrix;

						cb0Data.modelMatrix = math::identity4<float>();
						shadowRenderer->updateMatricesForShadowPass( *gxDriverState.cmdContext, cb0Data.modelMatrix, cameraViewMatrix );
						gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0Data );
						gxDriverState.cmdContext->cmdDrawDirectIndexed( 6, 42 );

						cb0Data.modelMatrix = modelMatrix1;
						shadowRenderer->updateMatricesForShadowPass( *gxDriverState.cmdContext, cb0Data.modelMatrix, cameraViewMatrix );
						gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0Data );
						gxDriverState.cmdContext->cmdDrawDirectIndexed( VNUM, 0 );

						cb0Data.modelMatrix = modelMatrix2;
						shadowRenderer->updateMatricesForShadowPass( *gxDriverState.cmdContext, cb0Data.modelMatrix, cameraViewMatrix );
						gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0Data );
						gxDriverState.cmdContext->cmdDrawDirectIndexed( VNUM, 0 );

						cb0Data.modelMatrix = modelMatrix3;
						shadowRenderer->updateMatricesForShadowPass( *gxDriverState.cmdContext, cb0Data.modelMatrix, cameraViewMatrix );
						gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0Data );
						gxDriverState.cmdContext->cmdDrawDirectIndexed( VNUM, 0 );
					}
					gxDriverState.presentationLayer->invalidateRenderTarget( gxDriverState.cmdContext.get() );
				}
				gxDriverState.cmdContext->endRenderPass();

				/*auto ts3ViewScreen = cameraController.computeViewMatrixLH();

				gxDriverState.presentationLayer->bindRenderTarget( gxDriverState.cmdContext.get() );

				gxDriverState.cmdContext->setIAVertexStreamState( vsds );
				gxDriverState.cmdContext->setGraphicsPipelineStateObject( *mainPSO );

				gxDriverState.cmdContext->beginRenderPass( *fboRenderPassState, 0 );
				{
					gxDriverState.cmdContext->cmdSetViewport( vpDescScreen );
					gxDriverState.cmdContext->cmdSetShaderConstantBuffer( 0, *cbuffer0 );
					gxDriverState.cmdContext->cmdSetShaderTextureSampler( 10, *defaultSampler );
					gxDriverState.cmdContext->cmdSetShaderTextureImage( 1, *tex0 );
					{
						cb0Data.projectionMatrix = math::perspectiveAspectLH<float>(
							cameraController.getPerspectiveFOVAngle(), ( float )rtSize.x / ( float )rtSize.y, 1.0f, 10.0f );
						cb0Data.viewMatrix = ts3ViewLight;

						cb0Data.modelMatrix = math::translation<float>( 0, 0, 2.0f );
						gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0Data );
						gxDriverState.cmdContext->cmdDrawDirectIndexed( VNUM, 0 );

						cb0Data.modelMatrix = math::translation<float>( 1.0f, 0, 4.0f );
						gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0Data );
						gxDriverState.cmdContext->cmdDrawDirectIndexed( VNUM, 0 );

						cb0Data.modelMatrix = math::translation<float>( 2.0f, 0, 6.0f );
						gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0Data );
						gxDriverState.cmdContext->cmdDrawDirectIndexed( VNUM, 0 );
					}
					gxDriverState.presentationLayer->invalidateRenderTarget( gxDriverState.cmdContext.get() );
				}
				gxDriverState.cmdContext->endRenderPass();*/
			}

			if( false )
			{

				cb0DataBase.modelMatrix = math::mul(
					math::translation<float>( 0, 0, 8.5f ),
					math::mul(
						math::scaling( 2.0f, 2.0f, 2.0f ),
						math::rotationAxisY( u2angle ) ) );

				auto ts3ViewScreen = cameraController.computeViewMatrixLH();

				gxDriverState.cmdContext->setRenderTargetBindingState( rtds );
				gxDriverState.cmdContext->setGraphicsPipelineStateObject( *mainPSO );
				gxDriverState.cmdContext->setIAVertexStreamState( vsds );

				gxDriverState.cmdContext->beginRenderPass( *fboRenderPassState, 0 );
				{
					gxDriverState.cmdContext->cmdSetViewport( vpDescTexture );
					gxDriverState.cmdContext->cmdSetShaderConstantBuffer( 0, *cbuffer0 );
					gxDriverState.cmdContext->cmdSetShaderTextureSampler( 10, *defaultSampler );
					gxDriverState.cmdContext->cmdSetShaderTextureImage( 1, *tex0 );
					{
						cb0DataBase.projectionMatrix = ts3ProjectionTexture;
						cb0DataBase.viewMatrix = ts3ViewTexture;
						cb0DataBase.modelMatrix = math::mul(
							math::translation<float>( 0, 0, 3.0f ),
							math::mul(
								math::scaling( 2.0f, 2.0f, 2.0f ),
								math::rotationAxisY( u1angle ) ) );
						gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0DataUploadDesc );
						gxDriverState.cmdContext->cmdDrawDirectIndexed( VNUM, 0 );
					}
					{
						cb0DataBase.projectionMatrix = ts3ProjectionTexture;
						cb0DataBase.viewMatrix = ts3ViewTexture;
						cb0DataBase.modelMatrix = math::mul(
							math::translation<float>( 0, 0, 8.5f ),
							math::mul(
								math::scaling( 2.0f, 2.0f, 2.0f ),
								math::rotationAxisY( u2angle ) ) );
						gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0DataUploadDesc );
						gxDriverState.cmdContext->cmdDrawDirectIndexed( VNUM, 0 );
					}
				}
				gxDriverState.cmdContext->endRenderPass();

				gxDriverState.presentationLayer->bindRenderTarget( gxDriverState.cmdContext.get() );

				gxDriverState.cmdContext->beginRenderPass( *scrRenderPassState, 0 );
				{
					gxDriverState.cmdContext->cmdSetViewport( vpDescScreen );
					gxDriverState.cmdContext->cmdSetShaderConstantBuffer( 0, *cbuffer0 );
					gxDriverState.cmdContext->cmdSetShaderTextureSampler( 10, *defaultSampler );
					gxDriverState.cmdContext->cmdSetShaderTextureImage( 1, *( texRTColor0RT->getTargetTextureRef().getRefTexture() ) );
					{
						cb0DataBase.projectionMatrix = math::perspectiveAspectLH<float>(
							cameraController.getPerspectiveFOVAngle(), ( float )rtSize.x / ( float )rtSize.y, 0.1f, 1000.0f );
						cb0DataBase.viewMatrix = ts3ViewScreen;
						cb0DataBase.modelMatrix = math::mul(
								math::scaling( 2.0f, 2.0f, 1.0f ),
								math::mul( math::translation<float>( -2, 0, 6.0f ), math::rotationAxisY( -1.0f ) ) );
						gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0DataUploadDesc );
						gxDriverState.cmdContext->cmdDrawDirectIndexed( 6, 36 );
					}
					{
						cb0DataBase.projectionMatrix = math::perspectiveAspectLH<float>(
							cameraController.getPerspectiveFOVAngle(), ( float )rtSize.x / ( float )rtSize.y, 0.1f, 1000.0f );
						cb0DataBase.viewMatrix = ts3ViewScreen;
						cb0DataBase.modelMatrix = math::mul(
								math::scaling( 2.0f, 2.0f, 1.0f ),
								math::mul( math::translation<float>( 2, 0, 6.0f ), math::rotationAxisY( 1.0f ) ) );
						gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0DataUploadDesc );
						gxDriverState.cmdContext->cmdDrawDirectIndexed( 6, 36 );
					}

					gxDriverState.presentationLayer->invalidateRenderTarget( gxDriverState.cmdContext.get() );
				}
				gxDriverState.cmdContext->endRenderPass();
			}

			gxDriverState.cmdContext->endCommandSequence();
			gxDriverState.cmdContext->submit();

			gxDriverState.presentationLayer->present();

			std::this_thread::sleep_for( std::chrono::milliseconds( 16 ) );
        }
        catch( ... )
        {
        }
    }

    return 0;
}

void initializeGraphicsDriver( SysContextHandle pSysContext, GraphicsDriverState & pGxDriverState )
{
    GPUDriverCreateInfo gpuDriverCreateInfo;
    gpuDriverCreateInfo.sysContext = pSysContext;
    gpuDriverCreateInfo.configFlags = E_GPU_DRIVER_CONFIG_FLAG_ENABLE_SHADER_DEBUG_INFO_BIT;
    pGxDriverState.driver = pGxDriverState.driverInterface->createDriver( gpuDriverCreateInfo );

    GPUDeviceCreateInfo gpuDeviceCreateInfo;
    gpuDeviceCreateInfo.adapterID = 0;
    gpuDeviceCreateInfo.flags = E_GPU_DEVICE_CREATE_FLAGS_DEFAULT;
    pGxDriverState.device = pGxDriverState.driver->createDevice( gpuDeviceCreateInfo );

    PresentationLayerCreateInfo presentationLayerCreateInfo;
    presentationLayerCreateInfo.screenRect.size.x = 1024;
    presentationLayerCreateInfo.screenRect.size.y = 600;
    presentationLayerCreateInfo.displayConfigFlags = 0;
    presentationLayerCreateInfo.displayConfigFlags = E_DISPLAY_CONFIGURATION_FLAG_FULLSCREEN_BIT;
    pGxDriverState.presentationLayer = pGxDriverState.driverInterface->createScreenPresentationLayer( *( pGxDriverState.device ), presentationLayerCreateInfo );

    pGxDriverState.device->setPresentationLayer( pGxDriverState.presentationLayer );
    pGxDriverState.cmdContext = pGxDriverState.device->getCommandSystem().acquireCommandContext<CommandContextDirectGraphics>();
}
