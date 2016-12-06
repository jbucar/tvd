/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "screen.h"
#include <util/log.h>
#include <util/cfg/configregistrator.h>

namespace canvas {

REGISTER_INIT_CONFIG( gui_screen ) {
	root().addNode( "screen" )
		.addValue( "use", "Screen to instance", std::string("custom") )
	;
}

/**
 * Crea y retorna una instancia de Screen.
 */
Screen *Screen::create() {
	const std::string &use = util::cfg::getValue<std::string>("gui.screen.use");
	LINFO("Screen", "Using screen: %s", use.c_str());

	if (use=="custom") {
		return new Screen();
	}

	return NULL;
}

/**
 * Constructor base. Luego de crear una instancia es necesario llamar al método initialize() para poder utilizar la misma.
 */
Screen::Screen()
{
	_isOn = false;
	_connector = canvas::connector::LAST_CONNECTOR;
	_aspect = aspect::LAST_ASPECT;
	_mode = mode::LAST_MODE;
}

/**
 * Destructor base. Antes de destruir una instancia es necesario llamar al método finalize().
 */
Screen::~Screen()
{
}

/**
 * Inicializa el estado de la pantalla. Este método debe ser llamado antes de llamar cualquier otro método.
 */
bool Screen::initialize() {
	if (init()) {
		//	Setup default mode
		connector::type conn = defaultConnector();
		mode( conn, defaultMode(conn) );

		//	Setup default aspect
		aspect( defaultAspect() );

		return true;
	}
	return false;
}

/**
 * Finaliza el estado de la pantalla. Este método debe ser llamado antes de destruir el objeto.
 */
void Screen::finalize() {
	fin();
	_connector = canvas::connector::LAST_CONNECTOR;
	_aspect = aspect::LAST_ASPECT;
	_mode = mode::LAST_MODE;
}

bool Screen::init() {
	return true;
}

void Screen::fin() {
}

/**
 * Cambia el estado de la pantalla a encendido.
 */
bool Screen::turnOn() {
	if (!_isOn && turnOnImpl()) {
		_isOn = true;
	}
	return _isOn;
}

/**
 * Cambia el estado de la pantalla a apagado.
 */
bool Screen::turnOff() {
	if (_isOn && turnOffImpl()) {
		_isOn = false;
	}
	return !_isOn;
}

/**
 * Retorna true si la pantalla está encendida o false en caso contrario.
 */
bool Screen::isOn() const {
	return _isOn;
}

bool Screen::turnOnImpl() {
	LDEBUG("Screen", "turnOn not implemented");
	return true;
}

bool Screen::turnOffImpl() {
	LDEBUG("Screen", "turnOff not implemented");
	return true;
}

// Connector methods
/**
 * Retorna el Connector que se encuentra activo.
 */
connector::type Screen::connector() const {
	return _connector;
}

/**
 * Retorna el Connector por defecto.
 */
connector::type Screen::defaultConnector() const {
	return connector::composite;
}

/**
 * Retorna una lista de Connectors soportados por la pantalla.
 */
std::vector<connector::type> Screen::supportedConnectors() const {
	std::vector<connector::type> tmp;
	tmp.push_back( connector::vga );
	tmp.push_back( connector::composite );
	tmp.push_back( connector::svideo );
	tmp.push_back( connector::component );
	tmp.push_back( connector::dvi );
	tmp.push_back( connector::hdmi );
	return tmp;
}

/**
 * Retorna true si el conector pasado por parámetro es un conector válido, false en caso contrario.
 */
bool Screen::connectorIsValid( connector::type conn ) const {
	if (conn < connector::LAST_CONNECTOR) {
		std::vector<connector::type> valid=supportedConnectors();
		std::vector<connector::type>::const_iterator it=std::find( valid.begin(), valid.end(), conn );
		if (it==valid.end()) {
			LWARN( "Screen", "Video display connector not supported: %d", conn );
			return false;
		}
	}
	return true;
}

// Aspect methods
/**
 * Modifica el aspecto de una pantalla.
 */
bool Screen::aspect( aspect::type vAspect ) {
	LDEBUG( "Screen", "Set video display aspect ratio: %d", vAspect );
	bool result=true;
	if (vAspect != _aspect) {
		result=false;
		std::vector<aspect::type> aspects = supportedAspects();
		std::vector<aspect::type>::const_iterator it=std::find( aspects.begin(), aspects.end(), vAspect );
		if (it != aspects.end()) {
			if (aspectImpl( vAspect )) {
				_onAspectChanged( vAspect );
				_aspect = vAspect;
				result=true;
			}
			else {
				LWARN( "Screen", "Cannot set video aspect mode: %d", vAspect );
			}
		}
		else {
			LWARN( "Screen", "Video aspect mode not supported: %d", vAspect );
		}
	}
	return result;
}

/**
 * Retorna el aspecto actual que esta usando la pantalla.
 */
aspect::type Screen::aspect() const {
	return _aspect;
}

/**
 * Retorna el aspecto por defecto de la pantalla.
 */
aspect::type Screen::defaultAspect() const {
	return aspect::automatic;
}

/**
 * Retorna una lista conteniendo los aspectos soportados por la pantalla.
 */
std::vector<aspect::type> Screen::supportedAspects() const {
	std::vector<aspect::type> tmp;
	tmp.push_back( aspect::automatic );
	return tmp;
}

/**
 * Notifica el cambio de aspecto de una pantalla.
 */
aspect::ChangedSignal &Screen::onAspectChanged() {
	return _onAspectChanged;
}

bool Screen::aspectImpl( aspect::type /*vAspect*/ ) {
	//	Do nothing
	return true;
}

// Mode methods
/**
 * Asigna el modo a una pantalla.
 */
bool Screen::mode( connector::type conn, mode::type vMode ) {
	if (!connectorIsValid(conn)) {
		LWARN( "Screen", "Output connector not valid: conn=%d", conn );
		return false;
	}

	bool result=false;
	if (conn != connector() || vMode != _mode) {
		std::vector<mode::type> modes = supportedModes( conn );
		std::vector<mode::type>::const_iterator it=std::find( modes.begin(), modes.end(), vMode );
		if (it != modes.end()) {
			Mode mode = mode::get( vMode );

			LDEBUG( "Screen", "Set video display mode: conn=%d, mode=%d, name=%s, type=%d, width=%d, height=%d, freq=%d",
				conn, vMode, mode.name, mode.type, mode.width, mode.height, mode.frequency );

			if (modeImpl( conn, vMode )) {
				_connector=conn;
				_mode = vMode;
				_onModeChanged( conn, vMode );
				result=true;
			}
			else {
				LWARN( "Screen", "Cannot set video display mode: %d", vMode );
			}
		}
		else {
			LWARN( "Screen", "Video display mode not supported: %d", vMode );
		}
	}
	else {
		//	The same mode && connector is active
		result=true;
	}
	return result;
}

/**
 * Retorna el modo actual soportado por el connector.
 */
mode::type Screen::mode() const {
	return _mode;
}

/**
 * Retorna el modo por defecto soportado por el connector.
 */
mode::type Screen::defaultMode( connector::type /*conn*/ ) const {
	return mode::m576i_50;
}

/**
 * Retorna una lista de modos soportados por el connector.
 */
std::vector<mode::type> Screen::supportedModes( connector::type conn ) const {
	std::vector<mode::type> tmp;

	switch (conn) {
		case connector::composite:
		case connector::svideo: {
			tmp.push_back( mode::m576i_50 );
			tmp.push_back( mode::mNTSC );
			break;
		}
		case connector::vga:
		case connector::component:
		case connector::dvi:
		case connector::hdmi: {
			tmp.push_back( mode::m576i_50 );
			tmp.push_back( mode::m576p_50 );

			tmp.push_back( mode::m720p_50 );
			tmp.push_back( mode::m720p_60 );

			tmp.push_back( mode::m720i_50 );
			tmp.push_back( mode::m720i_60 );

			tmp.push_back( mode::m1080i_25 );
			tmp.push_back( mode::m1080i_30 );
			tmp.push_back( mode::m1080i_50 );
			tmp.push_back( mode::m1080i_60 );

			tmp.push_back( mode::m1080p_24 );
			tmp.push_back( mode::m1080p_25 );
			tmp.push_back( mode::m1080p_30 );
			tmp.push_back( mode::m1080p_50 );
			tmp.push_back( mode::m1080p_60 );
			break;
		}
		default: {
			LWARN( "Screen", "Invalid type of connector: conn=%02x", conn );
		}
	};
	return tmp;
}

mode::ChangedSignal &Screen::onModeChanged() {
	return _onModeChanged;
}

bool Screen::modeImpl( connector::type /*conn*/, mode::type /*mode*/ ) {
	//	Do nothing ...
	return true;
}

}
