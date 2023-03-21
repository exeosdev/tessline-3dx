
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_INPUT_ASSEMBLER_H__
#define __TS3DRIVER_GPUAPI_DX11_INPUT_ASSEMBLER_H__

#include "../DX11_prerequisites.h"
#include <ts3/gpuapi/resources/shaderCommon.h>
#include <ts3/gpuapi/state/inputAssemblerImmutableStates.h>

namespace ts3::gpuapi
{

	using DX11IAVertexAttributeInfoArray = std::array<D3D11_INPUT_ELEMENT_DESC, gpm::IA_MAX_VERTEX_ATTRIBUTES_NUM>;

	struct DX11IAInputLayoutDefinition
	{
		uint32 activeAttributesNum;
		DX11IAVertexAttributeInfoArray attributeArray;
		D3D11_PRIMITIVE_TOPOLOGY primitiveTopology;
	};

	struct DX11IAIndexBufferBinding
	{
		ID3D11Buffer * buffer;
		uint32 offset;
		DXGI_FORMAT format;

		explicit operator bool() const noexcept
		{
			return buffer != nullptr;
		}
	};

	struct DX11IAVertexBuffersBindings
	{
		struct BindingData
		{
			ID3D11Buffer * bufferArray[gpm::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM];

			UINT offsetArray[gpm::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM];

			UINT strideArray[gpm::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM];
		};

		IAVertexBufferRangeList activeRanges;

		BindingData bindingData;

		explicit operator bool() const
		{
			return !activeRanges.empty();
		}
	};

	/// @brief
	struct DX11IAVertexStreamDefinition
	{
		Bitmask<EIAVertexStreamBindingFlags> activeBindingsMask;

		DX11IAVertexBuffersBindings vertexBufferBindings;

		DX11IAIndexBufferBinding indexBufferBinding;

	};

	///
	class DX11IAInputLayoutImmutableState : public IAInputLayoutImmutableState
	{
	public:
		ComPtr<ID3D11InputLayout> const mD3D11InputLayout;
		D3D11_PRIMITIVE_TOPOLOGY const mD3D11PrimitiveTopology;

	public:
		DX11IAInputLayoutImmutableState(
				DX11GPUDevice & pGPUDevice,
				const IAInputLayoutStateCommonProperties & pCommonProperties,
				ComPtr<ID3D11InputLayout> pD3D11InputLayout,
				D3D11_PRIMITIVE_TOPOLOGY pD3D11PrimitiveTopology );

		virtual ~DX11IAInputLayoutImmutableState();

		static GpaHandle<DX11IAInputLayoutImmutableState> createInstance(
				DX11GPUDevice & pGPUDevice,
				const IAInputLayoutDefinition & pInputLayoutDefinition,
				const ShaderBinary & pVertexShaderBinary );
	};

	///
	class DX11IAVertexStreamImmutableState : public IAVertexStreamImmutableState
	{
	public:
		DX11IAVertexStreamDefinition const mDX11VertexStreamDefinition;

	public:
		DX11IAVertexStreamImmutableState(
				DX11GPUDevice & pGPUDevice,
				const IAVertexStreamStateCommonProperties & pCommonProperties,
				const DX11IAVertexStreamDefinition & pDX11VertexStreamDefinition );

		virtual ~DX11IAVertexStreamImmutableState();

		static GpaHandle<DX11IAVertexStreamImmutableState> createInstance(
				DX11GPUDevice & pGPUDevice,
				const IAVertexStreamDefinition & pVertexStreamDefinition );
	};
	
	
	namespace smutil
	{

		TS3_ATTR_NO_DISCARD D3D11_INPUT_ELEMENT_DESC translateIAVertexAttributeInfoDX11(
				const IAVertexAttributeInfo & pAttributeInfo );

		TS3_ATTR_NO_DISCARD DX11IAInputLayoutDefinition translateIAInputLayoutDefinitionDX11(
				const IAInputLayoutDefinition & pDefinition );
		
		TS3_ATTR_NO_DISCARD DX11IAVertexStreamDefinition translateIAVertexStreamDefinitionDX11(
				const IAVertexStreamDefinition & pDefinition );
		
	}

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX11_INPUT_ASSEMBLER_H__
