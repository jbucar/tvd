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

#include <node/util.h>
#include <boost/signals2.hpp>

namespace pvr {
	class Channels;
	class Channel;
}

namespace tvd {

class Channels : public node::ObjectWrap {
public:
	Channels( pvr::Channels *chs );
	virtual ~Channels();

	static void Init();
	void stop();

	//	Methods
	DECL_METHOD(count);
	DECL_METHOD(get);
	DECL_METHOD(getAll);
	DECL_METHOD(remove);

protected:
	static pvr::Channels *channels(const v8::FunctionCallbackInfo<v8::Value> &args);
	void onChannelAdded( pvr::Channel *ch );
	void onChannelRemoved( pvr::Channel *ch, bool removeAll );

private:
	pvr::Channels *_chs;
	static v8::Persistent<v8::Function> _constructor;
	boost::signals2::connection _cNew, _cRemoved;
};

}

