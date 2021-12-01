
#ifndef __TS3_STDEXT_GDS_STD_STL_H__
#define __TS3_STDEXT_GDS_STD_STL_H__

#include "gdsStdCore.h"
#include <ts3/platform/gds.h>

namespace std
{

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

	namespace gds
	{

		struct StdContainerProxy
		{

			/// @brief Computes the actual size of binary representation of an std:: container.
			template <template <typename...> typename TpC, typename... TpCArgs, typename TpCast>
			static inline gds_size_t evalByteSize( const TpC<TpCArgs...> & pContainer, const TpCast & pCastTag )
			{
				// Compute the size for the specified container.
				// The space required is the serialized size + binary representation of all elements in the container.

				gds_size_t byteSize = 0;

				// The first member is the number of elements in a container. We are not interested in its value, though.
				byteSize += gds::evalByteSize( emptySizeType() );

				// Enumerate over the container's elements.
				for( const auto & entry : pContainer )
				{
					// Evaluate the size of each element. Propagate cast tag to support container-wise value casting.
					byteSize += gds::evalByteSize( entry, pCastTag );
				}

				return byteSize;
			}

			/// @brief Computes the minimum size of a binary representation of an std:: container.
			template <template <typename...> typename TpC, typename... TpCArgs, typename TpCast>
			static inline gds_size_t queryMinByteSize( const TpC<TpCArgs...> & pContainer, const TpCast & pCastTag )
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
			static inline gds_size_t serialize(
					byte * pOutputBuffer,
					EByteOrder pByteOrder,
					const TpC<TpCArgs...> & pContainer,
					const TpCast & pCastTag )
			{
				// First, get the size of the container (number of elements).
				size_t containerSize = std::size( pContainer );

				// size_t may have different sizes on different platforms. asSizeTypeValue() ensures this is properly
				// handled by the GDS, so we can deserialize the data on a different platform, and get expected results.
				gds_size_t byteSize = gds::serialize( pOutputBuffer, pByteOrder, asSizeType( containerSize ) );

				// Enumerate over the container's elements.
				for( const auto & entry : pContainer )
				{
					// Serialize the element, add the size to the total byte size.
					// Propagate cast tag to support container-wise value casting.
					byteSize += gds::serialize( pOutputBuffer + byteSize, pByteOrder, entry, pCastTag );
				}

				return byteSize;
			}

			template <template <typename...> typename TpC, typename... TpCArgs, typename TpCast, typename TpIns>
			static inline gds_size_t deserialize(
					const byte * pInputData,
					EByteOrder pByteOrder,
					TpC<TpCArgs...> & pContainer,
					const TpCast & pCastTag,
					const TpIns & pInsertCallback )
			{
				// The variable to store the container's size.
				size_t containerSize = 0;

				// Deserialize the size. Since we used asSizeTypeValue() during serialization, we must now use asSizeTypeRef()
				// to properly retrieve the deserialized value. The output will be properly stored in the wrapped variable.
				gds_size_t byteSize = gds::deserialize( pInputData, pByteOrder, asSizeType( containerSize ) );

				for( size_t entryIndex = 0; entryIndex < containerSize; ++entryIndex )
				{
					// Temporary for the deserialized state.
					typename TpC<TpCArgs...>::value_type tempEntry{};

					// Deserialize current element. Propagate cast tag to support container-wise value casting.
					byteSize += gds::deserialize( pInputData + byteSize, pByteOrder, tempEntry, pCastTag );

					// Call the insert callback to add it to the container. Index is needed e.g. for std::array.
					pInsertCallback( pContainer, entryIndex, std::move( tempEntry ) );
				}

				return byteSize;
			}

			/// @brief
			template <template <typename...> typename TpM, typename TpK, typename TpV, typename... TpCArgs, typename TpCast>
			static inline gds_size_t serializeMap(
					byte * pOutputBuffer,
					EByteOrder pByteOrder,
					const TpM<TpK, TpV, TpCArgs...> & pMap,
					const TpCast & pCastTag )
			{
				size_t mapSize = std::size( pMap );

				gds_size_t byteSize = gds::serialize( pOutputBuffer, pByteOrder, asSizeType( mapSize ) );

				for( const auto & mapEntry : pMap )
				{
					byteSize += gds::serialize( pOutputBuffer + byteSize, pByteOrder, mapEntry.first );
					byteSize += gds::serialize( pOutputBuffer + byteSize, pByteOrder, mapEntry.second, pCastTag );
				}

				return byteSize;
			}

