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

#include "session.h"
#include <pvr/parental/session.h>
#include <pvr/parental/control.h>
#include <util/assert.h>
#include <util/log.h>

// NOTE:
// NodeJs c++ modules docs: http://nodejs.org/api/addons.html
// V8 docs: https://developers.google.com/v8/

namespace tvd {

Session::Session( pvr::parental::Session *s )
{
	_session = s;
	v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), _constructor);
	v8::Local<v8::Object> instance = cons->NewInstance();
	this->Wrap( instance );
	this->Ref();

	{	//	Connect to onEnabled changed signal
		pvr::parental::Session::EnabledChangedSignal::slot_type fnc = boost::bind(&Session::onEnabled,this,_1);
		_cEnabled = _session->onEnabledChanged().connect(fnc);
	}

	{	//	Connect to onActive changed signal
		pvr::parental::Session::ActiveChangedSignal::slot_type fnc = boost::bind(&Session::onActive,this,_1);
		_cActive = _session->onActiveChanged().connect(fnc);
	}
}

Session::~Session()
{
}

pvr::parental::Session *Session::session(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Session *wrap = node::ObjectWrap::Unwrap<Session>(args.This());
	DTV_ASSERT(wrap);
	pvr::parental::Session *session = wrap->_session;
	DTV_ASSERT(session);
	return session;
}

void Session::stop() {
	_cEnabled.disconnect();
	_cActive.disconnect();
	this->Unref();
	_session = NULL;
}

void Session::onEnabled( bool isEnabled ) {
	LINFO("tvd", "Sesssion enabled: isEnabled=%d", isEnabled );
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	v8::Handle<v8::Value> argv[2];
	argv[0] = _val("sessionEnabled"); // event name
	argv[1] = _val(isEnabled);
	//	Call
	makeCallback( this, "emit", 2, argv );
}

void Session::onActive( bool isActive ) {
	LINFO("tvd", "Sesssion changed: isActive=%d", isActive );
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	v8::Handle<v8::Value> argv[2];
	argv[0] = _val("sessionChanged"); // event name
	argv[1] = _val(isActive);
	//	Call
	makeCallback( this, "emit", 2, argv );
}

//	Node implementation
v8::Persistent<v8::Function> Session::_constructor;

void Session::Init() {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	//	Create template
	v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate);
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Session"));
	NODE_SET_PROTOTYPE_METHOD(tpl, "enable",    Session::enable);
	NODE_SET_PROTOTYPE_METHOD(tpl, "isEnabled", Session::isEnabled);

	//	Check session/expiration
	NODE_SET_PROTOTYPE_METHOD(tpl, "check",     Session::check);
	NODE_SET_PROTOTYPE_METHOD(tpl, "isBlocked", Session::isBlocked);
	NODE_SET_PROTOTYPE_METHOD(tpl, "expire",    Session::expire);

	//	Time expiration
	NODE_SET_PROTOTYPE_METHOD(tpl, "getTimeExpiration", Session::getTimeExpiration);
	NODE_SET_PROTOTYPE_METHOD(tpl, "setTimeExpiration", Session::setTimeExpiration);

	//	Parental control
	NODE_SET_PROTOTYPE_METHOD(tpl, "restrictSex", Session::restrictSex);
	NODE_SET_PROTOTYPE_METHOD(tpl, "isSexRestricted", Session::isSexRestricted);

	NODE_SET_PROTOTYPE_METHOD(tpl, "restrictViolence", Session::restrictViolence);
	NODE_SET_PROTOTYPE_METHOD(tpl, "isViolenceRestricted", Session::isViolenceRestricted);

	NODE_SET_PROTOTYPE_METHOD(tpl, "restrictDrugs", Session::restrictDrugs);
	NODE_SET_PROTOTYPE_METHOD(tpl, "isDrugsRestricted", Session::isDrugsRestricted);

	NODE_SET_PROTOTYPE_METHOD(tpl, "restrictAge", Session::restrictAge);
	NODE_SET_PROTOTYPE_METHOD(tpl, "ageRestricted", Session::ageRestricted);

	_constructor.Reset(isolate, tpl->GetFunction());
}

//	Methods
void Session::enable( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	std::string p;
	if (args.Length() == 1 && impl::get( args[0], p )) {
		session(args)->pass(p);
	}
	else {
		LWARN("tvd", "Called enable with invalid arguments");
	}
	args.GetReturnValue().SetUndefined();
}

void Session::isEnabled( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	bool en = session(args)->isEnabled();
	args.GetReturnValue().Set(_val(en));
}

void Session::check( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	bool result;
	std::string p;
	if (args.Length() == 1 && impl::get( args[0], p )) {
		result=session(args)->check(p);
	}
	else {
		LWARN("tvd", "Called check with invalid arguments");
		result=false;
	}
	args.GetReturnValue().Set(_val(result));
}

void Session::isBlocked( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	bool result = session(args)->isBlocked();
	args.GetReturnValue().Set(_val(result));
}

void Session::expire( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	session(args)->expire();
	args.GetReturnValue().SetUndefined();
}

//	Time expiration
void Session::getTimeExpiration( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	int result = session(args)->timeExpiration();
	args.GetReturnValue().Set(_val(result));
}

void Session::setTimeExpiration( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	int param;
	if (args.Length() == 1 && impl::get( args[0], param )) {
		session(args)->timeExpiration(param);
	}
	else {
		LWARN("tvd", "Called setTimeExpiration with invalid arguments");
	}
	args.GetReturnValue().SetUndefined();
}

//	Restrict sex
void Session::restrictSex( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	bool param;
	if (args.Length() == 1 && impl::get( args[0], param )) {
		session(args)->restrictSex(param);
	}
	else {
		LWARN("tvd", "Called restrictSex with invalid arguments");
	}
	args.GetReturnValue().SetUndefined();
}

void Session::isSexRestricted( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	bool result = session(args)->isSexRestricted();
	args.GetReturnValue().Set(_val(result));
}

//	Restrict violence
void Session::restrictViolence( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	bool param;
	if (args.Length() == 1 && impl::get( args[0], param )) {
		session(args)->restrictViolence(param);
	}
	else {
		LWARN("tvd", "Called restrictViolence with invalid arguments");
	}
	args.GetReturnValue().SetUndefined();
}

void Session::isViolenceRestricted( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	bool result = session(args)->isViolenceRestricted();
	args.GetReturnValue().Set(_val(result));
}

//	Restrict drugs
void Session::restrictDrugs( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	bool param;
	if (args.Length() == 1 && impl::get( args[0], param )) {
		session(args)->restrictDrugs(param);
	}
	else {
		LWARN("tvd", "Called restrictDrugs with invalid arguments");
	}
	args.GetReturnValue().SetUndefined();
}

void Session::isDrugsRestricted( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	bool result = session(args)->isDrugsRestricted();
	args.GetReturnValue().Set(_val(result));
}

//	Restrict age
void Session::restrictAge( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	int param;
	if (args.Length() == 1 && impl::get( args[0], param )) {
		session(args)->restrictAge(param);
	}
	else {
		LWARN("tvd", "Called restrictAge with invalid arguments");
	}
	args.GetReturnValue().SetUndefined();
}

void Session::ageRestricted( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	int result = session(args)->ageRestricted();
	args.GetReturnValue().Set(_val(result));
}

}

