
#include "hash.h"
#include "staticLimits.h"
#include <zlib/zlib.h>

namespace ts3
{

	uint32 HashTraits<EHashAlgo::Adler32>::compute( const void * pInput, size_t pInputSize )
	{
		return update( sHashInitValue, pInput, pInputSize );
	}

	uint32 HashTraits<EHashAlgo::Adler32>::update( uint32 pHash, const void * pInput, size_t pInputSize )
	{
		uint32 result = pHash;
		if( pInputSize > 0 )
		{
			const byte * inputBytes = reinterpret_cast<const byte *>( pInput );
			auto tmpRes = adler32( pHash, inputBytes, static_cast<uInt>( pInputSize ) );
			result = static_cast<uint32>( tmpRes );
		}
		return result;
	}


	uint32 HashTraits<EHashAlgo::CRC32>::compute( const void * pInput, size_t pInputSize )
	{
		return update( sHashInitValue, pInput, pInputSize );
	}

	uint32 HashTraits<EHashAlgo::CRC32>::update( uint32 pHash, const void * pInput, size_t pInputSize )
	{
		uint32 result = pHash;
		if( pInputSize > 0 )
		{
			const byte * inputBytes = reinterpret_cast<const byte *>( pInput );
		#if( EBS_EIS_SUPPORT_LEVEL & ECL_EIS_FEATURE_SSE42 )
			for( ; pInputSize >= 4; inputBytes += 4, pInputSize -= 4 )
			{
				auto tmpres = _mm_crc32_u32( result, *( reinterpret_cast<const uint32 *>( inputBytes ) ) );
				result = static_cast<uint32>( tmpres );
			}
		#endif
			auto tmpres = crc32( pHash, inputBytes, static_cast<uInt>( pInputSize ) );
			result = static_cast<uint32>( tmpres );
		}
		return result;
	}


	uint32 HashTraits<EHashAlgo::DJB2>::compute( const void * pInput, size_t pInputSize )
	{
		return update( sHashInitValue, pInput, pInputSize );
	}

	uint32 HashTraits<EHashAlgo::DJB2>::update( uint32 pHash, const void * pInput, size_t pInputSize )
	{
		uint32 result = pHash;
		if( pInputSize > 0 )
		{
			const byte * inputBytes = reinterpret_cast<const byte *>( pInput );
			for( size_t byteIndex = 0; byteIndex < pInputSize; ++byteIndex )
			{
				result = ( ( result << 5 ) + result ) ^ ( inputBytes[byteIndex] );
			}
		}
		return result;
	}


	constexpr uint32 sFNV32PrimeValue = 0x01000193;

	uint32 HashTraits<EHashAlgo::FNV1A32>::compute( const void * pInput, size_t pInputSize )
	{
		return update( sHashInitValue, pInput, pInputSize );
	}

	uint32 HashTraits<EHashAlgo::FNV1A32>::update( uint32 pHash, const void * pInput, size_t pInputSize )
	{
		uint32 result = pHash;
		if( pInputSize > 0 )
		{
			const byte * inputBytes = reinterpret_cast<const byte *>( pInput );
			for( size_t byteIndex = 0; byteIndex < pInputSize; ++byteIndex )
			{
				result = result ^ static_cast<uint32>( inputBytes[byteIndex] );
				result = result * sFNV32PrimeValue;
			}
		}
		return result;
	}


	constexpr uint64 sFNV64PrimeValue = 0x100000001B3;
	constexpr uint64 sU64Mask = Limits<uint64>::maxValue << 8;

	uint64 HashTraits<EHashAlgo::FNV1A64>::compute( const void * pInput, size_t pInputSize )
	{
		return update( sHashInitValue, pInput, pInputSize );
	}

	uint64 HashTraits<EHashAlgo::FNV1A64>::update( uint64 pHash, const void * pInput, size_t pInputSize )
	{
		uint64 result = pHash;
		if( pInputSize > 0 )
		{
			const byte * inputBytes = reinterpret_cast<const byte *>( pInput );
			for( size_t byteIndex = 0; byteIndex < pInputSize; ++byteIndex )
			{
				result = result ^ static_cast<uint64>( inputBytes[byteIndex] );
				result = result * sFNV64PrimeValue;
			}
		}
		return result;
	}


	uint32 HashTraits<EHashAlgo::SDBM>::compute( const void * pInput, size_t pInputSize )
	{
		return update( sHashInitValue, pInput, pInputSize );
	}

	uint32 HashTraits<EHashAlgo::SDBM>::update( uint32 pHash, const void * pInput, size_t pInputSize )
	{
		uint32 result = pHash;
		if( pInputSize > 0 )
		{
			const byte * inputBytes = reinterpret_cast<const byte *>( pInput );
			for( size_t byteIndex = 0; byteIndex < pInputSize; ++byteIndex )
			{
				result = ( inputBytes[byteIndex] ) + ( result << 6 ) + ( result << 16 ) - result;
			}
		}
		return result;
	}

}
