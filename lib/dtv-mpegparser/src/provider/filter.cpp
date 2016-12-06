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
#include "filter.h"


namespace tuner {

/**
 * Crea un filtro para el TS identificado por @b pid.
 * @param pid Id del TS a filtrar
 */
Filter::Filter( ID pid )
	: _timeout(0), _pid(pid)
{
}

/**
 * Crea un filtro para el TS identificado por @b pid.
 * @param pid Id del TS a filtrar
 * @param timeout del filtro
 */
Filter::Filter( ID pid, util::DWORD timeout )
	: _timeout(timeout), _pid(pid)
{
}

/**
 * Destructor base.
 */
Filter::~Filter()
{
}

/**
 * @return true si el filtro fue inicializado, false en caso contrario.
 */
bool Filter::initialized() const {
	return true;
}

/**
 * Inicializa el filtro.
 * @return true si no hubo errores durante la inicialización, false en caso contrario.
 */
bool Filter::initialize() {
	return true;
}

/**
 * Finaliza el estado del filtro.
 */
void Filter::deinitialize() {
}

/**
 * Comienza la ejecución el filtro, provocando que se comience a demultiplexar los TS identificados por el id indicado al crear el filtro.
 * @return true si pudo comenzar la ejecución correctamente, false en caso contrario.
 */
bool Filter::start() {
	return true;
}

/**
 * Detiene la ejecución del filtro.
 */
void Filter::stop() {
}

/**
 * Detiene la ejecución del filtro y lo vuelve a iniciar.
 */
void Filter::restart() {
	stop();
	start();
}

/**
 * @return El identificador con el cual fue creado el filtro.
 */
ID Filter::pid() const {
	return _pid;
}

/**
 * @return El timeout con el cual fue creado el filtro.
 */
util::DWORD Filter::timeout() const {
	return _timeout;
}

}

