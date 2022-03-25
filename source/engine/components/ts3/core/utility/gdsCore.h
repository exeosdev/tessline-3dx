
#ifndef __TS3_CORE_GDS_CORE_H__
#define __TS3_CORE_GDS_CORE_H__

#include "gdsCommon.h"
#include "../exception.h"
#include <ts3/platform/gds.h>
#include <ts3/stdext/byteArray.h>
#include <functional>

namespace std
{

	template <typename _Ty1, typename _Ty2>
	struct pair;

	template <typename _T, size_t _N>
	class array;

	template <typename _T, typename _Alloc>
	class vector;

	template <typename _T, typename _Alloc>
	class list;

	template <typename _T, typename _Alloc>
	class deque;

	template <typename _T, typename _Cnt>
	class queue;

	template <typename _T, typename _Cnt>
	class stack;

	template <typename _K, typename _V, typename _Cmp, typename _Alloc>
	class map;

	template <typename _T, typename _Cmp, typename _Alloc>
	class set;

	template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc>
	class unordered_map;

	template <typename _K, typename _H, typename _Eq, typename _Alloc>
	class unordered_set;

}

namespace ts3
{

	template <typename TpIntegral>
	class AtomicBitmask;

	template <typename TpIntegral>
	class Bitmask;

	template <typename Tp, typename TpCompare, typename TpAlloc>
	class SortedArray;

	class GdsCore
	{
	public:
		/// @brief Typedef for a callable compatible with ReadCallback requirements for GDS deserializeExternal() functions.
		using DataReadCallback = std::function<uint64 /* pReadBytes */( void * /* pTargetBuffer */, uint64 /* pReadSize */ )>;

		/// @brief Typedef for a callable compatible with WriteCallback requirements for GDS serializeExternal() functions.
		using DataWriteCallback = std::function<uint64 /* pWrittenBytes */( const void * /* pDataToWrite */, uint64 /* pWriteSize */ )>;

		/***********************************************************************************************************/
		/**************************************** Core API - evalByteSize() ****************************************/
		/***********************************************************************************************************/

		template <typename Tp, std::enable_if_t<IsGdsSerializable<Tp>::sValue, int> = 0>
		static gds_size_t evalByteSize( const Tp & pValue );

		template <typename Tp>
		static gds_size_t evalByteSize( const Tp & pValue );

		template <typename TpRef, typename TpInternal>
		static gds_size_t evalByteSize( const gds::ValueRef<TpRef, TpInternal> & pValueRef );

		template <typename Tp, typename TpInternal, std::enable_if_t<std::is_void<TpInternal>::value, int> = 0>
		static gds_size_t evalByteSize( const Tp & pValue, const gds::TypeCastTag<TpInternal> & );

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int> = 0>
		static gds_size_t evalByteSize( const Tp & pValue, const gds::TypeCastTag<TpInternal> & );

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int> = 0>
		static gds_size_t evalByteSize( const gds::TypeCastInfo<Tp, TpInternal> & pCastInfo );

		/***********************************************************************************************************/
		/***************************************** Core API - serialize() ******************************************/
		/***********************************************************************************************************/

		template <typename Tp, std::enable_if_t<IsGdsSerializable<Tp>::sValue, int> = 0>
		static gds_size_t serialize( byte * pOutputBuffer, const Tp & pValue );

		template <typename Tp>
		static gds_size_t serialize( byte * pOutputBuffer, const Tp & pValue );

		template <typename TpRef, typename TpInternal>
		static gds_size_t serialize( byte * pOutputBuffer, const gds::ValueRef<TpRef, TpInternal> & pValueRef );

		template <typename Tp, typename TpInternal, std::enable_if_t<std::is_void<TpInternal>::value, int> = 0>
		static gds_size_t serialize( byte * pOutputBuffer, const Tp & pValue, const gds::TypeCastTag<TpInternal> & );

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int> = 0>
		static gds_size_t serialize( byte * pOutputBuffer, const Tp & pValue, const gds::TypeCastTag<TpInternal> & );

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int> = 0>
		static gds_size_t serialize( byte * pOutputBuffer, const gds::TypeCastInfo<Tp, TpInternal> & pCastInfo );

		/***********************************************************************************************************/
		/**************************************** Core API - deserialize() *****************************************/
		/***********************************************************************************************************/

