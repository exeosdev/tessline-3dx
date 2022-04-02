
#ifndef __TS3_CORE_GDS_CORE_H__
#define __TS3_CORE_GDS_CORE_H__

#include "gdsCommon.h"
#include "../exception.h"

#include <ts3/platform/gds.h>
#include <ts3/stdext/byteArray.h>

#include <array>
#include <functional>
#include <vector>

namespace ts3
{

	template <typename TpIntegral>
	class AtomicBitmask;

	template <typename TpIntegral>
	class Bitmask;

	template <typename Tp, typename TpCompare, typename TpAlloc>
	class SortedArray;

	namespace gdscore
	{
		
		/// @brief Typedef for a callable compatible with ReadCallback requirements for GDS deserializeExternal() functions.
		using DataReadCallback = std::function<uint64 /* pReadBytes */( void * /* pTargetBuffer */, uint64 /* pReadSize */ )>;

		/// @brief Typedef for a callable compatible with WriteCallback requirements for GDS serializeExternal() functions.
		using DataWriteCallback = std::function<uint64 /* pWrittenBytes */( const void * /* pDataToWrite */, uint64 /* pWriteSize */ )>;

		/***********************************************************************************************************/
		/**************************************** Core API - evalByteSize() ****************************************/
		/***********************************************************************************************************/

		template <typename Tp, std::enable_if_t<IsGdsSerializable<Tp>::sValue, int>>
		gds_size_t evalByteSize( const Tp & pValue );

		template <typename Tp>
		gds_size_t evalByteSize( const Tp & pValue );

		template <typename TpRef, typename TpInternal>
		gds_size_t evalByteSize( const gds::ValueRef<TpRef, TpInternal> & pValueRef );

		template <typename Tp, typename TpInternal, std::enable_if_t<std::is_void<TpInternal>::value, int>>
		gds_size_t evalByteSize( const Tp & pValue, const gds::TypeCastTag<TpInternal> & );

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
		gds_size_t evalByteSize( const Tp & pValue, const gds::TypeCastTag<TpInternal> & );

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
		gds_size_t evalByteSize( const gds::TypeCastInfo<Tp, TpInternal> & pCastInfo );

		/***********************************************************************************************************/
		/***************************************** Core API - serialize() ******************************************/
		/***********************************************************************************************************/

		template <typename Tp, std::enable_if_t<IsGdsSerializable<Tp>::sValue, int>>
		gds_size_t serialize( byte * pOutputBuffer, const Tp & pValue );

		template <typename Tp>
		gds_size_t serialize( byte * pOutputBuffer, const Tp & pValue );

		template <typename TpRef, typename TpInternal>
		gds_size_t serialize( byte * pOutputBuffer, const gds::ValueRef<TpRef, TpInternal> & pValueRef );

		template <typename Tp, typename TpInternal, std::enable_if_t<std::is_void<TpInternal>::value, int>>
		gds_size_t serialize( byte * pOutputBuffer, const Tp & pValue, const gds::TypeCastTag<TpInternal> & );

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
		gds_size_t serialize( byte * pOutputBuffer, const Tp & pValue, const gds::TypeCastTag<TpInternal> & );

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
		gds_size_t serialize( byte * pOutputBuffer, const gds::TypeCastInfo<Tp, TpInternal> & pCastInfo );

		/***********************************************************************************************************/
		/**************************************** Core API - deserialize() *****************************************/
		/***********************************************************************************************************/

		template <typename Tp, std::enable_if_t<IsGdsSerializable<Tp>::sValue, int>>
		gds_size_t deserialize( const byte * pInputData, Tp & pValue );

		template <typename Tp>
		gds_size_t deserialize( const byte * pInputData, Tp & pValue );

		template <typename TpRef, typename TpInternal>
		gds_size_t deserialize( const byte * pInputData, const gds::ValueRef<TpRef, TpInternal> & pValueRef );

		template <typename Tp, typename TpInternal, std::enable_if_t<std::is_void<TpInternal>::value, int>>
		gds_size_t deserialize( const byte * pInputData, Tp & pRef, const gds::TypeCastTag<TpInternal> & );

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
		gds_size_t deserialize( const byte * pInputData, Tp & pRef, const gds::TypeCastTag<TpInternal> & );

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
		gds_size_t deserialize( const byte * pInputData, const gds::TypeCastInfo<Tp, TpInternal> & pCastInfo );

		/***********************************************************************************************************/
		/******************************************** Core API - MetaData ******************************************/
		/***********************************************************************************************************/

		/// @brief Returns an evaluated byte size of the specified object which will be serialized using metadata.
		template <typename Tp>
		gds_size_t evalByteSizeWithMetaData( const Tp & pValue );

		/// @brief Serializes given object with additional metadata.
		template <typename Tp>
		gds_size_t serializeWithMetaData( byte * pOutputBuffer, const Tp & pValue );

		/// @brief Performs deserialization of the specified data, written with serializeWithMetaData().
		template <typename Tp>
		gds_size_t deserializeWithMetaData( const byte * pInputData, Tp & pValue );

		/***********************************************************************************************************/
		/******************************************** Core API - Variadic ******************************************/
		/***********************************************************************************************************/

		template <typename Tp>
		gds_size_t evalByteSizeAll( const Tp & pValue );

		template <typename Tp, typename... TpRest>
		gds_size_t evalByteSizeAll( const Tp & pValue, TpRest && ...pRest );

		template <typename Tp>
		gds_size_t serializeAll( byte * pOutputBuffer, const Tp & pValue );

		template <typename Tp, typename... TpRest>
		gds_size_t serializeAll( byte * pOutputBuffer, const Tp & pValue, TpRest && ...pRest );

		template <typename Tp>
		gds_size_t deserializeAll( const byte * pInputData, Tp & pValue );

		template <typename Tp, typename... TpRest>
		gds_size_t deserializeAll( const byte * pInputData, Tp & pValue, TpRest && ...pRest );

		/***********************************************************************************************************/
		/********************************************** Core API - Auto ********************************************/
		/***********************************************************************************************************/

