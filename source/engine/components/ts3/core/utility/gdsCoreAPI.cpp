
#include "gdsCoreAPI.h"
#include <ts3/stdext/byteBuffer.h>

namespace ts3
{
    namespace gds
    {

        InstanceMetaData readInstanceMetaDataExternal( const DataReadCallback & pReadCallback, ByteBuffer & pReadBuffer )
        {
            const auto metaDataSize = getInstanceMetaDataSize();
            pReadBuffer.resize( metaDataSize );

            const auto metaDataReadSize = pReadCallback( pReadBuffer.dataPtr(), metaDataSize );
            if( metaDataReadSize != metaDataSize )
            {
                return {};
            }

            const auto metaData = readInstanceMetaData( pReadBuffer.dataPtr() );
            if( !metaData )
            {
                return {};
            }

            pReadBuffer.resize( metaData.outputBlockSize );

            const auto objectDataReadSize = pReadCallback( pReadBuffer.dataPtr() + metaDataSize, metaData.objectDataSize );
            if( objectDataReadSize != metaData.objectDataSize )
            {
                return {};
            }

            return metaData;
        }

    }
}
