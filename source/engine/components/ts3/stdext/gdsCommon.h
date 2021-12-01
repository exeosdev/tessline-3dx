
#ifndef __TS3_STDEXT_GDS_COMMON_H__
#define __TS3_STDEXT_GDS_COMMON_H__

#include "arrayView.h"
#include <ts3/platform/gds.h>

namespace ts3
{

	class MemoryBuffer;
	class DynamicMemoryBuffer;

	template <size_t tpSize>
	class FixedMemoryBuffer;

	enum : gds_type_id_t
	{
		E_GDS_TYPE_ID_STDX_ARRAY_VIEW = 0x3F0001,
	};

	namespace gds
	{

		/***********************************************************************************************************/
		/************************************************* ArrayView ***********************************************/
		/***********************************************************************************************************/
		template <typename Tp, typename TpCast = void>
		inline gds_size_t evalByteSize(
				const ArrayView<Tp> & pArrayView,
				const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			gds_size_t byteSize = evalByteSize( emptyTypeID() );
			byteSize += evalByteSize( emptySizeType() );

			for( const auto & entry : pArrayView )
			{
				byteSize += evalByteSize( entry, pCastTag );
			}

			return byteSize;
		}

		template <typename Tp, typename TpCast = void>
		inline gds_size_t queryMinByteSize(
				const ArrayView<Tp> & pArrayView,
				const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			gds_size_t byteSize = queryMinByteSize( emptyTypeID() );
			byteSize += queryMinByteSize( emptySizeType() );
			return byteSize;
		}

		template <typename Tp, typename TpCast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				EByteOrder pByteOrder,
				const ArrayView<Tp> & pArrayView,
				const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			gds_size_t byteSize = serialize( pOutputBuffer, pByteOrder, asTypeID( E_GDS_TYPE_ID_STDX_ARRAY_VIEW ) );
			byteSize += serialize( pOutputBuffer + byteSize, pByteOrder, asSizeType( pArrayView.size ) );

			for( const auto & value : pArrayView )
			{
				byteSize += serialize(  pOutputBuffer + byteSize, pByteOrder, value, pCastTag );
			}

			return byteSize;
		}

		template <typename Tp, typename TpCast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				EByteOrder pByteOrder,
				ArrayView<Tp> & pArrayView,
				const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			gds_type_id_t typeID = E_GDS_TYPE_ID_UNKNOWN;
			gds_size_t byteSize = deserialize( pInputData, pByteOrder, asTypeID( typeID ) );

			size_t arraySize = 0;
			byteSize += deserialize( pInputData + byteSize, pByteOrder, asSizeType( arraySize ) );

			for( auto & value : pArrayView )
			{
				byteSize += deserialize( pInputData + byteSize, pByteOrder, value, pCastTag );
			}

			return byteSize;
		}

	}

}

#endif // __TS3_STDEXT_GDS_COMMON_H__
