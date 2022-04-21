
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DXCOMMON_ERROR_UTILS_H__
#define __TS3DRIVER_GPUAPI_DXCOMMON_ERROR_UTILS_H__

#include <ts3/core/exception.h>

namespace ts3::gpuapi
{

	class HresultErrorException : public UserExternalException
	{
	public:
		using BaseException = UserExternalException;

		explicit HresultErrorException( ts3::ExceptionInfo pInfo )
		: BaseException( std::move( pInfo ) )
		{}
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DXCOMMON_ERROR_UTILS_H__
