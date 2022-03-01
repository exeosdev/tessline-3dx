
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
		
		/***********************************************************************************************************/
		/************************************************ std::array ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, size_t _N, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::array<_T, _N> & pArray,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalStdContainerByteSize( pArray, pCastTag );
		}

		template <typename _T, size_t _N, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				const std::array<_T, _N> & pArray,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeStdContainer( pOutputBuffer, pArray, pCastTag );
		}

		template <typename _T, size_t _N, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				std::array<_T, _N> & pArray,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeStdContainer(
				// std::array doesn't have any push/insert methods, so that's the one case where index becomes useful.
				// We simply assign the incoming elements under their indexes passed by the helper method. Simple.
				pInputData, pArray, pCastTag, []( auto & A, auto I, auto && E ) -> void { A[I] = std::move( E ); } );
		}


		/***********************************************************************************************************/
		/*********************************************** std::vector ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::vector<_T, _Alloc> & pVector,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalStdContainerByteSize( pVector, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				const std::vector<_T, _Alloc> & pVector,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeStdContainer( pOutputBuffer, pVector, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				std::vector<_T, _Alloc> & pVector,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeStdContainer(
				pInputData, pVector, pCastTag, []( auto & V, auto, auto && E ) -> void { V.push_back( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************ std::list ************************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::list<_T, _Alloc> & pList,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalStdContainerByteSize( pList, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				const std::list<_T, _Alloc> & pList,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeStdContainer( pOutputBuffer, pList, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				std::list<_T, _Alloc> & pList,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeStdContainer(
				pInputData, pList, pCastTag, []( auto & L, auto, auto && E ) -> void { L.push_back( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************ std::deque ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::deque<_T, _Alloc> & pDeque,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalStdContainerByteSize( pDeque, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t queryMinByteSize(
				const std::deque<_T, _Alloc> & pDeque,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return queryStdContainerMinByteSize( pDeque, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				const std::deque<_T, _Alloc> & pDeque,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeStdContainer( pOutputBuffer, pDeque, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				std::deque<_T, _Alloc> & pDeque,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeStdContainer(
				pInputData, pDeque, pCastTag, []( auto & D, auto, auto && E ) -> void { D.push_back( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************ std::queue ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::queue<_T, _Cnt> & pQueue,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalStdContainerByteSize( pQueue, pCastTag );
		}

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				const std::queue<_T, _Cnt> & pQueue,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeStdContainer( pOutputBuffer, pQueue, pCastTag );
		}

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				std::queue<_T, _Cnt> & pQueue,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeStdContainer(
				pInputData, pQueue, pCastTag, []( auto & Q, auto, auto && E ) -> void { Q.push( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************ std::stack ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::stack<_T, _Cnt> & pStack,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalStdContainerByteSize( pStack, pCastTag );
		}

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				const std::stack<_T, _Cnt> & pStack,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeStdContainer( pOutputBuffer, pStack, pCastTag );
		}

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				std::stack<_T, _Cnt> & pStack,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeStdContainer(
				pInputData, pStack, pCastTag, []( auto & S, auto, auto && E ) -> void { S.push( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************* std::set ************************************************/
		/***********************************************************************************************************/

		template <typename _K, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::set<_K, _Cmp, _Alloc> & pSet,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalStdContainerByteSize( pSet, pCastTag );
		}

		template <typename _K, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				const std::set<_K, _Cmp, _Alloc> & pSet,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeStdContainer( pOutputBuffer, pSet, pCastTag );
		}

		template <typename _K, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				std::set<_K, _Cmp, _Alloc> & pSet,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeStdContainer(
				pInputData, pSet, pCastTag, []( auto & S, auto, auto && E ) -> void { S.insert( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/******************************************** std::unordered_set *******************************************/
		/***********************************************************************************************************/

		template <typename _K, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::unordered_set<_K, _H, _Eq, _Alloc> & pUOSet,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalStdContainerByteSize( pUOSet, pCastTag );
		}

		template <typename _K, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				const std::unordered_set<_K, _H, _Eq, _Alloc> & pUOSet,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeStdContainer( pOutputBuffer, pUOSet, pCastTag );
		}

		template <typename _K, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				std::unordered_set<_K, _H, _Eq, _Alloc> & pUOSet,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeStdContainer(
				pInputData, pUOSet, pCastTag, []( auto & S, auto, auto && E ) -> void { S.insert( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************* std::map ************************************************/
		/***********************************************************************************************************/

		template <typename _K, typename _V, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::map<_K, _V, _Cmp, _Alloc> & pStdMap,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalStdMapByteSize( pStdMap, pCastTag );
		}

		template <typename _K, typename _V, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				const std::map<_K, _V, _Cmp, _Alloc> & pStdMap,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeStdMap( pOutputBuffer, pStdMap, pCastTag );
		}

		template <typename _K, typename _V, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				std::map<_K, _V, _Cmp, _Alloc> & pStdMap,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeStdMap( pInputData, pStdMap, pCastTag );
		}


		/***********************************************************************************************************/
		/******************************************** std::unordered_map *******************************************/
		/***********************************************************************************************************/

		template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize(
				const std::unordered_map<_K, _V, _H, _Eq, _Alloc> & pUOStdMap,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalStdMapByteSize( pUOStdMap, pCastTag );
		}

		template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				const std::unordered_map<_K, _V, _H, _Eq, _Alloc> & pUOStdMap,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeStdMap( pOutputBuffer, pUOStdMap, pCastTag );
		}

		template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				std::unordered_map<_K, _V, _H, _Eq, _Alloc> & pUOStdMap,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeStdMap( pInputData, pUOStdMap, pCastTag );
		}

	}

}

#endif // __TS3_STDEXT_GDS_STD_STL_H__
