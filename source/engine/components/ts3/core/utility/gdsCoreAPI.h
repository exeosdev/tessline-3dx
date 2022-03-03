
#ifndef __TS3_CORE_GDS_CORE_API_H__
#define __TS3_CORE_GDS_CORE_API_H__

#include "../prerequisites.h"
#include <ts3/platform/gds.h>

#include <functional>

namespace std
{

    template <typename _T, typename _Alloc>
    class vector;

}

namespace ts3
{

    class ByteBuffer;

    namespace gds
    {

        using DataReadCallback = std::function<uint64( void *, uint64 )>;
        using DataWriteCallback = std::function<uint64( const void *, uint64 )>;

        template <typename Tp>
        inline gds_size_t serialize( ByteBuffer & pOutputBuffer, const Tp & pValue )
        {
            const auto totalByteSize = evalByteSize( pValue );
            if( totalByteSize > 0 )
            {
                pOutputBuffer.resize( trunc_numeric_cast<size_t>( totalByteSize ) );
                serialize( pOutputBuffer.dataPtr(), pValue );
            }
            return totalByteSize;
        }

        template <typename Tp>
        inline gds_size_t serialize( std::vector<byte> & pOutputBuffer, const Tp & pValue )
        {
            const auto totalByteSize = evalByteSize( pValue );
            if( totalByteSize > 0 )
            {
                pOutputBuffer.resize( trunc_numeric_cast<size_t>( totalByteSize ) );
                serialize( pOutputBuffer.data(), pValue );
            }
            return totalByteSize;
        }

        template <typename Tp>
        inline gds_size_t serializeWithMetaData( ByteBuffer & pOutputBuffer, const Tp & pValue )
        {
            const auto totalByteSize = evalByteSizeWithMetaData( pValue );
            if( totalByteSize > 0 )
            {
                pOutputBuffer.resize( trunc_numeric_cast<size_t>( totalByteSize ) );
                serializeWithMetaData( pOutputBuffer.dataPtr(), pValue );
            }
            return totalByteSize;
        }

        template <typename Tp>
        inline gds_size_t serializeWithMetaData( std::vector<byte> & pOutputBuffer, const Tp & pValue )
        {
            const auto totalByteSize = evalByteSizeWithMetaData( pValue );
            if( totalByteSize > 0 )
            {
                pOutputBuffer.resize( trunc_numeric_cast<size_t>( totalByteSize ) );
                serializeWithMetaData( pOutputBuffer.data(), pValue );
            }
            return totalByteSize;
        }

        template <typename Tp>
        inline gds_size_t serializeExternal( ByteBuffer & pOutputBuffer, const Tp & pValue, const DataWriteCallback & pWriteCallback )
        {
            const auto writeSize = serializeWithMetaData( pOutputBuffer, pValue );
            if( writeSize > 0 )
            {
                pWriteCallback( pOutputBuffer.dataPtr(), writeSize );
            }
            return writeSize;
        }

        template <typename Tp>
        inline gds_size_t serializeExternal( std::vector<byte> & pOutputBuffer, const Tp & pValue, const DataWriteCallback & pWriteCallback )
        {
            const auto writeSize = serializeWithMetaData( pOutputBuffer, pValue );
            if( writeSize > 0 )
            {
                pWriteCallback( pOutputBuffer.data(), writeSize );
            }
            return writeSize;
        }

        InstanceMetaData readInstanceMetaDataExternal( const DataReadCallback & pReadCallback, ByteBuffer & pReadBuffer );

        template <typename Tp>
        inline gds_size_t deserializeExternal( Tp & pValue, const DataReadCallback & pReadCallback )
        {
            ByteBuffer localReadBuffer{};
            return deserializeExternal( pValue, pReadCallback, localReadBuffer );
        }

        template <typename Tp>
        inline gds_size_t deserializeExternal( Tp & pValue, const DataReadCallback & pReadCallback, ByteBuffer & pReadBuffer )
        {
            const auto metaData = readInstanceMetaDataExternal( pReadCallback, pReadBuffer );
            if( !metaData )
            {
                return 0;
            }

            const auto deserializedSize = deserializeWithMetaData( pReadBuffer.dataPtr(), pValue );
            if( deserializedSize != metaData.outputBlockSize )
            {
                return 0;
            }

            return deserializedSize;
        }

    }

}

#endif // __TS3_CORE_GDS_CORE_API_H__
