
#pragma once

#ifndef __TS3_GPUAPI_SHADER_COMMON_H__
#define __TS3_GPUAPI_SHADER_COMMON_H__

#include "../prerequisites.h"

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( Shader );

	inline constexpr uint32 ecMakeShaderType( uint32 pStageIndex )
	{
		return ( pStageIndex << 16 ) | ecMakeShaderStageBit( pStageIndex );
	}

	/// @brief
	enum EShaderCreateFlags : uint32
	{
		E_SHADER_CREATE_FLAG_DEBUG_BIT                = 0x0001,
		E_SHADER_CREATE_FLAG_OPTIMIZATION_DISABLE_BIT = 0x0010,
		E_SHADER_CREATE_FLAG_OPTIMIZATION_L0_BIT      = 0x0020,
		E_SHADER_CREATE_FLAG_OPTIMIZATION_L1_BIT      = 0x0040,
		E_SHADER_CREATE_FLAG_OPTIMIZATION_LMAX_BIT    = 0x0080,
		E_SHADER_CREATE_FLAGS_DEFAULT = 0
	};

	enum class EShaderType : uint32
	{
		GSVertex         = ecMakeShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX           ),
		GSTessControl    = ecMakeShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_TESS_CONTROL     ),
		GSTessEvaluation = ecMakeShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_TESS_EVALUATION  ),
		GSGeometry       = ecMakeShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_GEOMETRY         ),
		GSAmplification  = ecMakeShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_AMPLIFICATION    ),
		GSMesh           = ecMakeShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_MESH             ),
		GSPixel          = ecMakeShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL            ),
		CSCompute        = ecMakeShaderType( E_SHADER_STAGE_INDEX_COMPUTE                   ),
		Unknown          = 0
	};

	enum class EGraphicsShaderStageID : uint32
	{
		Vertex         = static_cast<uint32>( EShaderType::GSVertex ),
		TessControl    = static_cast<uint32>( EShaderType::GSTessControl ),
		TessEvaluation = static_cast<uint32>( EShaderType::GSTessEvaluation ),
		Geometry       = static_cast<uint32>( EShaderType::GSGeometry ),
		Amplification  = static_cast<uint32>( EShaderType::GSAmplification ),
		Mesh           = static_cast<uint32>( EShaderType::GSMesh ),
		Pixel          = static_cast<uint32>( EShaderType::GSPixel ),
		Unknown        = static_cast<uint32>( EShaderType::Unknown ),
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

		TS3_FUNC_NO_DISCARD bool empty() const
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

	template <typename TpEnum>
	struct IsValidShaderTypeEnum
	{
		static constexpr bool sValue = std::is_same<TpEnum, EShaderType>::value || std::is_same<TpEnum, EGraphicsShaderStageID>::value;
	};

	template <typename TpGetAs = EShaderStageIndex, typename TpEnum, std::enable_if_t<IsValidShaderTypeEnum<TpEnum>::sValue, int> = 0>
	TS3_FUNC_NO_DISCARD inline constexpr TpGetAs ecGetShaderStageIndex( TpEnum pShaderType )
	{
		return static_cast<TpGetAs>( ( static_cast<uint32>( pShaderType ) >> 16 ) & 0xFFFF );
	}

	template <typename TpGetAs = EShaderStageFlags, typename TpEnum, std::enable_if_t<IsValidShaderTypeEnum<TpEnum>::sValue, int> = 0>
	TS3_FUNC_NO_DISCARD inline constexpr TpGetAs ecGetShaderStageBit( TpEnum pShaderType )
	{
		return static_cast<TpGetAs>( static_cast<uint32>( pShaderType ) & 0xFFFF );
	}

	template <typename TpEnum, std::enable_if_t<IsValidShaderTypeEnum<TpEnum>::sValue, int> = 0>
	TS3_FUNC_NO_DISCARD inline constexpr bool ecIsGraphicsShaderType( TpEnum pShaderType )
	{
		return ecGetShaderStageIndex( pShaderType ) <= E_SHADER_STAGE_INDEX_MAX_GRAPHICS;
	}

	template <typename TpEnum, std::enable_if_t<IsValidShaderTypeEnum<TpEnum>::sValue, int> = 0>
	TS3_FUNC_NO_DISCARD inline constexpr bool ecIsValidShaderType( TpEnum pShaderType )
	{
		return ecGetShaderStageIndex( pShaderType ) <= E_SHADER_STAGE_INDEX_MAX;
	}

	TS3_FUNC_NO_DISCARD inline constexpr EGraphicsShaderStageID ecShaderTypeToGraphicsShaderStageID( EShaderType pShaderType )
	{
		return ecIsGraphicsShaderType( pShaderType ) ? static_cast<EGraphicsShaderStageID>( pShaderType ) : EGraphicsShaderStageID::Unknown;
	}

	TS3_FUNC_NO_DISCARD inline constexpr EShaderType ecGraphicsShaderStageIDToShaderType( EGraphicsShaderStageID pGraphicsShaderStageID )
	{
		return ecIsGraphicsShaderType( pGraphicsShaderStageID ) ? static_cast<EShaderType>( pGraphicsShaderStageID ) : EShaderType::Unknown;
	}

	TS3_FUNC_NO_DISCARD inline constexpr EShaderType ecGetShaderTypeFromStageIndex( uint32 pStageIndex )
	{
		return ( pStageIndex <= E_SHADER_STAGE_INDEX_MAX ) ?
		       static_cast<EShaderType>( ecMakeShaderType( pStageIndex ) ) :
		       EShaderType::Unknown;
	}

	TS3_FUNC_NO_DISCARD inline constexpr EGraphicsShaderStageID ecGetGraphicsShaderStageIDFromStageIndex( uint32 pStageIndex )
	{
		return ( pStageIndex <= E_SHADER_STAGE_INDEX_MAX_GRAPHICS ) ?
		       static_cast<EGraphicsShaderStageID>( ecMakeShaderType( pStageIndex ) ) :
		       EGraphicsShaderStageID::Unknown;
	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_SHADER_COMMON_H__