		template <typename Tp, std::enable_if_t<IsGdsSerializable<Tp>::sValue, int> = 0>
		static gds_size_t deserialize( const byte * pInputData, Tp & pValue );

		template <typename Tp>
		static gds_size_t deserialize( const byte * pInputData, Tp & pValue );

		template <typename TpRef, typename TpInternal>
		static gds_size_t deserialize( const byte * pInputData, const gds::ValueRef<TpRef, TpInternal> & pValueRef );

		template <typename Tp, typename TpInternal, std::enable_if_t<std::is_void<TpInternal>::value, int> = 0>
		static gds_size_t deserialize( const byte * pInputData, Tp & pRef, const gds::TypeCastTag<TpInternal> & );

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int> = 0>
		static gds_size_t deserialize( const byte * pInputData, Tp & pRef, const gds::TypeCastTag<TpInternal> & );

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int> = 0>
		static gds_size_t deserialize( const byte * pInputData, const gds::TypeCastInfo<Tp, TpInternal> & pCastInfo );

		/***********************************************************************************************************/
		/******************************************** Core API - MetaData ******************************************/
		/***********************************************************************************************************/

		/// @brief Returns an evaluated byte size of the specified object which will be serialized using metadata.
		template <typename Tp>
		static gds_size_t evalByteSizeWithMetaData( const Tp & pValue );

		/// @brief Serializes given object with additional metadata.
		template <typename Tp>
		static gds_size_t serializeWithMetaData( byte * pOutputBuffer, const Tp & pValue );

		/// @brief Performs deserialization of the specified data, written with serializeWithMetaData().
		template <typename Tp>
		static gds_size_t deserializeWithMetaData( const byte * pInputData, Tp & pValue );

		/***********************************************************************************************************/
		/********************************************** Core API - Auto ********************************************/
		/***********************************************************************************************************/


