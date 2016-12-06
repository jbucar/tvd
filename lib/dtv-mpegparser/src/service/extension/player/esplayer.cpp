/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "esplayer.h"
#include "delegate.h"
#include "streaminfo.h"
#include "player.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>

namespace tuner {
namespace player {
namespace es {

bool TagPriority( const Stream &i1, const Stream &i2 ) {
	return i1.tag < i2.tag;
}

Player::Player( Delegate *delegate, pes::type::type type, int first )
	: _delegate(delegate), _pesType( type ), _first(first)
{
	DTV_ASSERT(delegate);
	_current = -1;
}

Player::Player( pes::type::type type, int first )
	: _pesType( type ), _first(first)
{
	_delegate = NULL;
	_current = -1;
}

Player::~Player()
{
	delete _delegate;
}

bool Player::initialize( player::Player *player ) {
	//	If no delegate, setup a forward delegate
	if (!_delegate) {
		_delegate = new ForwardDelegate( player->delegate() );
	}

	if (!_delegate->initialize( player, _pesType )) {
		LWARN( "es::Player", "Cannot initialize delegate" );
		DEL(_delegate);
		return false;
	}

	//	Setup
	_current = -1;

	return true;
}

void Player::finalize() {
	if (_delegate) {
		_delegate->finalize();
	}
}

void Player::start() {
	DTV_ASSERT(_current == -1);

	//	Sort streams by tag
	std::sort( _currentStreams.begin(), _currentStreams.end(), TagPriority );

	//	Start with the first stream collected
	start( first() );
}

void Player::stop( bool clean ) {
	if (_current >= 0) {
		LINFO( "mpeg::Player", "Stop stream: type=%d", _pesType );
		const Stream *sInfo = getStream(_current);
		DTV_ASSERT(sInfo);
		stop( sInfo );
		_current = -1;
	}

	if (clean) {
		_currentStreams.clear();
	}
}

int Player::first() const {
	return (_currentStreams.size() > 0) ? _first : -1;
}

int Player::current() const {
	return _current;
}

int Player::next() {
	//	Get next stream
	int nextID = next( _current, 1, _currentStreams.size() );
	LDEBUG( "mpeg::Player", "Next stream: type=%d, _current=%d, next=%d", _pesType, _current, nextID );
	if (nextID != _current) {
		//	Stop current stream
		stop(false);

		//	Start new
		start( nextID );
	}
	return _current;
}

bool Player::canNext() const {
	int nextID = next( _current, 1, _currentStreams.size() );
	return (nextID != _current);
}

int Player::count() const {
	return static_cast<int>(_currentStreams.size());
}

const Stream *Player::get( int id/*=-1*/ ) const {
	return getStream( (id == -1) ? _current : id );
}

pes::type::type Player::esType() const {
	return _pesType;
}

StreamInfo *Player::canPlay( const ElementaryInfo &info, ID tag ) const {
	return dlg()->canPlay( info, tag );
}

void Player::addStream( StreamInfo *sInfo, const ElementaryInfo &info, ID tag ) {
	Stream stream;
	stream.info = info;
	stream.tag = tag;
	stream.sInfo = sInfo;

	addStream( stream );
}

void Player::addStream( const Stream &stream ) {
	LINFO( "mpeg::Player", "Add stream: pid=%04x, es=%d, type=%04x, name=%s",
		stream.info.pid, stream.sInfo->esType(), stream.info.streamType, stream.sInfo->name().c_str() );

	_currentStreams.push_back( stream );
}

const Stream *Player::findStream( fncFindStream fnc ) {
	DTV_ASSERT(fnc);
	BOOST_FOREACH( const Stream &sInfo, _currentStreams ) {
		if (fnc(&sInfo)) {
			return &sInfo;
		}
	}
	return NULL;
}

void Player::start( int id ) {
	if (id >= 0) {
		const Stream *sInfo = getStream(id);
		DTV_ASSERT(sInfo);
		LINFO( "mpeg::Player", "Start stream: type=%d, id=%d, pid=%04x", _pesType, id, sInfo->info.pid );
		if (start( sInfo )) {
			_current = id;
		}
		else {
			LWARN( "mpeg::Player", "Cannot start stream: type=%d, id=%d, pid=%04x", _pesType, id, sInfo->info.pid );
		}
	}
}

bool Player::start( const Stream *s ) {
	dlg()->start();
	if (!dlg()->startStream( s->info.pid, s->sInfo->esType() )) {
		dlg()->stop();
		return false;
	}
	return true;
}

void Player::stop( const Stream *s ) {
	dlg()->stopStream( s->info.pid );
	dlg()->stop();
}

const Stream *Player::getStream( int id ) const {
	if (id >= 0 && id < static_cast<int>(_currentStreams.size())) {
		return &_currentStreams[id];
	}
	else {
		return NULL;
	}
}

int Player::next( int pos, int factor, int size ) const {
	if (!size || ((_first == -1) && (pos >= 0 && pos == size-1))) {
		return -1;
	}
	else {
		int a = (pos + factor);
		return (int)(a - floor(a/(double)size)*size);
	}
}

Delegate *Player::dlg() const {
	DTV_ASSERT(_delegate);
	return _delegate;
}

}
}
}

