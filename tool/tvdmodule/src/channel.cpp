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

#include "channel.h"
#include "show.h"
#include <pvr/dvb/tuner.h>
#include <pvr/parental/control.h>
#include <pvr/logos/service.h>
#include <pvr/parental/session.h>
#include <pvr/show.h>
#include <pvr/channel.h>
#include <pvr/channels.h>

// NOTE:
// NodeJs c++ modules docs: http://nodejs.org/api/addons.html
// V8 docs: https://developers.google.com/v8/

namespace tvd {

bool getInfoFromID( const std::string &uri, const std::string &net, v8::Handle<v8::Object> &info ) {
	pvr::dvb::id::ChannelID id;
	if (pvr::dvb::id::fromString( uri, id )) {
		setValue(info, "type", "dvb");
		setValue(info, "frecuency",  net );
		setValue(info, "nitID", id.nitID);
		setValue(info, "tsID", id.tsID);
		setValue(info, "srvID", id.srvID);
		return true;
	}
	return false;
}

Channel::Channel( pvr::Channel *ch )
{
	_ch = ch;

	//	Create template
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate);
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	tpl->SetClassName(_val("Channel"));
	NODE_SET_PROTOTYPE_METHOD(tpl, "isFavorite",      Channel::isFavorite);
	NODE_SET_PROTOTYPE_METHOD(tpl, "toggleFavorite",  Channel::toggleFavorite);
	NODE_SET_PROTOTYPE_METHOD(tpl, "isBlocked",       Channel::isBlocked);
	NODE_SET_PROTOTYPE_METHOD(tpl, "toggleBlocked",   Channel::toggleBlocked);
	NODE_SET_PROTOTYPE_METHOD(tpl, "isProtected",     Channel::isProtected);
	NODE_SET_PROTOTYPE_METHOD(tpl, "parentalAge",     Channel::parentalAge);
	NODE_SET_PROTOTYPE_METHOD(tpl, "parentalContent", Channel::parentalContent);
	NODE_SET_PROTOTYPE_METHOD(tpl, "getShow",         Channel::getShow);
	NODE_SET_PROTOTYPE_METHOD(tpl, "getShowsBetween", Channel::getShowsBetween);

	//	Create instance
	v8::Local<v8::Function> function = tpl->GetFunction();
	v8::Local<v8::Object> instance = function->NewInstance();

	setValue( instance, "id",       ch->channelID() );
	setValue( instance, "channel",  ch->channel()   );
	setValue( instance, "name",     ch->name()      );
	setValue( instance, "logo",     ch->logo()      );
	setValue( instance, "isMobile", ch->isMobile()  );
	setValue( instance, "category", ch->category()  );

	{	//	Export specific channel info
		v8::Handle<v8::Object> info = v8::Object::New(isolate);
		if (getInfoFromID( ch->uri(), ch->network(), info )) {
			setValue( instance, "info", info );
		}
	}

	this->Wrap( instance );
}

Channel::~Channel()
{
}

pvr::Channel *Channel::channel(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Channel *wrap = node::ObjectWrap::Unwrap<Channel>(args.This());
	DTV_ASSERT(wrap);
	pvr::Channel *ch = wrap->_ch;
	DTV_ASSERT(ch);
	return ch;
}

//	Node implementation
void Channel::isFavorite( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	bool result = channel(args)->isFavorite();
	args.GetReturnValue().Set(_val(result));
}

void Channel::toggleFavorite( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	channel(args)->toggleFavorite();
	args.GetReturnValue().SetUndefined();
}

void Channel::isBlocked( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	bool result = channel(args)->isBlocked();
	args.GetReturnValue().Set(_val(result));
}

void Channel::toggleBlocked( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	channel(args)->toggleBlocked();
	args.GetReturnValue().SetUndefined();
}

void Channel::isProtected( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	bool result=channel(args)->isProtected();
	args.GetReturnValue().Set(_val(result));
}

void Channel::parentalAge( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	std::string result=channel(args)->parentalControl()->getAgeName();
	args.GetReturnValue().Set(_val(result));
}

void Channel::parentalContent( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	std::string result=channel(args)->parentalControl()->getContentName();
	args.GetReturnValue().Set(_val(result));
}

void Channel::getShow( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	pvr::Show *prg=channel(args)->getCurrentShow();
	Show *nShow = new Show(prg);
	args.GetReturnValue().Set( nShow->persistent() );
}

void Channel::getShowsBetween( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::HandleScope scope(isolate);
	if (args.Length() == 2) {
		std::string from, to;
		impl::get( args[0], from );
		impl::get( args[1], to );
		pvr::Channel::Shows shows;
		bpt::ptime start = bpt::time_from_string( from );
		bpt::ptime end = bpt::time_from_string( to );
		bpt::time_period tp(start, end);
		channel(args)->getShowsBetween( shows, tp );
		v8::Handle<v8::Array> result = v8::Array::New( isolate, shows.size() );
		for (size_t i=0; i<shows.size(); i++) {
			Show *nShow = new Show(shows[i]);
			result->Set( i, v8::Local<v8::Object>::New(isolate, nShow->persistent()) );
		}
		args.GetReturnValue().Set(result);
		return;
	}

	LWARN("channels", "Called getShowsBetween with invalid arguments");
	args.GetReturnValue().Set(v8::Array::New(isolate));
}

}

