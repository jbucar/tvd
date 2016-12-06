/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#include "extension.h"
#include "../servicemanager.h"
#include "../../demuxer/psi/psi.h"
#include "../../resourcemanager.h"
#include <util/assert.h>

namespace tuner {

/**
 * Constructor base.
 */
Extension::Extension()
{
	_srvMgr = NULL;
	_resMgr = NULL;
}

/**
 * Destructor base.
 */
Extension::~Extension()
{
	delete _resMgr;
}

/**
 * Inicializa la extensión.
 * @param srvMgr Instancia de @b ServiceManager.
 * @return true si la inicialización finalizó correctamente, false en caso contrario.
 */
bool Extension::initialize( ServiceManager *srvMgr ) {
	DTV_ASSERT(!_srvMgr);
	_srvMgr = srvMgr;
	return onInitialize();
}

/**
 * Finaliza la extensión.
 */
void Extension::finalize() {
	onFinalize();
	_srvMgr = NULL;
}

bool Extension::isInitialized() const {
	return _srvMgr ? true : false;
}

/**
 * Setea el ResourceManager que utilizará la extensión.
 * @param resMgr El ResourceManager a setear.
 */
void Extension::resMgr( ResourceManager *resMgr ) {
	delete _resMgr;
	_resMgr = resMgr;
}

/**
 * @return El ServiceManager con el cual se creo la extensión.
 */
ServiceManager *Extension::srvMgr() {
	DTV_ASSERT(_srvMgr);
	return _srvMgr;
}

//	Notifications
bool Extension::onInitialize() { return true; }
void Extension::onFinalize() {}

/**
 * Este método se llama cuando se arranca el ServiceManager.
 */
void Extension::onStart() {}

/**
 * Este método se llama cuando se frena el ServiceManager.
 */
void Extension::onStop() {}

/**
 * Indica un cambio de estado en el ServiceProvider.
 * @param bool Indica si se debe iniciar el filtro correspondiente o frenar el mismo.
 */
void Extension::onReady( bool ) {}

/**
 * Indica un cambio de estado en el servicio @b srv.
 * @param srv El servicio que cambio de estado.
 * @param ready Indica si el servicio esta listo o no.
 */
void Extension::onServiceReady( Service * /*srv*/, bool /*ready*/ ) {}

/**
 * Indica un cambio de estado en el servicio @b srv.
 * @param srv El servicio que se sintonizó/cambio.
 * @param started Indica si se sintonizó o se cambió el servicio.
 */
void Extension::onServiceStarted( Service * /*srv*/, bool /*started*/ ) {}

/**
 * @param inScan Indica se se esta escaneando canales.
 * @return true si la extensión espera recibir notificaciones, false en caso contrario.
 */
bool Extension::mustNotify( bool inScan ) const {
	return inScan ? false : true;
}

bool Extension::checkTS( ID tsID ) const {
	return _srvMgr->currentTS() == tsID;
}

bool Extension::checkNit( ID nitID ) const {
	ID currentNit = _srvMgr->currentNit();
	return (currentNit == NIT_ID_RESERVED || currentNit == nitID);
}

/**
 * @return Instancia de la clase @b ResourceManager.
 */
ResourceManager *Extension::resMgr() {
	return _resMgr ? _resMgr : _srvMgr->resMgr();
}

}
