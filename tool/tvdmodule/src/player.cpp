/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-module implementation.

    DTV-module is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-module is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-module.

    DTV-module es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-module se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "player.h"
#include "mediaplayer.h"
#include <pvr/player.h>
#include <util/assert.h>
#include <util/log.h>

// NOTE:
// NodeJs c++ modules docs: http://nodejs.org/api/addons.html
// V8 docs: https://developers.google.com/v8/

namespace tvd {

Player::Player( pvr::Player *p, MediaPlayer *media )
{
	//	Create instance
	v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), _constructor);
	v8::Local<v8::Object> instance = cons->NewInstance();
	this->Wrap( instance );
	this->Ref();

	_player = p;
	_media = media;
	_media->init();
	_player->start(false);
	p->onMediaChanged( boost::bind(&Player::onMedia,this,_1,_2) );

	{	//	Connect to protected changed signal
		pvr::Player::ChannelProtectedSignal::slot_type fnc = boost::bind(&Player::onProtectedChanged,this,_1);
		_cProtected = _player->onChannelProtected().connect(fnc);
	}

	{	//	Connect to changed signal
		pvr::Player::ChangeChannelSignal::slot_type fnc = boost::bind(&Player::onChannelChanged,this,_1,_2);
		_cChanged = _player->onChangeChannel().connect(fnc);
	}

	{	//	Connect to show changed signal
		pvr::Player::ShowChangedSignal::slot_type fnc = boost::bind(&Player::onCurrentShowChanged,this);
		_cShowChanged = _player->onShowChanged().connect(fnc);
	}
}

Player::~Player()
{
}

pvr::Player *Player::player(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Player *wrap = node::ObjectWrap::Unwrap<Player>(args.This());
	DTV_ASSERT(wrap);
	pvr::Player *player = wrap->_player;
	DTV_ASSERT(player);
	return player;
}

void Player::onMedia( const std::string &url, bool started ) {
	if (started) {
		_media->play( url );
	}
	else {
		_media->stop();
	}

	v8::Isolate *isolate = v8::Isolate::GetCurrent();
	v8::HandleScope scope(isolate);
	v8::Handle<v8::Value> argv[3];
	argv[0] = v8::String::NewFromUtf8(isolate, "playerMediaChanged"); // event name
	argv[1] = v8::String::NewFromUtf8(isolate, url.c_str());
	argv[2] = v8::Boolean::New(isolate, started);
	//	Call
	node::MakeCallback( isolate, handle(), "emit", 3, argv );

}

pvr::Player *Player::player() const {
	return _player;
}

pvr::Player *Player::stop() {
	LDEBUG( "tvd", "Stop player" );

	pvr::Player *tmp = _player;
	_player->stop();
	_cProtected.disconnect();
	_cChanged.disconnect();
	_cShowChanged.disconnect();
	_player = NULL;

	_media->fin();
	delete _media;
	_media = NULL;

	this->Unref();
	return tmp;
}

MediaPlayer *Player::media() const {
	return _media;
}

//	Node implementation
v8::Persistent<v8::Function> Player::_constructor;

void Player::Init() {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	//	Create template
	v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate);
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Player"));

	//	Change channel
	NODE_SET_PROTOTYPE_METHOD(tpl, "change", Player::change);
	NODE_SET_PROTOTYPE_METHOD(tpl, "nextChannel", Player::nextChannel);
	NODE_SET_PROTOTYPE_METHOD(tpl, "nextFavorite", Player::nextFavorite);
	NODE_SET_PROTOTYPE_METHOD(tpl, "showMobile", Player::showMobile);

	//	Status
	NODE_SET_PROTOTYPE_METHOD(tpl, "current", Player::current);
	NODE_SET_PROTOTYPE_METHOD(tpl, "isProtected", Player::isProtected);
	NODE_SET_PROTOTYPE_METHOD(tpl, "currentNetworkName", Player::currentNetworkName);
	NODE_SET_PROTOTYPE_METHOD(tpl, "status", Player::status);

	//	Player methods
	NODE_SET_PROTOTYPE_METHOD(tpl, "nextVideo", Player::nextVideo);
	NODE_SET_PROTOTYPE_METHOD(tpl, "videoInfo", Player::videoInfo);

	NODE_SET_PROTOTYPE_METHOD(tpl, "nextAudio", Player::nextAudio);
	NODE_SET_PROTOTYPE_METHOD(tpl, "audioInfo", Player::audioInfo);

	NODE_SET_PROTOTYPE_METHOD(tpl, "nextSubtitle", Player::nextSubtitle);
	NODE_SET_PROTOTYPE_METHOD(tpl, "subtitleInfo", Player::subtitleInfo);

	_constructor.Reset(isolate, tpl->GetFunction());
}

void Player::current( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	int result = player(args)->current();
	args.GetReturnValue().Set(_val(result));
}

void Player::currentNetworkName( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	const std::string &result = player(args)->currentNetworkName();
	args.GetReturnValue().Set(_val(result));
}

void Player::status( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	int stSignal, stQuality;
	player(args)->status( stSignal, stQuality );
	v8::Handle<v8::Object> st = v8::Object::New(v8::Isolate::GetCurrent());
	setValue(st, "signal", stSignal);
	setValue(st, "quality", stQuality);
	args.GetReturnValue().Set(st);
}

