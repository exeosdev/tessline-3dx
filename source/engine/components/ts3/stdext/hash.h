
#ifndef __TS3_STDEXT_HASH_OBJECT_H__
#define __TS3_STDEXT_HASH_OBJECT_H__

#include "prerequisites.h"
#include <string>

namespace ts3
{

	enum class HashAlgo : enum_default_value_t
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
		HashInput( const std::basic_string<TpChar> & pString )
		: data( pString.c_str() )
		, dataSize( pString.length() * sizeof( TpChar ) )
		{}

		template <typename Tp>
		HashInput( const Tp & pObject )
		: data( &pObject )
		, dataSize( sizeof( Tp ) )
		{}
	};

	template <HashAlgo tpHashAlgo>
	struct HashTraits;
	
	template <HashAlgo tpHashAlgo>
	struct HashObject
	{
	public:
		using value_t = typename HashTraits<tpHashAlgo>::hash_value_t;

		value_t hashValue;

	public:
		HashObject( const HashObject & ) = default;
		HashObject & operator=( const HashObject & ) = default;
		
		constexpr HashObject() noexcept
		: hashValue( static_cast<value_t>( 0 ) )
		{}

		constexpr explicit HashObject( value_t pValue ) noexcept
		: hashValue( pValue )
		{}

		constexpr explicit operator value_t() const noexcept
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

	template <HashAlgo tpHashAlgo>
	inline constexpr HashObject<tpHashAlgo> cvEmptyHash{ HashTraits<tpHashAlgo>::hashInitValue };

	struct HashGen
	{
		template <HashAlgo tpHashAlgo>
		static HashObject<tpHashAlgo> compute( const void * pData, size_t pDataLength )
		{
			auto hashValue = HashTraits<tpHashAlgo>::compute( pData, pDataLength );
			return HashObject<tpHashAlgo>{ hashValue };
		}

		template <HashAlgo tpHashAlgo>
		static HashObject<tpHashAlgo> compute( const HashInput & pInput )
		{
			return compute<tpHashAlgo>( pInput.data, pInput.dataSize );
		}

		template <HashAlgo tpHashAlgo>
		static void computeInplace( HashObject<tpHashAlgo> & pHash, const void * pData, size_t pDataLength )
		{
			pHash.hashValue = HashTraits<tpHashAlgo>::compute( pData, pDataLength );
		}

		template <HashAlgo tpHashAlgo>
		static void computeInplace( HashObject<tpHashAlgo> & pHash, const HashInput & pInput )
		{
			computeInplace( pHash, pInput.data, pInput.dataSize );
		}

		template <HashAlgo tpHashAlgo>
		static HashObject<tpHashAlgo> update( const HashObject<tpHashAlgo> & pHash, const void * pData, size_t pDataLength )
		{
			auto hashValue = HashTraits<tpHashAlgo>::update( pHash.hashValue, pData, pDataLength );
			return HashObject<tpHashAlgo>{ hashValue };
		}

		template <HashAlgo tpHashAlgo>
		static HashObject<tpHashAlgo> update( const HashObject<tpHashAlgo> & pHash, const HashInput & pInput )
		{
			return update( pHash, pInput.data, pInput.dataSize );
		}

		template <HashAlgo tpHashAlgo>
		static void updateInplace( HashObject<tpHashAlgo> & pHash, const void * pData, size_t pDataLength )
		{
			pHash.hashValue = HashTraits<tpHashAlgo>::update( pHash.hashValue, pData, pDataLength );
		}

		template <HashAlgo tpHashAlgo>
		static void updateInplace( HashObject<tpHashAlgo> & pHash, const HashInput & pInput )
		{
			updateInplace( pHash.hashValue, pInput.data, pInput.dataSize );
		}
	};

	template <HashAlgo tpHashAlgo>
	constexpr inline bool operator==( const HashObject<tpHashAlgo> & pLhs, const HashObject<tpHashAlgo> & pRhs )
	{
		return pLhs.hashValue == pRhs.hashValue;
	}

	template <HashAlgo tpHashAlgo>
	constexpr inline bool operator!=( const HashObject<tpHashAlgo> & pLhs, const HashObject<tpHashAlgo> & pRhs )
	{
		return pLhs.hashValue != pRhs.hashValue;
	}

	template <HashAlgo tpHashAlgo>
	constexpr inline bool operator<( const HashObject<tpHashAlgo> & pLhs, const HashObject<tpHashAlgo> & pRhs )
	{
		return pLhs.hashValue < pRhs.hashValue;
	}

	template <HashAlgo tpHashAlgo>
	constexpr inline bool operator<=( const HashObject<tpHashAlgo> & pLhs, const HashObject<tpHashAlgo> & pRhs )
	{
		return pLhs.hashValue <= pRhs.hashValue;
	}

	template <HashAlgo tpHashAlgo>
	constexpr inline bool operator>( const HashObject<tpHashAlgo> & pLhs, const HashObject<tpHashAlgo> & pRhs )
	{
		return pLhs.hashValue > pRhs.hashValue;
	}

	template <HashAlgo tpHashAlgo>
	constexpr inline bool operator>=( const HashObject<tpHashAlgo> & pLhs, const HashObject<tpHashAlgo> & pRhs )
	{
		return pLhs.hashValue >= pRhs.hashValue;
	}


	template <typename TpHashValueType>
	struct HashCommonTraits
	{
		using hash_value_t = TpHashValueType;
		static constexpr size_t byteSize = sizeof( TpHashValueType );
		static constexpr size_t hashHexLength = byteSize * 8 / 4;
	};

	template <>
	struct HashTraits<HashAlgo::Adler32> : public HashCommonTraits<uint32>
	{
		static constexpr uint32 hashInitValue = 1u;
		static uint32 compute( const void * pInput, size_t pInputSize );
		static uint32 update( uint32 pHash, const void * pInput, size_t pInputSize );
	};

	template <>
	struct HashTraits<HashAlgo::CRC32> : public HashCommonTraits<uint32>
	{
		static constexpr uint32 hashInitValue = 0xFFFFFFFF;
		static uint32 compute( const void * pInput, size_t pInputSize );
		static uint32 update( uint32 pHash, const void * pInput, size_t pInputSize );
	};

	template <>
	struct HashTraits<HashAlgo::DJB2> : public HashCommonTraits<uint32>
	{
		static constexpr uint32 hashInitValue = 0x1505;
		static uint32 compute( const void * pInput, size_t pInputSize );
		static uint32 update( uint32 pHash, const void * pInput, size_t pInputSize );
	};

	template <>
	struct HashTraits<HashAlgo::FNV1A> : public HashCommonTraits<uint64>
	{
		static constexpr uint64 hashInitValue = 0xCBF29CE484222325;
		static uint64 compute( const void * pInput, size_t pInputSize );
		static uint64 update( uint64 pHash, const void * pInput, size_t pInputSize );
	};

	template <>
	struct HashTraits<HashAlgo::SDBM> : public HashCommonTraits<uint32>
	{
		static constexpr uint32 hashInitValue = 0u;
		static uint32 compute( const void * pInput, size_t pInputSize );
		static uint32 update( uint32 pHash, const void * pInput, size_t pInputSize );
	};

}

namespace std
{

	template <ts3::HashAlgo tpHashAlgo>
	struct hash< ts3::HashObject<tpHashAlgo> >
	{
		size_t operator()( const ts3::HashObject<tpHashAlgo> & pHashObject ) const noexcept
		{
			return pHashObject.hashValue;
		}
	};

}

#endif // __TS3_STDEXT_HASH_OBJECT_H__
