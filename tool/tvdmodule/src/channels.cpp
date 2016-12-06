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

#include "channels.h"
#include "channel.h"
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

Channels::Channels( pvr::Channels *chs )
{
	_chs = chs;
	v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), _constructor);
	v8::Local<v8::Object> instance = cons->NewInstance();
	this->Wrap( instance );
	this->Ref();

	{	//	Connect to protected changed signal
		pvr::Channels::NewChannelSignal::slot_type fnc = boost::bind(&Channels::onChannelAdded,this,_1);
		_cNew = _chs->onNewChannel().connect(fnc);
	}

	{	//	Connect to changed signal
		pvr::Channels::ChannelRemovedSignal::slot_type fnc = boost::bind(&Channels::onChannelRemoved,this,_1,_2);
		_cRemoved = _chs->onChannelRemoved().connect(fnc);
	}
}

Channels::~Channels()
{
}

pvr::Channels *Channels::channels(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Channels *wrap = node::ObjectWrap::Unwrap<Channels>(args.This());
	DTV_ASSERT(wrap);
	pvr::Channels *chs = wrap->_chs;
	DTV_ASSERT(chs);
	return chs;
}

void Channels::stop() {
	_cNew.disconnect();
	_cRemoved.disconnect();
	this->Unref();
	_chs = NULL;
}

void Channels::onChannelAdded( pvr::Channel *ch ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	v8::Handle<v8::Value> argv[2];
	argv[0] = _val("channelAdded"); // event name
	argv[1] = _val(ch->channelID());
	//	Call
	makeCallback( this, "emit", 2, argv );
}

void Channels::onChannelRemoved( pvr::Channel *ch, bool removeAll ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	v8::Handle<v8::Value> argv[3];
	argv[0] = _val("channelRemoved"); // event name
	argv[1] = _val(ch->channelID());
	argv[2] = _val(removeAll);
	//	Call
	makeCallback( this, "emit", 3, argv );
}

//	Node implementation
v8::Persistent<v8::Function> Channels::_constructor;

void Channels::Init() {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	//	Create template
	v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate);
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	tpl->SetClassName(_val("Channels"));
	NODE_SET_PROTOTYPE_METHOD(tpl, "count",  Channels::count);
	NODE_SET_PROTOTYPE_METHOD(tpl, "getAll", Channels::getAll);
	NODE_SET_PROTOTYPE_METHOD(tpl, "get",    Channels::get);
	NODE_SET_PROTOTYPE_METHOD(tpl, "remove", Channels::remove);

	_constructor.Reset(isolate, tpl->GetFunction());
}

void Channels::count( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	const size_t n = channels(args)->getAll().size();
	args.GetReturnValue().Set(_val(n));
}

void Channels::get( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	int chID=-1;
	if (args.Length() == 1 && impl::get(args[0], chID)) {
		pvr::Channel *ch = channels(args)->get(chID);
		if (ch) {
			Channel *nCh = new Channel(ch);
			args.GetReturnValue().Set( nCh->persistent() );
			return;
		}
	}

	LWARN("tvd", "Called get with invalid arguments");
	args.GetReturnValue().SetUndefined();
}

void Channels::getAll( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	const pvr::ChannelList &chs = channels(args)->getAll();
	const size_t n = chs.size();
	v8::Handle<v8::Array> channels = v8::Array::New(v8::Isolate::GetCurrent(), n);
	for (size_t i=0; i<n; i++) {
		channels->Set( i, _val(chs[i]->channelID()) );
	}
	args.GetReturnValue().Set(channels);
}

void Channels::remove( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	int chID=-1;
	if (args.Length() == 1 && impl::get( args[0], chID )) {
		channels(args)->remove( chID );
	}
	else {
		LWARN("channels", "Called remove with invalid arguments");
	}
	args.GetReturnValue().SetUndefined();
}

}

