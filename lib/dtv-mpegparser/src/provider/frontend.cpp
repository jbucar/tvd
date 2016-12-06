/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-mpegparser implementation.

    DTV-mpegparser is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-mpegparser is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-mpegparser.

    DTV-mpegparser es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-mpegparser se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "frontend.h"

namespace tuner {

/**
 * Constructor base.
 */
Frontend::Frontend()
{
}

/**
 * Destructor base.
 */
Frontend::~Frontend()
{
}

/**
 * Inicializa el estado del Frontend. Este método debe llamarse luego de crear el objeto y antes de llamar a cualquier otro método del mismo.
 * @return true si la inicialización finalizó correctamente, false en caso contrario.
 */
bool Frontend::initialize() {
	return true;
}

/**
 * Finaliza el estado del Frontend.
 */
void Frontend::finalize() {
}

/**
 * Detiene la lectura de TS.
 */
void Frontend::stop() {
}

/**
 * Actualiza el estado del Frontend.
 * @param[out] st Estado del Frontend.
 * @return true si pudo obtener a información correctamente, false en caso contrario.
 */
bool Frontend::getStatus( status::Type & /*st*/ ) const {
	return false;
}

}

