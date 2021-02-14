
#include "DX11_shader.h"
#include <ts3/gpuapiDX11/DX11_coreAPIProxy.h>
#include <ts3/gpuapiDX11/DX11_gpuDevice.h>
#include <ts3/stdext/memory.h>

namespace ts3::gpuapi
{

	DX11Shader::DX11Shader( DX11GPUDevice & pGPUDevice, ComPtr<ID3D11VertexShader> pD3D11VertexShader, ShaderBinary pShaderBinary )
	: Shader( pGPUDevice, EShaderType::VertexShader, std::move( pShaderBinary ) )
	, mD3D11VertexShader( pD3D11VertexShader.Get() )
	, mD3D11ShaderReference( pD3D11VertexShader )
	{}

	DX11Shader::DX11Shader( DX11GPUDevice & pGPUDevice, ComPtr<ID3D11HullShader> pD3D11HullShader, ShaderBinary pShaderBinary )
	: Shader( pGPUDevice, EShaderType::TessControlShader, std::move( pShaderBinary ) )
	, mD3D11HullShader( pD3D11HullShader.Get() )
	, mD3D11ShaderReference( pD3D11HullShader )
	{}

	DX11Shader::DX11Shader( DX11GPUDevice & pGPUDevice, ComPtr<ID3D11DomainShader> pD3D11DomainShader, ShaderBinary pShaderBinary )
	: Shader( pGPUDevice, EShaderType::TessEvaluationShader, std::move( pShaderBinary ) )
	, mD3D11DomainShader( pD3D11DomainShader.Get() )
	, mD3D11ShaderReference( pD3D11DomainShader )
	{}

	DX11Shader::DX11Shader( DX11GPUDevice & pGPUDevice, ComPtr<ID3D11GeometryShader> pD3D11GeometryShader, ShaderBinary pShaderBinary )
	: Shader( pGPUDevice, EShaderType::GeometryShader, std::move( pShaderBinary ) )
	, mD3D11GeometryShader( pD3D11GeometryShader.Get() )
	, mD3D11ShaderReference( pD3D11GeometryShader )
	{}

	DX11Shader::DX11Shader( DX11GPUDevice & pGPUDevice, ComPtr<ID3D11PixelShader> pD3D11PixelShader, ShaderBinary pShaderBinary )
	: Shader( pGPUDevice, EShaderType::PixelShader, std::move( pShaderBinary ) )
	, mD3D11PixelShader( pD3D11PixelShader.Get() )
	, mD3D11ShaderReference( pD3D11PixelShader )
	{}

	DX11Shader::DX11Shader( DX11GPUDevice & pGPUDevice, ComPtr<ID3D11ComputeShader> pD3D11ComputeShader, ShaderBinary pShaderBinary )
	: Shader( pGPUDevice, EShaderType::ComputeShader, std::move( pShaderBinary ) )
	, mD3D11ComputeShader( pD3D11ComputeShader.Get() )
	, mD3D11ShaderReference( pD3D11ComputeShader )
	{}

	DX11Shader::~DX11Shader() = default;

