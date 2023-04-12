
#include "gpaSharedStateLibrary.h"
#include "shaderLibrary.h"
#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/gpuapi/state/inputAssemblerImmutableStates.h>
#include <ts3/gpuapi/state/pipelineImmutableStateCache.h>

namespace ts3
{

	GpaSharedStateLibrary::GpaSharedStateLibrary( const CoreEngineState & pCES, ShaderLibraryHandle pShaderLibrary )
	: CoreEngineObject( pCES )
	, mShaderLibrary( pShaderLibrary ? pShaderLibrary : createDynamicInterfaceObject<ShaderLibrary>( pCES ) )
	{}

	GpaSharedStateLibrary::~GpaSharedStateLibrary() = default;

	void GpaSharedStateLibrary::initializeDefaultImmutableBlendStates()
	{
		{
			mCES.mGPUDevice->createCachedImmutableState<gpuapi::BlendImmutableState>(
					cxdefs::GPA_STATE_ID_IA_INPUT_LAYOUT_DEFAULT,
					gpuapi::defaults::cvPipelineBlendConfigDefault );
		}
	}

	void GpaSharedStateLibrary::initializeDefaultImmutableDepthStencilStates()
	{
		{
			mCES.mGPUDevice->createCachedImmutableState<gpuapi::DepthStencilImmutableState>(
					cxdefs::GPA_STATE_ID_IA_INPUT_LAYOUT_DEFAULT,
					gpuapi::defaults::cvPipelineDepthStencilConfigDefault );
		}
		{
			mCES.mGPUDevice->createCachedImmutableState<gpuapi::DepthStencilImmutableState>(
					cxdefs::GPA_STATE_ID_DEPTH_STENCIL_DEPTH_TEST_ENABLE,
					gpuapi::defaults::cvPipelineDepthStencilConfigEnableDepthTest );
		}
	}

	void GpaSharedStateLibrary::initializeDefaultImmutableRasterizerStates()
	{
		{
			gpuapi::RasterizerConfig rasterizerConfig = gpuapi::defaults::cvPipelineRasterizerConfigDefault;
			rasterizerConfig.cullMode = gpuapi::ECullMode::Back;
			rasterizerConfig.frontFaceVerticesOrder = gpuapi::ETriangleVerticesOrder::CounterClockwise;

			mCES.mGPUDevice->createCachedImmutableState<gpuapi::RasterizerImmutableState>(
					cxdefs::GPA_STATE_ID_RASTERIZER_DEFAULT_CULL_BACK_CCW,
					rasterizerConfig );
		}
		{
			gpuapi::RasterizerConfig rasterizerConfig = gpuapi::defaults::cvPipelineRasterizerConfigDefault;
			rasterizerConfig.cullMode = gpuapi::ECullMode::Back;
			rasterizerConfig.frontFaceVerticesOrder = gpuapi::ETriangleVerticesOrder::Clockwise;

			mCES.mGPUDevice->createCachedImmutableState<gpuapi::RasterizerImmutableState>(
					cxdefs::GPA_STATE_ID_RASTERIZER_DEFAULT_CULL_BACK_CW,
					rasterizerConfig );
		}
		{
			gpuapi::RasterizerConfig rasterizerConfig = gpuapi::defaults::cvPipelineRasterizerConfigDefault;
			rasterizerConfig.cullMode = gpuapi::ECullMode::Front;
			rasterizerConfig.frontFaceVerticesOrder = gpuapi::ETriangleVerticesOrder::CounterClockwise;

			mCES.mGPUDevice->createCachedImmutableState<gpuapi::RasterizerImmutableState>(
					cxdefs::GPA_STATE_ID_RASTERIZER_DEFAULT_CULL_FRONT_CCW,
					rasterizerConfig );
		}
		{
			gpuapi::RasterizerConfig rasterizerConfig = gpuapi::defaults::cvPipelineRasterizerConfigDefault;
			rasterizerConfig.cullMode = gpuapi::ECullMode::Front;
			rasterizerConfig.frontFaceVerticesOrder = gpuapi::ETriangleVerticesOrder::Clockwise;

			mCES.mGPUDevice->createCachedImmutableState<gpuapi::RasterizerImmutableState>(
					cxdefs::GPA_STATE_ID_RASTERIZER_DEFAULT_CULL_FRONT_CW,
					rasterizerConfig );
		}
	}

