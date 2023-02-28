
#pragma once

#ifndef __TS3_GPUAPI_COMMON_TYPES_H__
#define __TS3_GPUAPI_COMMON_TYPES_H__

#include <ts3/math/color.h>
#include <ts3/math/vector.h>

namespace ts3::gpuapi
{

	struct ViewportDesc
	{
		struct DepthRange
		{
			float zNear;
			float zFar;
		};

		DepthRange depthRange;
		math::Vec2u32 origin;
		math::Size2u size;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_COMMON_TYPES_H__
