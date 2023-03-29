
#pragma once

#ifndef __TS3_ENGINE_SHADER_LIBRARY_H__
#define __TS3_ENGINE_SHADER_LIBRARY_H__

#include "commonRendererDefs.h"
#include <ts3/gpuapi/resources/shaderCommon.h>
#include <unordered_map>

namespace ts3
{

	class ShaderLibrary : public CoreEngineObject
	{
	public:
		explicit ShaderLibrary( const CoreEngineState & pCES );
		~ShaderLibrary();

		TS3_ATTR_NO_DISCARD bool empty() const noexcept;

		TS3_ATTR_NO_DISCARD gpuapi::ShaderHandle getShader( GpaUniqueObjectID pShaderID ) const noexcept;

		TS3_ATTR_NO_DISCARD gpuapi::ShaderHandle getShader( const GpaUniqueObjectName & pShaderName ) const noexcept;

		uint32 append( const ShaderLibrary & pOtherLibrary );

		bool registerShader( GpaUniqueObjectID pShaderID, gpuapi::ShaderHandle pShaderObject );

		bool registerShader( const GpaUniqueObjectName & pShaderName, gpuapi::ShaderHandle pShaderObject );

	private:
		std::unordered_map<GpaUniqueObjectID, gpuapi::ShaderHandle> _shaderMap;
	};

	inline bool ShaderLibrary::empty() const noexcept
	{
		return _shaderMap.empty();
	}

} // namespace ts3

#endif // __TS3_ENGINE_SHADER_LIBRARY_H__
