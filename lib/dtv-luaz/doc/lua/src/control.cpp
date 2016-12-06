/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-luaz implementation.

    DTV-luaz is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-luaz is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-luaz.

    DTV-luaz es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-luaz se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

/**
 * Provee funcionalidad para el manejo de control parental.
 */
namespace control {

/**
 * Establece la clave de control parental.
 * @param string_LuaParam_str La clave a establecer.
 */
luatype setPIN( luatype str );

/**
 * Borra la clave de control parental.
 */
luatype resetPIN();

/**
 * Valida el codigo de control parental indicado en @b str.
 * @param string_LuaParam_str El codigo a validar.
 * @return True si el codigo es correcto, false en caso contrario.
 */
luatype checkPIN( luatype str );

/**
 * Checkea si el control parental esta habilitado.
 * @return 1 si el control parental esta habilitado, 0 en caso contrario.
 */
luatype isSet();

/**
 * Indica si se accedio al control parental y la sesión se encuentra activa.
 * @return 1 si la sesión esta activa, 0 en caso contrario.
 */
luatype isSessionOpened();

/**
 * @return El valor de la edad de control parental.
 */
luatype getParentalAge();

/**
 * Setea la edad utilizada para el control parental.
 * @param number_LuaParam_age El valor de la edad de control parental a setear.
 */
luatype setParentalAge( luatype age );

/**
 * @return La configuración utilizada de control parental.
 */
luatype getParentalContent();

/**
 * Setea la configuración de control parental.
 * @param number_LuaParam_byte La configuración de control parental a utilizar.
 */
luatype setParentalContent( luatype byte );

}
