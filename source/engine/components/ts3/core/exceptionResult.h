
#pragma once

#ifndef __TS3_CORE_EXCEPTION_RESULT_H__
#define __TS3_CORE_EXCEPTION_RESULT_H__

#include "exceptionCommon.h"

namespace ts3
{

    /// @brief
    struct ResultWrapper
    {
    public:
        //
        Result result;
        //
        std::exception_ptr exception;

    public:
        ResultWrapper() = default;

        explicit operator bool() const
        {
            return !isError();
        }

        void setResult( Result pResult )
        {
            result = pResult;
        }

        void setException( std::exception_ptr pExceptionPtr )
        {
            exception = std::move( pExceptionPtr );
        }

        void saveCurrentException()
        {
            exception = std::current_exception();
        }

        void rethrowException()
        {
            auto localException = std::exception_ptr();
            std::swap( exception, localException );
            std::rethrow_exception( localException );
        }

        TS3_PCL_ATTR_NO_DISCARD bool hasException() const
        {
            return exception ? true : false;
        }

        TS3_PCL_ATTR_NO_DISCARD bool hasResult() const
        {
            return !result.empty();
        }

        TS3_PCL_ATTR_NO_DISCARD bool isError() const
        {
            return ts3ResultIsError( result ) || exception;
        }
    };

    /// @brief
    template <typename TpValue>
    struct ReturnValueWrapper : public ResultWrapper
    {
    public:
        //
        mutable TpValue value;

    public:
        ReturnValueWrapper() = default;

        explicit ReturnValueWrapper( ResultWrapper pResultWrapper )
        : ResultWrapper( std::move( pResultWrapper ) )
        {}

        TpValue & operator*() const
        {
            return value;
        }

        TpValue * operator->() const
        {
            return &( value );
        }
    };

    /// @brief
    template <>
    struct ReturnValueWrapper<void> : public ResultWrapper
    {
    public:
        explicit ReturnValueWrapper() = default;
    };

} // namespace ts3

#endif // __TS3_CORE_EXCEPTION_RESULT_H__
