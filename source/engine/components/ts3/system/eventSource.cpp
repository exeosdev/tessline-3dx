
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
        return _eventSourceNativeData.get();
    }

    void * EventSource::getEventSourcePlatformData() const
    {
        return _eventSourcePlatformData.get();
    }

    void EventSource::setEventController( EventControllerHandle pEventController )
    {
        _eventControllerActiveRef = pEventController;
    }

    void EventSource::setEventSourceNativeData( EventSourceInternalDataPtr pNativeDataPtr )
    {
        _eventSourceNativeData = std::move( pNativeDataPtr );
    }

    void EventSource::setEventSourceNativeData( void * pNativeData, EventSourceInternalDataDeleter pDeleter )
    {
        _eventSourceNativeData = EventSourceInternalDataPtr{ pNativeData, std::move( pDeleter ) };
    }

    void EventSource::resetEventSourceNativeData()
    {
        _eventSourceNativeData.reset();
    }

    void EventSource::setEventSourcePlatformData( EventSourceInternalDataPtr pPlatformDataPtr )
    {
        _eventSourcePlatformData = std::move( pPlatformDataPtr );
    }

    void EventSource::setEventSourcePlatformData( void * pPlatformData, EventSourceInternalDataDeleter pDeleter )
    {
        _eventSourcePlatformData = EventSourceInternalDataPtr{ pPlatformData, std::move( pDeleter ) };
    }

    void EventSource::resetEventSourcePlatformData()
    {
        _eventSourcePlatformData.reset();
    }

} // namespace ts3::system