		template <typename TpValue>
		static gds_size_t serializeAuto( const ReadWriteMemoryView & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue, size_t tpSize>
		static gds_size_t serializeAuto( std::array<byte, tpSize> & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue>
		static gds_size_t serializeAutoWithMetaData( const ReadWriteMemoryView & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue, size_t tpSize>
		static gds_size_t serializeAutoWithMetaData( std::array<byte, tpSize> & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue>
		static gds_size_t serializeAuto( DynamicByteArray & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue>
		static gds_size_t serializeAuto( DynamicMemoryBuffer & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue>
		static gds_size_t serializeAuto( std::vector<byte> & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue>
		static gds_size_t serializeAutoWithMetaData( DynamicByteArray & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue>
		static gds_size_t serializeAutoWithMetaData( DynamicMemoryBuffer & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue>
		static gds_size_t serializeAutoWithMetaData( std::vector<byte> & pOutputBuffer, const TpValue & pValue );

		template <typename TpBuffer, typename TpValue>
		static gds_size_t deserializeAuto( const TpBuffer & pInputData, TpValue & pValue );

		template <typename TpBuffer, typename TpValue>
		static gds_size_t deserializeAutoWithMetaData( const TpBuffer & pInputData, TpValue & pValue );

		/***********************************************************************************************************/
		/******************************************** Core API - Variadic ******************************************/
		/***********************************************************************************************************/

		template <typename Tp>
		static gds_size_t evalByteSizeAll( const Tp & pValue );

		template <typename Tp, typename... TpRest>
		static gds_size_t evalByteSizeAll( const Tp & pValue, TpRest && ...pRest );

		template <typename Tp>
		static gds_size_t serializeAll( byte * pOutputBuffer, const Tp & pValue );

		template <typename Tp, typename... TpRest>
		static gds_size_t serializeAll( byte * pOutputBuffer, const Tp & pValue, TpRest && ...pRest );

		template <typename Tp>
		static gds_size_t deserializeAll( const byte * pInputData, Tp & pValue );

		template <typename Tp, typename... TpRest>
		static gds_size_t deserializeAll( const byte * pInputData, Tp & pValue, TpRest && ...pRest );

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
		static gds_size_t serializeExternal( const TpValue & pValue, const DataWriteCallback & pWriteCallback, TpBuffer & pGdsCache );

		/// @brief Serializes the specified object and writes the byte representation using the specified callback.
		/// @param pValue The object to be serialized
		/// @param pWriteCallback The write callback for data writing. Cannot be empty.
		template <typename TpValue>
		static gds_size_t serializeExternal( const TpValue & pValue, const DataWriteCallback & pWriteCallback );

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
		static gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, const ReadWriteMemoryView & pGdsCache );

		template <typename TpValue, size_t tpSize>
		static gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, std::array<byte, tpSize> & pGdsCache );

		template <typename TpValue>
		static gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, DynamicByteArray & pGdsCache );

		template <typename TpValue>
		static gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, DynamicMemoryBuffer & pGdsCache );

		template <typename TpValue>
		static gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, std::vector<byte> & pGdsCache );

		/// @brief Deserializes object of the specified type and stores it in pValue. Binary data is read using the specified read callback.
		/// @param pValue The object to store the deserialized state.
		/// @param pReadCallback The read callback for data reading. Cannot be empty.
		template <typename TpValue>
		static gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback );

		/***********************************************************************************************************/
		/******************************************* Container/Map Helpers *****************************************/
		/***********************************************************************************************************/

		template <template <typename...> typename TpC, typename... TpCArgs>
		static gds_size_t evalContainerByteSize( const TpC<TpCArgs...> & pContainer );

		template <template <typename...> typename TpC, typename... TpCArgs>
		static gds_size_t serializeContainer( byte * pOutputBuffer, const TpC<TpCArgs...> & pContainer );

		template <template <typename...> typename TpC, typename... TpCArgs, typename TpIns>
		static gds_size_t deserializeContainer( const byte * pInputData, TpC<TpCArgs...> & pContainer, const TpIns & pInsertCallback );

		template <template <typename...> typename TpC, typename... TpCArgs>
		static gds_size_t evalMapByteSize( const TpC<TpCArgs...> & pMap );

		template <template <typename...> typename TpM, typename TpK, typename TpV, typename... TpCArgs>
		static gds_size_t serializeMap( byte * pOutputBuffer, const TpM<TpK, TpV, TpCArgs...> & pMap );

		template <template <typename...> typename TpM, typename TpK, typename TpV, typename... TpCArgs>
		static gds_size_t deserializeMap( const byte * pInputData, TpM<TpK, TpV, TpCArgs...> & pMap );

		/***********************************************************************************************************/
		/************************************************* ArrayView ***********************************************/
		/***********************************************************************************************************/

		template <typename Tp>
		static gds_size_t evalByteSize( const ArrayView<Tp> & pArrayView );

		template <typename Tp>
		static gds_size_t serialize( byte * pOutputBuffer, const ArrayView<Tp> & pArrayView );

		template <typename Tp>
		static gds_size_t deserialize( const byte * pInputData, ArrayView<Tp> & pArrayView );

		/***********************************************************************************************************/
		/*********************************************** AtomicBitmask *********************************************/
		/***********************************************************************************************************/

		template <typename TpIntegral>
		static gds_size_t evalByteSize( const AtomicBitmask<TpIntegral> & pBitmask );

		template <typename TpIntegral>
		static gds_size_t serialize( byte * pOutputBuffer, const AtomicBitmask<TpIntegral> & pBitmask );

		template <typename TpIntegral>
		static gds_size_t deserialize( const byte * pInputData, AtomicBitmask<TpIntegral> & pBitmask );

		/***********************************************************************************************************/
		/************************************************** Bitmask ************************************************/
		/***********************************************************************************************************/

		template <typename TpIntegral>
		static gds_size_t evalByteSize( const Bitmask<TpIntegral> & pBitmask );

		template <typename TpIntegral>
		static gds_size_t serialize( byte * pOutputBuffer, const Bitmask<TpIntegral> & pBitmask );

		template <typename TpIntegral>
		static gds_size_t deserialize( const byte * pInputData, Bitmask<TpIntegral> & pBitmask );

		/***********************************************************************************************************/
		/************************************************ SortedArray **********************************************/
		/***********************************************************************************************************/

		template <typename Tp, typename TpCompare, typename TpAlloc>
		static gds_size_t evalByteSize( const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray );

		template <typename Tp, typename TpCompare, typename TpAlloc>
		static gds_size_t serialize( byte * pOutputBuffer, const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray );

		template <typename Tp, typename TpCompare, typename TpAlloc>
		static gds_size_t deserialize( const byte * pInputData, const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray );

		/***********************************************************************************************************/
		/******************************************** std::basic_string ********************************************/
		/***********************************************************************************************************/

		template <typename _Ch, typename _Tr, typename _Alloc>
		static gds_size_t evalByteSize( const std::basic_string<_Ch, _Tr, _Alloc> & pString );
		
		template <typename _Ch, typename _Tr, typename _Alloc>
		static gds_size_t serialize( byte * pOutputBuffer, const std::basic_string<_Ch, _Tr, _Alloc> & pString );
		
		template <typename _Ch, typename _Tr, typename _Alloc>
		static gds_size_t deserialize( const byte * pInputData, std::basic_string<_Ch, _Tr, _Alloc> & pString );

		/***********************************************************************************************************/
		/************************************************ std::pair ************************************************/
		/***********************************************************************************************************/
		
		template <typename _Ty1, typename _Ty2>
		static gds_size_t evalByteSize( const std::pair<_Ty1, _Ty2> & pPair );

		template <typename _Ty1, typename _Ty2>
		static gds_size_t serialize( byte * pOutputBuffer, const std::pair<_Ty1, _Ty2> & pPair );

		template <typename _Ty1, typename _Ty2>
		static gds_size_t deserialize( const byte * pInputData, std::pair<_Ty1, _Ty2> & pPair );

		/***********************************************************************************************************/
		/************************************************ std::array ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, size_t _N>
		static gds_size_t evalByteSize( const std::array<_T, _N> & pArray );

		template <typename _T, size_t _N>
		static gds_size_t serialize( byte * pOutputBuffer, const std::array<_T, _N> & pArray );

		template <typename _T, size_t _N>
		static gds_size_t deserialize( const byte * pInputData, std::array<_T, _N> & pArray );

		/***********************************************************************************************************/
		/*********************************************** std::vector ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Alloc>
		static gds_size_t evalByteSize( const std::vector<_T, _Alloc> & pVector );

		template <typename _T, typename _Alloc>
		static gds_size_t serialize( byte * pOutputBuffer, const std::vector<_T, _Alloc> & pVector );

		template <typename _T, typename _Alloc>
		static gds_size_t deserialize( const byte * pInputData, std::vector<_T, _Alloc> & pVector );

		/***********************************************************************************************************/
		/************************************************ std::list ************************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Alloc>
		static gds_size_t evalByteSize( const std::list<_T, _Alloc> & pList );

		template <typename _T, typename _Alloc>
		static gds_size_t serialize( byte * pOutputBuffer, const std::list<_T, _Alloc> & pList );

		template <typename _T, typename _Alloc>
		static gds_size_t deserialize( const byte * pInputData, std::list<_T, _Alloc> & pList );

		/***********************************************************************************************************/
		/************************************************ std::deque ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Alloc>
		static gds_size_t evalByteSize( const std::deque<_T, _Alloc> & pDeque );

		template <typename _T, typename _Alloc>
		static gds_size_t serialize( byte * pOutputBuffer, const std::deque<_T, _Alloc> & pDeque );

		template <typename _T, typename _Alloc>
		static gds_size_t deserialize( const byte * pInputData, std::deque<_T, _Alloc> & pDeque );

		/***********************************************************************************************************/
		/************************************************ std::queue ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Cnt>
		static gds_size_t evalByteSize( const std::queue<_T, _Cnt> & pQueue );

		template <typename _T, typename _Cnt>
		static gds_size_t serialize( byte * pOutputBuffer, const std::queue<_T, _Cnt> & pQueue );

		template <typename _T, typename _Cnt>
		static gds_size_t deserialize( const byte * pInputData, std::queue<_T, _Cnt> & pQueue );

		/***********************************************************************************************************/
		/************************************************ std::stack ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Cnt>
		static gds_size_t evalByteSize( const std::stack<_T, _Cnt> & pStack );

		template <typename _T, typename _Cnt>
		static gds_size_t serialize( byte * pOutputBuffer, const std::stack<_T, _Cnt> & pStack );

		template <typename _T, typename _Cnt>
		static gds_size_t deserialize( const byte * pInputData, std::stack<_T, _Cnt> & pStack );

		/***********************************************************************************************************/
		/************************************************* std::set ************************************************/
		/***********************************************************************************************************/

		template <typename _K, typename _Cmp, typename _Alloc>
		static gds_size_t evalByteSize( const std::set<_K, _Cmp, _Alloc> & pSet );

		template <typename _K, typename _Cmp, typename _Alloc>
		static gds_size_t serialize( byte * pOutputBuffer, const std::set<_K, _Cmp, _Alloc> & pSet );

		template <typename _K, typename _Cmp, typename _Alloc>
		static gds_size_t deserialize( const byte * pInputData, std::set<_K, _Cmp, _Alloc> & pSet );

		/***********************************************************************************************************/
		/******************************************** std::unordered_set *******************************************/
		/***********************************************************************************************************/

		template <typename _K, typename _H, typename _Eq, typename _Alloc>
		static gds_size_t evalByteSize( const std::unordered_set<_K, _H, _Eq, _Alloc> & pUOSet );

		template <typename _K, typename _H, typename _Eq, typename _Alloc>
		static gds_size_t serialize( byte * pOutputBuffer, const std::unordered_set<_K, _H, _Eq, _Alloc> & pUOSet );

		template <typename _K, typename _H, typename _Eq, typename _Alloc>
		static gds_size_t deserialize( const byte * pInputData, std::unordered_set<_K, _H, _Eq, _Alloc> & pUOSet );

		/***********************************************************************************************************/
		/************************************************* std::map ************************************************/
		/***********************************************************************************************************/

		template <typename _K, typename _V, typename _Cmp, typename _Alloc>
		static gds_size_t evalByteSize( const std::map<_K, _V, _Cmp, _Alloc> & pMap );

		template <typename _K, typename _V, typename _Cmp, typename _Alloc>
		static gds_size_t serialize( byte * pOutputBuffer, const std::map<_K, _V, _Cmp, _Alloc> & pMap );

		template <typename _K, typename _V, typename _Cmp, typename _Alloc>
		static gds_size_t deserialize( const byte * pInputData, std::map<_K, _V, _Cmp, _Alloc> & pMap );

		/***********************************************************************************************************/
		/******************************************** std::unordered_map *******************************************/
		/***********************************************************************************************************/

		template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc>
		static gds_size_t evalByteSize( const std::unordered_map<_K, _V, _H, _Eq, _Alloc> & pUOMap );

		template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc>
		static gds_size_t serialize( byte * pOutputBuffer, const std::unordered_map<_K, _V, _H, _Eq, _Alloc> & pUOMap );

		template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc>
		static gds_size_t deserialize( const byte * pInputData, std::unordered_map<_K, _V, _H, _Eq, _Alloc> & pUOMap );

	private:
		template <typename TpFixedBuffer, typename TpValue>
		static gds_size_t _serializeAutoFixed( TpFixedBuffer & pOutputBuffer, const TpValue & pValue );

		template <typename TpFixedBuffer, typename TpValue>
		static gds_size_t _serializeAutoWithMetaDataFixed( TpFixedBuffer & pOutputBuffer, const TpValue & pValue );

		template <typename TpResizableBuffer, typename TpValue>
		static gds_size_t _serializeAutoResizable( TpResizableBuffer & pOutputBuffer, const TpValue & pValue );

		template <typename TpResizableBuffer, typename TpValue>
		static gds_size_t _serializeAutoWithMetaDataResizable( TpResizableBuffer & pOutputBuffer, const TpValue & pValue );

		template <typename TpValue, typename TpFixedBuffer>
		static gds_size_t _deserializeExternalFixed( TpValue & pValue, const DataReadCallback & pReadCallback, TpFixedBuffer & pGdsCache );

		template <typename TpValue, typename TpResizableBuffer>
		static gds_size_t _deserializeExternalResizable( TpValue & pValue, const DataReadCallback & pReadCallback, TpResizableBuffer & pGdsCache );

		template <typename TpFixedBuffer>
		static bool _readDataExternalFixed( const DataReadCallback & pReadCallback, TpFixedBuffer & pReadBuffer );

		template <typename TpResizableBuffer>
		static bool _readDataExternalResizable( const DataReadCallback & pReadCallback, TpResizableBuffer & pReadBuffer );
	};

} // namespace ts3

#define __TS3_CORE_GDS_INLINE_DEF_INCLUDE_SECTION__
#include "gdsImplCommon.inl"
#include "gdsImplFuncAutoVariadic.inl"
#include "gdsImplFuncExternal.inl"
#include "gdsImplTypeSupportInternal.inl"
#include "gdsImplTypeSupportStd.inl"
#undef __TS3_CORE_GDS_INLINE_DEF_INCLUDE_SECTION__

#endif // __TS3_CORE_GDS_CORE_H__
