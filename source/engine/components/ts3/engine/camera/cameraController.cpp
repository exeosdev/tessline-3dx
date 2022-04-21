
#include "cameraController.h"

namespace ts3
{

	void CameraController::initialize( const math::Vec3f & pOrigin, const math::Vec3f & pTarget, float pFOVAngle )
	{
		auto vForward = pTarget - pOrigin;
		mCameraState.configuration.movementSensitivity = 1.0f;
		mCameraState.configuration.rollSensitivity = 0.0975f;
		mCameraState.configuration.rotationSensitivity = 0.0075f;
		mCameraState.orientation.origin = pOrigin;
		mCameraState.orientation.target = pTarget;
		mCameraState.orientation.originTargetDistance = math::length( vForward );
		mCameraState.orientation.vForward = math::normalize( vForward );
		mCameraState.orientation.vRight = math::normalize( math::cross( math::Vec3f( 0, 1, 0 ), mCameraState.orientation.vForward ) );
		mCameraState.orientation.vUp = math::normalize( math::cross( mCameraState.orientation.vForward, mCameraState.orientation.vRight ) );
		mCameraState.rotation.roll = 0.0f;

		auto xzLength = math::length( math::Vec2f( mCameraState.orientation.vForward.x, mCameraState.orientation.vForward.z ) );
		auto xzSin = mCameraState.orientation.vForward.x / xzLength;
		auto xzCos = mCameraState.orientation.vForward.z / xzLength;
		mCameraState.rotation.yaw = static_cast<float>( atan2( xzSin, xzCos ) );

		auto yzLength = math::length( math::Vec2f( mCameraState.orientation.vForward.y, mCameraState.orientation.vForward.z ) );
		auto yzSin = mCameraState.orientation.vForward.y / yzLength;
		auto yzCos = mCameraState.orientation.vForward.z / yzLength;
		mCameraState.rotation.pitch = static_cast<float>( atan2( yzSin, yzCos ) );

		mCameraState.projection.fovAngleDiff = 0.0f;
		mCameraState.projection.fovAngle = pFOVAngle;
	}

	void CameraController::setTarget( const math::Vec2f & pTarget )
	{
		auto target = math::Vec3f( pTarget.x, pTarget.y, mCameraState.orientation.target.z );
		auto vForward = target - mCameraState.orientation.origin;

		mCameraState.orientation.target = target;
		mCameraState.orientation.originTargetDistance = math::length( vForward );
		mCameraState.orientation.vForward = math::normalize( vForward );

		updateRotation();
	}

	void CameraController::move( float pFactor )
	{
		mCameraState.orientation.origin += pFactor * mCameraState.orientation.vForward;
		mCameraState.orientation.target = mCameraState.orientation.origin + ( mCameraState.orientation.vForward * mCameraState.orientation.originTargetDistance );
	}

	void CameraController::moveFlat( float pFactor )
	{
		mCameraState.orientation.origin += pFactor * math::Vec3f( mCameraState.orientation.vForward.x, 0.0f, mCameraState.orientation.vForward.z );
		mCameraState.orientation.target = mCameraState.orientation.origin + ( mCameraState.orientation.vForward * mCameraState.orientation.originTargetDistance );
	}

	void CameraController::moveSide( float pFactor )
	{
		mCameraState.orientation.origin += pFactor * mCameraState.orientation.vRight;
		mCameraState.orientation.target = mCameraState.orientation.origin + ( mCameraState.orientation.vForward * mCameraState.orientation.originTargetDistance );
	}

	void CameraController::moveSideFlat( float pFactor )
	{
		mCameraState.orientation.origin += pFactor * math::Vec3f( mCameraState.orientation.vRight.x, 0.0f, mCameraState.orientation.vRight.z );
		mCameraState.orientation.target = mCameraState.orientation.origin + ( mCameraState.orientation.vForward * mCameraState.orientation.originTargetDistance );
	}

	void CameraController::moveUpDown( float pFactor )
	{
		mCameraState.orientation.origin += pFactor * math::Vec3f( 0, 1, 0 );
		mCameraState.orientation.target = mCameraState.orientation.origin + ( mCameraState.orientation.vForward * mCameraState.orientation.originTargetDistance );
	}

	void CameraController::roll( float pRoll )
	{
		mCameraState.rotation.roll += ( pRoll * mCameraState.configuration.rollSensitivity );

		updateRotation();
	}

	void CameraController::rotateAroundOrigin( float pYaw, float pPitch )
	{
		mCameraState.rotation.yaw += ( pYaw * mCameraState.configuration.rotationSensitivity );
		mCameraState.rotation.pitch -= ( pPitch * mCameraState.configuration.rotationSensitivity );

		updateRotation();

		mCameraState.orientation.target = mCameraState.orientation.origin + ( mCameraState.orientation.vForward * mCameraState.orientation.originTargetDistance );
	}

