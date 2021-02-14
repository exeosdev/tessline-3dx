
#pragma once

#ifndef __TS3_CORE_SIGNAL_PROXY_H__
#define __TS3_CORE_SIGNAL_PROXY_H__

#include "signalCommon.h"

namespace ts3
{

	class SignalProxy
	{
	public:
		SignalProxy()
		{}

		virtual ~SignalProxy()
		{}

		virtual void onConnect( signal_code_value_t pSignalCode, void * pReceiver ) = 0;

		virtual void onDisconnect( signal_code_value_t pSignalCode, void * pReceiver, bool pResult ) = 0;

		virtual void onEmit( signal_code_value_t pSignalCode ) = 0;

		virtual void onHandler( signal_code_value_t pSignalCode, void * pReceiver ) = 0;
	};

}

#endif // __TS3_CORE_SIGNAL_PROXY_H__