	DX11ShaderHandle DX11Shader::create( DX11GPUDevice & pDX11GPUDevice, const ShaderCreateInfo & pCreateInfo )
	{
		const char * entryPoint = pCreateInfo.entryPointName ? pCreateInfo.entryPointName : "main";

		auto compileFlags = DXCoreAPIProxy::translateDXShaderCompileFlags( pCreateInfo.createFlags, pDX11GPUDevice.isDebugDevice() );

		DX11ShaderHandle dx11Shader;

		if( pCreateInfo.shaderType == EShaderType::VertexShader )
		{
			if( auto shaderBinary = compileShader( pCreateInfo.sourceCode, pCreateInfo.sourceCodeLength, entryPoint, DXShaderTarget::SM_5_0_VS, compileFlags ) )
			{
				ComPtr<ID3D11VertexShader> d3d11VertexShader;
				auto hResult = pDX11GPUDevice.mD3D11Device1->CreateVertexShader( shaderBinary.rawBuffer.data(),
				                                                                 shaderBinary.rawBufferSize,
				                                                                 nullptr,
				                                                                 d3d11VertexShader.GetAddressOf() );
				if( SUCCEEDED( hResult ) )
				{
					dx11Shader = createDynamicInterfaceObject<DX11Shader>( pDX11GPUDevice, d3d11VertexShader, std::move( shaderBinary ) );
				}
			}
		}
		else if( pCreateInfo.shaderType == EShaderType::TessControlShader )
		{
			if( auto shaderBinary = compileShader( pCreateInfo.sourceCode, pCreateInfo.sourceCodeLength, entryPoint, DXShaderTarget::SM_5_0_HS, compileFlags ) )
			{
				ComPtr<ID3D11HullShader> d3d11HullShader;
				auto hResult = pDX11GPUDevice.mD3D11Device1->CreateHullShader( shaderBinary.rawBuffer.data(),
				                                                               shaderBinary.rawBufferSize,
				                                                               nullptr,
				                                                               d3d11HullShader.GetAddressOf() );
				if( SUCCEEDED( hResult ) )
				{
					dx11Shader = createDynamicInterfaceObject<DX11Shader>( pDX11GPUDevice, d3d11HullShader, std::move( shaderBinary ) );
				}
			}
		}
		else if( pCreateInfo.shaderType == EShaderType::TessEvaluationShader )
		{
			if( auto shaderBinary = compileShader( pCreateInfo.sourceCode, pCreateInfo.sourceCodeLength, entryPoint, DXShaderTarget::SM_5_0_DS, compileFlags ) )
			{
				ComPtr<ID3D11DomainShader> d3d11DomainShader;
				auto hResult = pDX11GPUDevice.mD3D11Device1->CreateDomainShader( shaderBinary.rawBuffer.data(),
				                                                                 shaderBinary.rawBufferSize,
				                                                                 nullptr,
				                                                                 d3d11DomainShader.GetAddressOf() );
				if( SUCCEEDED( hResult ) )
				{
					dx11Shader = createDynamicInterfaceObject<DX11Shader>( pDX11GPUDevice, d3d11DomainShader, std::move( shaderBinary ) );
				}
			}
		}
		else if( pCreateInfo.shaderType == EShaderType::GeometryShader )
		{
			if( auto shaderBinary = compileShader( pCreateInfo.sourceCode, pCreateInfo.sourceCodeLength, entryPoint, DXShaderTarget::SM_5_0_GS, compileFlags ) )
			{
				ComPtr<ID3D11GeometryShader> d3d11GeometryShader;
				auto hResult = pDX11GPUDevice.mD3D11Device1->CreateGeometryShader( shaderBinary.rawBuffer.data(),
				                                                                   shaderBinary.rawBufferSize,
				                                                                   nullptr,
				                                                                   d3d11GeometryShader.GetAddressOf() );
				if( SUCCEEDED( hResult ) )
				{
					dx11Shader = createDynamicInterfaceObject<DX11Shader>( pDX11GPUDevice, d3d11GeometryShader, std::move( shaderBinary ) );
				}
			}
		}
		else if( pCreateInfo.shaderType == EShaderType::PixelShader )
		{
			if( auto shaderBinary = compileShader( pCreateInfo.sourceCode, pCreateInfo.sourceCodeLength, entryPoint, DXShaderTarget::SM_5_0_PS, compileFlags ) )
			{
				ComPtr<ID3D11PixelShader> d3d11PixelShader;
				auto hResult = pDX11GPUDevice.mD3D11Device1->CreatePixelShader( shaderBinary.rawBuffer.data(),
				                                                                shaderBinary.rawBufferSize,
				                                                                nullptr,
				                                                                d3d11PixelShader.GetAddressOf() );
				if( SUCCEEDED( hResult ) )
				{
					dx11Shader = createDynamicInterfaceObject<DX11Shader>( pDX11GPUDevice, d3d11PixelShader, std::move( shaderBinary ) );
				}
			}
		}
		else if( pCreateInfo.shaderType == EShaderType::ComputeShader )
		{
			if( auto shaderBinary = compileShader( pCreateInfo.sourceCode, pCreateInfo.sourceCodeLength, entryPoint, DXShaderTarget::SM_5_0_CS, compileFlags ) )
			{
				ComPtr<ID3D11ComputeShader> d3d11ComputeShader;
				auto hResult = pDX11GPUDevice.mD3D11Device1->CreateComputeShader( shaderBinary.rawBuffer.data(),
				                                                                  shaderBinary.rawBufferSize,
				                                                                  nullptr,
				                                                                  d3d11ComputeShader.GetAddressOf() );
				if( SUCCEEDED( hResult ) )
				{
					dx11Shader = createDynamicInterfaceObject<DX11Shader>( pDX11GPUDevice, d3d11ComputeShader, std::move( shaderBinary ) );
				}
			}
		}

		return dx11Shader;
	}

	ShaderBinary DX11Shader::compileShader( const void * pCode, size_t pCodeLength, const char * pEntryPoint, DXShaderTarget pShaderTarget, Bitmask<UINT> pCompileFlags )
	{
		const char * shaderTargetStr = DX11CoreAPIProxy::getDXShaderTargetStr( pShaderTarget );

		ComPtr<ID3DBlob> compiledBinaryBuffer;
		ComPtr<ID3DBlob> errorMessagesBuffer;

		auto hResult = D3DCompile( pCode,
		                           pCodeLength,
		                           nullptr,
		                           nullptr,
		                           nullptr,
		                           pEntryPoint,
		                           shaderTargetStr,
		                           pCompileFlags,
		                           0,
		                           compiledBinaryBuffer.GetAddressOf(),
		                           errorMessagesBuffer.GetAddressOf() );

		if( FAILED( hResult ) )
		{
			if( errorMessagesBuffer )
			{
				auto * errorStr = (const char *)errorMessagesBuffer->GetBufferPointer();
				std::string err{ errorStr };;
				ts3DebugOutput( errorStr );
				ts3DebugInterrupt();
			}
			return nullptr;
		}

		auto binarySize = compiledBinaryBuffer->GetBufferSize();
		if( binarySize == 0 )
		{
			return nullptr;
		}

		ShaderBinary binary;
		binary.driverSpecificID = 0;
		binary.driverSpecificType = static_cast<uint64>( pShaderTarget );
		binary.rawBufferSize = trunc_numeric_cast<uint32>( binarySize );
		binary.rawBuffer.resize( binarySize );
		memCopy( binary.rawBuffer.data(), binary.rawBuffer.size(), compiledBinaryBuffer->GetBufferPointer(), binarySize );

		return binary;
	}

}
