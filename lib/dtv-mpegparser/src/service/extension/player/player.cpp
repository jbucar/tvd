#include "player.h"
#include "esplayer.h"
#include "delegate.h"
#include "streaminfo.h"
#include "extension.h"
#include "../../service.h"
#include "../../servicemanager.h"
#include "../../../provider/filter.h"
#include "../../../demuxer/descriptors/generic.h"
#include "../../../demuxer/ts.h"
#include <util/url.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/foreach.hpp>

namespace tuner {
namespace player {

Player::Player( Delegate *delegate )
	: _delegate(delegate)
{
	DTV_ASSERT(delegate);

	_ext = NULL;
	_srvID = 0;
	_pcrPID = TS_PID_NULL;
	_needPCRFilter = false;
	_started = false;

	_nextAuxPlayer = 0;
}

Player::~Player()
{
	DTV_ASSERT(!_ext);

	BOOST_FOREACH( const Players::value_type &value, _players ) {
		delete value.second;
	}

	delete _delegate;

	CLEAN_ALL( StreamInfo *, _supportedStreams );
}

void Player::addPlayer( es::Player *player ) {
	DTV_ASSERT(!_started);
	Players::iterator it=_players.find( player->esType() );
	if (it != _players.end()) {
		delete it->second;
	}
	_players[player->esType()] = player;
}

es::Player *Player::getPlayer( pes::type::type pesType ) const {
	Players::const_iterator it=_players.find( pesType );
	return (it == _players.end()) ? NULL : it->second;
}

void Player::addStreams( const std::vector<StreamInfo *> &supported ) {
	DTV_ASSERT(!_started);
	CLEAN_ALL( StreamInfo *, _supportedStreams );
	_supportedStreams = supported;
}

void Player::addDefaultsStreams() {
	DTV_ASSERT(!_started);
	//	Fill supported streams
	CLEAN_ALL( StreamInfo *, _supportedStreams );
	defaultsStreams( _supportedStreams );
}

const std::vector<StreamInfo *> &Player::supportedStreams() const {
	return _supportedStreams;
}

//	Play service
void Player::playService( ID srvID ) {
	DTV_ASSERT(_ext);
	DTV_ASSERT(!_srvID);
	_srvID = srvID;
	if (extension()->srvMgr()->isRunning( srvID )) {
		onStartService( extension()->srvMgr()->get( srvID ) );
	}
}

void Player::stopService() {
	stop();
	_srvID = 0;
}

bool Player::isStarted() const {
	return _srvID != 0 && _started;
}

//	Notifications
void Player::onStarted( const OnStartedCallback &callback ) {
	_onStarted = callback;
}

void Player::started( bool value ) {
	LINFO( "Player", "Started: value=%d, url=%s", value, url().c_str() );
	if (!_onStarted.empty()) {
		_onStarted( value );
	}
}

const std::string Player::url() const {
	return isStarted() ? _delegate->url() : "";
}

//	Stream methods
ID Player::pcrPID() const {
	return isStarted() ? _pcrPID : TS_PID_NULL;
}

ID Player::srvID() const {
	return _srvID;
}

bool Player::startPCR( ID pid ) {
	return _delegate->startStream( pid, pes::type::pcr );
}

int Player::nextStream( pes::type::type pesType ) {
	int id=-1;
	if (isStarted()) {
		es::Player *p = getPlayer( pesType );
		if (p) {
			if (p->canNext()) {
				LDEBUG( "Player", "Next stream: type=%d", pesType );
				onStop();
				id = p->next();
				onStart();
			}
			else {
				id = p->current();
			}
		}
	}
	return id;
}

int Player::streamCount( pes::type::type pesType ) const {
	if (isStarted()) {
		es::Player *p = getPlayer( pesType );
		return p ? p->count() : 0;
	}
	return 0;
}

const Stream *Player::get( int id/*=-1*/, pes::type::type pesType ) const {
	es::Player *p = getPlayer( pesType );
	return p ? p->get( id ) : NULL;
}

const Stream *Player::findStream( fncFindStream fnc ) {
	DTV_ASSERT(fnc);
	BOOST_FOREACH( const Players::value_type &value, _players ) {
		const Stream *result = value.second->findStream(fnc);
		if (result) {
			return result;
		}
	}
	return NULL;
}

//	Services notifications
bool Player::initialize() {
	DTV_ASSERT(!_ext);

	//	Initialize delegate
	bool result=_delegate->initialize( this, pes::type::other );
	if (result) {
		//	Initialize es player
		BOOST_FOREACH( const Players::value_type &value, _players ) {
			result &= value.second->initialize( this );
		}

		if (result) {
			//	Create extension
			_ext = new Extension(this);
		}
	}
	return result;
}

void Player::finalize() {
	if (_ext) {
		DTV_ASSERT( !_ext->isInitialized() );

		BOOST_FOREACH( const Players::value_type &value, _players ) {
			value.second->finalize();
		}
		_delegate->finalize();

		DEL(_ext);
	}
}

ca::Conditional *Player::ca() const {
	if (_ext && _ext->isInitialized()) {
		return _ext->srvMgr()->ca();
	}
	return NULL;
}

Extension *Player::extension() const {
	DTV_ASSERT(_ext);
	return _ext;
}

void Player::onStartService( Service *srv ) {
	LDEBUG( "Player", "On start service: ext=%p, ID=%04x",
		this, srv->id() );

	//	Save PCR and reset flag
	_pcrPID = srv->pcrPID();
	_needPCRFilter = (_pcrPID != TS_PID_NULL);

	//	Collect all streams supported
	const std::vector<ElementaryInfo> &elements = srv->elements();
	BOOST_FOREACH( const ElementaryInfo &info, elements ) {
		bool collected=false;

		//	Get tag from descriptor
		util::BYTE tag=desc::getStreamIdentifier( info.descriptors );

		//	Get elementary stream info for that type
		BOOST_FOREACH( StreamInfo *s, _supportedStreams ) {
			//	Check if stream is supported
			es::Player *p = getPlayer( s->esType() );
			if (p && s->check(info,tag)) {
				collected=true;
				p->addStream( s, info, tag );
			}
		}

		BOOST_FOREACH( const AuxPlayers::value_type &item, _aux ) {
			es::Player *p = item.second;
			StreamInfo *s = p->canPlay( info, tag );
			if (s) {
				collected=true;
				p->addStream( s, info, tag );
			}
		}

		if (!collected) {
			LDEBUG( "Player", "Stream not supported: type=%02x, tag=%02x, desc=%s",
				info.streamType, tag, info.descriptors.show().c_str() );
		}

		if (_pcrPID == info.pid) {
			_needPCRFilter = false;
		}
	}

	start();
}

void Player::start() {
	//	Start all stream
	BOOST_FOREACH( const Players::value_type &value, _players ) {
		value.second->start();
	}

	//	Start all aux streams
	BOOST_FOREACH( const AuxPlayers::value_type &item, _aux ) {
		item.second->start();
	}

	//	Start PCR
	if (_needPCRFilter) {
		_needPCRFilter = startPCR( _pcrPID );
	}

	onStart();
}

void Player::onStart() {
	DTV_ASSERT(!_started);
	_started = true;

	//	Start delegate
	_delegate->start();

	//	Notify when stream was started
	started( true );
}

void Player::onStopService( Service *srv ) {
	LDEBUG( "Player", "On stop service: ext=%p, ID=%04x", this, srv->id() );
	stop();
}

void Player::stop() {
	if (_started) {
		//	Notify stop
		onStop();

		{	//	Stop all aux streams and clear it
			AuxPlayers::iterator it=_aux.begin();
			while (it != _aux.end()) {
				stopAux( it->first );
				it = _aux.begin();
			}
			_nextAuxPlayer = 0;
		}

		//	Stop all streams (and clean metadata)
		BOOST_FOREACH( const Players::value_type &value, _players ) {
			value.second->stop(true);
		}

		//	Stop PCR
		if (_needPCRFilter) {
			extension()->stopStream( _pcrPID );
		}

		_pcrPID = TS_PID_NULL;
		_needPCRFilter = false;
	}
}

void Player::onStop() {
	DTV_ASSERT(_started);
	_started = false;

	//	Stop delegate
	_delegate->stop();

	//	Notify before stream stop
	started( false );
}

int Player::startAux( Delegate *dlg, pes::type::type pesType, int streamID/*=-1*/ ) {
	DTV_ASSERT(dlg);

	//	Create player
	tuner::player::es::Player *p = new tuner::player::es::Player( dlg, pesType );

	//	Get stream if necesary
	if (_started && streamID >= 0) {
		const tuner::player::Stream *s = getVideo( streamID );
		if (!s) {
			LERROR( "Player", "Cannot play extrea elementary stream; streamID not exist: streamID=%d", streamID );
			delete dlg;
			delete p;
			return -1;
		}

		p->addStream(*s);
	}

	LDEBUG( "Player", "Start extra elementary stream: pesType=%d, streamID=%d", pesType, streamID );

	if (!p->initialize(this)) {
		delete p;
		return -1;
	}

	if (_started) {
		p->start();
	}

	//	Add stream
	int id = _nextAuxPlayer++;
	_aux[id] = p;
	return id;
}

void Player::stopAux( int playerID ) {
	AuxPlayers::iterator it = _aux.find(playerID);
	if (it != _aux.end()) {
		LDEBUG( "Player", "Stop extra elementary stream: playerID=%d", playerID );
		es::Player *p = it->second;

		p->stop(true);
		p->finalize();
		delete p;

		_aux.erase(it);
	}
	else {
		LWARN( "Player", "Cannot stop elementary stream: playerID=%d", playerID );
	}
}

Delegate *Player::delegate() const {
	return _delegate;
}

//	Aux URL
const std::string Player::makeURL( const std::string &urlType ) {
	util::Url url;
	url.type( urlType );

	{	//	Add video
		const Stream *s = getVideo();
		if (s) {
			url.addParam( "videoPID", s->info.pid );
			url.addParam( "videoType", s->info.streamType );
		}
	}

	{	//	Add audio
		const Stream *s = getAudio();
		if (s) {
			url.addParam( "audioPID", s->info.pid );
			url.addParam( "audioType", s->info.streamType );
		}
	}

	{	//	Add CC
		const Stream *s = getSubtitle();
		if (s) {
			url.addParam( "subtitlePID", s->info.pid );
			url.addParam( "subtitleType", s->info.streamType );
		}
	}

	{	//	PCR
		url.addParam( "pcrPID", _pcrPID );
	}

	return url.id();
}

}
}

