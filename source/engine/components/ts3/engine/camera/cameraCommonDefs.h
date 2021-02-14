
#pragma once

#ifndef __TS3_ENGINE_CAMERA_CAMERA_COMMON_DEFS_H__
#define __TS3_ENGINE_CAMERA_CAMERA_COMMON_DEFS_H__

#include "../prerequisites.h"

namespace ts3
{

	class CameraProjection
	{
	};

	class CameraProjectionOrtho : public CameraProjection
	{
	};

	class CameraProjectionPerspective : public CameraProjection
	{
	};

	struct CameraState
	{
		struct Configuration
		{
			float movementSensitivity;
			float rollSensitivity;
			float rotationSensitivity;
			uint32 maxZoomLevel = 8;
			float zoomBaseFactor = 4.0f;
		};
		struct Orientation
		{
			math::Vec3f origin;
			math::Vec3f target;
			math::Vec3f vForward;
			math::Vec3f vRight;
			math::Vec3f vUp;
			float originTargetDistance;
		};
		struct Projection
		{
			float fovAngle;
			float fovAngleDiff;
			uint32 zoomLevel = 1;
		};
		struct Rotation
		{
			float yaw;
			float pitch;
			float roll;
		};

		Configuration configuration;
		Orientation orientation;
		Projection projection;
		Rotation rotation;

	};

}

#endif // __TS3_ENGINE_CAMERA_CAMERA_COMMON_DEFS_H__
