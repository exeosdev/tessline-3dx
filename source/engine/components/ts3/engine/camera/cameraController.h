
#pragma once

#ifndef __TS3_ENGINE_CAMERA_CAMERA_CONTROLLER_H__
#define __TS3_ENGINE_CAMERA_CAMERA_CONTROLLER_H__

#include "cameraCommonDefs.h"

namespace ts3
{

	class TS3_ENGINE_CLASS CameraController
	{
	public:
		CameraState mCameraState;

		void initialize( const math::Vec3f & pOrigin, const math::Vec3f & pTarget, float pFOVAngle );

		void setTarget( const math::Vec2f & pTarget );

		void move( float pFactor = 1.0f );
		void moveFlat( float pFactor = 1.0f );
		void moveSide( float pFactor = 1.0f );
		void moveSideFlat( float pFactor = 1.0f );
		void moveUpDown( float pFactor = 1.0f );
		void roll( float pRoll );
		void rotateAroundOrigin( float pYaw, float pPitch );
		void rotateAroundTarget( float pYaw, float pPitch );
		void zoom( int32 pZoomFactor );

		math::Mat4f computeViewMatrixLH();
		math::Mat4f computeViewMatrixRH();

		float getPerspectiveFOVAngle() const;

	private:
		void updateRotation();
	};

} // namespace ts3

#endif // __TS3_ENGINE_CAMERA_CAMERA_CONTROLLER_H__
