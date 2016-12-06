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

#include "mainplayeradapter.h"
#include "controller.h"
#include <node/util.h>

namespace tvd {
namespace ginga {

MainPlayerAdapter::MainPlayerAdapter( Controller *ctrl )
{
	_ctrl = ctrl;
}

MainPlayerAdapter::~MainPlayerAdapter()
{
}

const std::string MainPlayerAdapter::name() {
	return "tac";
}

bool MainPlayerAdapter::playImpl( const std::string &url ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	v8::Handle<v8::Object> evt = v8::Object::New(v8::Isolate::GetCurrent());
	setValue( evt, "method", "play" );
	setValue( evt, "url", url );
	_ctrl->notify( "mainVideo", evt );
	return true;
}

void MainPlayerAdapter::stopImpl() {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	v8::Handle<v8::Object> evt = v8::Object::New(v8::Isolate::GetCurrent());
	setValue( evt, "method", "stop" );
	_ctrl->notify( "mainVideo", evt );
}

void MainPlayerAdapter::moveResizeImpl( int x, int y, int w, int h ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	v8::Handle<v8::Object> evt = v8::Object::New(v8::Isolate::GetCurrent());
	setValue( evt, "method", "resize" );

	{	//	Rect
		v8::Handle<v8::Object> dstRect = v8::Object::New(v8::Isolate::GetCurrent());
		setValue( dstRect, "x", x );
		setValue( dstRect, "y", y );
		setValue( dstRect, "w", w );
		setValue( dstRect, "h", h );

		setValue( evt, "rect", dstRect );
	}

	_ctrl->notify( "mainVideo", evt );
}

}
}

