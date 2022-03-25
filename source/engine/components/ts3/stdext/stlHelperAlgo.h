
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
		template <typename TpRange, typename TpValue>
		typename TpRange::value_type * findByRefValueOrGetNull( TpRange & pRange, const TpValue & pRef )
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
		template <typename TpRange, typename TpValue>
		const typename TpRange::value_type * findByRefValueOrGetNull( const TpRange & pRange, const TpValue & pRef )
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
		template <typename TpRange, typename TpPredicate>
		typename TpRange::value_type * findIfOrGetNull( TpRange & pRange, TpPredicate pPredicate )
		{
			const auto elementIter = std::find_if( begin( pRange ), end( pRange ), std::forward<TpPredicate>( pPredicate ) );
			if( elementIter != end( pRange ) )
			{
				return &( *elementIter );
			}
			return nullptr;
		}

		/// @brief Finds a value in the specified range which fulfills the specified predicate. Returns the pointer to the element or nullptr.
		/// @param pRange A range to search withing. Must support begin()/end().
		/// @param pPredicate A predicate to evaluate on each element. Must be a callable, accepting a single range's value-type param.
		template <typename TpRange, typename TpPredicate>
		const typename TpRange::value_type * findIfOrGetNull( const TpRange & pRange, TpPredicate pPredicate )
		{
			const auto elementIter = std::find_if( begin( pRange ), end( pRange ), std::forward<TpPredicate>( pPredicate ) );
			if( elementIter != end( pRange ) )
			{
				return &( *elementIter );
			}
			return nullptr;
		}

		template <typename TpMap, typename TpKey = typename TpMap::key_type>
		typename TpMap::mapped_type * getMapValuePtrOrNull( TpMap & pMap, const TpKey & pKey )
		{
			const auto valueIter = pMap.find( pKey );
			if( valueIter != pMap.end() )
			{
				return &( valueIter->second );
			}
			return nullptr;
		}

		template <typename TpMap, typename TpKey = typename TpMap::key_type>
		const typename TpMap::mapped_type * getMapValuePtrOrNull( const TpMap & pMap, const TpKey & pKey )
		{
			const auto valueIter = pMap.find( pKey );
			if( valueIter != pMap.end() )
			{
				return &( valueIter->second );
			}
			return nullptr;
		}

		template <typename TpMap, typename TpKey = typename TpMap::key_type>
		typename TpMap::mapped_type & getMapValueRefOrDefault( const TpMap & pMap, const TpKey & pKey, typename TpMap::mapped_type & pDefault )
		{
			const auto valueIter = pMap.find( pKey );
			if( valueIter != pMap.end() )
			{
				return valueIter->second;
			}
			return pDefault;
		}

		template <typename TpMap, typename TpKey = typename TpMap::key_type>
		const typename TpMap::mapped_type & getMapValueRefOrDefault( const TpMap & pMap, const TpKey & pKey, const typename TpMap::mapped_type & pDefault )
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
