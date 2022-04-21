
#pragma once

#ifndef __TS3_GPUAPI_COMMON_TYPES_H__
#define __TS3_GPUAPI_COMMON_TYPES_H__

#include <ts3/math/color.h>


namespace ts3::gpuapi
{

	struct ViewportDesc
	{
		struct DepthRange
		{
			float zNear;
			float zFar;
		};
		struct ViewportRect
		{
			uint32 opX;
			uint32 opY;
			uint32 sizeX;
			uint32 sizeY;
		};
		DepthRange depthRange;
		ViewportRect vpRect;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_COMMON_TYPES_H__
