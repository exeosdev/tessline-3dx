
#include "scfIODefs.h"
#include <ts3/core/utility/gdsCoreAPI.h>
#include <ts3/core/utility/gdsTypeSupportStdCore.h>

namespace ts3
{

	namespace gds
	{

		gds_size_t serialize( byte * pOutputBuffer, const SCFIOEntryDesc & pValue )
		{
			return serializeAll( pOutputBuffer,
								 pValue.entryType,
								 pValue.name,
								 pValue.uid );
		}

		gds_size_t deserialize( const byte * pInputData, SCFIOEntryDesc & pValue )
		{
			return deserializeAll( pInputData,
								   pValue.entryType,
								   pValue.name,
								   pValue.uid );
		}

		gds_size_t evalByteSize( const SCFIOEntryDesc & pValue )
		{
			return evalByteSizeAll( pValue.entryType,
									pValue.name,
									pValue.uid );
		}

		gds_size_t serialize( byte * pOutputBuffer, const SCFIOResourceDesc & pValue )
		{
			return serializeAll( pOutputBuffer,
                                 static_cast<const SCFIOEntryDesc &>( pValue ),
								 pValue.dataOffset,
								 pValue.dataSize );
		}

		gds_size_t deserialize( const byte * pInputData, SCFIOResourceDesc & pValue )
		{
			return deserializeAll( pInputData,
                                   static_cast<SCFIOEntryDesc &>( pValue ),
								   pValue.dataOffset,
								   pValue.dataSize );
		}

		gds_size_t evalByteSize( const SCFIOResourceDesc & pValue )
		{
			return evalByteSizeAll( static_cast<const SCFIOEntryDesc &>( pValue ),
									pValue.dataOffset,
									pValue.dataSize );
		}

		gds_size_t serialize( byte * pOutputBuffer, const SCFIOVirtualFolderDesc & pValue )
		{
			return serializeAll( pOutputBuffer,
                                 static_cast<const SCFIOEntryDesc &>( pValue ),
								 pValue.resourcesNum,
								 pValue.subFoldersNum );
		}

		gds_size_t deserialize( const byte * pInputData, SCFIOVirtualFolderDesc & pValue )
		{
			return deserializeAll( pInputData,
                                   static_cast<SCFIOEntryDesc &>( pValue ),
								   pValue.resourcesNum,
								   pValue.subFoldersNum );
		}

		gds_size_t evalByteSize( const SCFIOVirtualFolderDesc & pValue )
		{
			return evalByteSizeAll( static_cast<const SCFIOEntryDesc &>( pValue ),
									pValue.resourcesNum,
									pValue.subFoldersNum );
		}

	}

}
