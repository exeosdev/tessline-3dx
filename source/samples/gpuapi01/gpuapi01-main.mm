//
// Created by Mateusz Grzejek on 01/04/2023.
//

#include <ts3/system/displaySystem.h>
#include <ts3/system/sysContext.h>
#include <ts3/system/eventCore.h>
#include <ts3/system/eventObject.h>
#include <ts3/system/openGLDriver.h>
#include <ts3/system/assetSystemNative.h>
#include <ts3/system/perfCounter.h>
#include <ts3/system/sysContextNative.h>

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
#include <ts3/engine/geometry/meshGroup.h>
#include <ts3/engine/geometry/meshLoader.h>

#include <ts3/engine/renderer/shaderLibrary.h>
#include <ts3/engine/renderer/shaderLoader.h>
#include <ts3/engine/renderer/effects/shadowRenderer.h>
#include <ts3/engine/geometry/geometryManager.h>
#include <ts3/engine/geometry/geometryStorage.h>
#include <ts3/engine/geometry/geometryDataTransfer.h>

#include <chrono>
#include <thread>

#include "gpuapi01-meshDefs.h"

using namespace ts3;
using namespace gpuapi;
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

DynamicMemoryBuffer loadFileDefault( AssetLoader & pAssetLoader, const std::string & pFilename )
{
	auto psAsset = pAssetLoader.openSubAsset( pFilename, 0 );

	DynamicMemoryBuffer resultBuffer;
	const auto sourceLength = psAsset->readAll( resultBuffer );

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
#define ENABLE_TS3DRV_D3D11 1

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

    evtDispatcher->setEventHandler(
            EEventCodeIndex::AppActivityDisplayInit,
            [&gfxState](const EventObject & pEvt) -> bool {
                initializeGraphicsGL( gfxState );
                gfxState.glContext->bindForCurrentThread( *(gfxState.glSurface) );
                gfxState.pauseAnimation = false;
                return true;
            });

    evtDispatcher->setEventHandler(
            EEventCodeIndex::AppActivityDisplayTerm,
            [&gfxState](const EventObject & pEvt) -> bool {
                //gfxState.glSystemDriver->invalidate();
                gfxState.glContext = nullptr;
                gfxState.glSurface = nullptr;
                gfxState.glSystemDriver = nullptr;
                gfxState.pauseAnimation = true;
                return true;
            });
#endif

	bool isFullscreen = true;
	bool updateLightPosition = true;

	ts3::CameraController cameraController;

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
			[&](const EventObject & pEvt) -> bool {
				auto & keyMap = pEvt.eInputKeyboard.inputKeyboardState->keyStateMap;
				if( pEvt.eInputKeyboard.keyCode == EKeyCode::Escape )
				{
					evtDispatcher->postEventAppQuit();
				}
				else if( keyMap[EKeyCode::Enter] )
				{
					isFullscreen = !isFullscreen;
					ts3DebugInterrupt();
					// gxDriverState.presentationLayer->setFullscreenMode( isFullscreen );
				}
				if( keyMap[EKeyCode::CtrlLeft] )
				{
					ts3DebugOutput("CTRL_LEFT");
				}
				if( keyMap[EKeyCode::CharW] )
				{
					cameraController.move( 0.075f );
				}
				if( keyMap[EKeyCode::CharS] )
				{
					cameraController.move( -0.075f );
				}
				if( keyMap[EKeyCode::CharD] )
				{
					cameraController.moveSide( 0.075f );
				}
				if( keyMap[EKeyCode::CharA] )
				{
					cameraController.moveSide( -0.075f );
				}
				if( keyMap[EKeyCode::Space] )
				{
					cameraController.moveUpDown( 0.075f );
				}
				if( keyMap[EKeyCode::CharC] )
				{
					cameraController.moveUpDown( -0.075f );
				}
				if( keyMap[EKeyCode::CharL] )
				{
					updateLightPosition = true;
				}
				if( keyMap[EKeyCode::CharK] )
				{
					updateLightPosition = false;
				}
				return true;
			});

	initializeGraphicsDriver( sysContext, gxDriverState );

	if( auto * eventSource = gxDriverState.presentationLayer->getInternalSystemEventSource() )
	{
		evtController->registerEventSource( *eventSource );
	}

	CoreEngineState ces{ sysContext, gxDriverState.device };

	GeometryDataFormat gdf;
	gdf.configureFixedAttribute( EFixedVertexAttributeID::Position, 0 );
	gdf.configureFixedAttribute( EFixedVertexAttributeID::Normal, 0 );
	gdf.configureFixedAttribute( EFixedVertexAttributeID::FixedColor, 0 );
	gdf.configureFixedAttribute( EFixedVertexAttributeID::TexCoord0, 0 );
	gdf.setIndexDataFormat( gpuapi::EIndexDataFormat::Uint32 );
	gdf.setPrimitiveTopology( gpuapi::EPrimitiveTopology::TriangleList );

	MeshLoader meshLoader;
	GeometryManager geometryManager{ ces };

	std::unique_ptr<MeshGroup> meshGroup;

	{
		MeshImporterAssimp meshImporter;
		GeometryDataGpuTransferDirect geometryDataGpuTransfer{ ces, *gxDriverState.cmdContext };

		MeshImportContext mic;
		mic.geometryManager = &geometryManager;
		mic.geometryDataFormat = &gdf;
		mic.importer = &meshImporter;
		mic.geometryDataTransfer = &geometryDataGpuTransfer;

		meshGroup = meshLoader.importMeshGroup( mic, "MeshGroupID_Default", {
				{ "MeshID_Default_TreeBase", "assets/meshes/tree/Tree.obj" }
			} );
	}

	auto * meshTree = meshGroup->findMesh( "MeshID_Default_TreeBase" );

	const auto rtSize = gxDriverState.presentationLayer->queryRenderTargetSize();
	auto shaderLibrary = ShaderLoader::createShaderLibrary( ces, {
			{ "SID_DEFAULT_PASSTHROUGH_VS", gpuapi::EShaderType::GSVertex, bindShaderSourceLoadCallbackDefault( *assetLoader, "default_passthrough_vs" ) },
			{ "SID_DEFAULT_PASSTHROUGH_PS", gpuapi::EShaderType::GSPixel, bindShaderSourceLoadCallbackDefault( *assetLoader, "default_passthrough_ps" ) },
			{ "SID_SHADOW_0_PASS1_LIGHT_VS", gpuapi::EShaderType::GSVertex, bindShaderSourceLoadCallbackDefault( *assetLoader, "shadow_0_pass1_light_vs" ) },
			{ "SID_SHADOW_0_PASS1_LIGHT_PS", gpuapi::EShaderType::GSPixel, bindShaderSourceLoadCallbackDefault( *assetLoader, "shadow_0_pass1_light_ps" ) },
			{ "SID_SHADOW_0_PASS2_SHADOW_VS", gpuapi::EShaderType::GSVertex, bindShaderSourceLoadCallbackDefault( *assetLoader, "shadow_0_pass2_shadow_vs" ) },
			{ "SID_SHADOW_0_PASS2_SHADOW_PS", gpuapi::EShaderType::GSPixel, bindShaderSourceLoadCallbackDefault( *assetLoader, "shadow_0_pass2_shadow_ps" ) },
	} );

	auto vertexShader = shaderLibrary->getShader( "SID_DEFAULT_PASSTHROUGH_VS" );
	auto pixelShader = shaderLibrary->getShader( "SID_DEFAULT_PASSTHROUGH_PS" );

	auto * gpuDevicePtr = gxDriverState.device.get();

	gpuapi::GPUBufferHandle cbuffer0;
	{
		gpuapi::GPUBufferCreateInfo cbci;
		cbci.memoryFlags = gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;

		cbci.resourceFlags = gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_STATIC_BIT;
		cbci.resourceFlags |= gpuapi::E_GPU_BUFFER_BIND_FLAG_CONSTANT_BUFFER_BIT;

		cbci.bufferSize = sizeof( CB0Data );
		cbuffer0 = gxDriverState.device->createGPUBuffer( cbci );
	}

	gpuapi::TextureHandle texRTColor0;
	gpuapi::RenderTargetTextureHandle texRTColor0RT;
	{
		gpuapi::TextureCreateInfo texRTColor0CI;
		texRTColor0CI.texClass = gpuapi::ETextureClass::T2D;
		texRTColor0CI.dimensions.width = rtSize.x;
		texRTColor0CI.dimensions.height = rtSize.y;
		texRTColor0CI.memoryFlags = gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
		texRTColor0CI.resourceFlags =
				gpuapi::E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_COLOR_BIT |
				gpuapi::E_GPU_RESOURCE_USAGE_FLAG_SHADER_INPUT_BIT;
		texRTColor0CI.internalFormat = gpuapi::ETextureFormat::RGBA8UN;
		texRTColor0 = gpuDevicePtr->createTexture( texRTColor0CI );

		gpuapi::RenderTargetTextureCreateInfo texRTColor0RTCI;
		texRTColor0RTCI.targetTexture = texRTColor0;
		texRTColor0RTCI.bindFlags =
				E_TEXTURE_BIND_FLAG_RENDER_TARGET_COLOR_ATTACHMENT_BIT |
				E_TEXTURE_BIND_FLAG_SHADER_INPUT_SAMPLED_IMAGE_BIT;
		texRTColor0RT = gpuDevicePtr->createRenderTargetTexture( texRTColor0RTCI );
	}

	gpuapi::TextureHandle texRTDepthStencil;
	gpuapi::RenderTargetTextureHandle texRTDepthStencilRT;
	{
		gpuapi::TextureCreateInfo texRTDepthStencilCI;
		texRTDepthStencilCI.texClass = gpuapi::ETextureClass::T2D;
		texRTDepthStencilCI.dimensions.width = rtSize.x;
		texRTDepthStencilCI.dimensions.height = rtSize.y;
		texRTDepthStencilCI.memoryFlags = gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
		texRTDepthStencilCI.resourceFlags =
				gpuapi::E_GPU_RESOURCE_USAGE_MASK_RENDER_TARGET_DEPTH_STENCIL;
		texRTDepthStencilCI.internalFormat = gpuapi::ETextureFormat::D24UNS8U;
		texRTDepthStencil = gpuDevicePtr->createTexture( texRTDepthStencilCI );

		gpuapi::RenderTargetTextureCreateInfo texRTDepthStencilRTCI;
		texRTDepthStencilRTCI.targetTexture = texRTDepthStencil;
		texRTDepthStencilRTCI.bindFlags =
				E_TEXTURE_BIND_FLAG_RENDER_TARGET_DEPTH_STENCIL_ATTACHMENT_BIT;
		texRTDepthStencilRT = gpuDevicePtr->createRenderTargetTexture( texRTDepthStencilRTCI );
	}

	gpuapi::RenderPassConfigurationImmutableStateHandle fboRenderPassState;
	gpuapi::RenderPassConfigurationImmutableStateHandle scrRenderPassState;
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

		// rpConfig.colorAttachments[0].clearConfig.colorValue = { 0.68f, 0.92f, 0.78f, 1.0f };
		scrRenderPassState = gpuDevicePtr->createRenderPassConfigurationImmutableState( rpConfig );
	}

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
		psoci.rasterizerConfig.cullMode = gpuapi::ECullMode::Back;
		psoci.rasterizerConfig.primitiveFillMode = gpuapi::EPrimitiveFillMode::Wireframe;
		psoci.rasterizerConfig.frontFaceVerticesOrder = gpuapi::ETriangleVerticesOrder::Clockwise;
		psoci.renderTargetLayout = rtLayout;
		psoci.inputLayoutDefinition = gdf.generateGpaInputLayoutDefinition();
		psoci.shaderSet.addShader( vertexShader );
		psoci.shaderSet.addShader( pixelShader );
		psoci.shaderInputSignatureDesc.activeShaderStagesMask = gpuapi::E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT | gpuapi::E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT;
		psoci.shaderInputSignatureDesc.descriptorSetsNum = 1;
		psoci.shaderInputSignatureDesc.descriptorSetArray[0].descriptorType = gpuapi::EShaderInputDescriptorType::Resource;
		psoci.shaderInputSignatureDesc.descriptorSetArray[0].descriptorsNum = 1;
		psoci.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[0] = {0, gpuapi::EShaderInputDescriptorType::Resource, gpuapi::E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT };
		psoci.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[0].uResourceDesc = { gpuapi::EShaderInputResourceType::CBVConstantBuffer, 0, 1 };

		mainPSO = gxDriverState.device->createGraphicsPipelineStateObject( psoci );
	}

	RenderTargetBindingDynamicState rtds;
	auto & c0 = rtds.setColorAttachmentBinding( 0 );
	c0.attachmentTexture = texRTColor0RT;
	auto & ds = rtds.setDepthStencilAttachmentBinding();
	ds.attachmentTexture = texRTDepthStencilRT;

	math::Vec3f cameraOriginPoint{ 0.0f, 2.0f,  -4.0f };
	math::Vec3f cameraTargetPoint{ 0.0f, 0.0f,  4.0f };
	cameraController.initialize( cameraOriginPoint, cameraTargetPoint, 60.0f );

	gpuapi::ViewportDesc vpDescScreen{};
	vpDescScreen.origin.x = 0;
	vpDescScreen.origin.y = 0;
	vpDescScreen.size.x = rtSize.x;
	vpDescScreen.size.y = rtSize.y;
	vpDescScreen.depthRange.zNear = 0.0f;
	vpDescScreen.depthRange.zFar = 1.0f;

	gpuapi::ViewportDesc vpDescTexture{};
	vpDescTexture.origin.x = 0;
	vpDescTexture.origin.y = 0;
	vpDescTexture.size.x = rtSize.x;
	vpDescTexture.size.y = rtSize.y;
	vpDescTexture.depthRange.zNear = 0.0f;
	vpDescTexture.depthRange.zFar = 1.0f;

	const auto ts3ViewTexture = lookAtLH(
			math::Vec3f{ 0.0f, 3.0f,  -1.0f },
			math::Vec3f{ 0.0f, 0.0f,  5.0f },
			math::Vec3f{ 0.0f, 1.0f,  0.0f } );

	const auto ts3ProjectionTexture = math::perspectiveAspectLH<float>(
			cameraController.getPerspectiveFOVAngle(), ( float )rtSize.x / ( float )rtSize.y, 0.1f, 1000.0f );

	const auto ts3CameraProjection = math::perspectiveAspectLH<float>(
			cameraController.getPerspectiveFOVAngle(), ( float )rtSize.x / ( float )rtSize.y, 1.0f, 1000.0f );

	CB0Data cb0DataBase =
	{
		math::identity4<float>(),
		math::identity4<float>(),
		math::identity4<float>(),
	};

	const uint32 VNUM = 36;

	ts3::system::perf_counter_value_t u1ts = ts3::system::PerfCounter::queryCurrentStamp();
	ts3::system::perf_counter_value_t u2ts = ts3::system::PerfCounter::queryCurrentStamp();

	const float update1ts = 25.0f;
	const float update2ts = 25.0f;

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
				//if( rotate )
				{
					const auto & emove = pEvt.eInputMouseMove;
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

	srand( time( nullptr ) );

	const auto treesNum = 36;
	const auto treeModelTransBase = math::translation( 0.0f, 0.48f, 1.0f);
	const auto treeModelScale = math::scaling<float>( 0.85f, 0.85f, 0.85f );

	const auto treeRangeX = std::make_pair( -4.0f, 4.0f );
	const auto treeRangeZ = std::make_pair( 0.0f, 6.0f );

	math::Mat4f treeModelMats[treesNum];
	for( uint32 iTree = 0; iTree < treesNum; ++iTree )
	{
		const auto xRangeLength = ( uint32 )( ( treeRangeX.second - treeRangeX.first ) * 100.0f );
		const auto zRangeLength = ( uint32 )( ( treeRangeZ.second - treeRangeZ.first ) * 100.0f );
		const auto xMod = rand() % xRangeLength;
		const auto zMod = rand() % zRangeLength;
		const auto xOff = xMod * 0.01f + treeRangeX.first;
		const auto zOff = zMod * 0.01f + treeRangeZ.first;

		treeModelMats[iTree] = math::mul(
				math::mul( treeModelTransBase, math::translation( xOff, 0.0f, zOff ) ),
				math::mul( math::rotationAxisY( ( rand() % 360 )  * math::constants::cxFloatRad1Degree ), treeModelScale ) );
	}

	while( runApp )
	{
		if( gxDriverState.pauseAnimation )
		{
			continue;
		}

		try
		{
			evtController->dispatchPendingEventsAuto();

			gxDriverState.cmdContext->beginCommandSequence();

			const auto cameraViewMatrix = cameraController.computeViewMatrixLH();

			gxDriverState.cmdContext->setGraphicsPipelineStateObject( *mainPSO );
			gxDriverState.cmdContext->setIAVertexStreamState( *meshGroup->getGeometryStorage().getGpaVertexStreamState() );

			cb0DataBase.projectionMatrix = ts3CameraProjection;
			cb0DataBase.viewMatrix = cameraViewMatrix;

			gxDriverState.presentationLayer->bindRenderTarget( gxDriverState.cmdContext.get() );

			gxDriverState.cmdContext->beginRenderPass( *scrRenderPassState );
			{
				gxDriverState.cmdContext->cmdSetViewport( vpDescScreen );
				gxDriverState.cmdContext->cmdSetShaderConstantBuffer( 0, *cbuffer0 );

				for( uint32 iTree = 0; iTree < treesNum; ++iTree )
				{
					cb0DataBase.modelMatrix = treeModelMats[iTree];
					gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0DataBase );

					for( uint32 iMeshComponent = 0; iMeshComponent < meshTree->getComponentsNum(); ++iMeshComponent )
					{
						auto * meshComponent = meshTree->getSubComponent( iMeshComponent );
						auto * geometryRef = meshComponent->geometryDataRef();


						gxDriverState.cmdContext->cmdDrawDirectIndexed(
								geometryRef->dataReference.indexDataRegion.sizeInElementsNum,
								geometryRef->dataReference.indexDataRegion.offsetInElementsNum,
								geometryRef->dataReference.vertexDataOffsetInElementsNum() );
					}
				}

				gxDriverState.presentationLayer->invalidateRenderTarget( gxDriverState.cmdContext.get() );
			}
			gxDriverState.cmdContext->endRenderPass();

			gxDriverState.cmdContext->endCommandSequence();
			gxDriverState.cmdContext->submit();

			gxDriverState.presentationLayer->present();

			// std::this_thread::sleep_for( std::chrono::milliseconds( 8 ) );
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
	presentationLayerCreateInfo.screenRect.size.x = 1920;
	presentationLayerCreateInfo.screenRect.size.y = 1080;
	presentationLayerCreateInfo.displayConfigFlags = 0;
	presentationLayerCreateInfo.displayConfigFlags = 0;//E_DISPLAY_CONFIGURATION_FLAG_FULLSCREEN_BIT;
	pGxDriverState.presentationLayer = pGxDriverState.driverInterface->createScreenPresentationLayer( *( pGxDriverState.device ), presentationLayerCreateInfo );

	pGxDriverState.device->setPresentationLayer( pGxDriverState.presentationLayer );
	pGxDriverState.cmdContext = pGxDriverState.device->getCommandSystem().acquireCommandContext<CommandContextDirectGraphics>();
}
