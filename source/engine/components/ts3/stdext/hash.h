
#ifndef __TS3_STDEXT_HASH_OBJECT_H__
#define __TS3_STDEXT_HASH_OBJECT_H__

#include "prerequisites.h"
#include <string>

namespace ts3
{

	enum class EHashAlgo : enum_default_value_t
	{
		Adler32,
		CRC32,
		DJB2,
		FNV1A,
		SDBM
	};

	struct HashInput
	{
		const void * data;
		size_t dataSize;

		template <typename TpChar>
		explicit HashInput( const std::basic_string<TpChar> & pString )
		: data( pString.data() )
		, dataSize( pString.length() * sizeof( TpChar ) )
		{}

		template <typename Tp>
		explicit HashInput( const ArrayView<Tp> & pArrayView )
		: data( pArrayView.data() )
		, dataSize( pArrayView.size() * sizeof( Tp ) )
		{}

		template <typename Tp>
		explicit HashInput( const Tp & pObject )
		: data( &pObject )
		, dataSize( sizeof( Tp ) )
		{}

		template <typename Tp>
		HashInput( const Tp * pData, size_t pCount )
		: data( pData )
		, dataSize( pCount * sizeof( Tp ) )
		{}
	};

	template <EHashAlgo tpHashAlgo>
	struct HashTraits;
	
	template <EHashAlgo tpHashAlgo>
	struct HashObject
	{
	public:
		using ValueType = typename HashTraits<tpHashAlgo>::hash_ValueType;

		static constexpr EHashAlgo sHashAlgo = tpHashAlgo;

		ValueType hashValue;

	public:
		HashObject( const HashObject & ) = default;
		HashObject & operator=( const HashObject & ) = default;
		
		constexpr HashObject() noexcept
		: hashValue( static_cast<ValueType>( 0 ) )
		{}

		constexpr explicit HashObject( ValueType pValue ) noexcept
		: hashValue( pValue )
		{}

		constexpr explicit operator ValueType() const noexcept
		{
			return hashValue;
		}

		void swap( HashObject & pOther ) noexcept
		{
			std::swap( hashValue, pOther.hashValue );
		}

		bool equals( const HashObject & pOther ) const noexcept
		{
			return hashValue == pOther.hashValue;
		}

		int32 compare( const HashObject & pOther ) const noexcept
		{
			return equals( pOther ) ? 0 : ( hashValue > pOther.hashValue ? 1 : -1 );
		}
	};

	template <EHashAlgo tpHashAlgo>
	inline constexpr HashObject<tpHashAlgo> CX_HASH_EMPTY{ 0u };

	template <EHashAlgo tpHashAlgo>
	inline constexpr HashObject<tpHashAlgo> CX_HASH_INIT{ HashTraits<tpHashAlgo>::hashInitValue };

	struct HashGen
	{
		template <EHashAlgo tpHashAlgo>
		TS3_FUNC_NO_DISCARD static HashObject<tpHashAlgo> compute( const void * pData, size_t pDataLength )
		{
			const auto hashValue = HashTraits<tpHashAlgo>::compute( pData, pDataLength );
			return HashObject<tpHashAlgo>{ hashValue };
		}

		template <EHashAlgo tpHashAlgo>
		TS3_FUNC_NO_DISCARD static HashObject<tpHashAlgo> compute( const HashInput & pInput )
		{
			return compute<tpHashAlgo>( pInput.data, pInput.dataSize );
		}

		template <EHashAlgo tpHashAlgo, typename Tp>
		TS3_FUNC_NO_DISCARD static HashObject<tpHashAlgo> compute( const ArrayView<Tp> & pInput )
		{
			return compute<tpHashAlgo>( pInput.data(), pInput.size() * sizeof( Tp ) );
		}

		template <EHashAlgo tpHashAlgo, typename Tp>
		TS3_FUNC_NO_DISCARD static HashObject<tpHashAlgo> compute( const Tp & pInput )
		{
			return compute<tpHashAlgo>( &pInput, sizeof( pInput ) );
		}

		template <EHashAlgo tpHashAlgo>
		static void computeInplace( HashObject<tpHashAlgo> & pHash, const void * pData, size_t pDataLength )
		{
			pHash.hashValue = HashTraits<tpHashAlgo>::compute( pData, pDataLength );
		}

		template <EHashAlgo tpHashAlgo>
		static void computeInplace( HashObject<tpHashAlgo> & pHash, const HashInput & pInput )
		{
			computeInplace( pHash, pInput.data, pInput.dataSize );
		}

		template <EHashAlgo tpHashAlgo, typename Tp>
		static void computeInplace( HashObject<tpHashAlgo> & pHash, const ArrayView<Tp> & pInput )
		{
			return computeInplace<tpHashAlgo>( pHash, pInput.data(), pInput.size() * sizeof( Tp ) );
		}

		template <EHashAlgo tpHashAlgo, typename Tp>
		static void computeInplace( HashObject<tpHashAlgo> & pHash, const Tp & pInput )
		{
			return computeInplace<tpHashAlgo>( pHash, &pInput, sizeof( pInput ) );
		}

		template <EHashAlgo tpHashAlgo>
		TS3_FUNC_NO_DISCARD static HashObject<tpHashAlgo> update( const HashObject<tpHashAlgo> & pHash, const void * pData, size_t pDataLength )
		{
			auto hashValue = HashTraits<tpHashAlgo>::update( pHash.hashValue, pData, pDataLength );
			return HashObject<tpHashAlgo>{ hashValue };
		}

		template <EHashAlgo tpHashAlgo>
		TS3_FUNC_NO_DISCARD static HashObject<tpHashAlgo> update( const HashObject<tpHashAlgo> & pHash, const HashInput & pInput )
		{
			return update( pHash, pInput.data, pInput.dataSize );
		}

