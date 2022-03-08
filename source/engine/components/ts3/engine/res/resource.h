
#pragma once

#ifndef __TS3_ENGINE_RES_RESOURCE_H__
#define __TS3_ENGINE_RES_RESOURCE_H__

#include "../prerequisites.h"

namespace ts3
{

    class Resource : public DynamicInterface
    {
    public:
        Resource() = default;
        virtual ~Resource() = default;
    };

}

#endif // __TS3_ENGINE_RES_RESOURCE_H__
