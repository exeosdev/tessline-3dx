
#ifndef __TS3_SYSTEM_EVENT_DEF_WINDOW_H__
#define __TS3_SYSTEM_EVENT_DEF_WINDOW_H__

#include "eventCommon.h"
#include "windowCommon.h"

namespace ts3::system
{

    template <event_code_value_t tpEventCode>
    struct EvtWindow : public EvtBase
    {
    public:
        EventSource * eventSource = nullptr;

    public:
        constexpr EvtWindow()
        : EvtBase( tpEventCode )
        {}

        template <typename TpObject>
        bool checkEventSource( const TpObject * pSource ) const
        {
            return static_cast<const void *>( pSource ) == eventSource;
        }
    };

    struct EvtWindowUpdateClose : public EvtWindow<E_EVENT_CODE_WINDOW_UPDATE_CLOSE>
    {
    };

    struct EvtWindowUpdateFullscreen : public EvtWindow<E_EVENT_CODE_WINDOW_UPDATE_FULLSCREEN>
    {
    };

    struct EvtWindowUpdateResize : public EvtWindow<E_EVENT_CODE_WINDOW_UPDATE_RESIZE>
    {
        WindowSize newSize;
    };

    struct EvtWindowUpdateVisibility : public EvtWindow<E_EVENT_CODE_WINDOW_UPDATE_VISIBILITY>
    {
        bool newVisibilityState;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_DEF_WINDOW_H__
