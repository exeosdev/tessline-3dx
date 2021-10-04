
#include <ts3/system/display.h>
#include <ts3/system/sysContextNative.h>
#include <ts3/system/windowNative.h>
#include <ts3/system/eventSystemNative.h>
#include <ts3/system/openGL.h>
#include <ts3/system/assetSystemNative.h>
#include <ts3/system/perfCounter.h>

#include <ts3/core/coreEngineState.h>
#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/gpuapi/gpuDriver.h>
#include <ts3/gpuapi/gpuDriverAPI.h>
#include <ts3/gpuapi/commandContext.h>
#include <ts3/gpuapi/commandSystem.h>
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

#include <ts3/engine/camera/cameraController.h>
#include <ts3/engine/gpuapi/vertexFormatDefs.h>
#include <ts3/engine/gpuapi/hwBuffer.h>
#include <ts3/engine/rcdata/font.h>
#include <ts3/engine/rcdata/fontManager.h>

#include <ts3ext/rcsupport/fonts/fontTypeFTF.h>
#include <ts3ext/rcsupport/images/bitmapCommon.h>

#include "sysTest-gpuapi-meshDefs.h"

using namespace ts3;
using namespace ts3::gpuapi;
using namespace ts3::system;
namespace math = ts3::math;

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
    std::string fxSrcPassThroughPs;
    std::string fxSrcPassThroughVs;
    ImageData txROG512Data;
};

void initializeGraphicsDriver( SysContextHandle pSysContext, GraphicsDriverState & pGxDriverState );

struct CB0Data
{
    math::Vec4f scolor;
    math::Mat4x4f modelMatrix;
    math::Mat4x4f viewMatrix;
    math::Mat4x4f projectionMatrix;
};

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )

#include <ts3/gpuapiGLES3/GLES3_gpuDriverAPI.h>
#include "../../engine/components/ts3/gpuapi/gpuDriver.h"