	void GpaSharedStateLibrary::initializeDefaultImmutableInputLayoutStates()
	{
		{
			gpuapi::IAInputLayoutDefinition inputLayoutDefinition;
			inputLayoutDefinition.primitiveTopology = gpuapi::EPrimitiveTopology::TriangleList;
			inputLayoutDefinition.activeAttributesMask =
					gpuapi::E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_0_BIT |
					gpuapi::E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_1_BIT |
					gpuapi::E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_2_BIT |
					gpuapi::E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_3_BIT;
			inputLayoutDefinition.attributeArray[0] = { 0, "POSITION", 0, ts3::gpuapi::EVertexAttribFormat::Vec3F32, 0 };
			inputLayoutDefinition.attributeArray[1] = { 0, "COLOR", 0, ts3::gpuapi::EVertexAttribFormat::Vec4F32, ts3::gpuapi::cxdefs::IA_VERTEX_ATTRIBUTE_OFFSET_APPEND };
			inputLayoutDefinition.attributeArray[2] = { 0, "NORMAL", 0, ts3::gpuapi::EVertexAttribFormat::Vec3F32, ts3::gpuapi::cxdefs::IA_VERTEX_ATTRIBUTE_OFFSET_APPEND };
			inputLayoutDefinition.attributeArray[3] = { 0, "TEXCOORD", 0, ts3::gpuapi::EVertexAttribFormat::Vec2F32, ts3::gpuapi::cxdefs::IA_VERTEX_ATTRIBUTE_OFFSET_APPEND };

			const auto shaderPassthroughIADefaultVS = mShaderLibrary->getShader( cxdefs::GPA_RESOURCE_ID_SHADER_PASSTHROUGH_IA_DEFAULT_VS );

			mCES.mGPUDevice->createCachedImmutableState<gpuapi::IAInputLayoutImmutableState>(
					cxdefs::GPA_STATE_ID_IA_INPUT_LAYOUT_DEFAULT,
					inputLayoutDefinition,
					*shaderPassthroughIADefaultVS );
		}
		{
			gpuapi::IAInputLayoutDefinition inputLayoutDefinition;
			inputLayoutDefinition.primitiveTopology = gpuapi::EPrimitiveTopology::TriangleList;
			inputLayoutDefinition.activeAttributesMask =
					gpuapi::E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_0_BIT |
					gpuapi::E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_1_BIT |
					gpuapi::E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_2_BIT |
					gpuapi::E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_3_BIT;
			inputLayoutDefinition.attributeArray[0] = { 0, "POSITION", 0, ts3::gpuapi::EVertexAttribFormat::Vec3F32, 0 };
			inputLayoutDefinition.attributeArray[1] = { 0, "COLOR", 0, ts3::gpuapi::EVertexAttribFormat::Vec4F32, ts3::gpuapi::cxdefs::IA_VERTEX_ATTRIBUTE_OFFSET_APPEND16 };
			inputLayoutDefinition.attributeArray[2] = { 0, "NORMAL", 0, ts3::gpuapi::EVertexAttribFormat::Vec3F32, ts3::gpuapi::cxdefs::IA_VERTEX_ATTRIBUTE_OFFSET_APPEND16 };
			inputLayoutDefinition.attributeArray[3] = { 0, "TEXCOORD", 0, ts3::gpuapi::EVertexAttribFormat::Vec2F32, ts3::gpuapi::cxdefs::IA_VERTEX_ATTRIBUTE_OFFSET_APPEND16 };

			const auto shaderPassthroughIADefaultVS = mShaderLibrary->getShader( cxdefs::GPA_RESOURCE_ID_SHADER_PASSTHROUGH_IA_DEFAULT_VS );

			mCES.mGPUDevice->createCachedImmutableState<gpuapi::IAInputLayoutImmutableState>(
					cxdefs::GPA_STATE_ID_IA_INPUT_LAYOUT_DEFAULT_16B,
					inputLayoutDefinition,
					*shaderPassthroughIADefaultVS );
		}
	}

	void GpaSharedStateLibrary::initializeDefaultPipelineStateObjects()
	{
	}

}
