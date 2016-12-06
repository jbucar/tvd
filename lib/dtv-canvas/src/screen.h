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

#pragma once

#include "types.h"
#include <vector>

namespace canvas {

/**
 * Esta clase representa la pantalla del sistema.
 */
class Screen {
public:
	Screen();
	virtual ~Screen();

	static Screen *create();

	bool initialize();
	void finalize();

	// Power
	bool turnOn();
	bool turnOff();
	bool isOn() const;

	// Aspect
	bool aspect( aspect::type vAspect );
	aspect::type aspect() const;
	virtual aspect::type defaultAspect() const;
	virtual std::vector<aspect::type> supportedAspects() const;
	aspect::ChangedSignal &onAspectChanged();

	// Modes
	connector::type connector() const;
	virtual connector::type defaultConnector() const;
	virtual std::vector<connector::type> supportedConnectors() const;
	
	bool mode( connector::type conn, mode::type vMode );
	mode::type mode() const;
	virtual mode::type defaultMode( connector::type conn ) const;
	virtual std::vector<mode::type> supportedModes( connector::type conn ) const;
	mode::ChangedSignal &onModeChanged();

protected:
	virtual bool init();
	virtual void fin();

	virtual bool turnOnImpl();
	virtual bool turnOffImpl();

	virtual bool aspectImpl( aspect::type vAspect );
	virtual bool modeImpl( connector::type conn, mode::type vMode );

	// Connectors aux
	bool setConnector( connector::type conn );
	bool connectorIsValid( connector::type conn ) const;

private:
	bool _isOn;
	connector::type _connector;
	aspect::type _aspect;
	mode::type _mode;

	// Signals
	aspect::ChangedSignal _onAspectChanged;
	mode::ChangedSignal _onModeChanged;
};

}
