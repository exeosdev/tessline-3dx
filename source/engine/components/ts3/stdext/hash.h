
#ifndef __TS3_STDEXT_HASH_OBJECT_H__
#define __TS3_STDEXT_HASH_OBJECT_H__

#include "arrayView.h"
#include <string>

namespace ts3
{

	enum class EHashAlgo : enum_default_value_t
	{
		Adler32,
		CRC32,
		DJB2,
		FNV1A32,
		FNV1A64,
		SDBM
	};

	struct HashInput
	{
		const void * data;
		size_t dataSize;

		template <typename TChar>
		explicit HashInput( const std::basic_string<TChar> & pString )
		: data( pString.data() )
		, dataSize( pString.length() * sizeof( TChar ) )
		{}

		explicit HashInput( const char * pCStr )
		: data( pCStr )
		, dataSize( std::strlen( pCStr ) )
		{}

		explicit HashInput( const wchar_t * pWCStr )
		: data( pWCStr )
		, dataSize( std::wcslen( pWCStr ) )
		{}

		template <typename TValue>
		explicit HashInput( const ArrayView<TValue> & pArrayView )
		: data( pArrayView.data() )
		, dataSize( pArrayView.size() * sizeof( TValue ) )
		{}

		template <typename TValue>
		explicit HashInput( const TValue & pObject )
		: data( &pObject )
		, dataSize( sizeof( TValue ) )
		{}

		template <typename TInput>
		explicit HashInput( const TInput * pData, size_t pCount )
		: data( pData )
		, dataSize( pCount * sizeof( TInput ) )
		{}
	};

	template <EHashAlgo tHashAlgo>
	struct HashTraits;
	
	template <EHashAlgo tHashAlgo>
	struct HashObject
	{
	public:
		using ValueType = typename HashTraits<tHashAlgo>::ValueType;

		static constexpr EHashAlgo sHashAlgo = tHashAlgo;

		ValueType hashValue;

	public:
		HashObject( const HashObject & ) = default;
		HashObject & operator=( const HashObject & ) = default;
		
		constexpr HashObject() noexcept
		: hashValue( HashTraits<tHashAlgo>::sHashInitValue )
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

		TS3_ATTR_NO_DISCARD bool equals( const HashObject & pOther ) const noexcept
		{
			return hashValue == pOther.hashValue;
		}

		TS3_ATTR_NO_DISCARD int32 compare( const HashObject & pOther ) const noexcept
		{
			return equals( pOther ) ? 0 : ( hashValue > pOther.hashValue ? 1 : -1 );
		}

		TS3_ATTR_NO_DISCARD constexpr bool operator==( const HashObject<tHashAlgo> & pRhs ) const noexcept
		{
			return hashValue == pRhs.hashValue;
		}

		TS3_ATTR_NO_DISCARD constexpr bool operator!=( const HashObject<tHashAlgo> & pRhs ) const noexcept
		{
			return hashValue != pRhs.hashValue;
		}

		TS3_ATTR_NO_DISCARD constexpr bool operator<( const HashObject<tHashAlgo> & pRhs ) const noexcept
		{
			return hashValue < pRhs.hashValue;
		}

		TS3_ATTR_NO_DISCARD constexpr bool operator<=( const HashObject<tHashAlgo> & pRhs ) const noexcept
		{
			return hashValue <= pRhs.hashValue;
		}

		TS3_ATTR_NO_DISCARD constexpr bool operator>( const HashObject<tHashAlgo> & pRhs ) const noexcept
		{
			return hashValue > pRhs.hashValue;
		}

		TS3_ATTR_NO_DISCARD constexpr bool operator>=( const HashObject<tHashAlgo> & pRhs ) const noexcept
		{
			return hashValue >= pRhs.hashValue;
		}
	};

	template <EHashAlgo tHashAlgo>
	inline constexpr HashObject<tHashAlgo> cvHashEmpty{ 0u };

	template <EHashAlgo tHashAlgo>
	inline constexpr HashObject<tHashAlgo> cvHashInit{ HashTraits<tHashAlgo>::sHashInitValue };



