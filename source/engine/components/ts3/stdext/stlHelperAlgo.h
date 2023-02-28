
#ifndef __TS3_STDEXT_STL_HELPER_ALGO_H__
#define __TS3_STDEXT_STL_HELPER_ALGO_H__

#include "prerequisites.h"

namespace ts3
{

	namespace stdx
	{

		/// @brief Finds a value in the specified range which equals to the ref value. Returns the pointer to the element or nullptr.
		/// @param pRange A range to search withing. Must support begin()/end().
		/// @param pRef A reference value to compare against.
		template <typename TRange, typename TValue>
		typename TRange::value_type * findByRefValueOrGetNull( TRange & pRange, const TValue & pRef )
		{
			const auto elementIter = std::find_if( begin( pRange ), end( pRange ), [&pRef]( const auto & pElement ) -> bool {
				return pRef == pElement;
			});
			if( elementIter != end( pRange ) )
			{
				return &( *elementIter );
			}
			return nullptr;
		}

		/// @brief Finds a value in the specified range which equals to the ref value. Returns the pointer to the element or nullptr.
		/// @param pRange A range to search withing. Must support begin()/end().
		/// @param pRef A reference value to compare against.
		template <typename TRange, typename TValue>
		const typename TRange::value_type * findByRefValueOrGetNull( const TRange & pRange, const TValue & pRef )
		{
			const auto elementIter = std::find_if( begin( pRange ), end( pRange ), [&pRef]( const auto & pElement ) -> bool {
				return pRef == pElement;
			});
			if( elementIter != end( pRange ) )
			{
				return &( *elementIter );
			}
			return nullptr;
		}

		/// @brief Finds a value in the specified range which fulfills the specified predicate. Returns the pointer to the element or nullptr.
		/// @param pRange A range to search withing. Must support begin()/end().
		/// @param pPredicate A predicate to evaluate on each element. Must be a callable, accepting a single range's value-type param.
		template <typename TRange, typename TPredicate>
		typename TRange::value_type * findIfOrGetNull( TRange & pRange, TPredicate pPredicate )
		{
			const auto elementIter = std::find_if( begin( pRange ), end( pRange ), std::forward<TPredicate>( pPredicate ) );
			if( elementIter != end( pRange ) )
			{
				return &( *elementIter );
			}
			return nullptr;
		}

		/// @brief Finds a value in the specified range which fulfills the specified predicate. Returns the pointer to the element or nullptr.
		/// @param pRange A range to search withing. Must support begin()/end().
		/// @param pPredicate A predicate to evaluate on each element. Must be a callable, accepting a single range's value-type param.
		template <typename TRange, typename TPredicate>
		const typename TRange::value_type * findIfOrGetNull( const TRange & pRange, TPredicate pPredicate )
		{
			const auto elementIter = std::find_if( begin( pRange ), end( pRange ), std::forward<TPredicate>( pPredicate ) );
			if( elementIter != end( pRange ) )
			{
				return &( *elementIter );
			}
			return nullptr;
		}

		template <typename TMap, typename TKey = typename TMap::key_type>
		typename TMap::mapped_type * getMapValuePtrOrNull( TMap & pMap, const TKey & pKey )
		{
			const auto valueIter = pMap.find( pKey );
			if( valueIter != pMap.end() )
			{
				return &( valueIter->second );
			}
			return nullptr;
		}

		template <typename TMap, typename TKey = typename TMap::key_type>
		const typename TMap::mapped_type * getMapValuePtrOrNull( const TMap & pMap, const TKey & pKey )
		{
			const auto valueIter = pMap.find( pKey );
			if( valueIter != pMap.end() )
			{
				return &( valueIter->second );
			}
			return nullptr;
		}

		template <typename TMap, typename TKey = typename TMap::key_type>
		typename TMap::mapped_type & getMapValueRefOrDefault( const TMap & pMap, const TKey & pKey, typename TMap::mapped_type & pDefault )
		{
			const auto valueIter = pMap.find( pKey );
			if( valueIter != pMap.end() )
			{
				return valueIter->second;
			}
			return pDefault;
		}

		template <typename TMap, typename TKey = typename TMap::key_type>
		const typename TMap::mapped_type & getMapValueRefOrDefault( const TMap & pMap, const TKey & pKey, const typename TMap::mapped_type & pDefault )
		{
			const auto valueIter = pMap.find( pKey );
			if( valueIter != pMap.end() )
			{
				return valueIter->second;
			}
			return pDefault;
		}



	} // namespace stdx

} // namespace ts3

#endif // __TS3_STDEXT_STL_HELPER_ALGO_H__
