
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

	template <typename Tp>
	struct Vector4SIMDData
	{
		using Type = Tp[4];
	};

#if ( TS3_MATH_SIMD_USE_VX128F )
	template <>
	struct Vector4SIMDData<float>
	{
		using Type = __m128;
	};
#endif

#if ( TS3_MATH_SIMD_USE_VX128I )
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

#if ( TS3_MATH_SIMD_USE_VX256D )
	template <>
	struct Vector4SIMDData<double>
	{
		using Type = __m256d;
	};
#endif

#if ( TS3_MATH_SIMD_USE_VX256I )
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

	template <typename Tp, size_t tSize>
	struct VectorTraits
	{
		static const uint32 sLength = tSize;
		static const uint32 sByteSize = sizeof( Tp ) * sLength;
	};

	template <typename Tp, size_t tSize>
	struct Vector;

	template <typename Tp>
	struct Vector<Tp, 2>
	{
	public:
		using Traits = VectorTraits<Tp, 2>;

		static const uint32 sLength = Traits::sLength;
		static const uint32 sByteSize = Traits::sByteSize;

		union
		{
			struct
			{
				// Explicit values for member access.
				Tp x, y;
			};

			struct
			{
				Tp values[2];
			};
		};

	public:
		constexpr Vector( const Vector & ) = default;
		constexpr Vector & operator=( const Vector & ) = default;

		constexpr Vector() = default;

		template <typename TpOther>
		constexpr explicit Vector( const Vector<TpOther, 2> & pVec ) noexcept
		: x( static_cast<Tp>( pVec.x ) )
		, y( static_cast<Tp>( pVec.y ) )
		{}

		template <typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		constexpr explicit Vector( TpScalar pScalar ) noexcept
		: x( static_cast<Tp>( pScalar ) )
		, y( static_cast<Tp>( pScalar ) )
		{}

		template <typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		constexpr explicit Vector( const TpScalar * pData ) noexcept
		: x( static_cast<Tp>( pData[0] ) )
		, y( static_cast<Tp>( pData[1] ) )
		{}

		template <typename TpX, typename TpY>
		constexpr Vector( TpX pX, TpY pY ) noexcept
		: x( static_cast<Tp>( pX ) )
		, y( static_cast<Tp>( pY ) )
		{}

		Tp * data() noexcept
		{
			return &( values[0] );
		}

		const Tp * data() const noexcept
		{
			return &( values[0] );
		}

		Tp & operator[]( size_t pIndex ) noexcept
		{
			ts3DebugAssert( pIndex < sLength )
			return values[pIndex];
		}

		const Tp & operator[]( size_t pIndex ) const noexcept
		{
			ts3DebugAssert( pIndex < sLength )
			return values[pIndex];
		}
	};

	template <typename Tp>
	struct Vector<Tp, 3>
	{
	public:
		using Traits = VectorTraits<Tp, 3>;

		static const uint32 sLength = Traits::sLength;
		static const uint32 sByteSize = Traits::sByteSize;

		union
		{
			struct
			{
				// Explicit values for member access.
				Tp x, y, z;
			};

			struct
			{
				Tp values[3];
			};
		};

	public:
		constexpr Vector( const Vector & ) = default;
		constexpr Vector & operator=( const Vector & ) = default;

		constexpr Vector() = default;

		template <typename TpOther>
		constexpr explicit Vector( const Vector<TpOther, 3> & pVec ) noexcept
		: x( static_cast<Tp>( pVec.x ) )
		, y( static_cast<Tp>( pVec.y ) )
		, z( static_cast<Tp>( pVec.z ) )
		{}

		template <typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		constexpr explicit Vector( TpScalar pScalar ) noexcept
		: x( static_cast<Tp>( pScalar ) )
		, y( static_cast<Tp>( pScalar ) )
		, z( static_cast<Tp>( pScalar ) )
		{}

		template <typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		constexpr explicit Vector( const TpScalar * pData ) noexcept
		: x( static_cast<Tp>( pData[0] ) )
		, y( static_cast<Tp>( pData[1] ) )
		, z( static_cast<Tp>( pData[2] ) )
		{}

		template <typename TpXY, typename TpZ>
		constexpr Vector( const Vector<TpXY, 2> & pVecXY, TpZ pZ ) noexcept
		: x( static_cast<Tp>( pVecXY.x ) )
		, y( static_cast<Tp>( pVecXY.y ) )
		, z( static_cast<Tp>( pZ ) )
		{}

		template <typename TpX, typename TpYZ>
		constexpr Vector( TpX pX, const Vector<TpYZ, 2> & pVecYZ ) noexcept
		: x( static_cast<Tp>( pX ) )
		, y( static_cast<Tp>( pVecYZ.y ) )
		, z( static_cast<Tp>( pVecYZ.z ) )
		{}

		template <typename TpX, typename TpY, typename TpZ>
		constexpr Vector( TpX pX, TpY pY, TpZ pZ ) noexcept
		: x( static_cast<Tp>( pX ) )
		, y( static_cast<Tp>( pY ) )
		, z( static_cast<Tp>( pZ ) )
		{}

		Tp * data() noexcept
		{
			return &( values[0] );
		}

		const Tp * data() const noexcept
		{
			return &( values[0] );
		}

		Tp & operator[]( size_t pIndex ) noexcept
		{
			ts3DebugAssert( pIndex < sLength )
			return values[pIndex];
		}

		const Tp & operator[]( size_t pIndex ) const noexcept
		{
			ts3DebugAssert( pIndex < sLength )
			return values[pIndex];
		}
	};

	template <typename Tp>
	struct Vector<Tp, 4>
	{
	public:
		using Traits = VectorTraits<Tp, 4>;
		using SIMDDataType = typename Vector4SIMDData<Tp>::Type;

		static const uint32 sLength = Traits::sLength;
		static const uint32 sByteSize = Traits::sByteSize;

		union
		{
			struct
			{
				// Explicit values for member access.
				Tp x, y, z, w;
			};

			struct
			{
				// Either generic array of values or SSE/AVX SIMD type (depends on support and build config).
				SIMDDataType mmv;
			};

			struct
			{
				Tp values[4];
			};
		};

	public:
		constexpr Vector( const Vector & ) = default;
		constexpr Vector & operator=( const Vector & ) = default;

		constexpr Vector() = default;

		constexpr explicit Vector( SIMDDataType pMMV ) noexcept
		: mmv( pMMV )
		{}

		template <typename TpOther>
		constexpr explicit Vector( const Vector<TpOther, 4> & pVec ) noexcept
		: x( static_cast<Tp>( pVec.x ) )
		, y( static_cast<Tp>( pVec.y ) )
		, z( static_cast<Tp>( pVec.z ) )
		, w( static_cast<Tp>( pVec.w ) )
		{}

		template <typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		constexpr explicit Vector( TpScalar pScalar ) noexcept
		: x( static_cast<Tp>( pScalar ) )
		, y( static_cast<Tp>( pScalar ) )
		, z( static_cast<Tp>( pScalar ) )
		, w( static_cast<Tp>( pScalar ) )
		{}

		template <typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		constexpr explicit Vector( const TpScalar * pData ) noexcept
		: x( static_cast<Tp>( pData[0] ) )
		, y( static_cast<Tp>( pData[1] ) )
		, z( static_cast<Tp>( pData[2] ) )
		, w( static_cast<Tp>( pData[3] ) )
		{}

		template <typename TpXY, typename TpZ, typename TpW>
		constexpr Vector( const Vector<TpXY, 2> & pVecXY, TpZ pZ, TpW pW ) noexcept
		: x( static_cast<Tp>( pVecXY.x ) )
		, y( static_cast<Tp>( pVecXY.y ) )
		, z( static_cast<Tp>( pZ ) )
		, w( static_cast<Tp>( pW ) )
		{}

		template <typename TpX, typename TpYZ, typename TpW>
		constexpr Vector( TpX pX, const Vector<TpYZ, 2> & pVecYZ, TpW pW ) noexcept
		: x( static_cast<Tp>( pX ) )
		, y( static_cast<Tp>( pVecYZ.y ) )
		, z( static_cast<Tp>( pVecYZ.z ) )
		, w( static_cast<Tp>( pW ) )
		{}

		template <typename TpX, typename TpY, typename TpZW>
		constexpr Vector( TpX pX, Tp pY, const Vector<TpZW, 2> & pVecZW ) noexcept
		: x( static_cast<Tp>( pX ) )
		, y( static_cast<Tp>( pY ) )
		, z( static_cast<Tp>( pVecZW.z ) )
		, w( static_cast<Tp>( pVecZW.w ) )
		{}

		template <typename TpXYZ, typename TpW>
		constexpr Vector( const Vector<TpXYZ, 3> & pVecXYZ, TpW pW ) noexcept
		: x( static_cast<Tp>( pVecXYZ.x ) )
		, y( static_cast<Tp>( pVecXYZ.y ) )
		, z( static_cast<Tp>( pVecXYZ.z ) )
		, w( static_cast<Tp>( pW ) )
		{}

		template <typename TpX, typename TpYZW>
		constexpr Vector( TpX pX, const Vector<TpYZW, 3> & pVecYZW ) noexcept
		: x( static_cast<Tp>( pX ) )
		, y( static_cast<Tp>( pVecYZW.y ) )
		, z( static_cast<Tp>( pVecYZW.z ) )
		, w( static_cast<Tp>( pVecYZW.w ) )
		{}

		template <typename TpX, typename TpY, typename TpZ, typename TpW>
		constexpr Vector( TpX pX, TpY pY, TpZ pZ, TpW pW ) noexcept
		: x( static_cast<Tp>( pX ) )
		, y( static_cast<Tp>( pY ) )
		, z( static_cast<Tp>( pZ ) )
		, w( static_cast<Tp>( pW ) )
		{}

		Tp & operator[]( size_t pIndex ) noexcept
		{
			ts3DebugAssert( pIndex < sLength )
			return values[pIndex];
		}

		const Tp & operator[]( size_t pIndex ) const noexcept
		{
			ts3DebugAssert( pIndex < sLength )
			return values[pIndex];
		}

		Tp * data() noexcept
		{
			return &( values[0] );
		}

		const Tp * data() const noexcept
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

	template <typename Tp>
	using Vec2 = Vector<Tp, 2>;

	template <typename Tp>
	using Vec3 = Vector<Tp, 3>;

	template <typename Tp>
	using Vec4 = Vector<Tp, 4>;

	template <typename Tp>
	using Vector2 = Vector<Tp, 2>;

	template <typename Tp>
	using Vector3 = Vector<Tp, 3>;

	template <typename Tp>
	using Vector4 = Vector<Tp, 4>;

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
