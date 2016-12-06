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

#pragma once

#include <node/v8.h>
#include <node/node.h>
#include <node/node_object_wrap.h>
#include <util/string.h>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

#define FIN	args.GetReturnValue().SetUndefined();

#define THROW(s)	\
	{ \
		std::string err = util::format( "[node: %s] %s", __FUNCTION__, s ); \
		isolate->ThrowException(v8::Exception::TypeError(_val(s))); \
	}

#define THROW_RETURN(s)	\
	THROW(s) \
	FIN; \
	return;

#define RO_ELEMENT static_cast<v8::PropertyAttribute>(v8::ReadOnly|v8::DontDelete)
#define DECL_METHOD(m) static void m( const v8::FunctionCallbackInfo<v8::Value> &args )

namespace tvd {

namespace bfs = boost::filesystem;
namespace bpt=boost::posix_time;

namespace impl {

inline bool get( const v8::Handle<v8::Value> &oValue, std::string &value ) {
	if (!oValue->IsString()) {
		return false;
	}
	v8::String::Utf8Value utf8Val(oValue->ToString());
	value = *utf8Val;
	return true;
}

inline bool get( const v8::Handle<v8::Value> &oValue, int &value ) {
	if (!oValue->IsInt32()) {
		return false;
	}
	value = oValue->Int32Value();
	return true;
}

inline bool get( const v8::Handle<v8::Value> &oValue, bool &value ) {
	if (!oValue->IsBoolean()) {
		return false;
	}
	value = oValue->BooleanValue();
	return true;
}

inline bool get( const v8::Handle<v8::Value> &oValue, v8::Handle<v8::Object> &result ) {
	if (!oValue->IsObject()) {
		return false;
	}
	result = oValue->ToObject();
	return true;
}

}

//	Object utils
inline v8::Local<v8::String> _val( const char *str ) {
	return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), str);
}

inline v8::Local<v8::String> _val( const std::string &value ) {
	return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), value.c_str());
}

inline v8::Local<v8::Integer> _val( int value ) {
	return v8::Integer::New(v8::Isolate::GetCurrent(), value);
}

inline v8::Local<v8::Integer> _val( unsigned int value ) {
	return v8::Integer::New(v8::Isolate::GetCurrent(), value);
}

inline v8::Local<v8::Integer> _val( long value ) {
	return v8::Integer::New(v8::Isolate::GetCurrent(), value);
}

inline v8::Local<v8::Integer> _val( unsigned long value ) {
	return v8::Integer::New(v8::Isolate::GetCurrent(), value);
}

inline v8::Local<v8::Boolean> _val( bool value ) {
	return v8::Boolean::New(v8::Isolate::GetCurrent(), value);
}

inline v8::Local<v8::String> _val( const bpt::ptime &pt ) {
	std::string time = pt.is_not_a_date_time() ? "" : bpt::to_simple_string(pt);
	return _val( time );
}

inline v8::Handle<v8::Value> _val( const v8::Handle<v8::Value> &val ) {
	return val;
}

inline bool check( const v8::Handle<v8::Object> &obj, const std::string &key, v8::Handle<v8::Object> &value ) {
	return impl::get( obj->Get(_val(key)), value );
}

inline bool check( const v8::Handle<v8::Object> &obj, const std::string &key, std::string &value ) {
	return impl::get( obj->Get(_val(key)), value );
}

inline bool check( const v8::Handle<v8::Object> &obj, const std::string &key, int32_t &value ) {
	return impl::get( obj->Get(_val(key)), value );
}

inline bool check( const v8::Handle<v8::Object> &obj, const std::string &key, bool &value ) {
	return impl::get( obj->Get(_val(key)), value );
}

template <typename target_t, typename value_t>
inline void setValue( target_t obj, const char* name, const value_t &value ) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	obj->ForceSet(v8::String::NewFromUtf8(isolate, name, v8::String::kInternalizedString),
			_val(value),
			RO_ELEMENT);
}

inline void makeCallback( node::ObjectWrap *o, const char *fnc, int argc, v8::Handle<v8::Value> *argv ) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	node::MakeCallback( isolate, o->handle(isolate), fnc, argc, argv );
}

}