	void CameraController::rotateAroundTarget( float pYaw, float pPitch )
	{
		mCameraState.rotation.yaw += ( pYaw * mCameraState.configuration.rotationSensitivity );
		mCameraState.rotation.pitch -= ( pPitch * mCameraState.configuration.rotationSensitivity );

		updateRotation();

		mCameraState.orientation.origin = mCameraState.orientation.target - ( mCameraState.orientation.vForward * mCameraState.orientation.originTargetDistance );
	}

	void CameraController::zoom( int32 pZoomFactor )
	{
		if( ( pZoomFactor > 0 ) && ( mCameraState.projection.zoomLevel == mCameraState.configuration.maxZoomLevel ) )
		{
			return;
		}
		if( ( pZoomFactor < 0 ) && ( mCameraState.projection.zoomLevel == 1 ) )
		{
			return;
		}
		if( pZoomFactor == 0 )
		{
			return;
		}
		if( pZoomFactor > 0 )
		{
			pZoomFactor = 1;
		}
		if( pZoomFactor < 0 )
		{
			pZoomFactor = -1;
		}

		mCameraState.projection.zoomLevel += pZoomFactor;

		const auto zoomLevel = mCameraState.projection.zoomLevel - 1;
		const auto fovModifier = mCameraState.configuration.zoomBaseFactor * zoomLevel;//( 2.0f - 1.0f / static_cast<float>( ( zoomLevel == 1 ) ? 1 : ( 2 << ( zoomLevel - 1 ) ) ) );

		mCameraState.projection.fovAngleDiff = fovModifier;
	}

	math::Mat4f CameraController::computeViewMatrixLH()
	{
		auto viewMatrix = math::lookAtLH( mCameraState.orientation.origin,
		                                  mCameraState.orientation.target,
		                                  mCameraState.orientation.vUp );
		return viewMatrix;
	}

	math::Mat4f CameraController::computeViewMatrixRH()
	{
		auto viewMatrix = math::lookAtRH( mCameraState.orientation.origin,
		                                  mCameraState.orientation.target,
		                                  mCameraState.orientation.vUp );
		return viewMatrix;
	}

	float CameraController::getPerspectiveFOVAngle() const
	{
		auto fovAngle = mCameraState.projection.fovAngle - mCameraState.projection.fovAngleDiff;
		return fovAngle * math::constants::cxFloatRad1Degree;
	}

	void CameraController::updateRotation()
	{
		constexpr auto cxFullAngle = math::constants::cxFloatPi * 2;
		
		if( mCameraState.rotation.yaw > cxFullAngle )
		{
			mCameraState.rotation.yaw -= cxFullAngle;
		}
		else if( mCameraState.rotation.yaw < -cxFullAngle )
		{
			mCameraState.rotation.yaw += cxFullAngle;
		}
		
		if( mCameraState.rotation.pitch > cxFullAngle )
		{
			mCameraState.rotation.pitch -= cxFullAngle;
		}
		else if( mCameraState.rotation.pitch < -cxFullAngle )
		{
			mCameraState.rotation.pitch += cxFullAngle;
		}
		
		if( mCameraState.rotation.roll > cxFullAngle )
		{
			mCameraState.rotation.roll -= cxFullAngle;
		}
		else if( mCameraState.rotation.roll < -cxFullAngle )
		{
			mCameraState.rotation.roll += cxFullAngle;
		}

		auto sinPitch = std::sin( mCameraState.rotation.pitch );
		auto cosPitch = std::cos( mCameraState.rotation.pitch );
		auto sinYaw = std::sin( mCameraState.rotation.yaw );
		auto cosYaw = std::cos( mCameraState.rotation.yaw );
		auto sinRoll = std::sin( mCameraState.rotation.roll );
		auto cosRoll = std::cos( mCameraState.rotation.roll );

		math::Vec3f vcForward;
		vcForward.x = cosPitch * sinYaw;
		vcForward.y = sinPitch;
		vcForward.z = cosPitch * cosYaw;

		math::Vec3f vcUp;
		vcUp.x = -sinRoll * cosYaw - cosRoll * sinPitch * sinYaw;
		vcUp.y =  cosRoll * cosPitch;
		vcUp.z =  sinRoll * sinYaw - cosRoll * sinPitch * cosYaw;

		mCameraState.orientation.vForward = vcForward;
		mCameraState.orientation.vUp = vcUp;
		mCameraState.orientation.vRight = math::normalize( math::cross( mCameraState.orientation.vUp, mCameraState.orientation.vForward ) );
	}

} // namespace ts3
