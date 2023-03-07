
#pragma once

#ifndef __TS3_GPUAPI_SHADER_COMMON_H__
#define __TS3_GPUAPI_SHADER_COMMON_H__

#include "../prerequisites.h"
#include <ts3/stdext/byteArray.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( Shader );

	namespace cxdefs
	{

		TS3_ATTR_NO_DISCARD inline constexpr uint32 makeShaderType( uint32 pStageIndex )
		{
			return ( pStageIndex << 16 ) | cxdefs::makeShaderStageBit( pStageIndex );
		}

	}

	/// @brief
	enum EShaderCreateFlags : uint32
	{
		E_SHADER_CREATE_FLAG_DEBUG_BIT                = 0x0001,
		E_SHADER_CREATE_FLAG_OPTIMIZATION_DISABLE_BIT = 0x0010,
		E_SHADER_CREATE_FLAG_OPTIMIZATION_L0_BIT      = 0x0020,
		E_SHADER_CREATE_FLAG_OPTIMIZATION_L1_BIT      = 0x0040,
		E_SHADER_CREATE_FLAG_OPTIMIZATION_MAX_BIT     = 0x0080,
		E_SHADER_CREATE_FLAGS_DEFAULT = 0
	};

	/// @brief
	enum class EShaderType : uint32
	{
		GSVertex        = cxdefs::makeShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX        ),
		GSHull          = cxdefs::makeShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_HULL          ),
		GSDomain        = cxdefs::makeShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_DOMAIN        ),
		GSGeometry      = cxdefs::makeShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_GEOMETRY      ),
		GSPixel         = cxdefs::makeShaderType( E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL         ),
		CSCompute       = cxdefs::makeShaderType( E_SHADER_STAGE_INDEX_COMPUTE                ),
		Unknown = 0,
	};

	struct alignas( 32 ) ShaderBinary
	{
		static constexpr size_t sDataBufferFixedSize = 12;

		uint64 driverSpecificID;
		uint64 driverSpecificType;
		uint32 dataSizeInBytes;
		byte dataBuffer[sDataBufferFixedSize];

		ShaderBinary() = default;

		ShaderBinary( std::nullptr_t )
		: dataSizeInBytes( 0 )
		{}

		TS3_ATTR_NO_DISCARD explicit operator bool() const
		{
			return !empty();
		}

		TS3_ATTR_NO_DISCARD bool empty() const
		{
			return dataSizeInBytes == 0;
		}

		/// @brief Allocates a ShaderBinary object capable of storing shader binary data of the specified size.
		/// @return An std::unique_ptr holding the created ShaderBinary.
		/// @notes Defined in shader.cpp file.
		TS3_GPUAPI_API_NO_DISCARD static std::unique_ptr<ShaderBinary> create( size_t pBinarySize );
	};

	/// @brief
	struct ShaderCreateInfo
	{
		EShaderType shaderType = EShaderType::Unknown;
		UniqueGPUObjectName shaderName;
		Bitmask<EShaderCreateFlags> createFlags = E_SHADER_CREATE_FLAGS_DEFAULT;
		std::unique_ptr<ShaderBinary> shaderBinary;
		DynamicByteArray shaderSource;
		ReadOnlyMemoryView shaderSourceView;
		const char * entryPointName = nullptr;

		explicit operator bool() const noexcept
		{
			return ( shaderType != EShaderType::Unknown ) && ( shaderBinary || !shaderSource.empty() || !shaderSourceView.empty() );
		}
	};

	namespace cxdefs
	{

		TS3_ATTR_NO_DISCARD inline constexpr bool isShaderStageIndexValid( uint32 pStageIndex )
		{
			return ( pStageIndex >= E_SHADER_STAGE_INDEX_BASE ) && ( pStageIndex <= E_SHADER_STAGE_INDEX_MAX );
		}

		TS3_ATTR_NO_DISCARD inline constexpr bool isShaderStageIndexValidGraphics( uint32 pStageIndex )
		{
			return ( pStageIndex >= E_SHADER_STAGE_INDEX_BASE ) && ( pStageIndex <= E_SHADER_STAGE_INDEX_MAX_GRAPHICS );
		}

		TS3_ATTR_NO_DISCARD inline constexpr uint32 getShaderStageIndex( EShaderType pShaderType )
		{
			return ( static_cast<uint32>( pShaderType ) >> 16 & 0xFFFF ) - E_SHADER_STAGE_INDEX_BASE;
		}

		TS3_ATTR_NO_DISCARD inline constexpr uint32 getShaderStageBit( EShaderType pShaderType )
		{
			return static_cast<uint32>( pShaderType ) & 0xFFFF;
		}

		TS3_ATTR_NO_DISCARD inline constexpr bool isShaderTypeGraphics( EShaderType pShaderType )
		{
			return getShaderStageIndex( pShaderType ) <= E_SHADER_STAGE_INDEX_MAX_GRAPHICS;
		}

		TS3_ATTR_NO_DISCARD inline constexpr bool isShaderTypeValid( EShaderType pShaderType )
		{
			return getShaderStageIndex( pShaderType ) <= E_SHADER_STAGE_INDEX_MAX;
		}

		TS3_ATTR_NO_DISCARD inline constexpr EShaderType getShaderTypeFromStageIndex( uint32 pStageIndex )
		{
			return isShaderStageIndexValid( pStageIndex ) ? static_cast<EShaderType>( cxdefs::makeShaderType( pStageIndex ) ) : EShaderType::Unknown;
		}
		
	}

	namespace rcutil
	{

		TS3_GPUAPI_API_NO_DISCARD EShaderType getShaderObjectType( Shader & pShader );

		TS3_GPUAPI_API_NO_DISCARD uint32 getShaderObjectStageIndex( Shader & pShader );

		TS3_GPUAPI_API_NO_DISCARD uint32 getShaderObjectStageBit( Shader & pShader );

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_SHADER_COMMON_H__
