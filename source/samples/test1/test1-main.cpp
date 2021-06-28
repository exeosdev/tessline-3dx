
#include <ts3/system/displayManager.h>
#include <ts3/core/coreEngineState.h>
#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/gpuapi/gpuDriver.h>
#include <ts3/gpuapi/commandContext.h>
#include <ts3/gpuapi/commandSystem.h>
#include <ts3/gpuapi/displayManager.h>
#include <ts3/gpuapi/presentationLayer.h>
#include <ts3/gpuapi/gpuUtils.h>
#include <ts3/gpuapi/resources/gpuBuffer.h>
#include <ts3/gpuapi/resources/sampler.h>
#include <ts3/gpuapi/resources/texture.h>
#include <ts3/gpuapi/state/renderTargetStateObject.h>
#include <ts3/gpuapi/state/shaderBinding.h>
#include <ts3/gpuapi/state/shaderInputSignature.h>
#include <ts3/gpuapi/state/pipelineStateObject.h>
#include <ts3/gpuapi/state/vertexStreamStateObject.h>
#include <ts3/gpuapiGL4/GL4_gpuDriverAPI.h>
#if( _WIN32 )
#include <ts3/gpuapiDX11/DX11_gpuDriverAPI.h>
#endif
#include <ts3/engine/camera/cameraController.h>
#include <ts3/engine/rcdata/font.h>
#include <ts3/engine/rcdata/fontManager.h>
#include <ts3ext/rcsupport/fonts/fontTypeFTF.h>
#include <ts3ext/rcsupport/images/bitmapCommon.h>

#include <ts3/math/matrixUtils.h>
#include <ts3/system/perfCounter.h>

#include "test1-meshDefs.h"

#include <ts3/engine/gpuapi/vertexFormatDefs.h>

namespace math = ts3::math;

static bool appExec = true;

#if( _WIN32 )
static const std::string sWorkspaceDirectory = "C:\\Repo\\ESD\\tessline-3dx-workspace\\modules\\tessline-3dx";
#elif( TS3_PCL_TARGET_OS_LINUX )
static const std::string sWorkspaceDirectory = "/home/mateusz/Dev/Projects/Exeos/Repo/tessline-3dx/";
#endif

struct GraphicsDriverState
{
	ts3::gpuapi::GPUDriverHandle driver;
	ts3::gpuapi::GPUDeviceHandle device;
	ts3::gpuapi::PresentationLayerHandle  presentationLayer;
	std::unique_ptr<ts3::gpuapi::CommandContextDirectGraphics> cmdContext = nullptr;
};

GraphicsDriverState initializeGraphicsDriver( ts3::SysContextHandle pSysContext, const std::string & pDriverID );

using namespace ts3::math;

struct CB0Data
{
	Vec4f scolor;
	Mat4x4f modelMatrix;
	Mat4x4f viewMatrix;
	Mat4x4f projectionMatrix;
};

#define D_OPENGL 1

struct VertexArray
{
	ts3::GPUVertexFormatData2DUI vertices[16];
};

#include <ts3/engine/gpuapi/hwBuffer.h>

struct EventReactor
{
	uint64 onEvent( ts3::HWBuffer & )
	{
		printf( "GPUBufferRef has been locked!\n" );
		return 0x77;
	}
};

