
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DXCOMMON_COMMON_SHADER_DEFS_H__
#define __TS3DRIVER_GPUAPI_DXCOMMON_COMMON_SHADER_DEFS_H__

namespace ts3::gpuapi
{

	enum class DXShaderTarget : uint64
	{
		SM_4_1_VS,
		SM_4_1_GS,
		SM_4_1_PS,
		SM_4_1_CS,
		SM_5_0_VS,
		SM_5_0_HS,
		SM_5_0_DS,
		SM_5_0_GS,
		SM_5_0_PS,
		SM_5_0_CS,
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DXCOMMON_COMMON_SHADER_DEFS_H__
