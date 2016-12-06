#include "eventmanager.h"
#include "../../../include/model/IEventListener.h"
#include "../../../include/model/FormatterEvent.h"
#include "../../../include/model/ExecutionObject.h"
#include <util/log.h>
#include <util/mcr.h>

namespace bptgnme = ::br::pucrio::telemidia::ginga::ncl::model::event;

#define logAction( action ) \
		LDEBUG("ncl::EventManager", "%s listener, type=%s, listenerId=%p, eventId=%p", #action, ((bptgnme::IEventListener*) listener->listenerId())->type().c_str(), listener->listenerId(), eventId );

namespace ncl30presenter {
namespace event {
class EventListener
{
public:
	EventListener( const ListenerId &listenerId, short priority ) :
			_listenerId( listenerId ),
			_enabled( true ),
			_priority( priority ) {
	}
	virtual ~EventListener( void ) {
	}

	const ListenerId &listenerId() const {
		return _listenerId;
	}

	short priority() const {
		return _priority;
	}

	bool isEnabled() const {
		return _enabled;
	}

	void enabled( bool enabled ) {
		_enabled = enabled;
	}

private:
	ListenerId _listenerId;
	bool _enabled;
	short _priority;
};

bool compare( EventListener *l1, EventListener *l2 ) {
	return l1->priority() < l2->priority();
}

EventManager::EventManager() {

}

EventManager::~EventManager() {
	BOOST_FOREACH( Listeners::value_type pair, _listeners )
	{
		CLEAN_ALL( EventListener *, pair.second );
	}
	LINFO("ncl::EventManager", "finalized");
}

bool EventManager::addListener( const ListenerId &listenerId, const EventId &eventId ) {

	BOOST_FOREACH( EventListener *listener, _listeners[eventId] )
	{
		if (listener->listenerId() == listenerId) {
			listener->enabled( true );
			return true;
		}
	}

	EventListener *listener = new EventListener( listenerId, ((bptgnme::IEventListener *) listenerId)->getPriorityType() );
	_listeners[eventId].push_back( listener );
	std::sort( _listeners[eventId].begin(), _listeners[eventId].end(), compare );
	logAction( Add );

	return true;
}

bool EventManager::delListener( const ListenerId &listenerId, const EventId &eventId ) {

	BOOST_FOREACH( EventListener *listener, _listeners[eventId] )
	{
		if (listener->listenerId() == listenerId) {
			listener->enabled( false );
			logAction( Del );
			return true;
		}
	}
	return false;

}

void EventManager::onEvent( const EventId &eventId, short transition, short previousState ) {

	EventListeners eventListeners = _listeners[eventId];
	BOOST_FOREACH( EventListener *listener, eventListeners )
	{
		if (listener->isEnabled()) {
			logAction( Before );
			bptgnme::FormatterEvent * e = (bptgnme::FormatterEvent *) eventId;
			//	Test if execution object is unprepared or if is a focus event
			if (e->instanceOf( "SwitchEvent" ) || e->getExecutionObject()->getMainEvent() != NULL) {
				((bptgnme::IEventListener*) listener->listenerId())->eventStateChanged( eventId, transition, previousState );
			}
			logAction( After );
		}
	}

	//	clean up disabled listeners
	{
		EventListener *listener;
		EventListeners::iterator it = _listeners[eventId].begin();
		while (it != _listeners[eventId].end()) {
			listener = (*it);
			if (!listener->isEnabled()) {
				it = _listeners[eventId].erase( it );
				delete listener;
			} else {
				it++;
			}
		}
	}

}

} /* namespace event */
} /* namespace ncl30presenter */
