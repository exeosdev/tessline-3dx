
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_SHADER_STATE_H__
#define __TS3_GPUAPI_GRAPHICS_SHADER_STATE_H__

#include "commonGPUStateDefs.h"
#include "../resources/shaderCommon.h"

namespace ts3::gpuapi
{

	class GraphicsShaderSet
	{
	private:
		GraphicsShaderArray _shaderArray;

	public:
		using ShaderRefType = GraphicsShaderArray::value_type;

		/// All shaders in a continuous array, indexed according to order defined by EShaderStageIndex enumeration.
		/// @see EShaderStageIndex
		const GraphicsShaderArray & commonShaderArray;

		/// A pointer to a shader object used for the vertex stage of the shader pipeline.
		ShaderRefType & vertexShader;

		/// A pointer to a shader object used for the hull stage of the shader pipeline.
		ShaderRefType & hullShader;

		/// A pointer to a shader object used for the domain stage of the shader pipeline.
		ShaderRefType & domainShader;

		/// A pointer to a shader object used for the geometry stage of the shader pipeline.
		ShaderRefType & geometryShader;

		/// A pointer to a shader object used for the pixel stage of the shader pipeline.
		ShaderRefType & pixelShader;

	public:
		GraphicsShaderSet();

		GraphicsShaderSet( const GraphicsShaderSet & pSource );

		explicit GraphicsShaderSet( const GraphicsShaderArray & pShaderArray );

		GraphicsShaderSet & operator=( const GraphicsShaderSet & pRhs );

		GraphicsShaderSet & operator=( const GraphicsShaderArray & pRhs );

		TS3_ATTR_NO_DISCARD Shader * operator[]( size_t pIndex ) const noexcept;

		TS3_ATTR_NO_DISCARD Shader * operator[]( EShaderType pShaderType ) const noexcept;

		TS3_ATTR_NO_DISCARD Bitmask<EShaderStageFlags> getActiveShaderStagesMask() const noexcept;

		TS3_ATTR_NO_DISCARD uint32 getActiveShaderStagesNum() const noexcept;

		TS3_ATTR_NO_DISCARD bool validateShaders() const noexcept;

		void addShader( ShaderHandle pShader ) noexcept;

		void setShaders( const GraphicsShaderArray & pShaderArray ) noexcept;

		void resetStage( uint32 pStageIndex ) noexcept;

		void resetStage( EShaderType pShaderType ) noexcept;
	};

	// State Management Utility API
	namespace smutil
	{

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD Bitmask<EShaderStageFlags> getActiveShaderStagesMask(
				const GraphicsShaderArray & pShaderArray ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD uint32 getActiveShaderStagesNum(
				const GraphicsShaderArray & pShaderArray ) noexcept;

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GRAPHICS_SHADER_STATE_H__