		template <EHashAlgo tpHashAlgo, typename Tp>
		TS3_FUNC_NO_DISCARD static HashObject<tpHashAlgo> update( const HashObject<tpHashAlgo> & pHash, const ArrayView<Tp> & pInput )
		{
			return update( pHash, pInput.data(), pInput.size() * sizeof( Tp ) );
		}

		template <EHashAlgo tpHashAlgo, typename Tp>
		TS3_FUNC_NO_DISCARD static HashObject<tpHashAlgo> update( const HashObject<tpHashAlgo> & pHash, const Tp & pInput )
		{
			return update( pHash, &pInput, sizeof( pInput ) );
		}

		template <EHashAlgo tpHashAlgo>
		static void updateInplace( HashObject<tpHashAlgo> & pHash, const void * pData, size_t pDataLength )
		{
			pHash.hashValue = HashTraits<tpHashAlgo>::update( pHash.hashValue, pData, pDataLength );
		}

		template <EHashAlgo tpHashAlgo>
		static void updateInplace( HashObject<tpHashAlgo> & pHash, const HashInput & pInput )
		{
			updateInplace( pHash.hashValue, pInput.data, pInput.dataSize );
		}

		template <EHashAlgo tpHashAlgo, typename Tp>
		static void updateInplace( HashObject<tpHashAlgo> & pHash, const ArrayView<Tp> & pInput )
		{
			return updateInplace( pHash, pInput.data(), pInput.size() * sizeof( Tp ) );
		}

		template <EHashAlgo tpHashAlgo, typename Tp>
		static void updateInplace( HashObject<tpHashAlgo> & pHash, const Tp & pInput )
		{
			return updateInplace( pHash, &pInput, sizeof( pInput ) );
		}
	};

	template <EHashAlgo tpHashAlgo>
	inline constexpr bool operator==( const HashObject<tpHashAlgo> & pLhs, const HashObject<tpHashAlgo> & pRhs )
	{
		return pLhs.hashValue == pRhs.hashValue;
	}

	template <EHashAlgo tpHashAlgo>
	inline constexpr bool operator!=( const HashObject<tpHashAlgo> & pLhs, const HashObject<tpHashAlgo> & pRhs )
	{
		return pLhs.hashValue != pRhs.hashValue;
	}

	template <EHashAlgo tpHashAlgo>
	inline constexpr bool operator<( const HashObject<tpHashAlgo> & pLhs, const HashObject<tpHashAlgo> & pRhs )
	{
		return pLhs.hashValue < pRhs.hashValue;
	}

	template <EHashAlgo tpHashAlgo>
	inline constexpr bool operator<=( const HashObject<tpHashAlgo> & pLhs, const HashObject<tpHashAlgo> & pRhs )
	{
		return pLhs.hashValue <= pRhs.hashValue;
	}

	template <EHashAlgo tpHashAlgo>
	inline constexpr bool operator>( const HashObject<tpHashAlgo> & pLhs, const HashObject<tpHashAlgo> & pRhs )
	{
		return pLhs.hashValue > pRhs.hashValue;
	}

	template <EHashAlgo tpHashAlgo>
	inline constexpr bool operator>=( const HashObject<tpHashAlgo> & pLhs, const HashObject<tpHashAlgo> & pRhs )
	{
		return pLhs.hashValue >= pRhs.hashValue;
	}


	template <typename TpHashValueType>
	struct HashCommonTraits
	{
		using hash_ValueType = TpHashValueType;
		static constexpr size_t byteSize = sizeof( TpHashValueType );
		static constexpr size_t hashHexLength = byteSize * 8 / 4;
	};

	template <>
	struct HashTraits<EHashAlgo::Adler32> : public HashCommonTraits<uint32>
	{
		static constexpr uint32 hashInitValue = 1u;
		static uint32 compute( const void * pInput, size_t pInputSize );
		static uint32 update( uint32 pHash, const void * pInput, size_t pInputSize );
	};

	template <>
	struct HashTraits<EHashAlgo::CRC32> : public HashCommonTraits<uint32>
	{
		static constexpr uint32 hashInitValue = 0xFFFFFFFF;
		static uint32 compute( const void * pInput, size_t pInputSize );
		static uint32 update( uint32 pHash, const void * pInput, size_t pInputSize );
	};

	template <>
	struct HashTraits<EHashAlgo::DJB2> : public HashCommonTraits<uint32>
	{
		static constexpr uint32 hashInitValue = 0x1505;
		static uint32 compute( const void * pInput, size_t pInputSize );
		static uint32 update( uint32 pHash, const void * pInput, size_t pInputSize );
	};

	template <>
	struct HashTraits<EHashAlgo::FNV1A> : public HashCommonTraits<uint64>
	{
		static constexpr uint64 hashInitValue = 0xCBF29CE484222325;
		static uint64 compute( const void * pInput, size_t pInputSize );
		static uint64 update( uint64 pHash, const void * pInput, size_t pInputSize );
	};

	template <>
	struct HashTraits<EHashAlgo::SDBM> : public HashCommonTraits<uint32>
	{
		static constexpr uint32 hashInitValue = 0u;
		static uint32 compute( const void * pInput, size_t pInputSize );
		static uint32 update( uint32 pHash, const void * pInput, size_t pInputSize );
	};

}

namespace std
{

	template <ts3::EHashAlgo tpHashAlgo>
	struct hash< ts3::HashObject<tpHashAlgo> >
	{
		size_t operator()( const ts3::HashObject<tpHashAlgo> & pHashObject ) const noexcept
		{
			return ts3::trunc_numeric_cast<size_t>( pHashObject.hashValue );
		}
	};

}

#endif // __TS3_STDEXT_HASH_OBJECT_H__
