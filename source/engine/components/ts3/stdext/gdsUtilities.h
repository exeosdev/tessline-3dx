
#ifndef __TS3_STDEXT_GDS_UTILITIES_H__
#define __TS3_STDEXT_GDS_UTILITIES_H__

#include "memoryBuffer.h"
#include <ts3/platform/gds.h>
#include <vector>

namespace ts3
{

	namespace gds
	{

		template <typename Tp>
		inline gds_size_t serializeAuto( std::vector<byte> & pOutputBuffer, const Tp & pValue )
		{
			const auto totalByteSize = evalByteSizeWithMetaInfo( pValue );

			if( totalByteSize > 0 )
			{
				pOutputBuffer.resize( trunc_numeric_cast<size_t>( totalByteSize ) );

				serializeWithMetaInfo( pOutputBuffer.data(), pValue );
			}

			return totalByteSize;
		}

		template <typename Tp>
		inline gds_size_t deserializeAuto( const byte * pInputData, Tp & pValue )
		{
			const auto metaInfo = readValueMetaInfo( pInputData );

			if( !metaInfo || ( metaInfo.dataSize == 0 ) )
			{
				throw 0;
			}

			const auto byteSize = deserializeWithMetaInfo( pInputData, pValue );

			return byteSize;
		}

		template <typename Tp>
		inline gds_size_t evalByteSizeAuto( const Tp & pValue )
		{
			return evalByteSizeWithMetaInfo( pValue );;
		}

	}

}

#endif // __TS3_STDEXT_GDS_UTILITIES_H__
