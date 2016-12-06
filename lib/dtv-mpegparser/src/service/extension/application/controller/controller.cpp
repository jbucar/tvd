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
#include "controller.h"
#include "runtask.h"
#include "../application.h"
#include "../extension.h"
#include "../factories/aitfactory.h"
#include "../factories/profile/ginga-ncl/nclprofile.h"
#include "../../../servicemanager.h"
#include <util/assert.h>
#include <util/log.h>
#include <boost/bind.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

namespace tuner {
namespace app {

namespace disable {

enum type {
	player=0x01,
	user  =0x02
};

}	//	namespace disable

Controller::Controller()
{
	_mgr = NULL;
	_extension = NULL;
	_disableMask = 0;
}

Controller::~Controller()
{
}

//	Create and setup extension
Extension *Controller::initialize() {
	DTV_ASSERT(!_extension);

	_disableMask = 0;

	//	Create extension
	_extension = new Extension( this );

	//	Add application factories
	setupFactories( _extension );

	return _extension;
}

void Controller::finalize() {
	delete _extension;
	_extension = NULL;
}

bool Controller::isInitialized() const {
	return _extension ? true : false;
}

void Controller::setupFactories( tuner::app::Extension *ext ) {
	//	Create AIT factory
	tuner::app::AitFactory *ait = new tuner::app::AitFactory( ext );

	//	Add supported profiles
	ait->addProfile( new tuner::app::NCLProfile( ext ) );

	//	Add factory
	ext->addFactory( ait );
}

void Controller::attach( ServiceManager *mgr ) {
	DTV_ASSERT(mgr);
	_mgr = mgr;
}

ServiceManager *Controller::mgr() const {
	DTV_ASSERT(_mgr);
	return _mgr;
}

void Controller::onApplicationAdded( Application * /*app*/ ) {
}

void Controller::onApplicationRemoved( Application * /*app*/ ) {
}

void Controller::onApplicationChanged( Application * /*app*/ ) {
}

void Controller::onDownloadProgress( Application * /*app*/, int /*step*/, int /*total*/ ) {
}

Extension *Controller::extension() const {
	DTV_ASSERT(_extension);
	return _extension;
}

//	Start application
void Controller::disableApplications( bool disable ) {
	disableMask( disable::user, disable );
}

void Controller::onChannelBlocked( bool blocked ) {
	disableMask( disable::player, blocked );
}

void Controller::disableMask( util::DWORD mask, bool set ) {
	bool old = _disableMask ? true : false;

	if (set) {
		_disableMask |= mask;
	}
	else {
		_disableMask &= ~mask;
	}

	bool state = _disableMask ? true : false;
	LDEBUG( "Controller", "Disable applications: state=%08x, mask=%08x, set=%d -> needQueue=%d", _disableMask, mask, set, state );
	if (isInitialized() && old != state) {
		if (_disableMask) {
			extension()->queueAll();
		}
		else {
			extension()->startQueued();
		}
	}
}

bool Controller::canStartApplications() const {
	return _disableMask ? false : true;
}

//	Keys
void Controller::reserveKeys( const ApplicationID & /*id*/, const Keys & /*keys*/, int /*priority*/ ) {
}

//	Graphic layer
void Controller::enableLayer( bool /*st*/ ) {
}

bool Controller::setupLayer( Application * /*app*/, int & /*w*/, int & /*h*/ ) {
	return false;
}

//	Player methods
PlayerAdapter *Controller::createMediaPlayerAdapter() const {
	return NULL;
}

PlayerAdapter *Controller::createElementaryPlayerAdapter( int /*id*/ ) const {
	return NULL;
}

PlayerAdapter *Controller::createMainPlayerAdapter() const {
	return NULL;
}

}
}

