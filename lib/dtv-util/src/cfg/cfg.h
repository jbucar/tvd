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

#include "propertynode.h"
#include <string>

/**
 * @file dtv-util/src/cfg/cfg.h
 * @namespace util::cfg
 * @brief Arhivo donde se agrupan funciones del árbol de configuración.
 * 
 * En este archivo se agrupan funciones para leer/escribir valores del árbol de configuración.
 */

namespace util {
namespace cfg {
  


typedef PropertyNode PropertyTree;

/**
 * @return El nodo raíz del árbol de configuración.
 */
PropertyTree &get();

/**
 * @param path Un string con una ruta hacia un sub-árbol.
 * @return Un árbol de configuración con raiz dada por el parámetro path.
 */
PropertyTree &get( const std::string &path );

/**
 * Se lee una propiedad del árbol.
 * @param path La propiedad a leer.
 * @return El valor de la propiedad especifícado en path.
 */
template<class T>
const T &getValue( const std::string &path ) {
	return get().get<T> (path);
}

/**
 * Se escribe un valor en una propiedad al árbol.
 * @param path La propiedad en la cual se va a escribir el nuevo valor.
 * @param val El valor a escribir.
 */
template<class T>
void setValue( const std::string &path, T val ) {
	get().set<T> (path, val);
}

}
}
