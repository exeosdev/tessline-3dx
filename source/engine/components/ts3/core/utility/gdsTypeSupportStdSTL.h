
#ifndef __TS3_CORE_GDS_TYPE_SUPPORT_STD_STL_H_H__
#define __TS3_CORE_GDS_TYPE_SUPPORT_STD_STL_H_H__

#include "gdsTypeSupportStdCore.h"
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
		inline gds_size_t evalByteSize( const std::array<_T, _N> & pArray,
                                        const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalContainerByteSize( pArray, pCastTag );
		}

		template <typename _T, size_t _N, typename _Cast = void>
		inline gds_size_t serialize( byte * pOutputBuffer,
                                     const std::array<_T, _N> & pArray,
                                     const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeContainer( pOutputBuffer, pArray, pCastTag );
		}

		template <typename _T, size_t _N, typename _Cast = void>
		inline gds_size_t deserialize( const byte * pInputData,
                                       std::array<_T, _N> & pArray,
                                       const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeContainer(
				// std::array doesn't have any push/insert methods, so that's the one case where index becomes useful.
				// We simply assign the incoming elements under their indexes passed by the helper method. Simple.
				pInputData, pArray, pCastTag, []( auto & A, auto I, auto && E ) -> void { A[I] = std::move( E ); } );
		}


		/***********************************************************************************************************/
		/*********************************************** std::vector ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize( const std::vector<_T, _Alloc> & pVector,
                                        const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalContainerByteSize( pVector, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize( byte * pOutputBuffer,
                                     const std::vector<_T, _Alloc> & pVector,
                                     const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeContainer( pOutputBuffer, pVector, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize( const byte * pInputData,
                                       std::vector<_T, _Alloc> & pVector,
                                       const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeContainer(
				pInputData, pVector, pCastTag, []( auto & V, auto, auto && E ) -> void { V.push_back( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************ std::list ************************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize( const std::list<_T, _Alloc> & pList,
                                        const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalContainerByteSize( pList, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize( byte * pOutputBuffer,
                                     const std::list<_T, _Alloc> & pList,
                                     const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeContainer( pOutputBuffer, pList, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize( const byte * pInputData,
                                       std::list<_T, _Alloc> & pList,
                                       const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeContainer(
				pInputData, pList, pCastTag, []( auto & L, auto, auto && E ) -> void { L.push_back( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************ std::deque ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize( const std::deque<_T, _Alloc> & pDeque,
                                        const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalContainerByteSize( pDeque, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t queryMinByteSize( const std::deque<_T, _Alloc> & pDeque,
                                            const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return queryContainerMinByteSize( pDeque, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize( byte * pOutputBuffer,
                                     const std::deque<_T, _Alloc> & pDeque,
                                     const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeContainer( pOutputBuffer, pDeque, pCastTag );
		}

		template <typename _T, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize( const byte * pInputData,
                                       std::deque<_T, _Alloc> & pDeque,
                                       const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeContainer(
				pInputData, pDeque, pCastTag, []( auto & D, auto, auto && E ) -> void { D.push_back( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************ std::queue ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t evalByteSize( const std::queue<_T, _Cnt> & pQueue,
                                        const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalContainerByteSize( pQueue, pCastTag );
		}

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t serialize( byte * pOutputBuffer,
                                     const std::queue<_T, _Cnt> & pQueue,
                                     const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeContainer( pOutputBuffer, pQueue, pCastTag );
		}

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t deserialize( const byte * pInputData,
                                       std::queue<_T, _Cnt> & pQueue,
                                       const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeContainer(
				pInputData, pQueue, pCastTag, []( auto & Q, auto, auto && E ) -> void { Q.push( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************ std::stack ***********************************************/
		/***********************************************************************************************************/

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t evalByteSize( const std::stack<_T, _Cnt> & pStack,
                                        const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalContainerByteSize( pStack, pCastTag );
		}

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t serialize( byte * pOutputBuffer,
                                     const std::stack<_T, _Cnt> & pStack,
                                     const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeContainer( pOutputBuffer, pStack, pCastTag );
		}

		template <typename _T, typename _Cnt, typename _Cast = void>
		inline gds_size_t deserialize( const byte * pInputData,
                                       std::stack<_T, _Cnt> & pStack,
                                       const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeContainer(
				pInputData, pStack, pCastTag, []( auto & S, auto, auto && E ) -> void { S.push( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************* std::set ************************************************/
		/***********************************************************************************************************/

		template <typename _K, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize( const std::set<_K, _Cmp, _Alloc> & pSet,
                                        const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalContainerByteSize( pSet, pCastTag );
		}

		template <typename _K, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize( byte * pOutputBuffer,
                                     const std::set<_K, _Cmp, _Alloc> & pSet,
                                     const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeContainer( pOutputBuffer, pSet, pCastTag );
		}

		template <typename _K, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize( const byte * pInputData,
                                       std::set<_K, _Cmp, _Alloc> & pSet,
                                       const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeContainer(
				pInputData, pSet, pCastTag, []( auto & S, auto, auto && E ) -> void { S.insert( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/******************************************** std::unordered_set *******************************************/
		/***********************************************************************************************************/

		template <typename _K, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize( const std::unordered_set<_K, _H, _Eq, _Alloc> & pUOSet,
                                        const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalContainerByteSize( pUOSet, pCastTag );
		}

		template <typename _K, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize( byte * pOutputBuffer,
                                     const std::unordered_set<_K, _H, _Eq, _Alloc> & pUOSet,
                                     const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeContainer( pOutputBuffer, pUOSet, pCastTag );
		}

		template <typename _K, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize( const byte * pInputData,
                                       std::unordered_set<_K, _H, _Eq, _Alloc> & pUOSet,
                                       const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeContainer(
				pInputData, pUOSet, pCastTag, []( auto & S, auto, auto && E ) -> void { S.insert( std::move( E ) ); } );
		}


		/***********************************************************************************************************/
		/************************************************* std::map ************************************************/
		/***********************************************************************************************************/

		template <typename _K, typename _V, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize( const std::map<_K, _V, _Cmp, _Alloc> & pMap,
                                        const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalMapByteSize( pMap, pCastTag );
		}

		template <typename _K, typename _V, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize( byte * pOutputBuffer,
                                     const std::map<_K, _V, _Cmp, _Alloc> & pMap,
                                     const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeMap( pOutputBuffer, pMap, pCastTag );
		}

		template <typename _K, typename _V, typename _Cmp, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize( const byte * pInputData,
                                       std::map<_K, _V, _Cmp, _Alloc> & pMap,
                                       const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeMap( pInputData, pMap, pCastTag );
		}


		/***********************************************************************************************************/
		/******************************************** std::unordered_map *******************************************/
		/***********************************************************************************************************/

		template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize( const std::unordered_map<_K, _V, _H, _Eq, _Alloc> & pUOMap,
                                        const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return evalMapByteSize( pUOMap, pCastTag );
		}

		template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize( byte * pOutputBuffer,
                                     const std::unordered_map<_K, _V, _H, _Eq, _Alloc> & pUOMap,
                                     const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return serializeMap( pOutputBuffer, pUOMap, pCastTag );
		}

		template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize( const byte * pInputData,
                                       std::unordered_map<_K, _V, _H, _Eq, _Alloc> & pUOMap,
                                       const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			return deserializeMap( pInputData, pUOMap, pCastTag );
		}

	}

}

#endif // __TS3_CORE_GDS_TYPE_SUPPORT_STD_STL_H_H__
