
#include "DX11_inputAssembler.h"
#include "../DX11_apiTranslationLayer.h"
#include "../DX11_gpuDevice.h"
#include "../resources/DX11_gpuBuffer.h"
#include "../resources/DX11_shader.h"

namespace ts3::gpuapi
{

	DX11IAInputLayoutImmutableState::DX11IAInputLayoutImmutableState(
			DX11GPUDevice & pGPUDevice,
			const IAInputLayoutStateCommonProperties & pCommonProperties,
			ComPtr<ID3D11InputLayout> pD3D11InputLayout,
			D3D11_PRIMITIVE_TOPOLOGY pD3D11PrimitiveTopology )
	: IAInputLayoutImmutableState( pGPUDevice, pCommonProperties )
	, mD3D11InputLayout( pD3D11InputLayout )
	, mD3D11PrimitiveTopology( pD3D11PrimitiveTopology )
	{}

	DX11IAInputLayoutImmutableState::~DX11IAInputLayoutImmutableState() = default;

	GpaHandle<DX11IAInputLayoutImmutableState> DX11IAInputLayoutImmutableState::createInstance(
			DX11GPUDevice & pGPUDevice,
			const IAInputLayoutDefinition & pInputLayoutDefinition,
			const ShaderBinary & pVertexShaderBinary )
	{
		const auto inputLayoutCommonProperties = smutil::getIAInputLayoutStateCommonProperties( pInputLayoutDefinition );
		const auto dx11InputLayoutDefinition = smutil::translateIAInputLayoutDefinitionDX11( pInputLayoutDefinition );

		ComPtr<ID3D11InputLayout> d3d11InputLayout;
		const auto hResult = pGPUDevice.mD3D11Device1->CreateInputLayout(
				dx11InputLayoutDefinition.attributeArray.data(),
				dx11InputLayoutDefinition.activeAttributesNum,
				pVertexShaderBinary.dataBuffer,
				pVertexShaderBinary.dataSizeInBytes,
				d3d11InputLayout.GetAddressOf() );

		if( FAILED( hResult ) )
		{
			ts3DebugInterrupt();
			return nullptr;
		}

		auto immutableState = createGPUAPIObject<DX11IAInputLayoutImmutableState>(
				pGPUDevice,
				inputLayoutCommonProperties,
				d3d11InputLayout,
				dx11InputLayoutDefinition.primitiveTopology );

		return immutableState;
	}


	DX11IAVertexStreamImmutableState::DX11IAVertexStreamImmutableState(
			DX11GPUDevice & pGPUDevice,
			const IAVertexStreamStateCommonProperties & pCommonProperties,
			const DX11IAVertexStreamDefinition & pDX11VertexStreamDefinition )
	: IAVertexStreamImmutableState( pGPUDevice, pCommonProperties )
	, mDX11VertexStreamDefinition( pDX11VertexStreamDefinition )
	{}

	DX11IAVertexStreamImmutableState::~DX11IAVertexStreamImmutableState() = default;

	GpaHandle<DX11IAVertexStreamImmutableState> DX11IAVertexStreamImmutableState::createInstance(
			DX11GPUDevice & pGPUDevice,
			const IAVertexStreamDefinition & pVertexStreamDefinition )
	{
		const auto vertexStreamCommonProperties = smutil::getIAVertexStreamStateCommonProperties( pVertexStreamDefinition );
		const auto dx11VertexStreamDefinition = smutil::translateIAVertexStreamDefinitionDX11( pVertexStreamDefinition );

		auto immutableState = createGPUAPIObject<DX11IAVertexStreamImmutableState>(
				pGPUDevice,
				vertexStreamCommonProperties,
				dx11VertexStreamDefinition );

		return immutableState;
	}

	
	namespace smutil
	{

		D3D11_INPUT_ELEMENT_DESC translateIAVertexAttributeInfoDX11(
				const IAVertexAttributeInfo & pAttributeInfo )
		{
			D3D11_INPUT_ELEMENT_DESC d3D11InputElementDesc;

			d3D11InputElementDesc.AlignedByteOffset = static_cast<UINT>( pAttributeInfo.relativeOffset );
			d3D11InputElementDesc.SemanticName = pAttributeInfo.semanticName.c_str();
			d3D11InputElementDesc.SemanticIndex = pAttributeInfo.semanticIndex;
			d3D11InputElementDesc.Format = atl::translateVertexAttribFormatDX( pAttributeInfo.format );
			d3D11InputElementDesc.InputSlot = pAttributeInfo.streamIndex;
			d3D11InputElementDesc.InputSlotClass = ( pAttributeInfo.instanceRate == 0 ) ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
			d3D11InputElementDesc.InstanceDataStepRate = pAttributeInfo.instanceRate;

			return d3D11InputElementDesc;
		}