int main( int argc, char ** argv )
{
	EventReactor er;
	ts3::HWBuffer hwb;
	hwb.mEventProxy.eLocked.connect( &er, &EventReactor::onEvent );
	hwb.mEventProxy.eLocked.connect( &er, &EventReactor::onEvent );

	srand( time( nullptr ) );

	Vec3f cameraOriginPoint{ 0.0f, 2.0f,  -1.0f };
	Vec3f cameraTargetPoint{ 0.0f, 0.0f,  5.0f };

	ts3::CameraController cameraController;
	cameraController.initialize( cameraOriginPoint, cameraTargetPoint, 60.0f );

	auto sysContext = ts3::sysCreateContext( {} );
	auto eventController = ts3::SysEventController::create( sysContext );
	auto eventDispatcher = eventController->createEventDispatcher();
	eventController->setActiveDispatcher( *eventDispatcher );

	eventDispatcher->bindEventHandler(
			ts3::ESysEventCodeIndex::AppActivityQuit,
			[]( ts3::SysEvent & e ) -> bool {
				appExec = false;
				return true;
			});
	eventDispatcher->bindEventHandler(
			ts3::ESysEventCodeIndex::InputMouseScroll,
			[&cameraController]( ts3::SysEvent & e ) -> bool {
				const auto & escroll = e.eInputMouseScroll;
				if( escroll.scrollDelta.y > 0 )
				{
					cameraController.zoom( 1 );
				}
				else if( escroll.scrollDelta.y < 0 )
				{
					cameraController.zoom( -1 );
				}
				return true;
			});
	eventDispatcher->bindEventHandler(
			ts3::ESysEventCodeIndex::InputMouseMove,
			[&cameraController]( ts3::SysEvent & e ) -> bool {
				const auto & emove = e.eInputMouseMove;
				if( emove.buttonStateMask.isSet( ts3::SYS_MOUSE_BUTTON_FLAG_LEFT_BIT ) )
				{
					cameraController.rotateAroundOrigin( emove.movementDelta.x, emove.movementDelta.y );
				}
				else if( emove.buttonStateMask.isSet( ts3::SYS_MOUSE_BUTTON_FLAG_RIGHT_BIT ) )
				{
					cameraController.rotateAroundTarget( emove.movementDelta.x, emove.movementDelta.y );
				}
				return true;
			});
	eventDispatcher->bindEventHandler(
			ts3::ESysEventCodeIndex::InputKeyboardKey,
			[&cameraController]( ts3::SysEvent & e ) -> bool {
				auto & ekey = e.eInputKeyboardKey;
				if( ekey.keyAction == ts3::SysKeyActionType::Press )
				{
					if( ekey.keyCode == ts3::SysKeyCode::CharW )
					{
						cameraController.move( 1.0f );
					}
					if( ekey.keyCode == ts3::SysKeyCode::CharS )
					{
						cameraController.move( -1.0f );
					}
					if( ekey.keyCode == ts3::SysKeyCode::CharA )
					{
						cameraController.moveSide( -1.0f );
					}
					if( ekey.keyCode == ts3::SysKeyCode::CharD )
					{
						cameraController.moveSide( 1.0f );
					}
					if( ekey.keyCode == ts3::SysKeyCode::CharQ )
					{
						cameraController.roll( 1.0f );
					}
					if( ekey.keyCode == ts3::SysKeyCode::CharE )
					{
						cameraController.roll( -1.0f );
					}
					if( ekey.keyCode == ts3::SysKeyCode::CharC )
					{
						cameraController.moveUpDown( -1.0f );
					}
					if( ekey.keyCode == ts3::SysKeyCode::Space )
					{
						cameraController.moveUpDown( 1.0f );
					}
					if( ekey.keyCode == ts3::SysKeyCode::Escape )
					{
						appExec = false;
					}
				}
				return true;
			});

#if ( D_OPENGL || !_WIN32 )
	const std::string cBaseShaderDir = "/assets/shaders";
	const std::string cDriverName = "GL4";
	const std::string cShaderExtension = "glsl";
#else
	const std::string cBaseShaderDir = "/assets/shaders";
	const std::string cDriverName = "DX11";
	const std::string cShaderExtension = "hlsl";
#endif

	auto imagePath = sWorkspaceDirectory + "/assets/bitmaps/rog-logo1-512.bmp";
	auto imageData = ts3::loadResourceFromFile( imagePath, ts3::loadBitmapFromMemory );

	auto gxDriverState = initializeGraphicsDriver( sysContext, cDriverName );
	auto & gpuDevice = *( gxDriverState.device );

	if( auto * eventSource = gxDriverState.presentationLayer->querySysEventSourceObject() )
	{
		eventController->addEventSource( *eventSource );
	}

	const std::string cVertexShaderFilePath = cBaseShaderDir + "/" + cDriverName + "/simple_passthrough_vs." + cShaderExtension;
	const std::string cPixelShaderFilePath = cBaseShaderDir + "/" + cDriverName + "/simple_passthrough_ps." + cShaderExtension;

	auto vertexShader = ts3::gpuapi::Utils::createShaderFromFile( gpuDevice,
	                                                              ts3::gpuapi::EShaderType::VertexShader,
	                                                              sWorkspaceDirectory + cVertexShaderFilePath );

	auto pixelShader = ts3::gpuapi::Utils::createShaderFromFile( gpuDevice,
	                                                             ts3::gpuapi::EShaderType::PixelShader,
	                                                             sWorkspaceDirectory + cPixelShaderFilePath );

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
		ts3::gpuapi::GPUBufferCreateInfo vbci;
		vbci.memoryFlags = ts3::gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
		vbci.resourceFlags = ts3::gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT;
		vbci.resourceFlags = ts3::gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_STATIC_BIT;
		vbci.initialTarget = ts3::gpuapi::EGPUBufferTarget::VertexBuffer;
		vbci.initDataDesc.pointer = cvMeshTexUnitCubeVertexData;
		vbci.initDataDesc.size    = cvMeshTexUnitCubeVertexDataSize;
		vbuffer = gxDriverState.device->createGPUBuffer( vbci );
	}
	ts3::gpuapi::GPUBufferHandle ibuffer;
	{
		ts3::gpuapi::GPUBufferCreateInfo ibci;
		ibci.memoryFlags = ts3::gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
		ibci.resourceFlags = ts3::gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT;
		ibci.resourceFlags = ts3::gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_STATIC_BIT;
		ibci.initialTarget = ts3::gpuapi::EGPUBufferTarget::IndexBuffer;
		ibci.initDataDesc.pointer = cvMeshTexUnitCubeIndexData;
		ibci.initDataDesc.size    = cvMeshTexUnitCubeIndexDataSize;
		ibuffer = gxDriverState.device->createGPUBuffer( ibci );
	}

	auto * gpuDevicePtr = gxDriverState.device.get();

	ts3::gpuapi::TextureHandle tex0;
	{
		ts3::gpuapi::TextureCreateInfo txci;
		txci.dimensionClass = ts3::gpuapi::ETextureDimensionClass::Texture2D;
		txci.dimensions.width = imageData.formatInfo.dimensions.x;
		txci.dimensions.height = imageData.formatInfo.dimensions.y;
		txci.memoryFlags = ts3::gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
		txci.resourceFlags = ts3::gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT;
		txci.pixelFormat = ts3::gpuapi::ETextureFormat::B8G8R8A8_UNORM;
		txci.initialTarget = ts3::gpuapi::ETextureTarget::ShaderInputSampledImage;
		txci.initDataDesc.initialize( txci.dimensions );
		txci.initDataDesc.subTextureInitDataBasePtr[0].mipLevelInitDataArray[0].pointer = imageData.pixelBuffer.dataPtr();
		txci.initDataDesc.subTextureInitDataBasePtr[0].mipLevelInitDataArray[0].size = imageData.sizeInBytes;
		tex0 = gpuDevicePtr->createTexture( txci );
	}

	ts3::gpuapi::TextureHandle texRTColor0;
	{
		ts3::gpuapi::TextureCreateInfo texRTColor0CI;
		texRTColor0CI.dimensionClass = ts3::gpuapi::ETextureDimensionClass::Texture2D;
		texRTColor0CI.dimensions.width = 1920;
		texRTColor0CI.dimensions.height = 1080;
		texRTColor0CI.memoryFlags = ts3::gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
		texRTColor0CI.resourceFlags = ts3::gpuapi::E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_COLOR_BIT |
		                              ts3::gpuapi::E_GPU_RESOURCE_USAGE_FLAG_SHADER_INPUT_BIT;
		texRTColor0CI.pixelFormat = ts3::gpuapi::ETextureFormat::B8G8R8A8_UNORM;
		texRTColor0CI.initialTarget = ts3::gpuapi::ETextureTarget::RenderTargetColorAttachment;
		texRTColor0 = gpuDevicePtr->createTexture( texRTColor0CI );
	}

	ts3::gpuapi::TextureHandle texRTDepthStencil;
	{
		ts3::gpuapi::TextureCreateInfo texRTDepthStencilCI;
		texRTDepthStencilCI.dimensionClass = ts3::gpuapi::ETextureDimensionClass::Texture2D;
		texRTDepthStencilCI.dimensions.width = 1920;
		texRTDepthStencilCI.dimensions.height = 1080;
		texRTDepthStencilCI.memoryFlags = ts3::gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
		texRTDepthStencilCI.resourceFlags = ts3::gpuapi::E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_DEPTH_STENCIL_BIT;
		texRTDepthStencilCI.pixelFormat = ts3::gpuapi::ETextureFormat::D24_UNORM_S8_UINT;
		texRTDepthStencilCI.initialTarget = ts3::gpuapi::ETextureTarget::RenderTargetDepthStencilAttachment;
		texRTDepthStencil = gpuDevicePtr->createTexture( texRTDepthStencilCI );
	}

	ts3::gpuapi::GraphicsShaderBindingDesc graphicsShaderPipelineDesc =
	{
		ts3::gpuapi::GraphicsShaderBindingDesc::ShaderStageDescArray
		{
			ts3::gpuapi::GraphicsShaderBindingDesc::ShaderStageDesc
			{
				ts3::gpuapi::EGraphicsShaderStageID::Vertex,
				vertexShader.get()
			},
			ts3::gpuapi::GraphicsShaderBindingDesc::ShaderStageDesc
			{
				ts3::gpuapi::EGraphicsShaderStageID::Pixel,
				pixelShader.get()
			}
		}
	};

	ts3::gpuapi::RenderTargetStateObjectHandle renderTargetSO;
	{
		ts3::gpuapi::RenderTargetStateObjectCreateInfo renderTargetSOCI;
		renderTargetSOCI.rtResourceBindingDesc.attachmentResourceBindingDescArray[0].attachmentID = ts3::gpuapi::ERenderTargetAttachmentID::RTColor0;
		renderTargetSOCI.rtResourceBindingDesc.attachmentResourceBindingDescArray[0].attachmentResourceType = ts3::gpuapi::ERenderTargetResourceType::Texture;
		renderTargetSOCI.rtResourceBindingDesc.attachmentResourceBindingDescArray[0].textureRef.texture = texRTColor0.get();
		renderTargetSOCI.rtResourceBindingDesc.attachmentResourceBindingDescArray[0].textureRef.textureSubResource.u2D.mipLevel = 0;
		renderTargetSOCI.rtResourceBindingDesc.attachmentResourceBindingDescArray[1].attachmentID = ts3::gpuapi::ERenderTargetAttachmentID::RTDepthStencil;
		renderTargetSOCI.rtResourceBindingDesc.attachmentResourceBindingDescArray[1].attachmentResourceType = ts3::gpuapi::ERenderTargetResourceType::Texture;
		renderTargetSOCI.rtResourceBindingDesc.attachmentResourceBindingDescArray[1].textureRef.texture = texRTDepthStencil.get();
		renderTargetSOCI.rtResourceBindingDesc.attachmentResourceBindingDescArray[1].textureRef.textureSubResource.u2D.mipLevel = 0;
		renderTargetSO = gpuDevicePtr->createRenderTargetStateObject( renderTargetSOCI );
	}

	ts3::gpuapi::SamplerCreateInfo samplerCreateInfo;
	samplerCreateInfo.samplerDesc = ts3::gpuapi::cvSamplerDescDefault;

	auto defaultSampler = gpuDevice.createSampler( samplerCreateInfo );

	ts3::gpuapi::VertexDataSourceBindingDesc vertexInputStreamDesc;
	vertexInputStreamDesc.indexBufferBindingDesc = ts3gaIndexBufferBindingDesc( ibuffer.get(), 0, ts3::gpuapi::EIndexDataFormat::Uint32 );
	vertexInputStreamDesc.vertexBufferBindingDescArray = {
		ts3gaVertexBufferBindingDesc( 0, vbuffer.get(), 0, sizeof( TexturedMeshVertex ) )
	};

	ts3::gpuapi::VertexInputFormatDesc inputFormatDesc;
	inputFormatDesc.vertexAttributeArray = {
		ts3gaVertexAttributeDesc( 0, 0, "POSITION", 0, ts3::gpuapi::EVertexAttribFormat::VEC3_FLOAT32, 0, 0 ),
		ts3gaVertexAttributeDesc( 1, 0, "TEXCOORD", 0, ts3::gpuapi::EVertexAttribFormat::VEC2_FLOAT32, 12, 0 ),
	};
	inputFormatDesc.primitiveTopology = ts3::gpuapi::EPrimitiveTopology::TriangleList;

	ts3::gpuapi::ShaderInputSignatureDesc inputSignatureDesc;
	inputSignatureDesc.activeShaderStagesMask = ts3::gpuapi::E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT | ts3::gpuapi::E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT;

	inputSignatureDesc.descriptorSetsNum = 2;

	inputSignatureDesc.descriptorSetArray[0].descriptorType = ts3::gpuapi::EShaderInputDescriptorType::Resource;
	inputSignatureDesc.descriptorSetArray[0].descriptorsNum = 2;
	inputSignatureDesc.descriptorSetArray[0].descriptorList[0] = {0, ts3::gpuapi::EShaderInputDescriptorType::Resource, ts3::gpuapi::E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT };
	inputSignatureDesc.descriptorSetArray[0].descriptorList[0].uResourceDesc = { ts3::gpuapi::EShaderInputResourceType::CBVConstantBuffer, 0, 1 };
	inputSignatureDesc.descriptorSetArray[0].descriptorList[1] = { 1, ts3::gpuapi::EShaderInputDescriptorType::Resource, ts3::gpuapi::E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT };
	inputSignatureDesc.descriptorSetArray[0].descriptorList[1].uResourceDesc = { ts3::gpuapi::EShaderInputResourceType::SRVTextureImage, 0, 1 };

	inputSignatureDesc.descriptorSetArray[1].descriptorType = ts3::gpuapi::EShaderInputDescriptorType::Sampler;
	inputSignatureDesc.descriptorSetArray[1].descriptorsNum = 1;
	inputSignatureDesc.descriptorSetArray[1].descriptorList[0] = { 10, ts3::gpuapi::EShaderInputDescriptorType::Sampler, ts3::gpuapi::E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT };
	inputSignatureDesc.descriptorSetArray[1].descriptorList[0].uSamplerDesc = { 0 };

	ts3::gpuapi::GraphicsPipelineStateObjectCreateInfo psoCreateInfo;
	psoCreateInfo.rasterizerDesc.cullMode = ts3::gpuapi::ECullMode::Back;
	psoCreateInfo.rasterizerDesc.primitiveFillMode = ts3::gpuapi::EPrimitiveFillMode::Solid;
    psoCreateInfo.rasterizerDesc.triangleFrontFaceOrder = ts3::gpuapi::ETriangleVerticesOrder::CounterClockwise;
	psoCreateInfo.depthStencilDesc = ts3::gpuapi::cvPipelineDescDepthStencilConfigEnabledDepthOnly;
	psoCreateInfo.vertexInputFormatDesc = inputFormatDesc;
	psoCreateInfo.shaderBindingDesc = &graphicsShaderPipelineDesc;
	psoCreateInfo.shaderInputSignatureDesc = &inputSignatureDesc;
	psoCreateInfo.renderTargetLayoutDesc = ts3::gpuapi::cvRenderTargetLayoutDescDefaultBGRA8D24S8;
	auto globalPSO = gpuDevice.createGraphicsPipelineStateObject( psoCreateInfo );

	ts3::gpuapi::VertexStreamStateObjectCreateInfo vssoCreateInfo;
	vssoCreateInfo.vertexDataSourceBindingDesc = vertexInputStreamDesc;
	auto globalVSSO = gpuDevice.createVertexStreamStateObject( vssoCreateInfo );

	auto rtSize = gxDriverState.presentationLayer->queryRenderTargetSize();
    // auto ts3View = ts3::math::lookAtLH(
    //     cameraOriginPoint,
    //     cameraTargetPoint,
    //     Vec3f{ 0.0f, 1.0f,  0.0f } );
	// auto ts3ProjectionScreen = perspectiveAspectLH<float>(
	// 		ts3::math::constants::cxFloatRad30Degree, ( float )rtSize.x / ( float )rtSize.y, 0.1f, 1000.0f );

	auto ts3ViewTexture = lookAtLH(
			Vec3f{ 0.0f, 3.0f,  -1.0f },
			Vec3f{ 0.0f, 0.0f,  5.0f },
			Vec3f{ 0.0f, 1.0f,  0.0f } );
	auto ts3ProjectionTexture = perspectiveAspectLH<float>(
			ts3::math::constants::cxFloatRad60Degree, ( float )rtSize.x / ( float )rtSize.y, 0.1f, 1000.0f );

	ts3::FontManager fontManager;
	fontManager.registerFontLoader( ts3::FONT_TYPE_FTF, [&gxDriverState]() { return std::make_unique<ts3::FreeTypeFontLoader>( *( gxDriverState.cmdContext ) ); } );

	const ts3::font_guid_t cxDefaultFontGUID = 0x1234567890;

	ts3::FreeTypeFontCreateInfo fontCreateInfo;
	fontCreateInfo.fontType = ts3::FONT_TYPE_FTF;
	fontCreateInfo.freeTypeFontID = "FNT_DEFAULT";
	fontCreateInfo.freeTypeFontFile = sWorkspaceDirectory + "/assets/fonts/ftf-calibri.ttf";
	fontCreateInfo.dynamicLayersNum = 0;
	fontCreateInfo.fontGUID = cxDefaultFontGUID;
	fontCreateInfo.fontSize = 32;
	fontCreateInfo.textureDimensions = { rtSize.x, rtSize.y };
	fontCreateInfo.fontResolutionHint.x = 48;
	fontCreateInfo.fontResolutionHint.y = 96;

	for( auto cp = 65; cp <= 122; ++cp )
	{
		fontCreateInfo.preloadGlyphSet.insert( cp );
	}

	auto defaultFont = fontManager.createFont( fontCreateInfo );

	CB0Data cb0Data =
	{
        Vec4f{0.12f,0.56f,0.92f,1.0f},
        identity4<float>(),
        identity4<float>(),
        identity4<float>(),
	};

	ts3::gpuapi::ViewportDesc vpDescScreen{};
	vpDescScreen.vpRect.opX = 0;
	vpDescScreen.vpRect.opY = 0;
	vpDescScreen.vpRect.sizeX = rtSize.x;
	vpDescScreen.vpRect.sizeY = rtSize.y;
	vpDescScreen.depthRange.zNear = 0.0f;
	vpDescScreen.depthRange.zFar = 1.0f;

	ts3::gpuapi::ViewportDesc vpDescTexture{};
	vpDescTexture.vpRect.opX = 0;
	vpDescTexture.vpRect.opY = 0;
	vpDescTexture.vpRect.sizeX = renderTargetSO->mRTResourceBinding.commonBufferSize.width;
	vpDescTexture.vpRect.sizeY = renderTargetSO->mRTResourceBinding.commonBufferSize.height;
	vpDescTexture.depthRange.zNear = 0.0f;
	vpDescTexture.depthRange.zFar = 1.0f;

	ts3::sys_perf_counter_value_t u1ts = ts3::SysPerfCounter::queryCurrentStamp();
	ts3::sys_perf_counter_value_t u2ts = ts3::SysPerfCounter::queryCurrentStamp();
	const float update1ts = 3.0f;
	const float update2ts = 10.0f;
	float u1angle = 0.0f;
	float u2angle = 0.0f;

	while( appExec )
	{
		auto pcstamp = ts3::SysPerfCounter::queryCurrentStamp();
		if( ts3::SysPerfCounter::convertToDuration<ts3::DurationPeriod::Millisecond>( pcstamp - u1ts ) >= update1ts )
		{
			u1angle += ts3::math::constants::cxFloatRad1Degree * 10 * ( 1.0f / update1ts );
			u1ts = pcstamp;
		}
		if( ts3::SysPerfCounter::convertToDuration<ts3::DurationPeriod::Millisecond>( pcstamp - u2ts ) >= update2ts )
		{
			u2angle += ts3::math::constants::cxFloatRad1Degree * 10 * ( 1.0f / update2ts);
			u2ts = pcstamp;
		}

		try
		{
			eventController->dispatchQueuedEvents();

			auto ts3ViewScreen = cameraController.computeViewMatrixLH();

			gxDriverState.cmdContext->beginCommandSequence();

			gxDriverState.cmdContext->setVertexStreamStateObject( *globalVSSO );
			gxDriverState.cmdContext->setGraphicsPipelineStateObject( *globalPSO );

			auto texFont = defaultFont->mTextureInfo.texture;

			gxDriverState.cmdContext->setShaderConstantBuffer( 0, *cbuffer0 );
			gxDriverState.cmdContext->setShaderTextureSampler( 10, *defaultSampler );

			const uint32 VNUM = 36;

			{
				gxDriverState.cmdContext->setRenderTargetStateObject( *renderTargetSO );
				gxDriverState.cmdContext->setViewport( vpDescTexture );
				gxDriverState.cmdContext->setColorBufferClearValue( ts3::math::RGBAColorU8 { 0x8F, 0x0F, 0x1F, 0xFF } );
				gxDriverState.cmdContext->setColorBufferClearValue( ts3::math::RGBAColorU8 { 0xFF, 0xFF, 0xFF, 0xFF } );
				gxDriverState.cmdContext->clearRenderTarget( ts3::gpuapi::E_RENDER_TARGET_ATTACHMENT_FLAGS_DEFAULT_C0DS );

				ts3::gpuapi::GPUBufferDataUploadDesc cb0DataUploadDesc;
				cb0DataUploadDesc.inputDataDesc.pointer = &cb0Data;
				cb0DataUploadDesc.inputDataDesc.size = sizeof(CB0Data);

				gxDriverState.cmdContext->setShaderTextureImage( 1, *tex0 );
				{
					cb0Data.projectionMatrix = ts3ProjectionTexture;
					cb0Data.viewMatrix = ts3ViewTexture;
                    cb0Data.modelMatrix = math::mul(
						math::translation<float>( 0, 0, 3.0f ),
						math::mul(
							math::scaling(2.0f, 2.0f, 2.0f),
							math::rotationAxisY( u1angle ) ) );
					gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0DataUploadDesc );
				}
				gxDriverState.cmdContext->drawDirectIndexed( VNUM, 0 );
				{
					cb0Data.projectionMatrix = ts3ProjectionTexture;
					cb0Data.viewMatrix = ts3ViewTexture;
                    cb0Data.modelMatrix = math::mul(
						math::translation<float>( 0, 0, 8.5f ),
						math::mul(
							math::scaling(2.0f, 2.0f, 2.0f),
							math::rotationAxisY( u2angle ) ) );
					gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0DataUploadDesc );
				}
				gxDriverState.cmdContext->drawDirectIndexed( VNUM, 0 );

				gxDriverState.presentationLayer->bindRenderTarget( gxDriverState.cmdContext.get() );
				gxDriverState.cmdContext->setViewport( vpDescScreen );
				gxDriverState.cmdContext->setColorBufferClearValue( gpuDevice.getDefaultClearColor() );
				gxDriverState.cmdContext->clearRenderTarget( ts3::gpuapi::E_RENDER_TARGET_ATTACHMENT_FLAGS_DEFAULT_C0DS );

				auto * rttTexture0 = renderTargetSO->mRTResourceBinding.colorAttachmentArray[0].uTextureRef.texture;
				gxDriverState.cmdContext->setShaderTextureImage( 1, *rttTexture0 );
				{
					cb0Data.projectionMatrix = perspectiveAspectLH<float>(
							cameraController.getPerspectiveFOVAngle(), ( float )rtSize.x / ( float )rtSize.y, 0.1f, 1000.0f );
					cb0Data.viewMatrix = ts3ViewScreen;
					cb0Data.modelMatrix = math::mul(
							math::translation<float>( -3, 0, 6.0f ),
							math::rotationAxisY( -1.0f ) );
					gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0DataUploadDesc );
				}
				gxDriverState.cmdContext->drawDirectIndexed( 6, 36 );
				{
					cb0Data.projectionMatrix = perspectiveAspectLH<float>(
							cameraController.getPerspectiveFOVAngle(), ( float )rtSize.x / ( float )rtSize.y, 0.1f, 1000.0f );
					cb0Data.viewMatrix = ts3ViewScreen;
					cb0Data.modelMatrix = math::mul(
							math::translation<float>( 3, 0, 6.0f ),
							math::rotationAxisY( 1.0f ) );
					gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0DataUploadDesc );
				}
				gxDriverState.cmdContext->drawDirectIndexed( 6, 36 );
			}

			gxDriverState.presentationLayer->invalidateRenderTarget( gxDriverState.cmdContext.get() );

			gxDriverState.cmdContext->endCommandSequence();
			gxDriverState.cmdContext->submit();

			gxDriverState.presentationLayer->present();
		}
		catch( bool pBreak )
		{
			if( pBreak )
			{
				break;
			}
		}
	}

	return 0;
}

