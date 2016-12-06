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

#include "closedcaptionplayer.h"
#include <util/log.h>

namespace tvd {

ClosedCaptionPlayer::ClosedCaptionPlayer( node::ObjectWrap *handle, bool isCC )
	: _handle(handle), _isCC(isCC)
{
}

ClosedCaptionPlayer::~ClosedCaptionPlayer()
{
}

void ClosedCaptionPlayer::enable( bool /*state*/ ) {
}

void ClosedCaptionPlayer::show( const std::string &text ) {
	LDEBUG("tvd", "Show CC: text=%s", text.c_str() );
	v8::HandleScope scope(v8::Isolate::GetCurrent());

	v8::Handle<v8::Value> argv[2];
	argv[0] = _val("closedCaption");
	argv[1] = _val(text);
	//	Call
	makeCallback(_handle, "emit", 2, argv );
}

}	//	namespace tvd