void Player::isProtected( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	bool result = player(args)->isProtected();
	args.GetReturnValue().Set(_val(result));
}

void Player::onProtectedChanged( bool isProtected ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	v8::Handle<v8::Value> argv[2];
	argv[0] = _val("playerProtectedChanged"); // event name
	argv[1] = _val(isProtected);
	//	Call
	makeCallback( this, "emit", 2, argv );
}

void Player::change( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	int newID=-1;
	int chID=-1;
	if (args.Length() == 1 && impl::get( args[0], newID )) {
		chID = player(args)->change(newID);
	}
	else {
		LWARN("tvd", "Invalid parameters. You must pass a channel ID");
	}
	args.GetReturnValue().Set(_val(chID));
}

void Player::onChannelChanged( int id, pvr::Channel * /*ch*/ ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	v8::Handle<v8::Value> argv[2];
	argv[0] = _val("playerChanged"); // event name
	argv[1] = _val(id);
	//	Call
	makeCallback( this, "emit", 2, argv );
}

void Player::onCurrentShowChanged() {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	v8::Handle<v8::Value> argv[1];
	argv[0] = _val("currentShowChanged"); // event name
	//	Call
	makeCallback( this, "emit", 1, argv );
}

//	Change channel utility
void Player::showMobile( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	if (args.Length() == 1) {
		bool st;
		if (impl::get( args[0], st )) {
			player(args)->showMobile(st);
		}
		else {
			LWARN("tvd", "Invalid parameters. You must pass a boolean");
		}
		args.GetReturnValue().SetUndefined();
	}
	else if (args.Length() == 0) {
		bool result = player(args)->showMobile();
		args.GetReturnValue().Set(_val(result));
	}
	else {
		LWARN("tvd", "Invalid parameters. You must pass a boolean or do un getter");
		args.GetReturnValue().SetUndefined();
	}
}

void Player::nextChannel( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	int ch=-1;
	if (args.Length() == 2) {
		int first, factor;
		if (impl::get( args[0], first ) && impl::get( args[1], factor )) {
			ch = player(args)->nextChannel( first, factor );
		}
	}
	args.GetReturnValue().Set(_val(ch));
}

void Player::nextFavorite( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	int ch=-1;
	if (args.Length() == 2) {
		int first, factor;
		if (impl::get( args[0], first ) && impl::get( args[1], factor )) {
			ch = player(args)->nextFavorite( first, factor );
		}
	}
	args.GetReturnValue().Set(_val(ch));
}

//	Player methods
void Player::nextVideo( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	int id = player(args)->nextVideo();
	args.GetReturnValue().Set(_val(id));
}

void Player::videoInfo( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	Player *wrap = node::ObjectWrap::Unwrap<Player>(args.This());
	DTV_ASSERT(wrap);
	v8::Handle<v8::Object> info = v8::Object::New(v8::Isolate::GetCurrent());

	{	//	Get stream information
		pvr::Player *p = player(args);
		int count = p->videoCount();
		pvr::info::Basic sinfo;
		p->videoInfo( sinfo );

		setValue(info, "count", count);
		setValue(info, "pid", sinfo.pid );
		setValue(info, "codec", sinfo.codec );
	}


	{	//	Get media information
		int w=-1, h=-1, fps=-1;
		MediaPlayer *media = wrap->media();
		if (media) {
			media->getVideoInfo( w, h, fps );
		}
		setValue(info, "width", w );
		setValue(info, "height", h );
		setValue(info, "fps", fps );
	}

	args.GetReturnValue().Set(info);
}

void Player::nextAudio( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	int id = player(args)->nextAudio();
	args.GetReturnValue().Set(_val(id));
}

void Player::audioInfo( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	Player *wrap = node::ObjectWrap::Unwrap<Player>(args.This());
	DTV_ASSERT(wrap);
	v8::Handle<v8::Object> info = v8::Object::New(v8::Isolate::GetCurrent());

	{	//	Get stream info
		pvr::Player *p = player(args);
		int count = p->audioCount();
		pvr::info::Audio sinfo;
		p->audioInfo( sinfo );

		setValue(info, "count", count);
		setValue(info, "pid", sinfo.pid );
		setValue(info, "codec", sinfo.codec );
		setValue(info, "lang", sinfo.lang );
	}

	{	//	Get media information
		int nChannels=-1, rate=-1;
		MediaPlayer *media = wrap->media();
		if (media) {
			media->getAudioInfo( nChannels, rate );
		}
		setValue(info, "channels", nChannels);
		setValue(info, "rate", rate);
	}

	args.GetReturnValue().Set(info);
}

void Player::nextSubtitle( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	int id = player(args)->nextSubtitle();
	args.GetReturnValue().Set(_val(id));
}

void Player::subtitleInfo( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	v8::Handle<v8::Object> info = v8::Object::New(v8::Isolate::GetCurrent());

	{	//	Get stream information
		pvr::Player *p = player(args);
		int count = p->subtitleCount();
		pvr::info::Basic sinfo;
		p->subtitleInfo( sinfo );

		setValue(info, "count", count);
		setValue(info, "pid", sinfo.pid );
		setValue(info, "codec", sinfo.codec );
	}

	args.GetReturnValue().Set(info);
}

}

