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

#pragma once

#include "../types.h"

namespace tuner {

/**
 * Frontend permite la lectura de TS desde distintos medios (DVB, archivo, RTP, etc).
 */
class Frontend {
public:
	Frontend();
	virtual ~Frontend();

	//	Initialization
	virtual bool initialize();
	virtual void finalize();

	//	Network getters
	/**
	 * Indica la cantidad de canales disponibles en el caso de dvb::Frontend, cantidad de archivos en el caso de FileFrontend, etc.
	 * @return La cantidad de redes disponibles.
	 */
	virtual size_t getCount() const=0;

	/**
	 * @param nIndex Índice de la red.
	 * @return El nombre de la red indicada por @b nIndex.
	 */
	virtual std::string getName( size_t nIndex ) const=0;

	/**
	 * @param name Nombre de la red.
	 * @return El índice de la red con nombre @b name.
	 */
	virtual size_t find( const std::string &name ) const=0;

	//	Network operations
	/**
	 * Comienza a leer TS desde la red indicada por @b nIndex.
	 * @param nIndex Índice de la red.
	 * @return true si pudo comenzar la lectura correctamente, false en caso contrario.
	 */
	virtual bool start( size_t nIndex )=0;
	virtual void stop();

	//	Status/signal
	virtual bool getStatus( status::Type &st ) const;
};

}
