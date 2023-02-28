
#ifndef __TS3_MATH_VECTOR_BASE_H__
#define __TS3_MATH_VECTOR_BASE_H__

#include "prerequisites.h"

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#  pragma clang diagnostic ignored "-Wnested-anon-types"
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wpedantic"
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  pragma warning( push )
#  pragma warning( disable: 4201 )  // 'Nonstandard extension used: nameless struct/union'
#endif

namespace ts3::math
{

	template <typename TVal>
	struct Vector4SIMDData
	{
		using Type = TVal[4];
	};

#if( TS3_MATH_SIMD_USE_VX128F )
	template <>
	struct Vector4SIMDData<float>
	{
		using Type = __m128;
	};
#endif

#if( TS3_MATH_SIMD_USE_VX128I )
	template <>
	struct Vector4SIMDData<int32>
	{
		using Type = __m128i;
	};

	template <>
	struct Vector4SIMDData<uint32>
	{
		using Type = __m128i;
	};
#endif

#if( TS3_MATH_SIMD_USE_VX256D )
	template <>
	struct Vector4SIMDData<double>
	{
		using Type = __m256d;
	};
#endif

#if( TS3_MATH_SIMD_USE_VX256I )
	template <>
	struct Vector4SIMDData<int64>
	{
		using Type = __m256i;
	};

	template <>
	struct Vector4SIMDData<uint64>
	{
		using Type = __m256i;
	};
#endif

	template <typename TVal, size_t tSize>
	struct VectorTraits
	{
		static const uint32 sLength = tSize;
		static const uint32 sByteSize = sizeof( TVal ) * sLength;
	};

	template <typename TVal, size_t tSize>
	struct Vector;

	template <typename TVal>
	struct Vector<TVal, 2>
	{
	public:
		using Traits = VectorTraits<TVal, 2>;

		static const uint32 sLength = Traits::sLength;
		static const uint32 sByteSize = Traits::sByteSize;

		union
		{
			struct
			{
				// Explicit values for member access.
				TVal x, y;
			};

			struct
			{
				TVal values[2];
			};
		};

	public:
		constexpr Vector( const Vector & ) = default;
		constexpr Vector & operator=( const Vector & ) = default;

		constexpr Vector() = default;

		template <typename TOther>
		constexpr explicit Vector( const Vector<TOther, 2> & pVec ) noexcept
		: x( static_cast<TVal>( pVec.x ) )
		, y( static_cast<TVal>( pVec.y ) )
		{}

		template <typename TScalar, enable_if_scalar_t<TScalar> = true>
		constexpr explicit Vector( TScalar pScalar ) noexcept
		: x( static_cast<TVal>( pScalar ) )
		, y( static_cast<TVal>( pScalar ) )
		{}

		template <typename TScalar, enable_if_scalar_t<TScalar> = true>
		constexpr explicit Vector( const TScalar * pData ) noexcept
		: x( static_cast<TVal>( pData[0] ) )
		, y( static_cast<TVal>( pData[1] ) )
		{}

		template <typename TX, typename TY>
		constexpr Vector( TX pX, TY pY ) noexcept
		: x( static_cast<TVal>( pX ) )
		, y( static_cast<TVal>( pY ) )
		{}

		TVal * data() noexcept
		{
			return &( values[0] );
		}

		const TVal * data() const noexcept
		{
			return &( values[0] );
		}

		TVal & operator[]( size_t pIndex ) noexcept
		{
			ts3DebugAssert( pIndex < sLength )
			return values[pIndex];
		}

		const TVal & operator[]( size_t pIndex ) const noexcept
		{
			ts3DebugAssert( pIndex < sLength )
			return values[pIndex];
		}
	};

	template <typename TVal>
	struct Vector<TVal, 3>
	{
	public:
		using Traits = VectorTraits<TVal, 3>;

		static const uint32 sLength = Traits::sLength;
		static const uint32 sByteSize = Traits::sByteSize;

		union
		{
			struct
			{
				// Explicit values for member access.
				TVal x, y, z;
			};

			struct
			{
				TVal values[3];
			};
		};

	public:
		constexpr Vector( const Vector & ) = default;
		constexpr Vector & operator=( const Vector & ) = default;

		constexpr Vector() = default;

		template <typename TOther>
		constexpr explicit Vector( const Vector<TOther, 3> & pVec ) noexcept
		: x( static_cast<TVal>( pVec.x ) )
		, y( static_cast<TVal>( pVec.y ) )
		, z( static_cast<TVal>( pVec.z ) )
		{}