			template <template <typename...> typename TpM, typename TpK, typename TpV, typename... TpCArgs, typename TpCast>
			static inline gds_size_t deserializeMap(
					const byte * pInputData,
					EByteOrder pByteOrder,
					TpM<TpK, TpV, TpCArgs...> & pMap,
					const TpCast & pCastTag )
			{
				size_t mapSize = 0;

				gds_size_t byteSize = gds::deserialize( pInputData, pByteOrder, asSizeType( mapSize ) );

				for( size_t entryIndex = 0; entryIndex < mapSize; ++entryIndex )
				{
					TpK tempKey{};
					byteSize += gds::deserialize( pInputData + byteSize, pByteOrder, tempKey );

					TpV tempValue{};
					byteSize += gds::deserialize( pInputData + byteSize, pByteOrder, tempValue, pCastTag );

					pMap.emplace( std::move( tempKey ), std::move( tempValue ) );
				}

				return byteSize;
			}
		};


		/***********************************************************************************************************/
		/************************************************ std::array ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, size_t _N, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::array<_T, _N> & pArray,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::evalByteSize( pArray, pCastTag );
		}

		template <typename _T, size_t _N, typename _Cast = void>
		inline gds_size_t queryMinByteSize(
				const std::array<_T, _N> & pArray,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::queryMinByteSize( pArray, pCastTag );
		}

		template <typename _T, size_t _N, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				EByteOrder pByteOrder,
				const std::array<_T, _N> & pArray,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::serialize( pOutputBuffer, pByteOrder, pArray, pCastTag );
		}

		template <typename _T, size_t _N, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				EByteOrder pByteOrder,
				std::array<_T, _N> & pArray,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::deserialize(
				// std::array doesn't have any push/insert methods, so that's the one case where index becomes useful.
				// We simply assign the incoming elements under their indexes passed by the helper method. Simple.
				pInputData, pByteOrder, pArray, pCastTag, []( auto & A, auto I, auto && E ) -> void { A[I] = std::move( E ); } );
		}


		/***********************************************************************************************************/
		/*********************************************** std::vector ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::vector<_T, _Alloc> & pVector,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::evalByteSize( pVector, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t queryMinByteSize(
				const std::vector<_T, _Alloc> & pVector,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::queryMinByteSize( pVector, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				EByteOrder pByteOrder,
				const std::vector<_T, _Alloc> & pVector,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::serialize( pOutputBuffer, pByteOrder, pVector, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				EByteOrder pByteOrder,
				std::vector<_T, _Alloc> & pVector,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::deserialize(
				pInputData, pByteOrder, pVector, pCastTag, []( auto & V, auto, auto && E ) -> void { V.push_back( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************ std::list ************************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::list<_T, _Alloc> & pList,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::evalByteSize( pList, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t queryMinByteSize(
				const std::list<_T, _Alloc> & pList,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::queryMinByteSize( pList, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				EByteOrder pByteOrder,
				const std::list<_T, _Alloc> & pList,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::serialize( pOutputBuffer, pByteOrder, pList, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				EByteOrder pByteOrder,
				std::list<_T, _Alloc> & pList,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::deserialize(
				pInputData, pByteOrder, pList, pCastTag, []( auto & L, auto, auto && E ) -> void { L.push_back( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************ std::deque ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::deque<_T, _Alloc> & pDeque,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::evalByteSize( pDeque, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t queryMinByteSize(
				const std::deque<_T, _Alloc> & pDeque,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::queryMinByteSize( pDeque, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				EByteOrder pByteOrder,
				const std::deque<_T, _Alloc> & pDeque,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::serialize( pOutputBuffer, pByteOrder, pDeque, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				EByteOrder pByteOrder,
				std::deque<_T, _Alloc> & pDeque,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::deserialize(
				pInputData, pByteOrder, pDeque, pCastTag, []( auto & D, auto, auto && E ) -> void { D.push_back( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************ std::queue ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::queue<_T, _Cnt> & pQueue,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::evalByteSize( pQueue, pCastTag );
		}

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t queryMinByteSize(
				const std::queue<_T, _Cnt> & pQueue,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::queryMinByteSize( pQueue, pCastTag );
		}

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				EByteOrder pByteOrder,
				const std::queue<_T, _Cnt> & pQueue,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::serialize( pOutputBuffer, pByteOrder, pQueue, pCastTag );
		}

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				EByteOrder pByteOrder,
				std::queue<_T, _Cnt> & pQueue,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::deserialize(
				pInputData, pByteOrder, pQueue, pCastTag, []( auto & Q, auto, auto && E ) -> void { Q.push( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************ std::stack ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::stack<_T, _Cnt> & pStack,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::evalByteSize( pStack, pCastTag );
		}

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t queryMinByteSize(
				const std::stack<_T, _Cnt> & pStack,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::queryMinByteSize( pStack, pCastTag );
		}

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				EByteOrder pByteOrder,
				const std::stack<_T, _Cnt> & pStack,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::serialize( pOutputBuffer, pByteOrder, pStack, pCastTag );
		}

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				EByteOrder pByteOrder,
				std::stack<_T, _Cnt> & pStack,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::deserialize(
				pInputData, pByteOrder, pStack, pCastTag, []( auto & S, auto, auto && E ) -> void { S.push( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************* std::set ************************************************/
		/***********************************************************************************************************/

