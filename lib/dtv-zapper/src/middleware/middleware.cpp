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

#include "middleware.h"
#include "provider.h"
#include "application.h"
#include <util/mcr.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/bind.hpp>

namespace zapper {
namespace middleware {

/**
 * Construye un objeto @c Manager.
 * @param tuner Un string con el tuner.
 * @param ctrl Controlador del objeto.
 */
Middleware::Middleware()
{
	_id = 0;
}

/**
 * Destruye a la instancia de la clase @c Manager.
 */
Middleware::~Middleware()
{
	CLEAN_ALL( Application *, _apps );
}

/**
 * @return Colección que contiene todas las aplicaciones (objeto @c Application) del middleware
 */
const std::vector<Application *> &Middleware::getApplications() const {
	return _apps;
}

/**
 * Inicia una aplicación.
 * @param id Identificador de la aplicación a iniciar.
 */
bool Middleware::startApplication( int id ) {
	bool result=false;
	AppFinder finder(id);
	std::vector<Application *>::const_iterator it=std::find_if(
		_apps.begin(), _apps.end(), finder );
	if (it != _apps.end()) {
		result=(*it)->start();
	}
	else {
		LWARN( "Manager", "Application not found: name=%d", id );
	}
	return result;
}

/**
 * Termina una aplicación.
 * @param id Identificador de la aplicación a terminar.
 */
void Middleware::stopApplication( int id ) {
	AppFinder finder(id);
	std::vector<Application *>::const_iterator it=std::find_if(
		_apps.begin(), _apps.end(), finder );
	if (it != _apps.end()) {
		(*it)->stop();
	}
	else {
		LWARN( "Manager", "Application not found: name=%s", id );
	}
}

void Middleware::stopAll() {
	BOOST_FOREACH( Application *app, _apps ) {
		app->stop();
	}
}

bool Middleware::isAnyRunning() const {
	bool running = false;
	BOOST_FOREACH( Application *app, _apps ) {
		running |= app->isRunning();
	}
	return running;
}

void Middleware::lockApplications( bool lock ) {
	BOOST_FOREACH( Provider *p, getAll() ) {
		p->lockApplications( lock );
	}
}

OnDownloadProgress &Middleware::onDownloadProgress() {
	return _onDownloadProgress;
}

int Middleware::addApplication( Application *info ) {
	int id = _id++;
	info->attach( id );
	_apps.push_back( info );
	return id;
}

void Middleware::rmApplication( int id ) {
	AppFinder finder(id);
	std::vector<Application *>::iterator it=std::find_if( _apps.begin(), _apps.end(), finder );
	if (it != _apps.end()) {
		delete (*it);
		_apps.erase( it );
	}
}

void Middleware::sendDownloadProgress( int id, int step, int total ) {
	_onDownloadProgress( id, step, total );
}

}
}

