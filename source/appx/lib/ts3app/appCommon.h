
#ifndef __TS3APP_COMMON_APP_COMMON_H__
#define __TS3APP_COMMON_APP_COMMON_H__

namespace ts3
{

    class AppContext;

    using AppEntryPoint = __cdecl bool(*)( AppContext * );

}

#endif // __TS3APP_COMMON_APP_COMMON_H__
