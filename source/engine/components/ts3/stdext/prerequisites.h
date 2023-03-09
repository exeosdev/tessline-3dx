
#ifndef __TS3_STDEXT_PREREQUISITES_H__
#define __TS3_STDEXT_PREREQUISITES_H__

#include <ts3/platform/debug.h>

#include <algorithm>
#include <type_traits>
#include <utility>

namespace ts3
{

	namespace cxdefs
	{

		/// @brief Represents max value which can be stored in the size_t type.
		inline constexpr size_t MAX_SIZE = static_cast<size_t>( -1 );

		/// @brief Represents and invalid position (e.g. in a container or a continuous range).
		inline constexpr size_t INVALID_POSITION = static_cast<size_t>( -1 );

	}

	template <typename TType>
	struct ArrayDelete
	{
		void operator()( TType * pInputPtr ) const
		{
			delete[] pInputPtr;
		}
	};

	template <typename TType>
	struct DefaultDelete
	{
		void operator()( TType * pInputPtr ) const
		{
			delete pInputPtr;
		}
	};

	template <typename TType>
	struct EmptyDelete
	{
		void operator()( TType * ) const
		{}
	};

	template <typename TResult, typename TInput>
	inline TResult numeric_cast( TInput pInput )
	{
		ts3DebugAssert( pInput <= std::numeric_limits<TResult>::max() );
		return static_cast<TResult>( pInput );
	}

}

#endif // __TS3_STDEXT_PREREQUISITES_H__
