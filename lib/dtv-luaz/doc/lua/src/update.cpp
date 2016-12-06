/********************************************************************************

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
 * Provee funcionalidad relacionada con las actualizaciones del zapper.
 */
namespace update {

/**
 * Evento que indica la presencia de una actualización del zapper.
 * Si la actualización es obligatoria se aplicará la misma, de lo contrario si esta función se encuentra definida en lua, será llamada con el id de la actualización como parametro.
 * @param number_LuaParam_id Identificador de la actualización.
 */
luatype zapperUpdateReceived( luatype id );

/**
 * Evento que indica el progreso de descarga de una actualización del zapper.
 * Si esta función se encuentra definida en lua, será llamada cuando progrese la descarga de una actualización del zapper.
 * @param number_LuaParam_step Indica el progreso actual de descarga.
 * @param number_LuaParam_total Indica el total de la descarga.
 */
luatype zapperUpdateDownloadProgress( luatype step, luatype total );

/**
 * @return 1 si las actualizaciones del zapper se encuentran activadas, 0 en caso contrario.
 */
luatype isEnabled();

/**
 * Activa/desactiva las actualizaciones del zapper.
 * @param number_LuaParam_val Indica si se deben activar o desactivar las actualizaciones del zapper (1=activar, 0=desactivar).
 */
luatype enable( luatype val );

/**
 * @param number_LuaParam_id El id de la actualización.
 * @param number_LuaParam_apply Indica si debe aplicar o descartar la actualización (1=aplicar, 0=descartar).
 */
luatype apply( luatype id, luatype apply );

}