	template <EHashAlgo tHashAlgo>
	TS3_ATTR_NO_DISCARD inline HashObject<tHashAlgo> hashCompute( const void * pData, size_t pDataLength )
	{
		return HashObject<tHashAlgo>{ HashTraits<tHashAlgo>::compute( pData, pDataLength ) };
	}

	template <EHashAlgo tHashAlgo>
	TS3_ATTR_NO_DISCARD inline HashObject<tHashAlgo> hashCompute( const HashInput & pInput )
	{
		return hashCompute<tHashAlgo>( pInput.data, pInput.dataSize );
	}

	template <EHashAlgo tHashAlgo, typename TValue>
	TS3_ATTR_NO_DISCARD inline HashObject<tHashAlgo> hashCompute( const TValue & pValue )
	{
		return hashCompute<tHashAlgo>( HashInput{ pValue } );
	}

	template <EHashAlgo tHashAlgo>
	TS3_ATTR_NO_DISCARD inline HashObject<tHashAlgo> hashComputeEx( const HashObject<tHashAlgo> & pInitHash, const void * pData, size_t pDataLength )
	{
		return HashObject<tHashAlgo>{ HashTraits<tHashAlgo>::update( pInitHash.hashValue, pData, pDataLength ) };
	}

	template <EHashAlgo tHashAlgo>
	TS3_ATTR_NO_DISCARD inline HashObject<tHashAlgo> hashComputeEx( const HashObject<tHashAlgo> & pInitHash, const HashInput & pInput )
	{
		return hashComputeEx<tHashAlgo>( pInitHash, pInput.data, pInput.dataSize );
	}

	template <EHashAlgo tHashAlgo, typename TValue>
	TS3_ATTR_NO_DISCARD inline HashObject<tHashAlgo> hashComputeEx( const HashObject<tHashAlgo> & pInitHash, const TValue & pValue )
	{
		return hashComputeEx<tHashAlgo>( pInitHash, HashInput{ pValue } );
	}

	template <EHashAlgo tHashAlgo, typename TValue, typename... TRest>
	TS3_ATTR_NO_DISCARD inline HashObject<tHashAlgo> hashCompute( const TValue & pValue, TRest && ...pRest )
	{
		return hashComputeEx<tHashAlgo>( hashCompute<tHashAlgo>( pValue ), std::forward<TRest>( pRest )... );
	}

	template <EHashAlgo tHashAlgo, typename TValue, typename... TRest>
	TS3_ATTR_NO_DISCARD inline HashObject<tHashAlgo> hashComputeEx( const HashObject<tHashAlgo> & pInitHash, const TValue & pValue, TRest && ...pRest )
	{
		return hashComputeEx<tHashAlgo>( hashComputeEx<tHashAlgo>( pInitHash, pValue ), std::forward<TRest>( pRest )... );
	}


	template <EHashAlgo tHashAlgo>
	inline void hashComputeInplace( HashObject<tHashAlgo> & pOutput, const void * pData, size_t pDataLength )
	{
		pOutput.hashValue = HashTraits<tHashAlgo>::compute( pData, pDataLength );
	}

	template <EHashAlgo tHashAlgo>
	inline void hashComputeInplace( HashObject<tHashAlgo> & pOutput, const HashInput & pInput )
	{
		hashComputeInplace<tHashAlgo>( pOutput, pInput.data, pInput.dataSize );
	}

	template <EHashAlgo tHashAlgo, typename TValue>
	inline void hashComputeInplace( HashObject<tHashAlgo> & pOutput, const ArrayView<TValue> & pInput )
	{
		hashComputeInplace<tHashAlgo>( pOutput, pInput.data(), pInput.size() * sizeof( TValue ) );
	}

	template <EHashAlgo tHashAlgo>
	inline void hashComputeInplaceEx( HashObject<tHashAlgo> & pOutput, const HashObject<tHashAlgo> & pInitHash, const void * pData, size_t pDataLength )
	{
		pOutput.hashValue = HashTraits<tHashAlgo>::update( pInitHash.hashValue, pData, pDataLength );
	}

	template <EHashAlgo tHashAlgo>
	inline void hashComputeInplaceEx( HashObject<tHashAlgo> & pOutput, const HashObject<tHashAlgo> & pInitHash, const HashInput & pInput )
	{
		hashComputeInplaceEx<tHashAlgo>( pOutput, pInitHash, pInput.data, pInput.dataSize );
	}

