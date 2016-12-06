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

#include "show.h"
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

Show::Show( pvr::Show *s )
{
	_show = s;

	//	Create template
	v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(v8::Isolate::GetCurrent());
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	tpl->SetClassName(_val("Show"));
	NODE_SET_PROTOTYPE_METHOD(tpl, "percentage", Show::percentage);

	//	Create instance
	v8::Local<v8::Function> function = tpl->GetFunction();
	v8::Local<v8::Object> instance = function->NewInstance();

	setValue( instance, "name",        s->name()              );
	setValue( instance, "description", s->description()       );
	setValue( instance, "startTime",   s->timeRange().begin() );
	setValue( instance, "endTime",     s->timeRange().end()   );
	setValue( instance, "category",    s->category()          );
	setValue( instance, "parentalContent", s->parentalControl()->getContentName() );
	setValue( instance, "parentalAge", s->parentalControl()->getAgeName() );

	this->Wrap( instance );
}

Show::~Show()
{
}

//	Node implementation
void  Show::percentage( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	Show *wrap = node::ObjectWrap::Unwrap<Show>(args.This());
	DTV_ASSERT(wrap);
	pvr::Show *show = wrap->_show;
	DTV_ASSERT(show);
	args.GetReturnValue().Set(_val(show->percentage()));
}

}

