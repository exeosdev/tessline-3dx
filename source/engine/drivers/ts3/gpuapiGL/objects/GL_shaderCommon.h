
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_COMMON_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_COMMON_H__

#include "../GL_prerequisites.h"
#include <map>

namespace ts3::gpuapi
{

	struct GLShaderDataLayoutMap
	{
		using ElementBindingMap = std::map<std::string, GLuint>;
		ElementBindingMap attributeLocations;
		ElementBindingMap fragDataLocations;
		ElementBindingMap samplerBindings;
		ElementBindingMap uniformBlockBindings;

		TS3_ATTR_NO_DISCARD bool empty() const noexcept
		{
			return attributeLocations.empty() && fragDataLocations.empty() && samplerBindings.empty() && uniformBlockBindings.empty();
		}
	};

	namespace rcutil
	{

		bool processGLShaderSourceExplicitLayoutQualifiers(
				const Version & pSupportedAPIVersion,
				std::string & pShaderSource,
				GLShaderDataLayoutMap & pOutputLayoutMap );

	}

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_COMMON_H__