		template <typename TpValue>
		gds_size_t serializeAuto( const ReadWriteMemoryView & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue, size_t tpSize>
		gds_size_t serializeAuto( std::array<byte, tpSize> & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue>
		gds_size_t serializeAutoWithMetaData( const ReadWriteMemoryView & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue, size_t tpSize>
		gds_size_t serializeAutoWithMetaData( std::array<byte, tpSize> & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue>
		gds_size_t serializeAuto( DynamicByteArray & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue>
		gds_size_t serializeAuto( DynamicMemoryBuffer & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue>
		gds_size_t serializeAuto( std::vector<byte> & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue>
		gds_size_t serializeAutoWithMetaData( DynamicByteArray & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue>
		gds_size_t serializeAutoWithMetaData( DynamicMemoryBuffer & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue>
		gds_size_t serializeAutoWithMetaData( std::vector<byte> & pOutputBuffer, const TpValue & pValue );

		template <typename TpBuffer, typename TpValue>
		gds_size_t deserializeAuto( const TpBuffer & pInputData, TpValue & pValue );

		template <typename TpBuffer, typename TpValue>
		gds_size_t deserializeAutoWithMetaData( const TpBuffer & pInputData, TpValue & pValue );

		/***********************************************************************************************************/
		/******************************************** Core API - External ******************************************/
		/***********************************************************************************************************/

		// External serialization/deserialization support.
		//
		// This functionality is probably the most commonly used in the real-life scenarios (including some engine parts).
		// Most of the time, serialized data ends up in some fom of an external storage (a file, a network, etc).
		// For big portions of data, loading everything into the memory for deserialization (or serializing everything
		// into a single buffer) may not be an option. Loading chunks creates a problem, though - we can end up having
		// half of the object's data in the buffer (we cannot know how much data to load).
		//
		// MetaData solves this problem by adding fixed-size information before the data itself. External API further
		// enables serialization/deserialization directly to/from an external storage.
		// Each of these functions accepts a write (serialize()) or a read (deserialize()) callback for interaction
		// with an external medium.

		// Serialization is not really a problem. We simply serialize the specified object into a temporary
		// buffer (optionally specified by the user) and write the data to the output using the write callback.

		/// @brief Serializes the specified object and writes the byte representation using the specified callback.
		/// @param pValue The object to be serialized
		/// @param pWriteCallback The write callback for data writing. Cannot be empty.
		/// @param pGdsCache Custom cache to which the data is serialized before writing.
		template <typename TpBuffer, typename TpValue>
		gds_size_t serializeExternal( const TpValue & pValue, const DataWriteCallback & pWriteCallback, TpBuffer & pGdsCache );

		/// @brief Serializes the specified object and writes the byte representation using the specified callback.
		/// @param pValue The object to be serialized
		/// @param pWriteCallback The write callback for data writing. Cannot be empty.
		template <typename TpValue>
		gds_size_t serializeExternal( const TpValue & pValue, const DataWriteCallback & pWriteCallback );

		// Deserialization is more problematic, because the data is usually dynamically-sized (containers, strings, ranges).
		// For the best flexibility, we assume, that the data source is a one-way stream (we can read bytes in order
		// and cannot rewind back). This assumption allows us to support all kind of streams directly.
		//
		// To achieve this, we use the MetaData feature (this is why serializeExternal() always does the process using
		// serializeAutoWithMetaData() internally) and obtain the info about how much data we need to fetch from the stream.
		//
		// The data must be obviously loaded into some kind of buffer first, so just like in case of serializeAuto(),
		// we use proxy class to enable usage of different types of buffers as this temporary storage.

		/// @brief Deserializes object of the specified type and stores it in pValue. Binary data is read using the specified read callback.
		/// @param pValue The object to store the deserialized state.
		/// @param pReadCallback The read callback for data reading. Cannot be empty.
		/// @param pGdsCache Custom cache that will be used as a read buffer.
		template <typename TpValue>
		gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, const ReadWriteMemoryView & pGdsCache );

		template <typename TpValue, size_t tpSize>
		gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, std::array<byte, tpSize> & pGdsCache );

		template <typename TpValue>
		gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, DynamicByteArray & pGdsCache );

		template <typename TpValue>
		gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, DynamicMemoryBuffer & pGdsCache );

		template <typename TpValue>
		gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, std::vector<byte> & pGdsCache );

		/// @brief Deserializes object of the specified type and stores it in pValue. Binary data is read using the specified read callback.
		/// @param pValue The object to store the deserialized state.
		/// @param pReadCallback The read callback for data reading. Cannot be empty.
		template <typename TpValue>
		gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback );

		/***********************************************************************************************************/
		/******************************************* Container/Map Helpers *****************************************/
		/***********************************************************************************************************/

		struct StdListInsert
		{
			template <typename L, typename E>
			void operator()( L & pList, size_t /* pIndex */, E pElement ) const
			{
				pList.push_back( std::forward<E>( pElement ) );
			}
		};

		struct StdDequeInsert
		{
			template <typename D, typename E>
			void operator()( D & pDeque, size_t /* pIndex */, E pElement ) const
			{
				pDeque.push_back( std::forward<E>( pElement ) );
			}
		};

		struct StdQueueInsert
		{
			template <typename Q, typename E>
			void operator()( Q & pQueue, size_t /* pIndex */, E pElement ) const
			{
				pQueue.push( std::forward<E>( pElement ) );
			}
		};

		struct StdStackInsert
		{
			template <typename S, typename E>
			void operator()( S & pStack, size_t /* pIndex */, E pElement ) const
			{
				pStack.push( std::forward<E>( pElement ) );
			}
		};

		struct StdSetInsert
		{
			template <typename S, typename E>
			void operator()( S & pSet, size_t /* pIndex */, E pElement ) const
			{
				pSet.insert( std::forward<E>( pElement ) );
			}
		};

		struct StdUnorderedSetInsert
		{
			template <typename UOS, typename E>
			void operator()( UOS & pUnorderedSet, size_t /* pIndex */, E pElement ) const
			{
				pUnorderedSet.insert( std::forward<E>( pElement ) );
			}
		};

		template <template <typename...> typename TpC, typename... TpCArgs>
		gds_size_t evalByteSizeContainer( const TpC<TpCArgs...> & pContainer );

		template <template <typename...> typename TpC, typename... TpCArgs>
		gds_size_t serializeContainer( byte * pOutputBuffer, const TpC<TpCArgs...> & pContainer );

		template <template <typename...> typename TpC, typename... TpCArgs, typename TpIns>
		gds_size_t deserializeContainer( const byte * pInputData, TpC<TpCArgs...> & pContainer, const TpIns & pInsertCallback );

		template <template <typename...> typename TpC, typename... TpCArgs>
		gds_size_t evalMapByteSize( const TpC<TpCArgs...> & pMap );

		template <template <typename...> typename TpM, typename TpK, typename TpV, typename... TpCArgs>
		gds_size_t serializeMap( byte * pOutputBuffer, const TpM<TpK, TpV, TpCArgs...> & pMap );

		template <template <typename...> typename TpM, typename TpK, typename TpV, typename... TpCArgs>
		gds_size_t deserializeMap( const byte * pInputData, TpM<TpK, TpV, TpCArgs...> & pMap );

		/***********************************************************************************************************/
		/************************************************* ArrayView ***********************************************/
		/***********************************************************************************************************/

		template <typename Tp>
		gds_size_t evalByteSize( const ArrayView<Tp> & pArrayView );

		template <typename Tp>
		gds_size_t serialize( byte * pOutputBuffer, const ArrayView<Tp> & pArrayView );

		template <typename Tp>
		gds_size_t deserialize( const byte * pInputData, ArrayView<Tp> & pArrayView );

		/***********************************************************************************************************/
		/*********************************************** AtomicBitmask *********************************************/
		/***********************************************************************************************************/

		template <typename TpIntegral>
		gds_size_t evalByteSize( const AtomicBitmask<TpIntegral> & pBitmask );

		template <typename TpIntegral>
		gds_size_t serialize( byte * pOutputBuffer, const AtomicBitmask<TpIntegral> & pBitmask );

		template <typename TpIntegral>
		gds_size_t deserialize( const byte * pInputData, AtomicBitmask<TpIntegral> & pBitmask );

		/***********************************************************************************************************/
		/************************************************** Bitmask ************************************************/
		/***********************************************************************************************************/

		template <typename TpIntegral>
		gds_size_t evalByteSize( const Bitmask<TpIntegral> & pBitmask );

		template <typename TpIntegral>
		gds_size_t serialize( byte * pOutputBuffer, const Bitmask<TpIntegral> & pBitmask );

		template <typename TpIntegral>
		gds_size_t deserialize( const byte * pInputData, Bitmask<TpIntegral> & pBitmask );

		/***********************************************************************************************************/
		/************************************************ SortedArray **********************************************/
		/***********************************************************************************************************/

		template <typename Tp, typename TpCompare, typename TpAlloc>
		gds_size_t evalByteSize( const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray );

		template <typename Tp, typename TpCompare, typename TpAlloc>
		gds_size_t serialize( byte * pOutputBuffer, const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray );

		template <typename Tp, typename TpCompare, typename TpAlloc>
		gds_size_t deserialize( const byte * pInputData, const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray );

		/***********************************************************************************************************/
		/******************************************** std::basic_string ********************************************/
		/***********************************************************************************************************/

		template <typename TpCh, typename TpTr, typename TpAlloc>
		gds_size_t evalByteSize( const std::basic_string<TpCh, TpTr, TpAlloc> & pString );
		
		template <typename TpCh, typename TpTr, typename TpAlloc>
		gds_size_t serialize( byte * pOutputBuffer, const std::basic_string<TpCh, TpTr, TpAlloc> & pString );
		
		template <typename TpCh, typename TpTr, typename TpAlloc>
		gds_size_t deserialize( const byte * pInputData, std::basic_string<TpCh, TpTr, TpAlloc> & pString );

		/***********************************************************************************************************/
		/************************************************ std::pair ************************************************/
		/***********************************************************************************************************/
		
		template <typename Tp1, typename Tp2>
		gds_size_t evalByteSize( const std::pair<Tp1, Tp2> & pPair );

		template <typename Tp1, typename Tp2>
		gds_size_t serialize( byte * pOutputBuffer, const std::pair<Tp1, Tp2> & pPair );

		template <typename Tp1, typename Tp2>
		gds_size_t deserialize( const byte * pInputData, std::pair<Tp1, Tp2> & pPair );

		/***********************************************************************************************************/
		/************************************************ std::array ***********************************************/
		/***********************************************************************************************************/

		template <typename TpValue, size_t tpSize>
		gds_size_t evalByteSize( const std::array<TpValue, tpSize> & pArray );

		template <typename TpValue, size_t tpSize>
		gds_size_t serialize( byte * pOutputBuffer, const std::array<TpValue, tpSize> & pArray );

		template <typename TpValue, size_t tpSize>
		gds_size_t deserialize( const byte * pInputData, std::array<TpValue, tpSize> & pArray );

		/***********************************************************************************************************/
		/*********************************************** std::vector ***********************************************/
		/***********************************************************************************************************/

		template <typename TpValue, typename TpAlloc>
		gds_size_t evalByteSize( const std::vector<TpValue, TpAlloc> & pVector );

		template <typename TpValue, typename TpAlloc>
		gds_size_t serialize( byte * pOutputBuffer, const std::vector<TpValue, TpAlloc> & pVector );

		template <typename TpValue, typename TpAlloc>
		gds_size_t deserialize( const byte * pInputData, std::vector<TpValue, TpAlloc> & pVector );

		namespace internal
		{

			template <typename TpFxBuffer, typename TpValue>
			gds_size_t serializeAutoFixed( TpFxBuffer & pOutputBuffer, const TpValue & pValue );

			template <typename TpFxBuffer, typename TpValue>
			gds_size_t serializeAutoWithMetaDataFixed( TpFxBuffer & pOutputBuffer, const TpValue & pValue );

			template <typename TpRsBuffer, typename TpValue>
			gds_size_t serializeAutoResizable( TpRsBuffer & pOutputBuffer, const TpValue & pValue );

			template <typename TpRsBuffer, typename TpValue>
			gds_size_t serializeAutoWithMetaDataResizable( TpRsBuffer & pOutputBuffer, const TpValue & pValue );

			template <typename TpValue, typename TpFxBuffer>
			gds_size_t deserializeExternalFixed( TpValue & pValue, const DataReadCallback & pReadCallback, TpFxBuffer & pGdsCache );

			template <typename TpValue, typename TpRsBuffer>
			gds_size_t deserializeExternalResizable( TpValue & pValue, const DataReadCallback & pReadCallback, TpRsBuffer & pGdsCache );

			template <typename TpFxBuffer>
			bool readDataExternalFixed( const DataReadCallback & pReadCallback, TpFxBuffer & pReadBuffer );

			template <typename TpRsBuffer>
			bool readDataExternalResizable( const DataReadCallback & pReadCallback, TpRsBuffer & pReadBuffer );

		}

	} // namespace gdscore

	/***********************************************************************************************************/
	/**************************************** Core API - evalByteSize() ****************************************/
	/***********************************************************************************************************/

	template <typename Tp, std::enable_if_t<IsGdsSerializable<Tp>::sValue, int>>
	inline gds_size_t gdscore::evalByteSize( const Tp & pValue )
	{
		return pValue.evalByteSize();
	}

	template <typename Tp>
	inline gds_size_t gdscore::evalByteSize( const Tp & pValue )
	{
		return gds::evalByteSize( pValue );
	}

	template <typename TpRef, typename TpInternal>
	inline gds_size_t gdscore::evalByteSize( const gds::ValueRef<TpRef, TpInternal> & pValueRef )
	{
		return evalByteSize( pValueRef.get() );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<std::is_void<TpInternal>::value, int>>
	inline gds_size_t gdscore::evalByteSize( const Tp & pValue, const gds::TypeCastTag<TpInternal> & )
	{
		return evalByteSize( pValue );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
	inline gds_size_t gdscore::evalByteSize( const Tp & pValue, const gds::TypeCastTag<TpInternal> & )
	{
		return evalByteSize( gds::ValueRef<const Tp, TpInternal>{ pValue } );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
	inline gds_size_t gdscore::evalByteSize( const gds::TypeCastInfo<Tp, TpInternal> & pCastInfo )
	{
		return evalByteSize( pCastInfo.refWrapper.get(), pCastInfo.castTag );
	}

	/***********************************************************************************************************/
	/***************************************** Core API - serialize() ******************************************/
	/***********************************************************************************************************/

	template <typename Tp, std::enable_if_t<IsGdsSerializable<Tp>::sValue, int>>
	inline gds_size_t gdscore::serialize( byte * pOutputBuffer, const Tp & pValue )
	{
		return pValue.serialize( pOutputBuffer );
	}

	template <typename Tp>
	inline gds_size_t gdscore::serialize( byte * pOutputBuffer, const Tp & pValue )
	{
		return gds::serialize( pOutputBuffer, pValue );
	}

	template <typename TpRef, typename TpInternal>
	inline gds_size_t gdscore::serialize( byte * pOutputBuffer, const gds::ValueRef<TpRef, TpInternal> & pValueRef )
	{
		return serialize( pOutputBuffer, pValueRef.get() );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<std::is_void<TpInternal>::value, int>>
	inline gds_size_t gdscore::serialize( byte * pOutputBuffer, const Tp & pValue, const gds::TypeCastTag<TpInternal> & )
	{
		return serialize( pOutputBuffer, pValue );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
	inline gds_size_t gdscore::serialize( byte * pOutputBuffer, const Tp & pValue, const gds::TypeCastTag<TpInternal> & )
	{
		return serialize( pOutputBuffer, gds::ValueRef<Tp, TpInternal>{ pValue } );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
	inline gds_size_t gdscore::serialize( byte * pOutputBuffer, const gds::TypeCastInfo<Tp, TpInternal> & pCastInfo )
	{
		return serialize( pOutputBuffer, pCastInfo.refWrapper.get(), pCastInfo.castTag );
	}

	/***********************************************************************************************************/
	/**************************************** Core API - deserialize() *****************************************/
	/***********************************************************************************************************/

	template <typename Tp, std::enable_if_t<IsGdsSerializable<Tp>::sValue, int>>
	inline gds_size_t gdscore::deserialize( const byte * pInputData, Tp & pValue )
	{
		return pValue.deserialize( pInputData );
	}

	template <typename Tp>
	inline gds_size_t gdscore::deserialize( const byte * pInputData, Tp & pValue )
	{
		return gds::deserialize( pInputData, pValue );
	}

	template <typename TpRef, typename TpInternal>
	inline gds_size_t gdscore::deserialize( const byte * pInputData, const gds::ValueRef<TpRef, TpInternal> & pValueRef )
	{
		return deserialize( pInputData, pValueRef.get() );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<std::is_void<TpInternal>::value, int>>
	inline gds_size_t gdscore::deserialize( const byte * pInputData, Tp & pRef, const gds::TypeCastTag<TpInternal> & )
	{
		return deserialize( pInputData, pRef );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
	inline gds_size_t gdscore::deserialize( const byte * pInputData, Tp & pRef, const gds::TypeCastTag<TpInternal> & )
	{
		return deserialize( pInputData, gds::ValueRef<Tp, TpInternal>{ pRef } );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
	inline gds_size_t gdscore::deserialize( const byte * pInputData, const gds::TypeCastInfo<Tp, TpInternal> & pCastInfo )
	{
		return deserialize( pInputData, pCastInfo.refWrapper.get(), pCastInfo.castTag );
	}

	/***********************************************************************************************************/
	/******************************************** Core API - MetaData ******************************************/
	/***********************************************************************************************************/

	/// @brief Returns an evaluated byte size of the specified object which will be serialized using metadata.
	template <typename Tp>
	inline gds_size_t gdscore::evalByteSizeWithMetaData( const Tp & pValue )
	{
		const auto metaDataSize = gds::getInstanceMetaDataSize();
		const auto valueByteSize = evalByteSize( pValue );

		// Technically, user-defined evalByteSize() can be defined to return 0.
		// We always skip empty entries, so in case of metadata let's do the same.
		// If the object itself has no data, stick to that and report 0 size.
		return ( valueByteSize > 0 ) ? ( metaDataSize + valueByteSize ) : 0u;
	}

	/// @brief Serializes given object with additional metadata.
	template <typename Tp>
	inline gds_size_t gdscore::serializeWithMetaData( byte * pOutputBuffer, const Tp & pValue )
	{
		// Size of the metadata, in bytes.
		const auto metaDataSize = gds::getInstanceMetaDataSize();

		// The actual serialization. Skip 'metaDataSize' bytes needed for metadata
		// and write after that. serialize() returns the number of bytes written.
		const auto valueByteSize = serialize( pOutputBuffer + metaDataSize, pValue );

		// MetaData for this instance.
		gds::InstanceMetaData metaData;
		metaData.controlKey = gds::CX_GDS_VALUE_META_DATA_CONTROL_KEY;
		metaData.objectDataSize = valueByteSize; // Size of the serialized data of the OBJECT only.
		metaData.outputBlockSize = metaDataSize + valueByteSize; // Size of the whole data block (including MetaData).

		// Now, serialize the metadata itself.
		gds::serialize( pOutputBuffer, metaData );

		return metaData.outputBlockSize;
	}

	/// @brief Performs deserialization of the specified data, written with serializeWithMetaData().
	template <typename Tp>
	inline gds_size_t gdscore::deserializeWithMetaData( const byte * pInputData, Tp & pValue )
	{
		// The metadata object to which we will deserialize.
		gds::InstanceMetaData metaData;

		// Deserialize the metadata first. It is always at the beginning of the data block.
		const auto metaDataSize = gds::deserialize( pInputData, metaData );

		// deserialize() returns the number of bytes read from the buffer.
		// This should match the byte size of the InstanceMetaData struct.
		ts3DebugAssert( metaDataSize == gds::getInstanceMetaDataSize() );

		const auto valueByteSize = deserialize( pInputData + metaDataSize, pValue );

		return metaDataSize + valueByteSize;
	}

	/***********************************************************************************************************/
	/******************************************** Core API - Variadic ******************************************/
	/***********************************************************************************************************/

	template <typename Tp>
	inline gds_size_t gdscore::evalByteSizeAll( const Tp & pValue )
	{
		return evalByteSize( pValue );
	}

	template <typename Tp, typename... TpRest>
	inline gds_size_t gdscore::evalByteSizeAll( const Tp & pValue, TpRest && ...pRest )
	{
		gds_size_t byteSize = evalByteSize( pValue );
		byteSize += evalByteSizeAll( std::forward<TpRest>( pRest )... );
		return byteSize;
	}

	template <typename Tp>
	inline gds_size_t gdscore::serializeAll( byte * pOutputBuffer, const Tp & pValue )
	{
		return serialize( pOutputBuffer, pValue );
	}

	template <typename Tp, typename... TpRest>
	inline gds_size_t gdscore::serializeAll( byte * pOutputBuffer, const Tp & pValue, TpRest && ...pRest )
	{
		gds_size_t byteSize = serialize( pOutputBuffer, pValue );
		byteSize += serializeAll( pOutputBuffer + byteSize, std::forward<TpRest>( pRest )... );
		return byteSize;
	}

	template <typename Tp>
	inline gds_size_t gdscore::deserializeAll( const byte * pInputData, Tp & pValue )
	{
		return deserialize( pInputData, pValue );
	}

	template <typename Tp, typename... TpRest>
	inline gds_size_t gdscore::deserializeAll( const byte * pInputData, Tp & pValue, TpRest && ...pRest )
	{
		gds_size_t byteSize = deserialize( pInputData, pValue );
		byteSize += deserializeAll( pInputData + byteSize, std::forward<TpRest>( pRest )... );
		return byteSize;
	}

	/***********************************************************************************************************/
	/********************************************** Core API - Auto ********************************************/
	/***********************************************************************************************************/

	template <typename TpValue>
	inline gds_size_t gdscore::serializeAuto( const ReadWriteMemoryView & pOutputBuffer, const TpValue & pValue )
	{
		return internal::serializeAutoFixed( pOutputBuffer, pValue );
	}

	template <typename TpValue, size_t tpSize>
	inline gds_size_t gdscore::serializeAuto( std::array<byte, tpSize> & pOutputBuffer, const TpValue & pValue )
	{
		return internal::serializeAutoFixed( pOutputBuffer, pValue );
	}

	template <typename TpValue>
	inline gds_size_t gdscore::serializeAutoWithMetaData( const ReadWriteMemoryView & pOutputBuffer, const TpValue & pValue )
	{
		return internal::serializeAutoWithMetaDataFixed( pOutputBuffer, pValue );
	}

	template <typename TpValue, size_t tpSize>
	inline gds_size_t gdscore::serializeAutoWithMetaData( std::array<byte, tpSize> & pOutputBuffer, const TpValue & pValue )
	{
		return internal::serializeAutoWithMetaDataFixed( pOutputBuffer, pValue );
	}

	template <typename TpValue>
	inline gds_size_t gdscore::serializeAuto( DynamicByteArray & pOutputBuffer, const TpValue & pValue )
	{
		return internal::serializeAutoResizable( pOutputBuffer, pValue );
	}

	template <typename TpValue>
	inline gds_size_t gdscore::serializeAuto( DynamicMemoryBuffer & pOutputBuffer, const TpValue & pValue )
	{
		return internal::serializeAutoResizable( pOutputBuffer, pValue );
	}

	template <typename TpValue>
	inline gds_size_t gdscore::serializeAuto( std::vector<byte> & pOutputBuffer, const TpValue & pValue )
	{
		return internal::serializeAutoResizable( pOutputBuffer, pValue );
	}

	template <typename TpValue>
	inline gds_size_t gdscore::serializeAutoWithMetaData( DynamicByteArray & pOutputBuffer, const TpValue & pValue )
	{
		return internal::serializeAutoWithMetaDataResizable( pOutputBuffer, pValue );
	}

	template <typename TpValue>
	inline gds_size_t gdscore::serializeAutoWithMetaData( DynamicMemoryBuffer & pOutputBuffer, const TpValue & pValue )
	{
		return internal::serializeAutoWithMetaDataResizable( pOutputBuffer, pValue );
	}

	template <typename TpValue>
	inline gds_size_t gdscore::serializeAutoWithMetaData( std::vector<byte> & pOutputBuffer, const TpValue & pValue )
	{
		return internal::serializeAutoWithMetaDataResizable( pOutputBuffer, pValue );
	}

	template <typename TpBuffer, typename TpValue>
	inline gds_size_t gdscore::deserializeAuto( const TpBuffer & pInputData, TpValue & pValue )
	{
		return deserialize( pInputData.data(), pValue );
	}

	template <typename TpBuffer, typename TpValue>
	inline gds_size_t gdscore::deserializeAutoWithMetaData( const TpBuffer & pInputData, TpValue & pValue )
	{
		return deserializeWithMetaData( pInputData.data(), pValue );
	}

	/***********************************************************************************************************/
	/******************************************** Core API - External ******************************************/
	/***********************************************************************************************************/

	template <typename TpBuffer, typename TpValue>
	inline gds_size_t gdscore::serializeExternal( const TpValue & pValue, const DataWriteCallback & pWriteCallback, TpBuffer & pGdsCache )
	{
		// Serialize the object into the cache. We can use our neat serializeAuto()
		// to properly handle different type of buffers here!
		const auto writeSize = serializeAutoWithMetaData( pGdsCache, pValue );

		if( writeSize > 0 )
		{
			if( !pWriteCallback )
			{
				// This cannot be an empty function.
				ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, "Empty write callback." );
			}

			// Write the data using the callback.
			pWriteCallback( pGdsCache.data(), writeSize );
		}

		return writeSize;
	}

	template <typename TpValue>
	inline gds_size_t gdscore::serializeExternal( const TpValue & pValue, const DataWriteCallback & pWriteCallback )
	{
		DynamicByteArray localGdsCache{};
		return serializeExternal( pValue, pWriteCallback, localGdsCache );
	}

	template <typename TpValue>
	inline gds_size_t gdscore::deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, const ReadWriteMemoryView & pGdsCache )
	{
		return internal::deserializeExternalFixed( pValue, pReadCallback, pGdsCache );
	}

	template <typename TpValue, size_t tpSize>
	inline gds_size_t gdscore::deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, std::array<byte, tpSize> & pGdsCache )
	{
		return internal::deserializeExternalFixed( pValue, pReadCallback, pGdsCache );
	}

	template <typename TpValue>
	inline gds_size_t gdscore::deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, DynamicByteArray & pGdsCache )
	{
		return internal::deserializeExternalResizable( pValue, pReadCallback, pGdsCache );
	}

	template <typename TpValue>
	inline gds_size_t gdscore::deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, DynamicMemoryBuffer & pGdsCache )
	{
		return internal::deserializeExternalResizable( pValue, pReadCallback, pGdsCache );
	}

	template <typename TpValue>
	inline gds_size_t gdscore::deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, std::vector<byte> & pGdsCache )
	{
		return internal::deserializeExternalResizable( pValue, pReadCallback, pGdsCache );
	}

	template <typename TpValue>
	inline gds_size_t gdscore::deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback )
	{
		DynamicByteArray localGdsCache{};
		return deserializeExternal( pValue, pReadCallback, localGdsCache );
	}

	/***********************************************************************************************************/
	/******************************************* Container/Map Helpers *****************************************/
	/***********************************************************************************************************/

	template <template <typename...> typename TpC, typename... TpCArgs>
	inline gds_size_t gdscore::evalByteSizeContainer( const TpC<TpCArgs...> & pContainer )
	{
		// Compute the size for the specified container.
		// The space required is the serialized size + binary representation of all elements in the container.
		gds_size_t byteSize = 0;

		// The first member is the number of elements in a container. We are not interested in its value, though.
		byteSize += gds::evalByteSize( gds::emptySizeType() );

		// Enumerate over the container's elements.
		for( const auto & entry : pContainer )
		{
			// Evaluate the size of each element and add it to the result.
			byteSize += gdscore::evalByteSize( entry );
		}

		return byteSize;
	}

	template <template <typename...> typename TpC, typename... TpCArgs>
	inline gds_size_t gdscore::serializeContainer( byte * pOutputBuffer, const TpC<TpCArgs...> & pContainer )
	{
		// First, get the size of the container (number of elements).
		const size_t containerSize = pContainer.size();

		// size_t may have different sizes on different platforms. asSizeTypeValue() ensures this is properly
		// handled by the GDS, so we can deserialize the data on a different platform, and get expected results.
		gds_size_t byteSize = gds::serialize( pOutputBuffer, gds::asSizeType( containerSize ) );

		// Enumerate over the container's elements.
		for( const auto & entry : pContainer )
		{
			// Serialize the element, add the size to the total byte size. Propagate cast tag.
			byteSize += gdscore::serialize( pOutputBuffer + byteSize, entry );
		}

		return byteSize;
	}

	template <template <typename...> typename TpC, typename... TpCArgs, typename TpIns>
	inline gds_size_t gdscore::deserializeContainer( const byte * pInputData, TpC<TpCArgs...> & pContainer, const TpIns & pInsertCallback )
	{
		// The variable to store the container's size.
		size_t containerSize = 0;

		// Deserialize the size. Since we used asSizeTypeValue() during serialization, we must now use asSizeTypeRef()
		// to properly retrieve the deserialized value. The output will be properly stored in the wrapped variable.
		gds_size_t byteSize = gds::deserialize( pInputData, gds::asSizeType( containerSize ) );

		for( size_t entryIndex = 0; entryIndex < containerSize; ++entryIndex )
		{
			// Temporary for the deserialized state.
			typename TpC<TpCArgs...>::value_type tempEntry{};

			// Deserialize current element. Propagate cast tag to support container-wise value casting.
			byteSize += gdscore::deserialize( pInputData + byteSize, tempEntry );

			// Call the insert callback to add it to the container. Index is needed e.g. for std::array.
			pInsertCallback( pContainer, entryIndex, std::move( tempEntry ) );
		}

		return byteSize;
	}

	template <template <typename...> typename TpC, typename... TpCArgs>
	inline gds_size_t gdscore::evalMapByteSize( const TpC<TpCArgs...> & pMap )
	{
		gds_size_t byteSize = 0;

		// The first member is the number of elements in a container. We are not interested in its value, though.
		byteSize += gds::evalByteSize( gds::emptySizeType() );

		// Enumerate over the container's elements.
		for( const auto & mapEntry : pMap )
		{
			// Evaluate the size of each element. Propagate cast tag to support container-wise value casting.
			byteSize += gdscore::evalByteSize( mapEntry.first );
			byteSize += gdscore::evalByteSize( mapEntry.second );
		}

		return byteSize;
	}

	template <template <typename...> typename TpM, typename TpK, typename TpV, typename... TpCArgs>
	inline gds_size_t gdscore::serializeMap( byte * pOutputBuffer, const TpM<TpK, TpV, TpCArgs...> & pMap )
	{
		{
			const size_t mapSize = pMap.size();

			gds_size_t byteSize = gds::serialize( pOutputBuffer, gds::asSizeType( mapSize ) );

			for( const auto & mapEntry : pMap )
			{
				byteSize += gdscore::serialize( pOutputBuffer + byteSize, mapEntry.first );
				byteSize += gdscore::serialize( pOutputBuffer + byteSize, mapEntry.second );
			}

			return byteSize;
		}
	}

	template <template <typename...> typename TpM, typename TpK, typename TpV, typename... TpCArgs>
	inline gds_size_t gdscore::deserializeMap( const byte * pInputData, TpM<TpK, TpV, TpCArgs...> & pMap )
	{
		size_t mapSize = 0;

		gds_size_t byteSize = gds::deserialize( pInputData, gds::asSizeType( mapSize ) );

		for( size_t entryIndex = 0; entryIndex < mapSize; ++entryIndex )
		{
			TpK tempKey{};
			byteSize += gdscore::deserialize( pInputData + byteSize, tempKey );

			TpV tempValue{};
			byteSize += gdscore::deserialize( pInputData + byteSize, tempValue );

			pMap.emplace( std::move( tempKey ), std::move( tempValue ) );
		}

		return byteSize;
	}

	/***********************************************************************************************************/
	/************************************************* ArrayView ***********************************************/
	/***********************************************************************************************************/

	template <typename Tp>
	inline gds_size_t gdscore::evalByteSize( const ArrayView<Tp> & pArrayView )
	{
		gds_size_t byteSize = gds::evalByteSize( gds::emptySizeType() );
		for( const auto & entry : pArrayView )
		{
			byteSize += gdscore::evalByteSize( entry );
		}
		return byteSize;
	}

	template <typename Tp>
	inline gds_size_t gdscore::serialize( byte * pOutputBuffer, const ArrayView<Tp> & pArrayView )
	{
		gds_size_t byteSize = gds::serialize( pOutputBuffer, gds::asSizeType( pArrayView.size() ) );
		for( const auto & value : pArrayView )
		{
			byteSize += gdscore::serialize(  pOutputBuffer + byteSize, value );
		}
		return byteSize;
	}

	template <typename Tp>
	inline gds_size_t gdscore::deserialize( const byte * pInputData, ArrayView<Tp> & pArrayView )
	{
		size_t arraySize = 0;
		gds_size_t byteSize = gds::deserialize( pInputData, gds::asSizeType( arraySize ) );
		for( auto & value : pArrayView )
		{
			byteSize += gdscore::deserialize( pInputData + byteSize, value );
		}
		return byteSize;
	}

	/***********************************************************************************************************/
	/*********************************************** AtomicBitmask *********************************************/
	/***********************************************************************************************************/

	template <typename TpIntegral>
	inline gds_size_t gdscore::evalByteSize( const AtomicBitmask<TpIntegral> & pBitmask )
	{
		return gds::evalByteSize( static_cast<TpIntegral>( pBitmask ) );
	}

	template <typename TpIntegral>
	inline gds_size_t gdscore::serialize( byte * pOutputBuffer, const AtomicBitmask<TpIntegral> & pBitmask )
	{
		return gds::serialize( pOutputBuffer, static_cast<TpIntegral>( pBitmask ) );
	}

	template <typename TpIntegral>
	inline gds_size_t gdscore::deserialize( const byte * pInputData, AtomicBitmask<TpIntegral> & pBitmask )
	{
		return deserialize( pInputData, static_cast<TpIntegral>( pBitmask ) );
	}

	/***********************************************************************************************************/
	/************************************************** Bitmask ************************************************/
	/***********************************************************************************************************/

	template <typename TpIntegral>
	inline gds_size_t gdscore::evalByteSize( const Bitmask<TpIntegral> & pBitmask )
	{
		return evalByteSize( static_cast<TpIntegral>( pBitmask ) );
	}

	template <typename TpIntegral>
	inline gds_size_t gdscore::serialize( byte * pOutputBuffer, const Bitmask<TpIntegral> & pBitmask )
	{
		return serialize( pOutputBuffer, static_cast<TpIntegral>( pBitmask ) );
	}

	template <typename TpIntegral>
	inline gds_size_t gdscore::deserialize( const byte * pInputData, Bitmask<TpIntegral> & pBitmask )
	{
		return deserialize( pInputData, static_cast<TpIntegral>( pBitmask ) );
	}

	/***********************************************************************************************************/
	/************************************************ SortedArray **********************************************/
	/***********************************************************************************************************/

	template <typename Tp, typename TpCompare, typename TpAlloc>
	inline gds_size_t gdscore::evalByteSize( const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray )
	{
		return evalByteSizeContainer( pSortedArray );
	}

	template <typename Tp, typename TpCompare, typename TpAlloc>
	inline gds_size_t gdscore::serialize( byte * pOutputBuffer, const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray )
	{
		return serializeContainer( pOutputBuffer, pSortedArray );
	}

	template <typename Tp, typename TpCompare, typename TpAlloc>
	inline gds_size_t gdscore::deserialize( const byte * pInputData, const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray )
	{
		return deserializeContainer( pInputData, pSortedArray,
			[]( auto & SA, auto, auto E ) -> void { SA.insert( std::forward<decltype( E )>( E ) ); } );
	}

	/***********************************************************************************************************/
	/******************************************** std::basic_string ********************************************/
	/***********************************************************************************************************/

	template <typename TpCh, typename TpTr, typename TpAlloc>
	inline gds_size_t gdscore::evalByteSize( const std::basic_string<TpCh, TpTr, TpAlloc> & pString )
	{
		gds_size_t byteSize = 0;

		// The first member is the length of the specified string.
		byteSize += gds::evalByteSize( gds::emptySizeType() );

		// Then we have the characters (potentially cast).
		byteSize += pString.length() * evalByteSize( TpCh{} );

		return byteSize;
	}

	template <typename TpCh, typename TpTr, typename TpAlloc>
	inline gds_size_t gdscore::serialize( byte * pOutputBuffer, const std::basic_string<TpCh, TpTr, TpAlloc> & pString )
	{
		const auto strLength = pString.length();

		gds_size_t byteSize = gds::serialize( pOutputBuffer, gds::asSizeType( strLength ) );

		for( size_t charIndex = 0; charIndex < strLength; ++charIndex )
		{
			byteSize += serialize( pOutputBuffer + byteSize, pString[charIndex] );
		}

		return byteSize;
	}

	template <typename TpCh, typename TpTr, typename TpAlloc>
	inline gds_size_t gdscore::deserialize( const byte * pInputData, std::basic_string<TpCh, TpTr, TpAlloc> & pString )
	{
		size_t strLength = 0;
		gds_size_t byteSize = gds::deserialize( pInputData, gds::asSizeType( strLength ) );

		pString.resize( strLength );

		for( size_t charIndex = 0; charIndex < strLength; ++charIndex )
		{
			byteSize += deserialize( pInputData + byteSize, pString[charIndex] );
		}

		return byteSize;
	}

	/***********************************************************************************************************/
	/************************************************ std::pair ************************************************/
	/***********************************************************************************************************/

	template <typename Tp1, typename Tp2>
	inline gds_size_t gdscore::evalByteSize( const std::pair<Tp1, Tp2> & pPair )
	{
		gds_size_t byteSize = 0;
		byteSize += evalByteSize( pPair.first );
		byteSize += evalByteSize( pPair.second );
		return byteSize;
	}

	template <typename Tp1, typename Tp2>
	inline gds_size_t gdscore::serialize( byte * pOutputBuffer, const std::pair<Tp1, Tp2> & pPair )
	{
		gds_size_t byteSize = 0;
		byteSize += serialize( pOutputBuffer, pPair.first );
		byteSize += serialize( pOutputBuffer, pPair.second );
		return byteSize;
	}

	template <typename Tp1, typename Tp2>
	inline gds_size_t gdscore::deserialize( const byte * pInputData, std::pair<Tp1, Tp2> & pPair )
	{
		gds_size_t byteSize = 0;
		byteSize += deserialize( pInputData, pPair.first );
		byteSize += deserialize( pInputData, pPair.second );
		return byteSize;
	}

	/***********************************************************************************************************/
	/************************************************ std::array ***********************************************/
	/***********************************************************************************************************/

	template <typename TpValue, size_t tpSize>
	inline gds_size_t gdscore::evalByteSize( const std::array<TpValue, tpSize> & pArray )
	{
		return evalByteSizeContainer( pArray );
	}

	template <typename TpValue, size_t tpSize>
	inline gds_size_t gdscore::serialize( byte * pOutputBuffer, const std::array<TpValue, tpSize> & pArray )
	{
		return serializeContainer( pOutputBuffer, pArray );
	}

	template <typename TpValue, size_t tpSize>
	inline gds_size_t gdscore::deserialize( const byte * pInputData, std::array<TpValue, tpSize> & pArray )
	{
		// std::array doesn't have any push/insert methods, so that's the one case where index becomes useful.
		// We simply assign the incoming elements under their indexes passed by the helper method. Simple.
		return deserializeContainer( pInputData, pArray,
			[]( auto & A, auto I, auto && E ) -> void { A[I] = std::forward<decltype( E )>( E ); } );
	}

	/***********************************************************************************************************/
	/*********************************************** std::vector ***********************************************/
	/***********************************************************************************************************/

	template <typename TpValue, typename TpAlloc>
	inline gds_size_t gdscore::evalByteSize( const std::vector<TpValue, TpAlloc> & pVector )
	{
		return evalByteSizeContainer( pVector );
	}

	template <typename TpValue, typename TpAlloc>
	inline gds_size_t gdscore::serialize( byte * pOutputBuffer, const std::vector<TpValue, TpAlloc> & pVector )
	{
		return serializeContainer( pOutputBuffer, pVector );
	}

	template <typename TpValue, typename TpAlloc>
	inline gds_size_t gdscore::deserialize( const byte * pInputData, std::vector<TpValue, TpAlloc> & pVector )
	{
		return deserializeContainer( pInputData, pVector,
			[]( auto & V, auto, auto && E ) -> void { V.push_back( std::forward<decltype( E )>( E ) ); } );
	}

	//
	//
	//
	//
	//

	template <typename TpFxBuffer, typename TpValue>
	inline gds_size_t gdscore::internal::serializeAutoFixed( TpFxBuffer & pOutputBuffer, const TpValue & pValue )
	{
		// Get the size of the serialized object (i.e. the minimum required size of the target buffer).
		const auto totalByteSize = evalByteSize( pValue );

		if( totalByteSize > 0 )
		{
			// The buffer is a fixed one - check if its size is large enough to store the whole data.
			if( totalByteSize > pOutputBuffer.size() )
			{
				// Buffer is too small, throw an error, nothing can be done here.
				ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, "Fixed buffer is to small for the output data." );
			}

			// Buffer is large enough, so we can use it as the target for the output.
			// Serialize the data into the buffer. This is the call to the actual, value-specific serialize().
			serialize( pOutputBuffer.data(), pValue );
		}

		return totalByteSize;
	}

	template <typename TpFxBuffer, typename TpValue>
	inline gds_size_t gdscore::internal::serializeAutoWithMetaDataFixed( TpFxBuffer & pOutputBuffer, const TpValue & pValue )
	{
		// Same thing as above, but we also include the MetaData block alongside the object itself.

		const auto totalByteSize = evalByteSizeWithMetaData( pValue );

		if( totalByteSize > 0 )
		{
			if( totalByteSize > pOutputBuffer.size() )
			{
				ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, "Fixed buffer is to small for the output data." );
			}
			serializeWithMetaData( pOutputBuffer.data(), pValue );
		}

		return totalByteSize;
	}

	template <typename TpRsBuffer, typename TpValue>
	inline gds_size_t gdscore::internal::serializeAutoResizable( TpRsBuffer & pOutputBuffer, const TpValue & pValue )
	{
		// Get the size of the serialized object (i.e. the minimum required size of the target buffer).
		const auto totalByteSize = evalByteSize( pValue );

		if( totalByteSize > 0 )
		{
			// Resize the output buffer.
			pOutputBuffer.resize( trunc_numeric_cast<size_t>( totalByteSize ) );

			// Serialize the data into the buffer. This is the call to the actual, value-specific serialize().
			serialize( pOutputBuffer.data(), pValue );
		}

		return totalByteSize;
	}

	template <typename TpRsBuffer, typename TpValue>
	inline gds_size_t gdscore::internal::serializeAutoWithMetaDataResizable( TpRsBuffer & pOutputBuffer, const TpValue & pValue )
	{
		// Same thing as above, but we also include the MetaData block alongside the object itself.

		const auto totalByteSize = evalByteSizeWithMetaData( pValue );

		if( totalByteSize > 0 )
		{
			pOutputBuffer.resize( trunc_numeric_cast<size_t>( totalByteSize ) );
			serializeWithMetaData( pOutputBuffer.data(), pValue );
		}

		return totalByteSize;
	}

	template <typename TpValue, typename TpFxBuffer>
	inline gds_size_t gdscore::internal::deserializeExternalFixed( TpValue & pValue, const DataReadCallback & pReadCallback, TpFxBuffer & pGdsCache )
	{
		if( !readDataExternalFixed( pReadCallback, pGdsCache ) )
		{
			return 0;
		}
		return deserializeWithMetaData( pGdsCache.data(), pValue );
	}

	template <typename TpValue, typename TpRsBuffer>
	inline gds_size_t gdscore::internal::deserializeExternalResizable( TpValue & pValue, const DataReadCallback & pReadCallback, TpRsBuffer & pGdsCache )
	{
		// Read data from the stream using the MetaData info. If this succeeds, pGdsCache will have
		// a byte representation of a serialized object (including its MetaData block, of course).
		if( !readDataExternalResizable( pReadCallback, pGdsCache ) )
		{
			return 0;
		}

		// Deserialize the object into the specified value. This will once again perform reading of the
		// MetaData block, but this is super cheap and we will get some additional validation as a bonus.
		return deserializeWithMetaData( pGdsCache.data(), pValue );
	}

	template <typename TpFxBuffer>
	inline bool gdscore::internal::readDataExternalFixed( const DataReadCallback & pReadCallback, TpFxBuffer & pReadBuffer )
	{
		// Get the size of the metadata. This is the amount of data we need to read from the stream first.
		const auto metaDataSize = gds::getInstanceMetaDataSize();

		// This is a fixed buffer and cannot be resized. If there is not enough space, we need to abort.
		if( metaDataSize > pReadBuffer.size() )
		{
			ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, "Fixed buffer is to small for the output data." );
		}

		const auto metaDataReadSize = pReadCallback( pReadBuffer.data(), metaDataSize );
		if( metaDataReadSize != metaDataSize )
		{
			return false;
		}

		const auto metaData = gds::readInstanceMetaData( pReadBuffer.data() );
		if( !metaData )
		{
			return false;
		}

		// Again - we cannot resize the buffer. If the additional data does not fit, report an error and quit.
		if( metaData.outputBlockSize > pReadBuffer.size() )
		{
			ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, "Fixed buffer is to small for the output data." );
		}

		const auto objectDataReadSize = pReadCallback( pReadBuffer.data() + metaDataSize, metaData.objectDataSize );
		if( objectDataReadSize != metaData.objectDataSize )
		{
			return false;
		}

		return true;
	}

