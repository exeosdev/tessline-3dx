
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_SHADER_INTERFACE_H__
#define __TS3DRIVER_GPUAPI_DX11_SHADER_INTERFACE_H__

#include "../DX11_prerequisites.h"
#include <ts3/gpuapi/resources/shader.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( DX11Shader );

	class TS3_GPUAPI_CLASS DX11Shader : public Shader
	{
	public:
		union
		{
			ID3D11VertexShader * const mD3D11VertexShader;
			ID3D11HullShader * const mD3D11HullShader;
			ID3D11DomainShader * const mD3D11DomainShader;
			ID3D11GeometryShader * const mD3D11GeometryShader;
			ID3D11PixelShader * const mD3D11PixelShader;
			ID3D11ComputeShader * const mD3D11ComputeShader;
		};
		ComPtr<ID3D11DeviceChild> const mD3D11ShaderReference;

		DX11Shader( DX11GPUDevice & pGPUDevice, ComPtr<ID3D11VertexShader> pD3D11VertexShader, std::unique_ptr<ShaderBinary> pShaderBinary );
		DX11Shader( DX11GPUDevice & pGPUDevice, ComPtr<ID3D11HullShader> pD3D11HullShader, std::unique_ptr<ShaderBinary> pShaderBinary );
		DX11Shader( DX11GPUDevice & pGPUDevice, ComPtr<ID3D11DomainShader> pD3D11DomainShader, std::unique_ptr<ShaderBinary> pShaderBinary );
		DX11Shader( DX11GPUDevice & pGPUDevice, ComPtr<ID3D11GeometryShader> pD3D11GeometryShader, std::unique_ptr<ShaderBinary> pShaderBinary );
		DX11Shader( DX11GPUDevice & pGPUDevice, ComPtr<ID3D11PixelShader> pD3D11PixelShader, std::unique_ptr<ShaderBinary> pShaderBinary );
		DX11Shader( DX11GPUDevice & pGPUDevice, ComPtr<ID3D11ComputeShader> pD3D11ComputeShader, std::unique_ptr<ShaderBinary> pShaderBinary );
		virtual ~DX11Shader();

		static DX11ShaderHandle create( DX11GPUDevice & pDX11GPUDevice, const ShaderCreateInfo & pCreateInfo );
	};


	namespace rcutil
	{

		TS3_ATTR_NO_DISCARD std::unique_ptr<ShaderBinary> compileShader(
				const void * pCode,
				size_t pCodeLength,
				const char * pEntryPoint,
				DXShaderTarget pShaderTarget,
				Bitmask<UINT> pCompileFlags );

	}

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX11_SHADER_INTERFACE_H__
