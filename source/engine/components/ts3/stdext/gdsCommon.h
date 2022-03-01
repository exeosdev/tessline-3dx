
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
	
	template <typename Tp, typename TpCompare, typename TpAlloc>
    class SortedArray;

	enum : gds_type_id_t
	{
		E_GDS_TYPE_ID_STDX_ARRAY_VIEW = 0x3F0001,
	};

	namespace gds
	{

		/// @brief Computes the actual size of binary representation of an std:: container.
		template <template <typename...> typename TpC, typename... TpCArgs, typename TpCast>
		inline gds_size_t evalStdContainerByteSize( const TpC<TpCArgs...> & pStdContainer, const TpCast & pCastTag )
		{
			// Compute the size for the specified container.
			// The space required is the serialized size + binary representation of all elements in the container.

			gds_size_t byteSize = 0;

			// The first member is the number of elements in a container. We are not interested in its value, though.
			byteSize += gds::evalByteSize( emptySizeType() );

			// Enumerate over the container's elements.
			for( const auto & entry : pStdContainer )
			{
				// Evaluate the size of each element. Propagate cast tag to support container-wise value casting.
				byteSize += gds::evalByteSize( entry, pCastTag );
			}

			return byteSize;
		}

		/// @brief Computes the minimum size of a binary representation of an std:: container.
		template <template <typename...> typename TpC, typename... TpCArgs, typename TpCast>
		inline gds_size_t queryStdContainerMinByteSize( const TpC<TpCArgs...> & pStdContainer, const TpCast & pCastTag )
		{
			// Compute the minimum size for a container - i.e. empty container.
			// The space required is the serialized size (and that's all).

			gds_size_t byteSize = 0;

			// The size of a container is always there, expressed as a size_type.
			byteSize += gds::evalByteSize( emptySizeType() );

			return byteSize;
		}

		/// @brief Serializes the content of a specified std:: container and stores the output in the provided buffer.
		template <template <typename...> typename TpC, typename... TpCArgs, typename TpCast>
		inline gds_size_t serializeStdContainer(
				byte * pOutputBuffer,
				const TpC<TpCArgs...> & pStdContainer,
				const TpCast & pCastTag )
		{
			// First, get the size of the container (number of elements).
			size_t containerSize = std::size( pStdContainer );

			// size_t may have different sizes on different platforms. asSizeTypeValue() ensures this is properly
			// handled by the GDS, so we can deserialize the data on a different platform, and get expected results.
			gds_size_t byteSize = gds::serialize( pOutputBuffer, asSizeType( containerSize ) );

			// Enumerate over the container's elements.
			for( const auto & entry : pStdContainer )
			{
				// Serialize the element, add the size to the total byte size.
				// Propagate cast tag to support container-wise value casting.
				byteSize += gds::serialize( pOutputBuffer + byteSize, entry, pCastTag );
			}

			return byteSize;
		}

		template <template <typename...> typename TpC, typename... TpCArgs, typename TpCast, typename TpIns>
		inline gds_size_t deserializeStdContainer(
				const byte * pInputData,
				TpC<TpCArgs...> & pStdContainer,
				const TpCast & pCastTag,
				const TpIns & pInsertCallback )
		{
			// The variable to store the container's size.
			size_t containerSize = 0;

			// Deserialize the size. Since we used asSizeTypeValue() during serialization, we must now use asSizeTypeRef()
			// to properly retrieve the deserialized value. The output will be properly stored in the wrapped variable.
			gds_size_t byteSize = gds::deserialize( pInputData, asSizeType( containerSize ) );

			for( size_t entryIndex = 0; entryIndex < containerSize; ++entryIndex )
			{
				// Temporary for the deserialized state.
				typename TpC<TpCArgs...>::value_type tempEntry{};

				// Deserialize current element. Propagate cast tag to support container-wise value casting.
				byteSize += gds::deserialize( pInputData + byteSize, tempEntry, pCastTag );

				// Call the insert callback to add it to the container. Index is needed e.g. for std::array.
				pInsertCallback( pStdContainer, entryIndex, std::move( tempEntry ) );
			}

			return byteSize;
		}

		/// @brief Computes the actual size of binary representation of an std:: container.
		template <template <typename...> typename TpC, typename... TpCArgs, typename TpCast>
		inline gds_size_t evalStdMapByteSize( const TpC<TpCArgs...> & pStdMap, const TpCast & pCastTag )
		{
			// Compute the size for the specified container.
			// The space required is the serialized size + binary representation of all elements in the container.

			gds_size_t byteSize = 0;

			// The first member is the number of elements in a container. We are not interested in its value, though.
			byteSize += gds::evalByteSize( emptySizeType() );

			// Enumerate over the container's elements.
			for( const auto & mapEntry : pStdMap )
			{
				// Evaluate the size of each element. Propagate cast tag to support container-wise value casting.
				byteSize += gds::evalByteSize( mapEntry.first );
				byteSize += gds::evalByteSize( mapEntry.second, pCastTag );
			}

			return byteSize;
		}

		/// @brief Computes the minimum size of a binary representation of an std:: container.
		template <template <typename...> typename TpC, typename... TpCArgs, typename TpCast>
		inline gds_size_t queryStdMapMinByteSize( const TpC<TpCArgs...> & pStdMap, const TpCast & pCastTag )
		{
			// Compute the minimum size for a container - i.e. empty container.
			// The space required is the serialized size (and that's all).

			gds_size_t byteSize = 0;

			// The size of a container is always there, expressed as a size_type.
			byteSize += gds::evalByteSize( emptySizeType() );

			return byteSize;
		}

		/// @brief
		template <template <typename...> typename TpM, typename TpK, typename TpV, typename... TpCArgs, typename TpCast>
		inline gds_size_t serializeStdMap(
				byte * pOutputBuffer,
				const TpM<TpK, TpV, TpCArgs...> & pStdMap,
				const TpCast & pCastTag )
		{
			size_t mapSize = std::size( pStdMap );

			gds_size_t byteSize = gds::serialize( pOutputBuffer, asSizeType( mapSize ) );

			for( const auto & mapEntry : pStdMap )
			{
				byteSize += gds::serialize( pOutputBuffer + byteSize, mapEntry.first );
				byteSize += gds::serialize( pOutputBuffer + byteSize, mapEntry.second, pCastTag );
			}

			return byteSize;
		}

		template <template <typename...> typename TpM, typename TpK, typename TpV, typename... TpCArgs, typename TpCast>
		inline gds_size_t deserializeStdMap(
				const byte * pInputData,
				TpM<TpK, TpV, TpCArgs...> & pStdMap,
				const TpCast & pCastTag )
		{
			size_t mapSize = 0;

			gds_size_t byteSize = gds::deserialize( pInputData, asSizeType( mapSize ) );

			for( size_t entryIndex = 0; entryIndex < mapSize; ++entryIndex )
			{
				TpK tempKey{};
				byteSize += gds::deserialize( pInputData + byteSize, tempKey );

				TpV tempValue{};
				byteSize += gds::deserialize( pInputData + byteSize, tempValue, pCastTag );

				pStdMap.emplace( std::move( tempKey ), std::move( tempValue ) );
			}

			return byteSize;
		}

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
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				const ArrayView<Tp> & pArrayView,
				const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			gds_size_t byteSize = serialize( pOutputBuffer, asTypeID( E_GDS_TYPE_ID_STDX_ARRAY_VIEW ) );
			byteSize += serialize( pOutputBuffer + byteSize, asSizeType( pArrayView.size ) );

			for( const auto & value : pArrayView )
			{
				byteSize += serialize(  pOutputBuffer + byteSize, value, pCastTag );
			}

			return byteSize;
		}

		template <typename Tp, typename TpCast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				ArrayView<Tp> & pArrayView,
				const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			gds_type_id_t typeID = E_GDS_TYPE_ID_UNKNOWN;
			gds_size_t byteSize = deserialize( pInputData, asTypeID( typeID ) );

			size_t arraySize = 0;
			byteSize += deserialize( pInputData + byteSize, asSizeType( arraySize ) );

			for( auto & value : pArrayView )
			{
				byteSize += deserialize( pInputData + byteSize, value, pCastTag );
			}

			return byteSize;
		}


		/***********************************************************************************************************/
		/************************************************ SortedArray **********************************************/
		/***********************************************************************************************************/

		template <typename Tp, typename TpCompare, typename TpAlloc, typename TpCast = void>
		inline gds_size_t evalByteSize(
				const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray,
				const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			return evalStdContainerByteSize( pSortedArray, pCastTag );
		}

		template <typename Tp, typename TpCompare, typename TpAlloc, typename TpCast = void>
		inline gds_size_t queryMinByteSize(
				const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray,
				const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			return queryStdContainerMinByteSize( pSortedArray, pCastTag );
		}

		template <typename Tp, typename TpCompare, typename TpAlloc, typename TpCast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray,
				const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			return serializeStdContainer( pOutputBuffer,  pSortedArray, pCastTag );
		}

		template <typename Tp, typename TpCompare, typename TpAlloc, typename TpCast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray,
				const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			return deserializeStdContainer(
				pInputData, pSortedArray, pCastTag, []( auto & SA, auto, auto && E ) -> void { SA.insert( std::move( E ) ); } );
		}

	}

}

#endif // __TS3_STDEXT_GDS_COMMON_H__
