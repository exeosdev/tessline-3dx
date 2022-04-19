
#pragma once

#ifndef __TS3_CORE_SIGNAL_EMITTER_H__
#define __TS3_CORE_SIGNAL_EMITTER_H__

#include "signalCommon.h"

#include <list> // Used for list of event handlers
#include <unordered_map> // Used for receiver->HandlerList mapping

namespace ts3
{

	template <typename TpClass, typename TpEvent>
	class EventEmitter;

	template <typename TpClass, event_code_value_t tpEventCode, typename... TpEventArgs>
	class EventEmitter< TpClass, Event<tpEventCode, TpEventArgs...> >
	{
	public:
		using Handler = std::function<void( TpClass &, TpEventArgs... )>;
		using HandlerList = std::list<Handler>;
		using HandlerMap = std::unordered_map<uintptr_t, HandlerList>;
		using HandlerRef = typename HandlerList::iterator;
		using HandlerRefList = std::list<HandlerRef>;

	public:
		explicit EventEmitter( TpClass & pSourceObjectRef )
		: _sourceObjectRef( pSourceObjectRef )
		{}

		void emit( TpEventArgs &&... pArgs )
		{
			for( auto & handlerRef : _handlerOrderedRefList )
			{
				( *handlerRef )( _sourceObjectRef, std::forward<TpEventArgs>( pArgs )... );
			}
		}

		template <typename TpRet, typename TpReceiver>
		void connect( uintptr_t pRefID, std::function<TpRet( TpClass &, TpEventArgs... )> pHandler ) const
		{
			auto & handlerList = _handlerMap[pRefID];

			auto newHandlerRef = handlerList.insert( handlerList.end(), std::move( pHandler ) );
			_handlerOrderedRefList.push_back( newHandlerRef );
		}

		template <typename TpRet, typename TpReceiver>
		void connect( TpReceiver * pReceiver, TpRet( TpReceiver:: * pSlot )( TpClass &, TpEventArgs... ) ) const
		{
			auto refID = reinterpret_cast<uintptr_t>( pReceiver );
			auto & handlerList = _handlerMap[refID];

			auto newHandlerRef = handlerList.insert(
				handlerList.end(),
				[pReceiver, pSlot]( TpClass & pSource, TpEventArgs &&... pEventArgs ) -> void {
					( pReceiver->*pSlot )( pSource, std::forward<TpEventArgs>( pEventArgs )... );
				} );
			_handlerOrderedRefList.push_back( newHandlerRef );
		}

		template <typename TpRet, typename TpReceiver>
		bool connectUnique( uintptr_t pRefID, std::function<TpRet( TpClass &, TpEventArgs... )> pHandler ) const
		{
			auto & handlerList = _handlerMap[pRefID];
			if( !handlerList.empty() )
			{
				return false;
			}

			auto newHandlerRef = handlerList.insert( handlerList.end(), std::move( pHandler ) );
			_handlerOrderedRefList.push_back( newHandlerRef );

			return true;
		}

		template <typename TpRet, typename TpReceiver>
		bool connectUnique( TpReceiver * pReceiver, TpRet( TpReceiver:: * pSlot )( TpClass &, TpEventArgs... ) ) const
		{
			auto refID = reinterpret_cast<uintptr_t>( pReceiver );
			auto & handlerList = _handlerMap[refID];
			if( !handlerList.empty() )
			{
				return false;
			}

			auto newHandlerRef = handlerList.insert(
					handlerList.end(),
					[pReceiver, pSlot]( TpClass & pSource, TpEventArgs &&... pEventArgs ) -> void {
						( pReceiver->*pSlot )( pSource, std::forward<TpEventArgs>( pEventArgs )... );
					} );
			_handlerOrderedRefList.push_back( newHandlerRef );

			return true;
		}

	private:
		TpClass & _sourceObjectRef;
		mutable HandlerMap _handlerMap;
		mutable HandlerRefList _handlerOrderedRefList;
	};

	// template <event_code_value_t tpEventCode, typename... TpEventArgs, typename TpRet, typename TpReceiver>
	// void eventConnect( const EventEmitter< Event<tpEventCode, TpEventArgs...> > & pEmitter, TpRet( TpReceiver:: * pSlot )( TpEventArgs... ), TpReceiver * pReceiver )
	// {
	// 	pEmitter.connect( pSlot, pReceiver );
	// }
	// template <event_code_value_t tpEventCode, typename... TpEventArgs, typename TpReceiver>
	// void eventDisconnect( const EventEmitter< Event<tpEventCode, TpEventArgs...> > & pEmitter, TpReceiver * pReceiver )
	// {
	// 	pEmitter.disconnect( pReceiver );
	// }

#define ts3AddEvent( pEventType, pVariableName ) \
	private: \
		EventEmitter<pEventType> _evt##pVariableName; \
	public: \
		const EventEmitter<pEventType> & mEvt##pVariableName = _evt##pVariableName;

#define slots

#define ts3DeclareEvents( pEvents ) \
	public: \
		struct Events \
		{ \
			pEvents \
		}; \
	private: \
		Events _events; \
	public: \
		const Events & mEvents = _events;

}

#endif // __TS3_CORE_SIGNAL_EMITTER_H__