		template <typename TScalar, enable_if_scalar_t<TScalar> = true>
		constexpr explicit Vector( TScalar pScalar ) noexcept
		: x( static_cast<TVal>( pScalar ) )
		, y( static_cast<TVal>( pScalar ) )
		, z( static_cast<TVal>( pScalar ) )
		{}

		template <typename TScalar, enable_if_scalar_t<TScalar> = true>
		constexpr explicit Vector( const TScalar * pData ) noexcept
		: x( static_cast<TVal>( pData[0] ) )
		, y( static_cast<TVal>( pData[1] ) )
		, z( static_cast<TVal>( pData[2] ) )
		{}

		template <typename TXY, typename TZ>
		constexpr Vector( const Vector<TXY, 2> & pVecXY, TZ pZ ) noexcept
		: x( static_cast<TVal>( pVecXY.x ) )
		, y( static_cast<TVal>( pVecXY.y ) )
		, z( static_cast<TVal>( pZ ) )
		{}

		template <typename TX, typename TYZ>
		constexpr Vector( TX pX, const Vector<TYZ, 2> & pVecYZ ) noexcept
		: x( static_cast<TVal>( pX ) )
		, y( static_cast<TVal>( pVecYZ.y ) )
		, z( static_cast<TVal>( pVecYZ.z ) )
		{}

		template <typename TX, typename TY, typename TZ>
		constexpr Vector( TX pX, TY pY, TZ pZ ) noexcept
		: x( static_cast<TVal>( pX ) )
		, y( static_cast<TVal>( pY ) )
		, z( static_cast<TVal>( pZ ) )
		{}

		TVal * data() noexcept
		{
			return &( values[0] );
		}

		const TVal * data() const noexcept
		{
			return &( values[0] );
		}

		TVal & operator[]( size_t pIndex ) noexcept
		{
			ts3DebugAssert( pIndex < sLength )
			return values[pIndex];
		}

		const TVal & operator[]( size_t pIndex ) const noexcept
		{
			ts3DebugAssert( pIndex < sLength )
			return values[pIndex];
		}
	};

	template <typename TVal>
	struct Vector<TVal, 4>
	{
	public:
		using Traits = VectorTraits<TVal, 4>;
		using SIMDDataType = typename Vector4SIMDData<TVal>::Type;

		static const uint32 sLength = Traits::sLength;
		static const uint32 sByteSize = Traits::sByteSize;

		union
		{
			struct
			{
				// Explicit values for member access.
				TVal x, y, z, w;
			};

			struct
			{
				// Either generic array of values or SSE/AVX SIMD type (depends on support and build config).
				SIMDDataType mmv;
			};

			struct
			{
				TVal values[4];
			};
		};

	public:
		constexpr Vector( const Vector & ) = default;
		constexpr Vector & operator=( const Vector & ) = default;

		constexpr Vector() = default;

		constexpr explicit Vector( SIMDDataType pMMV ) noexcept
		: mmv( pMMV )
		{}

		template <typename TOther>
		constexpr explicit Vector( const Vector<TOther, 4> & pVec ) noexcept
		: x( static_cast<TVal>( pVec.x ) )
		, y( static_cast<TVal>( pVec.y ) )
		, z( static_cast<TVal>( pVec.z ) )
		, w( static_cast<TVal>( pVec.w ) )
		{}

		template <typename TScalar, enable_if_scalar_t<TScalar> = true>
		constexpr explicit Vector( TScalar pScalar ) noexcept
		: x( static_cast<TVal>( pScalar ) )
		, y( static_cast<TVal>( pScalar ) )
		, z( static_cast<TVal>( pScalar ) )
		, w( static_cast<TVal>( pScalar ) )
		{}

		template <typename TScalar, enable_if_scalar_t<TScalar> = true>
		constexpr explicit Vector( const TScalar * pData ) noexcept
		: x( static_cast<TVal>( pData[0] ) )
		, y( static_cast<TVal>( pData[1] ) )
		, z( static_cast<TVal>( pData[2] ) )
		, w( static_cast<TVal>( pData[3] ) )
		{}

		template <typename TXY, typename TZ, typename TW>
		constexpr Vector( const Vector<TXY, 2> & pVecXY, TZ pZ, TW pW ) noexcept
		: x( static_cast<TVal>( pVecXY.x ) )
		, y( static_cast<TVal>( pVecXY.y ) )
		, z( static_cast<TVal>( pZ ) )
		, w( static_cast<TVal>( pW ) )
		{}