int ts3AndroidAppMain( int argc, char ** argv, AndroidAppState * pAppState )
{
    const std::string sGxDriverName = "GLES3";

    SysContextCreateInfo sysContextCreateInfo {};
    sysContextCreateInfo.nativeParams.aCommonAppState = pAppState;
    sysContextCreateInfo.flags = 0;
    auto sysContext = nativeSysContextCreate( sysContextCreateInfo );

    AssetLoaderCreateInfo aslCreateInfo;
    aslCreateInfo.sysContext = sysContext;
    auto assetLoader = createAssetLoader( aslCreateInfo );

    GraphicsDriverState gxDriverState;
    gxDriverState.driverID = "GLES3";
    gxDriverState.driverInterface = std::make_unique<GLES3GPUDriverInterface>();

#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )

#include <ts3/gpuapiGL4/GL4_gpuDriverAPI.h>

int main( int pArgc, const char ** pArgv )
{
    const std::string sGxDriverName = "GL4";

    SysContextCreateInfo sysContextCreateInfo;
    sysContextCreateInfo.flags = 0;
    sysContextCreateInfo.nativeParams.appExecModuleHandle = ::GetModuleHandleA( nullptr );
    auto sysContext = nativeSysContextCreate( sysContextCreateInfo );

    AssetLoaderCreateInfo aslCreateInfo;
    aslCreateInfo.sysContext = sysContext;
    aslCreateInfo.nativeParams.relativeAssetRootDir = "../../../../../tessline-3dx/assets";
    auto assetLoader = createAssetLoader( aslCreateInfo );

    GraphicsDriverState gxDriverState;
    gxDriverState.driverID = "GL4";
    gxDriverState.driverInterface = std::make_unique<GL4GPUDriverInterface>();

#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )

#include <ts3/gpuapiGL4/GL4_gpuDriverAPI.h>

int main( int pArgc, const char ** pArgv )
{
    const std::string sGxDriverName = "GL4";

    SysContextCreateInfo sysContextCreateInfo;
    sysContextCreateInfo.flags = 0;
    auto sysContext = nativeSysContextCreate( sysContextCreateInfo );

    AssetLoaderCreateInfo aslCreateInfo;
    aslCreateInfo.sysContext = sysContext;
    aslCreateInfo.nativeParams.relativeAssetRootDir = "../../../../../tessline-3dx/assets";
    auto assetLoader = createAssetLoader( aslCreateInfo );

    GraphicsDriverState gxDriverState;
    gxDriverState.driverID = "GL4";
    gxDriverState.driverInterface = std::make_unique<GL4GPUDriverInterface>();

#endif

    srand( time( nullptr ) );

    bool runApp = true;

    auto evtController = createSysObject<EventController>( sysContext );
    auto evtDispatcher = evtController->createEventDispatcher();
    evtController->setActiveEventDispatcher( *evtDispatcher );

#if( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_OS_ANDROID )
    bool waitForDisplay = true;

    evtDispatcher->bindEventHandler(
            EEventCodeIndex::AppActivityDisplayInit,
            [&waitForDisplay,&sysContext](const EventObject & pEvt) -> bool {
                waitForDisplay = false;
                return true;
            });

    while( waitForDisplay )
    {
        evtController->updateSysQueueAuto();
    }

//    evtDispatcher->bindEventHandler(
//            EEventCodeIndex::AppActivityDisplayInit,
//            [&gfxState](const EventObject & pEvt) -> bool {
//                initializeGraphicsGL( gfxState );
//                gfxState.glContext->bindForCurrentThread( *(gfxState.glSurface) );
//                gfxState.pauseAnimation = false;
//                return true;
//            });
//
//    evtDispatcher->bindEventHandler(
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

    evtDispatcher->bindEventHandler(
            EEventCodeIndex::AppActivityQuit,
            [&runApp,&gxDriverState](const EventObject & pEvt) -> bool {
                runApp = false;
                return true;
            });
    evtDispatcher->bindEventHandler(
            EEventCodeIndex::WindowUpdateClose,
            [evtDispatcher,&gxDriverState](const EventObject & pEvt) -> bool {
                if( pEvt.eWindowUpdateClose.checkEventSource( gxDriverState.presentationLayer->getInternalSystemEventSource() ) )
                {
                    evtDispatcher->postEventAppQuit();
                }
                return true;
            });
    evtDispatcher->bindEventHandler(
            EEventCodeIndex::InputKeyboardKey,
            [evtDispatcher,&gxDriverState](const EventObject & pEvt) -> bool {
                if( pEvt.eInputKeyboardKey.keyCode == EKeyCode::Escape )
                {
                    evtDispatcher->postEventAppQuit();
                }
                return true;
            });

    AppSharedResources appResources;

    {
        auto psAsset = assetLoader->openSubAsset( "shaders/"+sGxDriverName+"/fx_passthrough_ps", E_ASSET_OPEN_FLAG_NO_EXTENSION_BIT );
        psAsset->readAll( appResources.fxSrcPassThroughPs );
    }
    {
        auto vsAsset = assetLoader->openSubAsset( "shaders/"+sGxDriverName+"/fx_passthrough_vs", E_ASSET_OPEN_FLAG_NO_EXTENSION_BIT );
        vsAsset->readAll( appResources.fxSrcPassThroughVs );
    }
    {
        auto txROG512Asset = assetLoader->openSubAsset( "bitmaps/rog-logo1-512", E_ASSET_OPEN_FLAG_NO_EXTENSION_BIT );

        DynamicMemoryBuffer tmpBuffer;
        txROG512Asset->readAll( tmpBuffer );
        appResources.txROG512Data = loadBitmapFromMemory( tmpBuffer.dataPtr(), tmpBuffer.size() );
    }

    initializeGraphicsDriver( sysContext, gxDriverState );

    if( auto * eventSource = gxDriverState.presentationLayer->getInternalSystemEventSource() )
    {
        evtController->registerEventSource( *eventSource );
    }

    auto vertexShader = utils::createShaderFromSource( *(gxDriverState.device), EShaderType::VertexShader, appResources.fxSrcPassThroughVs );
    auto pixelShader = utils::createShaderFromSource( *(gxDriverState.device), EShaderType::PixelShader, appResources.fxSrcPassThroughPs );

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
        txci.dimensions.width = appResources.txROG512Data.formatInfo.dimensions.x;
        txci.dimensions.height = appResources.txROG512Data.formatInfo.dimensions.y;
        txci.memoryFlags = ts3::gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
        txci.resourceFlags = ts3::gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT;
        txci.pixelFormat = ts3::gpuapi::ETextureFormat::R8G8B8A8_UNORM;
        txci.initialTarget = ts3::gpuapi::ETextureTarget::ShaderInputSampledImage;
        txci.initDataDesc.initialize( txci.dimensions );
        txci.initDataDesc.subTextureInitDataBasePtr[0].mipLevelInitDataArray[0].pointer = appResources.txROG512Data.pixelBuffer.dataPtr();
        txci.initDataDesc.subTextureInitDataBasePtr[0].mipLevelInitDataArray[0].size = appResources.txROG512Data.sizeInBytes;
        tex0 = gpuDevicePtr->createTexture( txci );
    }

    ts3::gpuapi::TextureHandle texRTColor0;
    {
        ts3::gpuapi::TextureCreateInfo texRTColor0CI;
        texRTColor0CI.dimensionClass = ts3::gpuapi::ETextureDimensionClass::Texture2D;
        texRTColor0CI.dimensions.width = 1920;
        texRTColor0CI.dimensions.height = 1080;
        texRTColor0CI.memoryFlags = ts3::gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
        texRTColor0CI.resourceFlags =
                ts3::gpuapi::E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_COLOR_BIT |
                ts3::gpuapi::E_GPU_RESOURCE_USAGE_FLAG_SHADER_INPUT_BIT;
        texRTColor0CI.pixelFormat = ts3::gpuapi::ETextureFormat::R8G8B8A8_UNORM;
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

    auto defaultSampler = gxDriverState.device->createSampler( samplerCreateInfo );

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
    psoCreateInfo.renderTargetLayoutDesc = ts3::gpuapi::cvRenderTargetLayoutDescDefaultRGBA8D24S8;
    auto globalPSO = gxDriverState.device->createGraphicsPipelineStateObject( psoCreateInfo );

    ts3::gpuapi::VertexStreamStateObjectCreateInfo vssoCreateInfo;
    vssoCreateInfo.vertexDataSourceBindingDesc = vertexInputStreamDesc;
    auto globalVSSO = gxDriverState.device->createVertexStreamStateObject( vssoCreateInfo );

    auto rtSize = gxDriverState.presentationLayer->queryRenderTargetSize();

    auto ts3ViewTexture = lookAtLH(
            math::Vec3f{ 0.0f, 3.0f,  -1.0f },
            math::Vec3f{ 0.0f, 0.0f,  5.0f },
            math::Vec3f{ 0.0f, 1.0f,  0.0f } );
    auto ts3ProjectionTexture = math::perspectiveAspectLH<float>(
            ts3::math::constants::cxFloatRad60Degree, ( float )rtSize.x / ( float )rtSize.y, 0.1f, 1000.0f );

    math::Vec3f cameraOriginPoint{ 0.0f, 2.0f,  -1.0f };
    math::Vec3f cameraTargetPoint{ 0.0f, 0.0f,  5.0f };

    ts3::CameraController cameraController;
    cameraController.initialize( cameraOriginPoint, cameraTargetPoint, 60.0f );

    bool rotate = false;

    evtDispatcher->bindEventHandler(
            ts3::system::EEventCodeIndex::InputMouseButton,
            [&cameraController,&rotate]( const ts3::system::EventObject & pEvt ) -> bool {
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
    evtDispatcher->bindEventHandler(
            ts3::system::EEventCodeIndex::InputMouseMove,
            [&cameraController,&rotate]( const ts3::system::EventObject & pEvt ) -> bool {
                //if( rotate )
                {
                    const auto & emove = pEvt.eInputMouseMove;
                    cameraController.rotateAroundTarget( emove.movementDelta.x, emove.movementDelta.y );
                    //if( emove.buttonStateMask.isSet( ts3::system::E_MOUSE_BUTTON_FLAG_LEFT_BIT ) )
                    //{
                    //    cameraController.rotateAroundOrigin( emove.movementDelta.x, emove.movementDelta.y );
                    //}
                    //else if( emove.buttonStateMask.isSet( ts3::system::E_MOUSE_BUTTON_FLAG_RIGHT_BIT ) )
                    //{
                    //    cameraController.rotateAroundTarget( emove.movementDelta.x, emove.movementDelta.y );
                    //}
                }
                return true;
            });

    CB0Data cb0Data =
    {
        math::Vec4f{0.12f,0.56f,0.92f,1.0f},
        math::identity4<float>(),
        math::identity4<float>(),
        math::identity4<float>(),
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

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////

    ts3::system::perf_counter_value_t u1ts = ts3::system::PerfCounter::queryCurrentStamp();
    ts3::system::perf_counter_value_t u2ts = ts3::system::PerfCounter::queryCurrentStamp();
    const float update1ts = 3.0f;
    const float update2ts = 10.0f;
    float u1angle = 0.0f;
    float u2angle = 0.0f;

    while( runApp )
    {
        if( gxDriverState.pauseAnimation )
        {
            continue;
        }

        auto pcstamp = PerfCounter::queryCurrentStamp();
        if( PerfCounter::convertToDuration<ts3::EDurationPeriod::Millisecond>( pcstamp - u1ts ) >= update1ts )
        {
            u1angle += math::constants::cxFloatRad1Degree * 10 * ( 1.0f / update1ts );
            u1ts = pcstamp;
        }
        if( PerfCounter::convertToDuration<ts3::EDurationPeriod::Millisecond>( pcstamp - u2ts ) >= update2ts )
        {
            u2angle += math::constants::cxFloatRad1Degree * 10 * ( 1.0f / update2ts);
            u2ts = pcstamp;
        }

        try
        {
            evtController->updateSysQueueAuto();

            auto ts3ViewScreen = cameraController.computeViewMatrixLH();
            gxDriverState.cmdContext->beginCommandSequence();
            gxDriverState.cmdContext->setVertexStreamStateObject( *globalVSSO );
            gxDriverState.cmdContext->setGraphicsPipelineStateObject( *globalPSO );
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
                gxDriverState.cmdContext->setColorBufferClearValue( gxDriverState.device->getDefaultClearColor() );
                gxDriverState.cmdContext->clearRenderTarget( ts3::gpuapi::E_RENDER_TARGET_ATTACHMENT_FLAGS_DEFAULT_C0DS );

                auto * rttTexture0 = renderTargetSO->mRTResourceBinding.colorAttachmentArray[0].uTextureRef.texture;
                gxDriverState.cmdContext->setShaderTextureImage( 1, *rttTexture0 );
                {
                    cb0Data.projectionMatrix = math::perspectiveAspectLH<float>(
                            cameraController.getPerspectiveFOVAngle(), ( float )rtSize.x / ( float )rtSize.y, 0.1f, 1000.0f );
                    cb0Data.viewMatrix = ts3ViewScreen;
                    cb0Data.modelMatrix = math::mul(
                            math::translation<float>( -3, 0, 6.0f ),
                            math::rotationAxisY( -1.0f ) );
                    gxDriverState.cmdContext->updateBufferDataUpload( *cbuffer0, cb0DataUploadDesc );
                }
                gxDriverState.cmdContext->drawDirectIndexed( 6, 36 );
                {
                    cb0Data.projectionMatrix = math::perspectiveAspectLH<float>(
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

void initializeGraphicsDriver( SysContextHandle pSysContext, GraphicsDriverState & pGxDriverState )
{
    GPUDriverCreateInfo gpuDriverCreateInfo;
    gpuDriverCreateInfo.sysContext = pSysContext;
    gpuDriverCreateInfo.configFlags = GPU_DRIVER_CONFIG_FLAG_ENABLE_SHADER_DEBUG_INFO_BIT;
    pGxDriverState.driver = pGxDriverState.driverInterface->createDriver( gpuDriverCreateInfo );

    GPUDeviceCreateInfo gpuDeviceCreateInfo;
    gpuDeviceCreateInfo.adapterID = 0;
    gpuDeviceCreateInfo.flags = E_GPU_DEVICE_CREATE_FLAGS_DEFAULT;
    pGxDriverState.device = pGxDriverState.driver->createDevice( gpuDeviceCreateInfo );

    PresentationLayerCreateInfo presentationLayerCreateInfo;
    presentationLayerCreateInfo.displayConfigFlags = E_DISPLAY_CONFIGURATION_FLAG_FULLSCREEN_BIT;
    pGxDriverState.presentationLayer = pGxDriverState.driverInterface->createScreenPresentationLayer( *( pGxDriverState.device ), presentationLayerCreateInfo );

    pGxDriverState.device->setPresentationLayer( pGxDriverState.presentationLayer );
    pGxDriverState.cmdContext = pGxDriverState.device->getCommandSystem()->acquireCommandContext<CommandContextDirectGraphics>();

    return;
}