	template <typename TpRsBuffer>
	inline bool gdscore::internal::readDataExternalResizable( const DataReadCallback & pReadCallback, TpRsBuffer & pReadBuffer )
	{
		// Get the size of the metadata. This is the amount of data we need to read from the stream first.
		const auto metaDataSize = gds::getInstanceMetaDataSize();

		// Resize the buffer accordingly.
		pReadBuffer.resize( metaDataSize );

		// Read the data from the stream. If there is not enough data, we cannot proceed.
		const auto metaDataReadSize = pReadCallback( pReadBuffer.data(), metaDataSize );
		if( metaDataReadSize != metaDataSize )
		{
			return false;
		}

		// Read the MetaData object itself. It is boolean-convertible, so we can validate it.
		const auto metaData = gds::readInstanceMetaData( pReadBuffer.data() );
		if( !metaData )
		{
			return false;
		}

		// MetaData contains information about the size. ::outputBlockSize is the size of the whole data block
		// (i.e. MetaData + ObjectData). Resize the buffer so we can read the remaining object data now.
		pReadBuffer.resize( trunc_numeric_cast<size_t>( metaData.outputBlockSize ) );

		// Read the object data. Save it after the MetaData (hence the offset), so we have a full representation.
		const auto objectDataReadSize = pReadCallback( pReadBuffer.data() + metaDataSize, metaData.objectDataSize );
		if( objectDataReadSize != metaData.objectDataSize )
		{
			return false;
		}

		return true;
	}

} // namespace ts3

#endif // __TS3_CORE_GDS_CORE_H__
