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
namespace parental {
	class Session;
}
}

namespace tvd {

class Session : public node::ObjectWrap {
public:
	Session( pvr::parental::Session *s );
	virtual ~Session();

	static void Init();
	void stop();

	//	Enable session
	DECL_METHOD(enable);
	DECL_METHOD(isEnabled);

	//	Check session/expiration
	DECL_METHOD(check);
	DECL_METHOD(isBlocked);
	DECL_METHOD(expire);

	//	Time expiration
	DECL_METHOD(getTimeExpiration);
	DECL_METHOD(setTimeExpiration);

	//	Parental control
	//TODO: bool isAllowed( pvr::parental::Control * );
	DECL_METHOD(restrictSex);
	DECL_METHOD(isSexRestricted);
	DECL_METHOD(restrictViolence);
	DECL_METHOD(isViolenceRestricted);
	DECL_METHOD(restrictDrugs);
	DECL_METHOD(isDrugsRestricted);
	DECL_METHOD(restrictAge);
	DECL_METHOD(ageRestricted);

protected:
	void onEnabled( bool isEnabled );
	void onActive( bool isActive );
	static pvr::parental::Session *session(const v8::FunctionCallbackInfo<v8::Value> &args);

private:
	pvr::parental::Session *_session;
	boost::signals2::connection _cEnabled, _cActive;
	static v8::Persistent<v8::Function> _constructor;
};

}