GraphicsDriverState initializeGraphicsDriver( ts3::SysContextHandle pSysContext, const std::string & pDriverID )
{
	ts3::gpuapi::GPUDriverInterface * gpuDriverInterface = nullptr;

	if( pDriverID == "GL4" )
	{
		gpuDriverInterface = new ts3::gpuapi::GL4GPUDriverInterface();
	}
#if( _WIN32 )
	if( pDriverID == "DX11" )
	{
		gpuDriverInterface = new ts3::gpuapi::DX11GPUDriverInterface();
	}
#endif
	ts3DebugAssert( gpuDriverInterface );

	ts3::gpuapi::GPUDriverCreateInfo drvci;
	drvci.sysContext = pSysContext;
	drvci.configFlags = ts3::gpuapi::GPU_DRIVER_CONFIG_FLAG_ENABLE_SHADER_DEBUG_INFO_BIT;
	auto gxdrv = gpuDriverInterface->createDriver(drvci);

	ts3::gpuapi::GPUDeviceCreateInfo devci;
	devci.adapterID = 0;
	devci.flags = ts3::gpuapi::E_GPU_DEVICE_CREATE_FLAGS_DEFAULT;
	auto gxdev = gxdrv->createDevice(devci);

	ts3::gpuapi::PresentationLayerCreateInfo scci;
	scci.sysDisplayManager = ts3::SysDisplayManager::create( pSysContext );
	scci.visualConfig = ts3::sysDsmGetDefaultVisualConfigForSystemWindow();
	scci.displayConfigFlags = ts3::gpuapi::E_DISPLAY_CONFIGURATION_FLAG_FULLSCREEN_BIT;
    scci.screenRect.offset.x = 10;
    scci.screenRect.offset.y = 10;
    scci.screenRect.size.x = 720;
    scci.screenRect.size.y = 720;
	auto scn = gpuDriverInterface->createScreenPresentationLayer( *gxdev, scci );

	gxdev->setPresentationLayer( scn );

	GraphicsDriverState driverState;
	driverState.driver = gxdrv;
	driverState.device = gxdev;
	driverState.presentationLayer = scn;
	driverState.cmdContext = gxdev->getCommandSystem()->acquireCommandContext<ts3::gpuapi::CommandContextDirectGraphics>();

	return driverState;
}
