
#pragma once

#ifndef __TS3_CORE_STD_CONTAINER_PROXY_H__
#define __TS3_CORE_STD_CONTAINER_PROXY_H__

#include "../prerequisites.h"
#include <memory>

namespace std
{

    template <class _Ty, class _Alloc>
    class list;

}

namespace ts3
{

    template <typename Tp, typename TpAllocator>
    struct ListProxy
    {
        using Type = std::list<Tp, TpAllocator>;
    };

    template < typename Tp, typename TpAllocator = std::allocator<Tp> >
    using List = typename ListProxy<Tp, TpAllocator>::Type;

}

#endif // __TS3_CORE_STD_CONTAINER_PROXY_H__
