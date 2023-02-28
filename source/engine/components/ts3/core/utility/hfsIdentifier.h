
#ifndef __TS3_CORE_HFS_IDENTIFIER_H__
#define __TS3_CORE_HFS_IDENTIFIER_H__

#include "../prerequisites.h"
#include <ts3/stdext/hash.h>

namespace ts3
{

	/// @brief Hybrid FNV1A32 + SDBM identifier for string-based IDs.
	struct HFSIdentifier
	{
	public:
		static constexpr auto sHashAlgo1 = EHashAlgo::FNV1A32;
		static constexpr auto sHashAlgo2 = EHashAlgo::SDBM;

		using Hash1 = HashObject<sHashAlgo1>;
		using Hash2 = HashObject<sHashAlgo2>;
		using ValueType = uint64;

		uint64 idValue;
		
	public:
		HFSIdentifier() = default;

		HFSIdentifier( const HFSIdentifier & ) = default;
		HFSIdentifier & operator=( const HFSIdentifier & ) = default;

		explicit constexpr HFSIdentifier( uint64 p64Value ) noexcept
		: idValue( p64Value )
		{}

		constexpr HFSIdentifier( uint32 pHashValue1, uint32 pHashValue2 ) noexcept
		: idValue( makeU64Value( pHashValue1, pHashValue2 ) )
		{}

		constexpr HFSIdentifier( const Hash1 & pHash1, const Hash2 & pHash2 ) noexcept
		: idValue( makeU64Value( pHash1.hashValue, pHash2.hashValue ) )
		{}

		explicit constexpr operator uint64() const noexcept
		{
			return idValue;
		}

		explicit constexpr operator bool() const noexcept
		{
			return !empty();
		}

		constexpr bool empty() const noexcept
		{
			return idValue != 0;
		}

		constexpr ValueType asValue() const noexcept
		{
			return idValue;
		}

		constexpr void set( uint32 pHashValue1, uint32 pHashValue2 ) noexcept
		{
			idValue = makeU64Value( pHashValue1, pHashValue2 );
		}

		constexpr void set( const Hash1 & pHash1, const Hash2 & pHash2 ) noexcept
		{
			set( pHash1.hashValue, pHash2.hashValue );
		}

		static constexpr uint64 makeU64Value( uint32 pHashValue1, uint32 pHashValue2 ) noexcept
		{
			return ( static_cast<uint64>( pHashValue1 ) << 32 ) | pHashValue2;
		}
	};

	inline HFSIdentifier generateHFSIdentifier( const char * pCStr )
	{
		const auto hash1 = hashCompute<EHashAlgo::FNV1A32>( pCStr );
		const auto hash2 = hashCompute<EHashAlgo::SDBM>( pCStr );
		return HFSIdentifier{ hash1, hash2 };
	}

	inline HFSIdentifier generateHFSIdentifier( const wchar_t * pWCStr )
	{
		const auto hash1 = hashCompute<EHashAlgo::FNV1A32>( pWCStr );
		const auto hash2 = hashCompute<EHashAlgo::SDBM>( pWCStr );
		return HFSIdentifier{ hash1, hash2 };
	}

	inline HFSIdentifier generateHFSIdentifier( const std::string & pString )
	{
		const auto hash1 = hashCompute<EHashAlgo::FNV1A32>( pString );
		const auto hash2 = hashCompute<EHashAlgo::SDBM>( pString );
		return HFSIdentifier{ hash1, hash2 };
	}

	inline HFSIdentifier generateHFSIdentifier( const std::wstring & pString )
	{
		const auto hash1 = hashCompute<EHashAlgo::FNV1A32>( pString );
		const auto hash2 = hashCompute<EHashAlgo::SDBM>( pString );
		return HFSIdentifier{ hash1, hash2 };
	}

} // namespace ts3

#endif // __TS3_CORE_HFS_IDENTIFIER_H__
