
#pragma once

#ifndef __TS3_CORE_SIGNAL_EMITTER_H__
#define __TS3_CORE_SIGNAL_EMITTER_H__

#include "signalProxy.h"

namespace ts3
{

	template <typename TpSignal>
	class SignalEmitter;

	template <signal_code_value_t tpSignalCode, typename... TpSignalArgs>
	class SignalEmitter< Signal<tpSignalCode, TpSignalArgs...> >
	{
	public:
		using Handler = std::function<void( TpSignalArgs &&... )>;

		struct HandlerInfo
		{
			Handler handler;
			void * receiver;
		};

	public:
		SignalEmitter( SignalProxy * pSignalProxy = nullptr )
		: _signalProxy( pSignalProxy )
		{}

		void setProxy( SignalProxy * pSignalProxy )
		{
			_signalProxy = pSignalProxy;
		}

		void emit( TpSignalArgs &&... pArgs )
		{
			if ( _signalProxy != nullptr )
			{
				_signalProxy->onEmit( tpSignalCode );
			}

			for ( auto & handlerInfo : _handlers )
			{
				if ( _signalProxy != nullptr )
				{
					_signalProxy->onHandler( tpSignalCode, handlerInfo.receiver );
				}

				handlerInfo.handler( std::forward<TpSignalArgs>( pArgs )... );
			}
		}

		template <typename TpRet, typename TpReceiver>
		void connect( TpRet( TpReceiver:: * pSlot )( TpSignalArgs... ), TpReceiver * pReceiver ) const
		{
			_handlers.push_back(
				HandlerInfo {
					[pReceiver, pSlot]( TpSignalArgs &&... pSignalArgs ) {
						( pReceiver->*pSlot )( std::forward<TpSignalArgs>( pSignalArgs )... );
					},
					pReceiver
				} );

			if ( _signalProxy != nullptr )
			{
				_signalProxy->onConnect( tpSignalCode, pReceiver );
			}
		}

		// template <typename TpRet, typename TpReceiver, typename... TpAdditionalArgs>
		// void connect( TpRet( TpReceiver:: * pSlot )( TpSignalArgs..., TpAdditionalArgs... ), TpReceiver * pReceiver, TpAdditionalArgs &&... pAdditionalArgs ) const
		// {
		// 	_handlers.emplace_back(
		// 		[pReceiver, pSlot, pAdditionalArgs...]( TpSignalArgs &&... pSignalArgs ) {
		// 			( pReceiver->*pSlot )( std::forward<TpSignalArgs>( pSignalArgs )..., std::forward<TpAdditionalArgs>( pAdditionalArgs )... );
		// 		},
		// 		pReceiver );
		// 
		// 	if ( _signalProxy != nullptr )
		// 	{
		// 		_signalProxy->onConnect( tpSignalCode, pReceiver );
		// 	}
		// }

		template <typename TpReceiver>
		void disconnect( TpReceiver * pReceiver ) const
		{
			bool result = false;

			auto handlerIter = _handlers.begin();
			auto handlersEnd = _handlers.end();

			for ( ; handlerIter != handlersEnd; ++handlerIter )
			{
				if ( handlerIter->receiver == pReceiver )
				{
					_handlers.erase( handlerIter );
					result = true;
					break;
				}
			}

			if ( _signalProxy != nullptr )
			{
				_signalProxy->onDisconnect( tpSignalCode, pReceiver, result );
			}
		}

	private:
		SignalProxy * _signalProxy;
		mutable std::vector<HandlerInfo> _handlers;
	};

	template <signal_code_value_t tpSignalCode, typename... TpSignalArgs, typename TpRet, typename TpReceiver>
	void signalConnect( const SignalEmitter< Signal<tpSignalCode, TpSignalArgs...> > & pEmitter, TpRet( TpReceiver:: * pSlot )( TpSignalArgs... ), TpReceiver * pReceiver )
	{
		pEmitter.connect( pSlot, pReceiver );
	}

	// template <signal_code_value_t tpSignalCode, typename... TpSignalArgs, typename TpRet, typename TpReceiver, typename... TpAdditionalArgs>
	// void signalConnect( const SignalEmitter< Signal<tpSignalCode, TpSignalArgs...> > & pEmitter, TpRet( TpReceiver:: * pSlot )( TpSignalArgs..., TpAdditionalArgs... ), TpReceiver * pReceiver, TpAdditionalArgs &&... pAdditionalArgs )
	// {
	// 	pEmitter.connect( pSlot, pReceiver, std::forward<TpAdditionalArgs>( pAdditionalArgs )... );
	// }

	template <signal_code_value_t tpSignalCode, typename... TpSignalArgs, typename TpReceiver>
	void signalDisconnect( const SignalEmitter< Signal<tpSignalCode, TpSignalArgs...> > & pEmitter, TpReceiver * pReceiver )
	{
		pEmitter.disconnect( pReceiver );
	}

#define slots

#define ts3DeclareSignals( pSignals ) \
	public: \
		struct Signals \
		{ \
			pSignals \
		}; \
	private: \
		Signals _signals; \
	public: \
		const Signals & signals = _signals;

}

#endif // __TS3_CORE_SIGNAL_EMITTER_H__