	template <EHashAlgo tHashAlgo, typename TValue>
	inline void hashComputeInplaceEx( HashObject<tHashAlgo> & pOutput, const HashObject<tHashAlgo> & pInitHash, const TValue & pValue )
	{
		hashComputeInplaceEx<tHashAlgo>( pOutput, pInitHash, HashInput{ pValue } );
	}

	template <EHashAlgo tHashAlgo, typename TValue, typename... TRest>
	inline void hashComputeInplace( HashObject<tHashAlgo> & pOutput, const TValue & pValue, TRest && ...pRest )
	{
		hashComputeInplaceEx<tHashAlgo>( pOutput, hashCompute<tHashAlgo>( pValue ), std::forward<TRest>( pRest )... );
	}

	template <EHashAlgo tHashAlgo, typename TValue, typename... TRest>
	inline void hashComputeInplaceEx( HashObject<tHashAlgo> & pOutput, const HashObject<tHashAlgo> & pInitHash, const TValue & pValue, TRest && ...pRest )
	{
		hashComputeInplaceEx<tHashAlgo>( pOutput, hashComputeEx<tHashAlgo>( pInitHash, pValue ), std::forward<TRest>( pRest )... );
	}

	template <typename THashValue>
	struct HashCommonTraits
	{
		/// Underlying type used to store hash value.
		using ValueType = THashValue;

		/// Size of the hash value, in bytes.
		static constexpr size_t sByteSize = sizeof( THashValue );

		/// Length of the hexadecimal string representation of a hash.
		static constexpr size_t sHexLength = sByteSize * 8 / 4;
	};

	template <>
	struct HashTraits<EHashAlgo::Adler32> : public HashCommonTraits<uint32>
	{
		static constexpr uint32 sHashInitValue = 1u;
		static uint32 compute( const void * pInput, size_t pInputSize );
		static uint32 update( uint32 pHash, const void * pInput, size_t pInputSize );
	};

	template <>
	struct HashTraits<EHashAlgo::CRC32> : public HashCommonTraits<uint32>
	{
		static constexpr uint32 sHashInitValue = 0xFFFFFFFF;
		static uint32 compute( const void * pInput, size_t pInputSize );
		static uint32 update( uint32 pHash, const void * pInput, size_t pInputSize );
	};

	template <>
	struct HashTraits<EHashAlgo::DJB2> : public HashCommonTraits<uint32>
	{
		static constexpr uint32 sHashInitValue = 0x1505;
		static uint32 compute( const void * pInput, size_t pInputSize );
		static uint32 update( uint32 pHash, const void * pInput, size_t pInputSize );
	};

	template <>
	struct HashTraits<EHashAlgo::FNV1A32> : public HashCommonTraits<uint32>
	{
		static constexpr uint32 sHashInitValue = 0x811C9DC5;
		static uint32 compute( const void * pInput, size_t pInputSize );
		static uint32 update( uint32 pHash, const void * pInput, size_t pInputSize );
	};

	template <>
	struct HashTraits<EHashAlgo::FNV1A64> : public HashCommonTraits<uint64>
	{
		static constexpr uint64 sHashInitValue = 0xCBF29CE484222325;
		static uint64 compute( const void * pInput, size_t pInputSize );
		static uint64 update( uint64 pHash, const void * pInput, size_t pInputSize );
	};

	template <>
	struct HashTraits<EHashAlgo::SDBM> : public HashCommonTraits<uint32>
	{
		static constexpr uint32 sHashInitValue = 0u;
		static uint32 compute( const void * pInput, size_t pInputSize );
		static uint32 update( uint32 pHash, const void * pInput, size_t pInputSize );
	};

}

namespace std
{

	template <ts3::EHashAlgo tHashAlgo>
	struct hash< ts3::HashObject<tHashAlgo> >
	{
		size_t operator()( const ts3::HashObject<tHashAlgo> & pHashObject ) const noexcept
		{
			return ts3::numeric_cast<size_t>( pHashObject.hashValue );
		}
	};

}

#endif // __TS3_STDEXT_HASH_OBJECT_H__
