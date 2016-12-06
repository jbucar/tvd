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

#pragma once

#include <string>

namespace zapper {
namespace middleware {

class Application {
public:
	explicit Application( const std::string &name );
	Application( const std::string &name, const std::string &icon );
	virtual ~Application();

	int id() const;
	const std::string &name() const;

	void icon( const std::string &ico );
	const std::string &icon() const;

	void user( void *ptr );
	void *user() const;

	virtual bool start()=0;
	virtual void stop()=0;
	virtual bool isRunning() const=0;
	virtual bool isAttachedToChannel() const;

	//	Implementation
	void attach( int id );

private:
	int _id;
	std::string _name;
	std::string _icon;
	void *_user;
};

struct AppFinder {
	AppFinder( int id ) : _id(id) {}
	bool operator()( const Application *app ) const {
		return app->id() == _id;
	}

private:
	int _id;
	AppFinder &operator=( AppFinder & /*af*/ ) { return *this; }
};

}
}