		template <typename _K, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::set<_K, _Cmp, _Alloc> & pSet,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::evalByteSize( pSet, pCastTag );
		}

		template <typename _K, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t queryMinByteSize(
				const std::set<_K, _Cmp, _Alloc> & pSet,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::queryMinByteSize( pSet, pCastTag );
		}

		template <typename _K, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				EByteOrder pByteOrder,
				const std::set<_K, _Cmp, _Alloc> & pSet,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::serialize( pOutputBuffer, pByteOrder, pSet, pCastTag );
		}

		template <typename _K, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				EByteOrder pByteOrder,
				std::set<_K, _Cmp, _Alloc> & pSet,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::deserialize(
				pInputData, pByteOrder, pSet, pCastTag, []( auto & S, auto, auto && E ) -> void { S.insert( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/******************************************** std::unordered_set *******************************************/
		/***********************************************************************************************************/

		template <typename _K, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::unordered_set<_K, _H, _Eq, _Alloc> & pUOSet,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::evalByteSize( pUOSet, pCastTag );
		}

		template <typename _K, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t queryMinByteSize(
				const std::unordered_set<_K, _H, _Eq, _Alloc> & pUOSet,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::queryMinByteSize( pUOSet, pCastTag );
		}

		template <typename _K, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				EByteOrder pByteOrder,
				const std::unordered_set<_K, _H, _Eq, _Alloc> & pUOSet,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::serialize( pOutputBuffer, pByteOrder, pUOSet, pCastTag );
		}

		template <typename _K, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				EByteOrder pByteOrder,
				std::unordered_set<_K, _H, _Eq, _Alloc> & pUOSet,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::deserialize(
				pInputData, pByteOrder, pUOSet, pCastTag, []( auto & S, auto, auto && E ) -> void { S.insert( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************* std::map ************************************************/
		/***********************************************************************************************************/

		template <typename _K, typename _V, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::map<_K, _V, _Cmp, _Alloc> & pMap,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::evalByteSize( pMap, pCastTag );
		}

		template <typename _K, typename _V, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t queryMinByteSize(
				const std::map<_K, _V, _Cmp, _Alloc> & pMap,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::queryMinByteSize( pMap, pCastTag );
		}

		template <typename _K, typename _V, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				EByteOrder pByteOrder,
				const std::map<_K, _V, _Cmp, _Alloc> & pMap,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::serializeMap( pOutputBuffer, pByteOrder, pMap, pCastTag );
		}

		template <typename _K, typename _V, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				EByteOrder pByteOrder,
				std::map<_K, _V, _Cmp, _Alloc> & pMap,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::deserializeMap( pInputData, pByteOrder, pMap, pCastTag );
		}


		/***********************************************************************************************************/
		/******************************************** std::unordered_map *******************************************/
		/***********************************************************************************************************/

		template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::unordered_map<_K, _V, _H, _Eq, _Alloc> & pUOMap,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::evalByteSize( pUOMap, pCastTag );
		}

		template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t queryMinByteSize(
				const std::unordered_map<_K, _V, _H, _Eq, _Alloc> & pUOMap,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::queryMinByteSize( pUOMap, pCastTag );
		}

		template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				EByteOrder pByteOrder,
				const std::unordered_map<_K, _V, _H, _Eq, _Alloc> & pUOMap,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::serializeMap( pOutputBuffer, pByteOrder, pUOMap, pCastTag );
		}

		template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				EByteOrder pByteOrder,
				std::unordered_map<_K, _V, _H, _Eq, _Alloc> & pUOMap,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return StdContainerProxy::deserializeMap( pInputData, pByteOrder, pUOMap, pCastTag );
		}

	}

}

#endif // __TS3_STDEXT_GDS_STD_STL_H__
