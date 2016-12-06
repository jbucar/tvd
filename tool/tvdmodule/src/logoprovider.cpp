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

#include "logoprovider.h"
#include "channel.h"
#include <pvr/channel.h>
#include <util/log.h>

namespace tvd {
namespace logos {

Provider::Provider( util::Settings *settings, const v8::Local<v8::Function> findFnc )
	: pvr::logos::Provider( "logos::Node", "1.0" ), _settings(settings)
{
	_findFunc.Reset(v8::Isolate::GetCurrent(), findFnc);
}

Provider::~Provider()
{
}

bool Provider::find( pvr::Channel *ch, std::string &logoFile, int &priority ) {
	LTRACE("tvd", "Find logo");
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::HandleScope scope(isolate);

	//	Setup js object
	v8::Handle<v8::Object> info = v8::Object::New(isolate);
	setValue( info, "id",       ch->channelID() );
	setValue( info, "channel",  ch->channel()   );
	setValue( info, "name",     ch->name()      );
	{	//	Setup info
		v8::Handle<v8::Object> chInfo = v8::Object::New(isolate);
		if (getInfoFromID( ch->uri(), ch->network(), chInfo )) {
			setValue( info, "info", chInfo );
		}
	}

	v8::Handle<v8::Value> argv[1];
	argv[0] = info;
	v8::TryCatch tryCatch;
	tryCatch.SetVerbose(true);
	v8::Local<v8::Function> findFunc = v8::Local<v8::Function>::New(isolate, _findFunc);
	v8::Local<v8::Value> result=findFunc->Call( isolate->GetCurrentContext()->Global(), 1, argv );
	if (tryCatch.HasCaught()) {
		std::string msg;
		impl::get( tryCatch.Message()->Get(), msg );
		LERROR( "tvd", "Catch javascript exception from javascript callback function: line=%d, msg=%s",
			tryCatch.Message()->GetLineNumber(), msg.c_str() );
		return false;
	}
	else if (result->IsObject()) {
		v8::Handle<v8::Object> obj = result->ToObject();
		if (check( obj, "logo", logoFile ) && check( obj, "priority", priority )) {
			LDEBUG( "tvd", "Logo found: logo=%s, priority=%d", logoFile.c_str(), priority );
			return true;
		}
	}
	return false;
}

util::Settings *Provider::settings() const {
	return _settings;
}

}
}