		template <typename TX, typename TYZ, typename TW>
		constexpr Vector( TX pX, const Vector<TYZ, 2> & pVecYZ, TW pW ) noexcept
		: x( static_cast<TVal>( pX ) )
		, y( static_cast<TVal>( pVecYZ.y ) )
		, z( static_cast<TVal>( pVecYZ.z ) )
		, w( static_cast<TVal>( pW ) )
		{}

		template <typename TX, typename TY, typename TZW>
		constexpr Vector( TX pX, TVal pY, const Vector<TZW, 2> & pVecZW ) noexcept
		: x( static_cast<TVal>( pX ) )
		, y( static_cast<TVal>( pY ) )
		, z( static_cast<TVal>( pVecZW.z ) )
		, w( static_cast<TVal>( pVecZW.w ) )
		{}

		template <typename TXYZ, typename TW>
		constexpr Vector( const Vector<TXYZ, 3> & pVecXYZ, TW pW ) noexcept
		: x( static_cast<TVal>( pVecXYZ.x ) )
		, y( static_cast<TVal>( pVecXYZ.y ) )
		, z( static_cast<TVal>( pVecXYZ.z ) )
		, w( static_cast<TVal>( pW ) )
		{}

		template <typename TX, typename TYZW>
		constexpr Vector( TX pX, const Vector<TYZW, 3> & pVecYZW ) noexcept
		: x( static_cast<TVal>( pX ) )
		, y( static_cast<TVal>( pVecYZW.y ) )
		, z( static_cast<TVal>( pVecYZW.z ) )
		, w( static_cast<TVal>( pVecYZW.w ) )
		{}

		template <typename TX, typename TY, typename TZ, typename TW>
		constexpr Vector( TX pX, TY pY, TZ pZ, TW pW ) noexcept
		: x( static_cast<TVal>( pX ) )
		, y( static_cast<TVal>( pY ) )
		, z( static_cast<TVal>( pZ ) )
		, w( static_cast<TVal>( pW ) )
		{}

		TVal & operator[]( size_t pIndex ) noexcept
		{
			ts3DebugAssert( pIndex < sLength )
			return values[pIndex];
		}

		const TVal & operator[]( size_t pIndex ) const noexcept
		{
			ts3DebugAssert( pIndex < sLength )
			return values[pIndex];
		}

		TVal * data() noexcept
		{
			return &( values[0] );
		}

		const TVal * data() const noexcept
		{
			return &( values[0] );
		}

		SIMDDataType * simdPtr() noexcept
		{
			return &mmv;
		}

		const SIMDDataType * simdPtr() const noexcept
		{
			return &mmv;
		}
	};

	template <typename TVal>
	using Vec2 = Vector<TVal, 2>;

	template <typename TVal>
	using Vec3 = Vector<TVal, 3>;

	template <typename TVal>
	using Vec4 = Vector<TVal, 4>;

	template <typename TVal>
	using Vector2 = Vector<TVal, 2>;

	template <typename TVal>
	using Vector3 = Vector<TVal, 3>;

	template <typename TVal>
	using Vector4 = Vector<TVal, 4>;

	using Vec2i16 = Vector2<int16>;
	using Vec2u16 = Vector2<uint16>;
	using Vec2i32 = Vector2<int32>;
	using Vec2u32 = Vector2<uint32>;
	using Vec2i64 = Vector2<int64>;
	using Vec2u64 = Vector2<uint64>;
	using Vec2f   = Vector2<float>;
	using Vec2d   = Vector2<double>;

	using Vec3i32 = Vector3<int32>;
	using Vec3u32 = Vector3<uint32>;
	using Vec3i64 = Vector3<int64>;
	using Vec3u64 = Vector3<uint64>;
	using Vec3f   = Vector3<float>;
	using Vec3d   = Vector3<double>;

	using Vec4i16 = Vector4<int16>;
	using Vec4u16 = Vector4<uint16>;
	using Vec4i32 = Vector4<int32>;
	using Vec4u32 = Vector4<uint32>;
	using Vec4i64 = Vector4<int64>;
	using Vec4u64 = Vector4<uint64>;
	using Vec4f   = Vector4<float>;
	using Vec4d   = Vector4<double>;

	using Pos2f  = Vector2<float>;
	using Pos2i  = Vector2<int32>;
	using Pos3f  = Vector3<float>;
	using Pos3i  = Vector3<int32>;
	using Size2f = Vector2<float>;
	using Size2u = Vector2<uint32>;
	using Size3f = Vector3<float>;
	using Size3u = Vector3<uint32>;

}

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  pragma GCC diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  pragma warning( pop )
#endif

#endif // __TS3_MATH_VECTOR_BASE_H__
