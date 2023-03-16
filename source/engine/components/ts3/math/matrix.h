
#ifndef __TS3_MATH_MATRIX_BASE_H__
#define __TS3_MATH_MATRIX_BASE_H__

#include "vector.h"

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

	// Traits container struct for common properties of matrix types.
	template <typename TVal, size_t tRows, size_t tColumns>
	struct MatrixTraits
	{
		static const uint32 sRowLength = tColumns;
		static const uint32 sRowsNum = tRows;
		static const uint32 sColumnsNumNum = tColumns;
		static const uint32 sLength = sRowsNum * sColumnsNumNum;
		static const uint32 sByteSize = sizeof( TVal ) * sLength;
		static const uint32 sRowBytePitch = sizeof( TVal ) * sRowLength;
	};

	template <typename TVal, size_t /* tRows */, size_t tColumns>
	struct MatrixRowSIMDData
	{
		using Type = TVal[tColumns];
	};

	template <typename TVal, size_t tRows>
	struct MatrixRowSIMDData<TVal, tRows, 4>
	{
		using Type = typename Vector4SIMDData<TVal>::Type;
	};

	// Common class for all matrix types. The default implementation is empty, because we need
	// more fine-grained control over members definition.
	// Internally, matrix values are represented as arrays of rows (similar to DXM library),
	// so there is a specialized definition for MatrixBase<T, 2, R>, MatrixBase<T, 3, R> and
	// MatrixBase<T, 4, R> for 2xR, 3xR and 4xR matrices of type T, respectively.
	template <typename TVal, size_t tRows, size_t tColumns>
	struct Matrix;


	template <typename TVal, size_t tColumns>
	struct Matrix<TVal, 2, tColumns>
	{
	public:
		static constexpr size_t sRowsNum = 2;
		static constexpr size_t sColumnsNum = tColumns;
		static constexpr size_t sMatrixSize = sRowsNum * sColumnsNum;

		using Traits = MatrixTraits<TVal, 2, tColumns>;
		using ColumnVector = Vector2<TVal>;
		using RowVector = Vector<TVal, tColumns>;
		using RowSIMDDataType = typename MatrixRowSIMDData<TVal, sRowsNum, tColumns>::Type;

		union
		{
			struct
			{
				RowSIMDDataType mm0, mm1;
			};

			struct
			{
				RowVector row0, row1;
			};

			struct
			{
				TVal values[sMatrixSize];
			};
		};

	public:
		constexpr Matrix( const Matrix & ) = default;
		constexpr Matrix & operator=( const Matrix & ) = default;

		constexpr Matrix() noexcept = default;

		template <typename T0, typename T1>
		constexpr Matrix( const Vector<T0, tColumns> & pRow0,
						  const Vector<T1, tColumns> & pRow1 ) noexcept
		: row0( pRow0 )
		, row1( pRow1 )
		{}

		template <typename TScalar, enable_if_scalar_t<TScalar> = true>
		constexpr explicit Matrix( TScalar pScalar ) noexcept
		: row0( pScalar )
		, row1( pScalar )
		{}

		Matrix( std::initializer_list<TVal> pData ) noexcept
		: row0( pData.begin() )
		, row1( pData.begin() + tColumns )
		{
			ts3DebugAssert( pData.size() == sMatrixSize );
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD Vector<TVal, tColumns> & operator[]( size_t pIndex ) noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<RowVector *>( &row0 )[pIndex];
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD const Vector<TVal, tColumns> & operator[]( size_t pIndex ) const noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<const RowVector *>( &row0 )[pIndex];
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD ColumnVector column( size_t pColumnIndex ) const noexcept
		{
			return { row0[pColumnIndex], row1[pColumnIndex] };
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD TVal * data() noexcept
		{
			return &( values[0] );
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD const TVal * data() const noexcept
		{
			return &( values[0] );
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD RowVector * rowPtr( size_t pIndex = 0 ) noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<RowVector *>( &row0 ) + pIndex;
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD const RowVector * rowPtr( size_t pIndex = 0 ) const noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<const RowVector *>( &row0 ) + pIndex;
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD RowSIMDDataType * simdPtr( size_t pIndex = 0 ) noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<RowSIMDDataType *>( &mm0 ) + pIndex;
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD const RowSIMDDataType * simdPtr( size_t pIndex = 0 ) const noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<const RowSIMDDataType *>( &mm0 ) + pIndex;
		}
	};


	template <typename TVal, size_t tColumns>
	struct Matrix<TVal, 3, tColumns>
	{
	public:
		static constexpr size_t sRowsNum = 3;
		static constexpr size_t sColumnsNum = tColumns;
		static constexpr size_t sMatrixSize = sRowsNum * sColumnsNum;
		using RowSIMDDataType = typename MatrixRowSIMDData<TVal, sRowsNum, tColumns>::Type;

		using Traits = MatrixTraits<TVal, 3, tColumns>;
		using ColumnVector = Vector<TVal, 3>;
		using RowVector = Vector<TVal, tColumns>;

		union
		{
			struct
			{
				RowSIMDDataType mm0, mm1, mm2;
			};

			struct
			{
				RowVector row0, row1, row2;
			};

			struct
			{
				TVal values[sMatrixSize];
			};
		};

	public:
		constexpr Matrix( const Matrix & ) = default;
		constexpr Matrix & operator=( const Matrix & ) = default;

		constexpr Matrix() noexcept = default;

		template <typename T0, typename T1, typename T2>
		constexpr Matrix( const Vector<T0, tColumns> & pRow0,
						  const Vector<T1, tColumns> & pRow1,
						  const Vector<T2, tColumns> & pRow2 ) noexcept
		: row0( pRow0 )
		, row1( pRow1 )
		, row2( pRow2 )
		{}

		template <typename TScalar, enable_if_scalar_t<TScalar> = true>
		constexpr explicit Matrix( TScalar pScalar ) noexcept
		: row0( pScalar )
		, row1( pScalar )
		, row2( pScalar )
		{}

		Matrix( std::initializer_list<TVal> pData ) noexcept
		: row0( pData.begin() )
		, row1( pData.begin() + tColumns )
		, row2( pData.begin() + 2*tColumns )
		{
			ts3DebugAssert( pData.size() == sMatrixSize );
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD Vector<TVal, tColumns> & operator[]( size_t pIndex ) noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<RowVector *>( &row0 )[pIndex];
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD const Vector<TVal, tColumns> & operator[]( size_t pIndex ) const noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<const RowVector *>( &row0 )[pIndex];
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD ColumnVector column( size_t pColumnIndex ) const noexcept
		{
			return { row0[pColumnIndex], row1[pColumnIndex], row2[pColumnIndex] };
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD TVal * data() noexcept
		{
			return &( values[0] );
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD const TVal * data() const noexcept
		{
			return &( values[0] );
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD RowVector * rowPtr( size_t pIndex = 0 ) noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<RowVector *>( &row0 ) + pIndex;
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD const RowVector * rowPtr( size_t pIndex = 0 ) const noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<const RowVector *>( &row0 ) + pIndex;
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD RowSIMDDataType * simdPtr( size_t pIndex = 0 ) noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<RowSIMDDataType *>( &mm0 ) + pIndex;
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD const RowSIMDDataType * simdPtr( size_t pIndex = 0 ) const noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<const RowSIMDDataType *>( &mm0 ) + pIndex;
		}
	};


	template <typename TVal, size_t tColumns>
	struct Matrix<TVal, 4, tColumns>
	{
	public:
		static constexpr size_t sRowsNum = 4;
		static constexpr size_t sColumnsNum = tColumns;
		static constexpr size_t sMatrixSize = sRowsNum * tColumns;
		static constexpr size_t sAlignment = alignof( TVal );
		using RowSIMDDataType = typename MatrixRowSIMDData<TVal, sRowsNum, tColumns>::Type;

		using Traits = MatrixTraits<TVal, sRowsNum, tColumns>;
		using ColumnVector = Vector<TVal, sRowsNum>;
		using RowVector = Vector<TVal, tColumns>;

		union
		{
			struct
			{
				RowSIMDDataType mm0, mm1, mm2, mm3;
			};

			struct
			{
				RowVector row0, row1, row2, row3;
			};

			struct
			{
				TVal values[sMatrixSize];
			};
		};

	public:
		constexpr Matrix( const Matrix & ) = default;
		constexpr Matrix & operator=( const Matrix & ) = default;

		constexpr Matrix() noexcept = default;

		template <typename T0, typename T1, typename T2, typename T3>
		constexpr Matrix( const Vector<T0, tColumns> & pRow0,
						  const Vector<T1, tColumns> & pRow1,
						  const Vector<T2, tColumns> & pRow2,
						  const Vector<T3, tColumns> & pRow3 ) noexcept
		: row0( pRow0 )
		, row1( pRow1 )
		, row2( pRow2 )
		, row3( pRow3 )
		{}

		template <typename TScalar, enable_if_scalar_t<TScalar> = true>
		constexpr explicit Matrix( TScalar pScalar ) noexcept
		: row0( pScalar )
		, row1( pScalar )
		, row2( pScalar )
		, row3( pScalar )
		{}

		Matrix( std::initializer_list<TVal> pData ) noexcept
		: row0( pData.begin() )
		, row1( pData.begin() + tColumns )
		, row2( pData.begin() + 2*tColumns )
		, row3( pData.begin() + 3*tColumns )
		{
			ts3DebugAssert( pData.size() == sMatrixSize );
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD Vector<TVal, tColumns> & operator[]( size_t pIndex ) noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<RowVector *>( &row0 )[pIndex];
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD const Vector<TVal, tColumns> & operator[]( size_t pIndex ) const noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<const RowVector *>( &row0 )[pIndex];
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD ColumnVector column( size_t pColumnIndex ) const noexcept
		{
			return { row0[pColumnIndex], row1[pColumnIndex], row2[pColumnIndex], row3[pColumnIndex] };
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD TVal * data() noexcept
		{
			return &( values[0] );
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD const TVal * data() const noexcept
		{
			return &( values[0] );
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD RowVector * rowPtr( size_t pIndex = 0 ) noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<RowVector *>( &row0 ) + pIndex;
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD const RowVector * rowPtr( size_t pIndex = 0 ) const noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<const RowVector *>( &row0 ) + pIndex;
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD RowSIMDDataType * simdPtr( size_t pIndex = 0 ) noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<RowSIMDDataType *>( &mm0 ) + pIndex;
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD const RowSIMDDataType * simdPtr( size_t pIndex = 0 ) const noexcept
		{
			ts3DebugAssert( pIndex < sRowsNum );
			return static_cast<const RowSIMDDataType *>( &mm0 ) + pIndex;
		}
	};

	template <typename TVal, size_t tColumns>
	using Matrix2xC = Matrix<TVal, 2, tColumns>;

	template <typename TVal, size_t tColumns>
	using Matrix3xC = Matrix<TVal, 3, tColumns>;

	template <typename TVal, size_t tColumns>
	using Matrix4xC = Matrix<TVal, 4, tColumns>;

	template <typename TVal, size_t tRows>
	using MatrixRx2 = Matrix<TVal, tRows, 2>;

	template <typename TVal, size_t tRows>
	using MatrixRx3 = Matrix<TVal, tRows, 3>;

	template <typename TVal, size_t tRows>
	using MatrixRx4 = Matrix<TVal, tRows, 4>;

	template <typename TVal>
	using Mat2x2 = Matrix<TVal, 2, 2>;

	template <typename TVal>
	using Mat3x3 = Matrix<TVal, 3, 3>;

	template <typename TVal>
	using Mat4x4 = Matrix<TVal, 4, 4>;

	template <typename TVal>
	using Matrix2x2 = Matrix<TVal, 2, 2>;

	template <typename TVal>
	using Matrix2x3 = Matrix<TVal, 2, 3>;

	template <typename TVal>
	using Matrix2x4 = Matrix<TVal, 2, 4>;

	template <typename TVal>
	using Matrix3x2 = Matrix<TVal, 3, 2>;

	template <typename TVal>
	using Matrix3x3 = Matrix<TVal, 3, 3>;

	template <typename TVal>
	using Matrix3x4 = Matrix<TVal, 3, 4>;

	template <typename TVal>
	using Matrix4x2 = Matrix<TVal, 4, 2>;

	template <typename TVal>
	using Matrix4x3 = Matrix<TVal, 4, 3>;

	template <typename TVal>
	using Matrix4x4 = Matrix<TVal, 4, 4>;

	using Mat2x2i   = Matrix2x2<int32>;
	using Mat2x2u   = Matrix2x2<uint32>;
	using Mat2x2i64 = Matrix2x2<int64>;
	using Mat2x2u64 = Matrix2x2<uint64>;
	using Mat2x2f   = Matrix2x2<float>;
	using Mat2x2d   = Matrix2x2<double>;

	using Mat3x3i   = Matrix3x3<int32>;
	using Mat3x3u   = Matrix3x3<uint32>;
	using Mat3x3i64 = Matrix3x3<int64>;
	using Mat3x3u64 = Matrix3x3<uint64>;
	using Mat3x3f   = Matrix3x3<float>;
	using Mat3x3d   = Matrix3x3<double>;

	using Mat4x4i   = Matrix4x4<int32>;
	using Mat4x4u   = Matrix4x4<uint32>;
	using Mat4x4i64 = Matrix4x4<int64>;
	using Mat4x4u64 = Matrix4x4<uint64>;
	using Mat4x4f   = Matrix4x4<float>;
	using Mat4x4d   = Matrix4x4<double>;

	using Mat2i   = Mat2x2i;
	using Mat2u   = Mat2x2u;
	using Mat2i64 = Mat2x2i64;
	using Mat2u64 = Mat2x2u64;
	using Mat2f   = Mat2x2f;

	using Mat3i   = Mat3x3i;
	using Mat3u   = Mat3x3u;
	using Mat3i64 = Mat3x3i64;
	using Mat3u64 = Mat3x3u64;
	using Mat3f   = Mat3x3f;

	using Mat4i   = Mat4x4i;
	using Mat4u   = Mat4x4u;
	using Mat4i64 = Mat4x4i64;
	using Mat4u64 = Mat4x4u64;
	using Mat4f   = Mat4x4f;

}

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  pragma GCC diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  pragma warning( pop )
#endif

#endif // __TS3_MATH_MATRIX_BASE_H__
