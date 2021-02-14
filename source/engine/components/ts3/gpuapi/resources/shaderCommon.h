
#pragma once

#ifndef __TS3_GPUAPI_SHADER_COMMON_H__
#define __TS3_GPUAPI_SHADER_COMMON_H__

#include "../prerequisites.h"

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( Shader );

	enum EShaderStageIndex : uint32
	{
		E_SHADER_STAGE_INDEX_BASE_GRAPHICS,
		E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX = E_SHADER_STAGE_INDEX_BASE_GRAPHICS,
		E_SHADER_STAGE_INDEX_GRAPHICS_TESS_CONTROL,
		E_SHADER_STAGE_INDEX_GRAPHICS_TESS_EVALUATION,
		E_SHADER_STAGE_INDEX_GRAPHICS_GEOMETRY,
		E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL,
		E_SHADER_STAGE_INDEX_MAX_GRAPHICS = E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL,
		E_SHADER_STAGE_INDEX_BASE_COMPUTE,
		E_SHADER_STAGE_INDEX_COMPUTE = E_SHADER_STAGE_INDEX_BASE_COMPUTE,
		E_SHADER_STAGE_INDEX_MAX_COMPUTE = E_SHADER_STAGE_INDEX_COMPUTE,
		E_SHADER_STAGE_INDEX_MAX = E_SHADER_STAGE_INDEX_MAX_COMPUTE
	};

	enum EShaderStageFlags : uint32
	{
		E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT          = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX,
		E_SHADER_STAGE_FLAG_GRAPHICS_TESS_CONTROL_BIT    = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_TESS_CONTROL,
		E_SHADER_STAGE_FLAG_GRAPHICS_TESS_EVALUATION_BIT = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_TESS_EVALUATION,
		E_SHADER_STAGE_FLAG_GRAPHICS_GEOMETRY_BIT        = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_GEOMETRY,
		E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT           = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL,
		E_SHADER_STAGE_FLAG_COMPUTE_BIT                  = 1u << E_SHADER_STAGE_INDEX_COMPUTE,
		E_SHADER_STAGE_FLAGS_GRAPHICS_ALL                = 0x003F,
		E_SHADER_STAGE_FLAGS_ALL                         = E_SHADER_STAGE_FLAGS_GRAPHICS_ALL | E_SHADER_STAGE_FLAG_COMPUTE_BIT,
		E_SHADER_STAGE_FLAGS_NONE                        = 0
	};

	constexpr inline uint32 ecDeclareEShaderType( uint32 pStageIndex, uint32 pStageBit )
	{
		return ( pStageIndex << 24 ) | ( pStageBit & E_SHADER_STAGE_FLAGS_ALL );
	}

	enum class EShaderType : uint32
	{
		VertexShader         = ecDeclareEShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX,          E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT ),
		TessControlShader    = ecDeclareEShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_TESS_CONTROL,    E_SHADER_STAGE_FLAG_GRAPHICS_TESS_CONTROL_BIT ),
		TessEvaluationShader = ecDeclareEShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_TESS_EVALUATION, E_SHADER_STAGE_FLAG_GRAPHICS_TESS_EVALUATION_BIT ),
		GeometryShader       = ecDeclareEShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_GEOMETRY,        E_SHADER_STAGE_FLAG_GRAPHICS_GEOMETRY_BIT ),
		PixelShader          = ecDeclareEShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL,           E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT ),
		ComputeShader        = ecDeclareEShaderType( E_SHADER_STAGE_INDEX_COMPUTE,                  E_SHADER_STAGE_FLAG_COMPUTE_BIT ),
		Unknown              = 0
	};

	enum class EGraphicsShaderStageID : uint32
	{
		Vertex         = static_cast<uint32>( EShaderType::VertexShader ),
		TessControl    = static_cast<uint32>( EShaderType::TessControlShader ),
		TessEvaluation = static_cast<uint32>( EShaderType::TessEvaluationShader ),
		Geometry       = static_cast<uint32>( EShaderType::GeometryShader ),
		Pixel          = static_cast<uint32>( EShaderType::PixelShader ),
		Unknown        = 0
	};

	constexpr inline uint32 ecGetEShaderStageIndex( EShaderType pShaderType )
	{
		return ( static_cast<uint32>( pShaderType ) >> 24 ) & 0xFF;
	}

	constexpr inline uint32 ecGetEShaderStageIndex( EGraphicsShaderStageID pStageID )
	{
		return ecGetEShaderStageIndex( static_cast<EShaderType>( pStageID ) );
	}

	constexpr inline EShaderStageFlags ecGetShaderStageMaskBit( EShaderType pShaderType )
	{
		return static_cast<EShaderStageFlags>( static_cast<uint32>( pShaderType ) & 0xFFFFFF );
	}

	constexpr inline EShaderStageFlags ecGetShaderStageMaskBit( EGraphicsShaderStageID pStageID )
	{
		return ecGetShaderStageMaskBit( static_cast<EShaderType>( pStageID ) );
	}

	constexpr inline EShaderStageFlags ecGetShaderStageMaskBit( uint32 pStageIndex )
	{
		return ( pStageIndex <= E_SHADER_STAGE_INDEX_MAX ) ? static_cast<EShaderStageFlags>( 1 << pStageIndex ) : static_cast<EShaderStageFlags>( 0 );
	}

	constexpr inline bool ecIsShaderGraphics( EShaderType pShaderType )
	{
		return ( ecGetShaderStageMaskBit( pShaderType ) & E_SHADER_STAGE_FLAGS_GRAPHICS_ALL ) != 0;
	}

	constexpr inline EGraphicsShaderStageID ecGetEGraphicsShaderStageID( EShaderType pShaderType )
	{
		return ecIsShaderGraphics( pShaderType ) ? static_cast<EGraphicsShaderStageID>( pShaderType ) : EGraphicsShaderStageID::Unknown;
	}

	constexpr inline EGraphicsShaderStageID ecGetEGraphicsShaderStageIDByIndex( uint32 pStageIndex )
	{
		return ( pStageIndex <= E_SHADER_STAGE_INDEX_MAX_GRAPHICS ) ?
		       static_cast<EGraphicsShaderStageID>( ecDeclareEShaderType( pStageIndex, 1 << pStageIndex ) ) :
		       EGraphicsShaderStageID::Unknown;
	}

	constexpr inline EShaderType ecGetEShaderTypeIDByIndex( uint32 pStageIndex )
	{
		return ( pStageIndex <= E_SHADER_STAGE_INDEX_MAX ) ?
		       static_cast<EShaderType>( ecDeclareEShaderType( pStageIndex, 1 << pStageIndex ) ) :
		       EShaderType::Unknown;
	}

	enum EShaderCreateFlags : uint32
	{
		E_SHADER_CREATE_FLAG_DEBUG_BIT                = 0x0001,
		E_SHADER_CREATE_FLAG_OPTIMIZATION_DISABLE_BIT = 0x0002,
		E_SHADER_CREATE_FLAG_OPTIMIZATION_L0_BIT      = 0x0010,
		E_SHADER_CREATE_FLAG_OPTIMIZATION_L1_BIT      = 0x0020,
		E_SHADER_CREATE_FLAGS_DEFAULT = 0
	};

	struct ShaderBinary
	{
		uint64 driverSpecificID;
		uint64 driverSpecificType;
		std::vector<byte> rawBuffer;
		uint32 rawBufferSize;

		ShaderBinary() = default;

		ShaderBinary( std::nullptr_t )
		: rawBufferSize( 0 )
		{}

		explicit operator bool() const
		{
			return !empty();
		}

		bool empty() const
		{
			return rawBuffer.empty() || ( rawBufferSize == 0 );
		}
	};

	struct ShaderCreateInfo
	{
		EShaderType shaderType;
		Bitmask<EShaderCreateFlags> createFlags = E_SHADER_CREATE_FLAGS_DEFAULT;
		const void * sourceCode = nullptr;
		size_t sourceCodeLength = 0;
		const char * entryPointName = nullptr;
	};

	constexpr inline EShaderStageFlags getGraphicsShaderStageFlag( uint32 pShaderStageIndex )
	{
		return static_cast<EShaderStageFlags>( ( 1 << pShaderStageIndex ) & E_SHADER_STAGE_FLAGS_GRAPHICS_ALL );
	}

	constexpr inline EShaderStageFlags getGraphicsShaderStageFlag( EShaderType pShaderType )
	{
		return getGraphicsShaderStageFlag( static_cast<uint32>( pShaderType ) );
	}

}

#endif // __TS3_GPUAPI_SHADER_COMMON_H__
