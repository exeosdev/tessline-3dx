
#ifndef __TS3_MATH_MATRIX_BASE_H__
#define __TS3_MATH_MATRIX_BASE_H__

#include "vector.h"

namespace ts3::math
{

	// Traits container struct for common properties of matrix types.
	template <typename Tp, size_t tRows, size_t tColumns>
	struct MatrixTraits
	{
		static const uint32 sRowLength = tColumns;
		static const uint32 sRowsNum = tRows;
		static const uint32 sColumnsNumNum = tColumns;
		static const uint32 sLength = sRowsNum * sColumnsNumNum;
		static const uint32 sByteSize = sizeof( Tp ) * sLength;
		static const uint32 sRowBytePitch = sizeof( Tp ) * sRowLength;
	};

	template <typename Tp, size_t /* tRows */, size_t tColumns>
	struct MatrixRowSIMDData
	{
		using Type = Tp[tColumns];
	};

	template <typename Tp, size_t tRows>
	struct MatrixRowSIMDData<Tp, tRows, 4>
	{
		using Type = typename Vector4SIMDData<Tp>::Type;
	};

	// Common class for all matrix types. The default implementation is empty, because we need
	// more fine-grained control over members definition.
	// Internally, matrix values are represented as arrays of rows (similar to DXM library),
	// so there is a specialized definition for MatrixBase<T, 2, R>, MatrixBase<T, 3, R> and
	// MatrixBase<T, 4, R> for 2xR, 3xR and 4xR matrices of type T, respectively.
	template <typename Tp, size_t tRows, size_t tColumns>
	struct Matrix;


	// MatrixBase for 2xR matrices of type Tp.
	template <typename Tp, size_t tColumns>
	struct Matrix<Tp, 2, tColumns>
	{
	public:
		static constexpr size_t sRowsNum = 2;
		static constexpr size_t sColumnsNum = tColumns;
		static constexpr size_t sMatrixSize = sRowsNum * sColumnsNum;

		using Traits = MatrixTraits<Tp, 2, tColumns>;
		using ColumnVector = Vector2<Tp>;
		using RowVector = Vector<Tp, tColumns>;
		using RowSIMDDataType = typename MatrixRowSIMDData<Tp, sRowsNum, tColumns>::Type;

		union
		{
			struct
			{
				RowSIMDDataType mm0, mm1;
			};
			struct
			{
				RowVector rv0, rv1;
			};
			RowSIMDDataType mmArray[sRowsNum];
			RowVector rvArray[sRowsNum];
			Tp valueArray[sMatrixSize];
		};

	public:
		constexpr Matrix( const Matrix & ) = default;
		constexpr Matrix & operator=( const Matrix & ) = default;

		constexpr Matrix() noexcept
		{}

		constexpr Matrix( std::initializer_list<Tp> pData ) noexcept
		: rv0( pData.begin() )
		, rv1( pData.begin() + tColumns )
		{}

		template <typename Tp0, typename Tp1>
		constexpr Matrix( const Vector<Tp0, tColumns> & pRow0,
		                  const Vector<Tp1, tColumns> & pRow1 ) noexcept
		: rv0( pRow0 ), rv1( pRow1 )
		{}

		template <typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		constexpr explicit Matrix( TpScalar pScalar ) noexcept
		: rv0( pScalar ), rv1( pScalar )
		{}

		Vector<Tp, tColumns> & operator[]( size_t pIndex ) noexcept
		{
			return staticArrayElement( rvArray, pIndex );
		}

		const Vector<Tp, tColumns> & operator[]( size_t pIndex ) const noexcept
		{
			return staticArrayElement( rvArray, pIndex );
		}

		ColumnVector column( size_t pCIndex ) const noexcept
		{
			return { rv0[pCIndex], rv1[pCIndex] };
		}
	};

	// MatrixBase for 3xR matrices of type Tp.
	template <typename Tp, size_t tColumns>
	struct Matrix<Tp, 3, tColumns>
	{
	public:
		static constexpr size_t sRowsNum = 3;
		static constexpr size_t sColumnsNum = tColumns;
		static constexpr size_t sMatrixSize = sRowsNum * sColumnsNum;
		using RowSIMDDataType = typename MatrixRowSIMDData<Tp, sRowsNum, tColumns>::Type;

		using Traits = MatrixTraits<Tp, 3, tColumns>;
		using ColumnVector = Vector<Tp, 3>;
		using RowVector = Vector<Tp, tColumns>;

		union
		{
			struct
			{
				RowSIMDDataType mm0, mm1, mm2;
			};
			struct
			{
				RowVector rv0, rv1, rv2;
			};
			RowSIMDDataType mmArray[sRowsNum];
			RowVector rvArray[sRowsNum];
			Tp valueArray[sMatrixSize];
		};

	public:
		constexpr Matrix( const Matrix & ) = default;
		constexpr Matrix & operator=( const Matrix & ) = default;

		constexpr Matrix() noexcept
		{}

		constexpr Matrix( std::initializer_list<Tp> pData ) noexcept
		: rv0( pData.begin() )
		, rv1( pData.begin() + tColumns )
		, rv2( pData.begin() + 2*tColumns )
		{}

		template <typename Tp0, typename Tp1, typename Tp2>
		constexpr Matrix( const Vector<Tp0, tColumns> & pRow0,
		                  const Vector<Tp1, tColumns> & pRow1,
		                  const Vector<Tp2, tColumns> & pRow2 ) noexcept
		: rv0( pRow0 ), rv1( pRow1 ), rv2( pRow2 )
		{}

