/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "types.h"
#include <string>

namespace util {

/**
 * La clase @a Process provee métodos y propiedades para gestionar la ejecución de procesos. Permite iniciar o detener la
 * ejecución, agregarle parámetros o variables de entorno.
 */
class Process {
public:
	explicit Process( const std::string &cmd );
	Process( const std::string &cmd, const Params &params );
	Process( const std::string &cmd, const Environment &env );
	Process( const std::string &cmd, const Params &params, const Environment &env );
	virtual ~Process();

	bool run();
	void wait();
	void kill( int msTimeout=0 );

	// Getters
	PROCESS_ID pid() const;
	bool isRunning();

	//	Params
	void parameters( const Params &params );
	void addParam( const std::string &param );
	void clearParams();

	//	Environments
	void addToEnvironment( const std::string &var ); //format "kay=value"
	void addToEnvironment( const std::string &key, const std::string &value );
	void clearEnvironment();

protected:
	bool checkStatus( bool wait );
	char **makeParams() const;
	char **makeEnv() const;
	void freeParams( char **params );

private:
	PROCESS_ID _pid;
	Params _parameters;
	Environment _environment;

	Process() {};
};

}