		DX11IAInputLayoutDefinition translateIAInputLayoutDefinitionDX11(
				const IAInputLayoutDefinition & pDefinition )
		{
			DX11IAInputLayoutDefinition dx11InputLayoutDefinition;

			const auto definedVertexAttributesNum = popCount( pDefinition.activeAttributesMask );

			uint32 currentVertexAttributesNum = 0;
			uint64 currentAttributePackedRelativeOffset = 0;

			for( uint32 attributeIndex = 0; attributeIndex < gpm::IA_MAX_VERTEX_ATTRIBUTES_NUM; ++attributeIndex )
			{
				const auto attributeBit = cxdefs::makeIAVertexAttributeFlag( attributeIndex );
				if( pDefinition.activeAttributesMask.isSet( attributeBit ) )
				{
					const auto & inputAttributeInfo = pDefinition.attributeArray[attributeIndex];
					auto & dx11AttributeInfo = dx11InputLayoutDefinition.attributeArray[currentVertexAttributesNum];

					// Translate the attribute data. This includes the relative offset.
					dx11AttributeInfo = translateIAVertexAttributeInfoDX11( inputAttributeInfo );

					if( inputAttributeInfo.relativeOffset == cxdefs::IA_VERTEX_ATTRIBUTE_OFFSET_APPEND )
					{
						// If the offset is APPEND, update it with the current packed offset calculated.
						dx11AttributeInfo.AlignedByteOffset = numeric_cast<uint32>( memGetAlignedValue( currentAttributePackedRelativeOffset, 4 ) );
					}
					else if( inputAttributeInfo.relativeOffset == cxdefs::IA_VERTEX_ATTRIBUTE_OFFSET_APPEND16 )
					{
						// If the offset is APPEND, update it with the current packed offset calculated.
						dx11AttributeInfo.AlignedByteOffset = numeric_cast<uint32>( memGetAlignedValue( currentAttributePackedRelativeOffset, 16 ) );
					}

					// Update the current packed offset.
					const auto attributeByteSize = cxdefs::getVertexAttribFormatByteSize( inputAttributeInfo.format );
					currentAttributePackedRelativeOffset = dx11AttributeInfo.AlignedByteOffset + attributeByteSize;

					++currentVertexAttributesNum;

					if( currentVertexAttributesNum == definedVertexAttributesNum )
					{
						break;
					}
				}
			}

			dx11InputLayoutDefinition.activeAttributesNum = currentVertexAttributesNum;
			dx11InputLayoutDefinition.primitiveTopology = atl::translateDX11PrimitiveTopology( pDefinition.primitiveTopology );

			return dx11InputLayoutDefinition;
		}

		DX11IAVertexStreamDefinition translateIAVertexStreamDefinitionDX11(
				const IAVertexStreamDefinition & pDefinition )
		{
			DX11IAVertexStreamDefinition dx11IaVertexStreamDefinition;

			if( pDefinition.indexBufferReference )
			{
				auto * dx11IndexBuffer = pDefinition.indexBufferReference.sourceBuffer->queryInterface<DX11GPUBuffer>();

				dx11IaVertexStreamDefinition.indexBufferBinding.buffer = dx11IndexBuffer->mD3D11Buffer.Get();

				dx11IaVertexStreamDefinition.indexBufferBinding.offset =
						numeric_cast<uint32>( pDefinition.indexBufferReference.relativeOffset );

				dx11IaVertexStreamDefinition.indexBufferBinding.format =
						atl::translateBaseDataTypeDX( static_cast<EBaseDataType>( pDefinition.indexBufferReference.indexFormat ) );
			}

			for( native_uint vertexInputStreamIndex = 0; vertexInputStreamIndex < gpm::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM; ++vertexInputStreamIndex )
			{
				if( const auto & vertexBufferReference = pDefinition.vertexBufferReferences[vertexInputStreamIndex] )
				{
					auto * dx11VertexBuffer = vertexBufferReference.sourceBuffer->queryInterface<DX11GPUBuffer>();

					dx11IaVertexStreamDefinition.vertexBufferBindings.bindingData.bufferArray[vertexInputStreamIndex] =
							dx11VertexBuffer->mD3D11Buffer.Get();

					dx11IaVertexStreamDefinition.vertexBufferBindings.bindingData.offsetArray[vertexInputStreamIndex] =
							numeric_cast<UINT>( vertexBufferReference.relativeOffset );

					dx11IaVertexStreamDefinition.vertexBufferBindings.bindingData.strideArray[vertexInputStreamIndex] =
							numeric_cast<UINT>( vertexBufferReference.vertexStride );
				}
			}

			dx11IaVertexStreamDefinition.activeBindingsMask = pDefinition.activeBindingsMask;
			dx11IaVertexStreamDefinition.vertexBufferBindings.activeRanges = smutil::generateActiveVertexBufferRanges( pDefinition.vertexBufferReferences );

			return dx11IaVertexStreamDefinition;
		}

	}

}
