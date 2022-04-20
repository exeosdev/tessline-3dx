
#include "eventSource.h"
#include "eventCore.h"

namespace ts3::system
{

	EventSource::EventSource( SysContextHandle pSysContext )
	: SysObject( std::move( pSysContext ) )
	{}

	EventSource::~EventSource() noexcept
	{
		if( _eventControllerActiveRef )
		{
			_eventControllerActiveRef->onEventSourceDestroy( *this );
			_eventControllerActiveRef = nullptr;
		}
	}

	void * EventSource::getEventSourceNativeData() const
	{
		return _eventSourceNativeData;
	}

	bool EventSource::isPrimaryEventSource() const
	{
		return _eventControllerActiveRef && ( this == _eventControllerActiveRef->getPrimaryEventSource() );
	}

	bool EventSource::isLastEventSource() const
	{
		if( _eventControllerActiveRef && ( _eventControllerActiveRef->getRegisteredEventSourcesNum() == 1 ) )
		{
			// EventController has one event source registered. They are indexed starting from 0 - get that last one.
			auto * lastRegEventSource = _eventControllerActiveRef->getRegisteredEventSourceByIndex( 0 );

			if( this == lastRegEventSource )
			{
				return true;
			}

			// This is veeery weird. If this source has a controller set (which means it is a registered event source),
			// it definitely should be present on that controller's event source list. Investigate if this gets triggered.
			ts3DebugInterrupt();
		}

		return false;
	}

	void EventSource::onDestroySystemObjectRequested()
	{
		if( _eventControllerActiveRef )
		{
			_eventControllerActiveRef->onEventSourceDestroy( *this );
			_eventControllerActiveRef = nullptr;
		}
	}

	void EventSource::setEventController( EventControllerHandle pEventController )
	{
		_eventControllerActiveRef = std::move( pEventController );
	}

	void EventSource::setEventSourceNativeData( void * pNativeData )
	{
		_eventSourceNativeData = pNativeData;
	}

	void EventSource::resetEventSourceNativeData()
	{
		_eventSourceNativeData = nullptr;
	}

} // namespace ts3::system