		template <typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		constexpr explicit Matrix( TpScalar pScalar ) noexcept
		: rv0( pScalar ), rv1( pScalar ), rv2( pScalar )
		{}

		Vector<Tp, tColumns> & operator[]( size_t pIndex ) noexcept
		{
			return staticArrayElement( rvArray, pIndex );
		}

		const Vector<Tp, tColumns> & operator[]( size_t pIndex ) const noexcept
		{
			return staticArrayElement( rvArray, pIndex );
		}

		ColumnVector column( size_t pCIndex ) const noexcept
		{
			return { rv0[pCIndex], rv1[pCIndex], rv2[pCIndex] };
		}
	};

	// MatrixBase for Rx3 matrices of type Tp.
	template <typename Tp, size_t tColumns>
	struct Matrix<Tp, 4, tColumns>
	{
	public:
		static constexpr size_t sRowsNum = 4;
		static constexpr size_t sColumnsNum = tColumns;
		static constexpr size_t sMatrixSize = sRowsNum * tColumns;
		using RowSIMDDataType = typename MatrixRowSIMDData<Tp, sRowsNum, tColumns>::Type;

		using Traits = MatrixTraits<Tp, sRowsNum, tColumns>;
		using ColumnVector = Vector<Tp, sRowsNum>;
		using RowVector = Vector<Tp, tColumns>;

		union
		{
			struct
			{
				RowSIMDDataType mm0, mm1, mm2, mm3;
			};
			struct
			{
				RowVector rv0, rv1, rv2, rv3;
			};
			RowSIMDDataType mmArray[sRowsNum];
			RowVector rvArray[sRowsNum];
			Tp valueArray[sMatrixSize];
		};

	public:
		constexpr Matrix( const Matrix & ) = default;
		constexpr Matrix & operator=( const Matrix & ) = default;

		constexpr Matrix() noexcept
		{}

		constexpr Matrix( std::initializer_list<Tp> pData ) noexcept
		: rv0( pData.begin() )
		, rv1( pData.begin() + tColumns )
		, rv2( pData.begin() + 2*tColumns )
		, rv3( pData.begin() + 3*tColumns )
		{}

		template <typename Tp0, typename Tp1, typename Tp2, typename Tp3>
		constexpr Matrix( const Vector<Tp0, tColumns> & pRow0,
		                  const Vector<Tp1, tColumns> & pRow1,
		                  const Vector<Tp2, tColumns> & pRow2,
		                  const Vector<Tp3, tColumns> & pRow3 ) noexcept
		: rv0( pRow0 ), rv1( pRow1 ), rv2( pRow2 ), rv3( pRow3 )
		{}

		template <typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		constexpr explicit Matrix( TpScalar pScalar ) noexcept
		: rv0( pScalar ), rv1( pScalar ), rv2( pScalar ), rv3( pScalar )
		{}

		Vector<Tp, tColumns> & operator[]( size_t pIndex ) noexcept
		{
			return staticArrayElement( rvArray, pIndex );
		}

		const Vector<Tp, tColumns> & operator[]( size_t pIndex ) const noexcept
		{
			return staticArrayElement( rvArray, pIndex );
		}

		ColumnVector column( size_t pCIndex ) const noexcept
		{
			return { rv0[pCIndex], rv1[pCIndex], rv2[pCIndex], rv3[pCIndex] };
		}
	};

	template <typename Tp, size_t tColumns>
	using Matrix2xC = Matrix<Tp, 2, tColumns>;

	template <typename Tp, size_t tColumns>
	using Matrix3xC = Matrix<Tp, 3, tColumns>;

	template <typename Tp, size_t tColumns>
	using Matrix4xC = Matrix<Tp, 4, tColumns>;

	template <typename Tp, size_t tRows>
	using MatrixRx2 = Matrix<Tp, tRows, 2>;

	template <typename Tp, size_t tRows>
	using MatrixRx3 = Matrix<Tp, tRows, 3>;

	template <typename Tp, size_t tRows>
	using MatrixRx4 = Matrix<Tp, tRows, 4>;

	template <typename Tp>
	using Mat2x2 = Matrix<Tp, 2, 2>;

	template <typename Tp>
	using Mat3x3 = Matrix<Tp, 3, 3>;

	template <typename Tp>
	using Mat4x4 = Matrix<Tp, 4, 4>;

	template <typename Tp>
	using Matrix2x2 = Matrix<Tp, 2, 2>;

	template <typename Tp>
	using Matrix2x3 = Matrix<Tp, 2, 3>;

	template <typename Tp>
	using Matrix2x4 = Matrix<Tp, 2, 4>;

	template <typename Tp>
	using Matrix3x2 = Matrix<Tp, 3, 2>;

	template <typename Tp>
	using Matrix3x3 = Matrix<Tp, 3, 3>;

	template <typename Tp>
	using Matrix3x4 = Matrix<Tp, 3, 4>;

	template <typename Tp>
	using Matrix4x2 = Matrix<Tp, 4, 2>;

	template <typename Tp>
	using Matrix4x3 = Matrix<Tp, 4, 3>;

	template <typename Tp>
	using Matrix4x4 = Matrix<Tp, 4, 4>;

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

	using Mat4i   = Mat4x4i;
	using Mat4u   = Mat4x4u;
	using Mat4i64 = Mat4x4i64;
	using Mat4u64 = Mat4x4u64;
	using Mat4f   = Mat4x4f;

}

#endif // __TS3_MATH_MATRIX_BASE_H__
