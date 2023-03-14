
#ifndef __TS3_MATH_UTILITY_H__
#define __TS3_MATH_UTILITY_H__

#include "prerequisites.h"

namespace ts3::math
{

	inline float sqrt( float pValue )
	{
		return std::sqrt( pValue );
	}

	inline double sqrt( double pValue )
	{
		return std::sqrt( pValue );
	}

	inline long double sqrt( long double pValue )
	{
		return std::sqrt( pValue );
	}

	template <typename TVal>
	inline double sqrt( TVal pValue )
	{
		return std::sqrt( static_cast<double>( pValue ) );
	}

	inline float invSqrt( float pValue )
	{
		return 1.0f / sqrt( pValue );
	}

	inline double invSqrt( double pValue )
	{
		return 1.0 / sqrt( pValue );
	}

	inline long double invSqrt( long double pValue )
	{
		return 1.0L / sqrt( pValue );
	}

	template <typename TVal>
	inline double invSqrt( TVal pValue )
	{
		return 1.0 / sqrt( pValue );
	}

	inline std::pair<float, float> sinCos( float pValue )
	{
		return { std::sin( pValue ), std::cos( pValue ) };
	}

}

#endif // __TS3_MATH_VECTOR_OPS_H__
