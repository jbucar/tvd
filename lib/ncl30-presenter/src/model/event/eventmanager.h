#include <boost/interprocess/containers/stable_vector.hpp>
#include <map>

namespace ncl30presenter {
namespace event {

class EventListener;
typedef void* ListenerId;
typedef void* EventId;
typedef boost::container::stable_vector<EventListener *> EventListeners;
typedef std::map< EventId, EventListeners > Listeners;

class EventManager
{
public:
	EventManager();
	virtual ~EventManager();

	bool addListener( const ListenerId &listenerId, const EventId &eventId );
	bool delListener( const ListenerId &listenerId, const EventId &eventId );
	void onEvent( const EventId &eventId, short transition, short previousState );

private:
	Listeners _listeners;
};

} /* namespace event */
} /* namespace ncl30presenter */
