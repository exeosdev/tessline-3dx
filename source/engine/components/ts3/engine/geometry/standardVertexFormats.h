
#pragma once

#ifndef __TS3_ENGINE_STANDARD_VERTEX_FORMATS_H__
#define __TS3_ENGINE_STANDARD_VERTEX_FORMATS_H__

#include "geometryVertexFormat.h"

namespace ts3
{

	enum class EStandardVertexFormatID : enum_default_value_t
	{
		VF2DDefault = 1,
		VF3DDefaultTex1 = 2,
		VF3DDefaultTex4 = 3,
		VF3DDefaultInstance1 = 4
	};

	struct SVF2DDefault
	{
		math::Vec3f position;
	};

} // namespace ts3

#endif // __TS3_ENGINE_STANDARD_VERTEX_FORMATS_H__
