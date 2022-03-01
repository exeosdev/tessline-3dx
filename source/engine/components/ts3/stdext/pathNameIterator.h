
#ifndef __TS3_STDEXT_PATH_NAME_ITERATOR_H__
#define __TS3_STDEXT_PATH_NAME_ITERATOR_H__

#include "prerequisites.h"
#include <string>

namespace ts3
{

	class PathNameIterator
	{
	public:
		PathNameIterator() = default;

		PathNameIterator( PathNameIterator && ) = default;
		PathNameIterator & operator=( PathNameIterator && ) = default;

		PathNameIterator( const PathNameIterator & ) = default;
		PathNameIterator & operator=( const PathNameIterator & ) = default;

		explicit PathNameIterator( std::string pPath )
		: _currentPathComponent{}
		, _remainingPath( std::move( pPath ) )
		{
			_moveFwd();
		}

		explicit operator bool() const
		{
			return !_currentPathComponent.empty();
		}

		PathNameIterator & operator++()
		{
			_moveFwd();
			return *this;
		}

		PathNameIterator operator++( int )
		{
			PathNameIterator copy{ *this };
			_moveFwd();
			return copy;
		}

		const std::string & name() const
		{
			return _currentPathComponent;
		}

		bool empty() const
		{
			return _currentPathComponent.empty();
		}

		bool hasNext() const
		{
			return !_remainingPath.empty();
		}

	private:
		void _moveFwd()
		{
			std::string nextPathComponent;
			
			if( !_remainingPath.empty() )
			{
				auto startPos = _remainingPath.find_first_not_of( "/\\" );
				auto endPos = _remainingPath.find_first_of( "/\\", startPos );

				if( ( startPos != std::string::npos ) && ( endPos != startPos ) )
				{
					nextPathComponent = _remainingPath.substr( startPos, endPos - startPos );
				}

				if( nextPathComponent.empty() )
				{
					_remainingPath.clear();
				}
				else
				{
					endPos = _remainingPath.find_first_not_of( "/\\", endPos );
					_remainingPath.erase( 0, endPos );
				}
			}

			_currentPathComponent = std::move( nextPathComponent );
		}

	private:
		std::string _currentPathComponent;
		std::string _remainingPath;
	};

}

#endif // __TS3_STDEXT_PATH_NAME_ITERATOR_H__
