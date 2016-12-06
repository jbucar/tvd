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
 * Provee funcionalidad para consultar información sobre canales y programación.
 */
namespace channels {

/**
 * Evento que indica que se encontró un nuevo canal.
 * Si esta función se encuentra definida en lua, será llamada cuando se encuentre un nuevo canal con el canal encontrado como parametro.
 * @param table_LuaParam_ch Tabla que representa al nuevo canal encontrado.
 * @see getAll para ver la estructura de un canal.
 */
luatype zapperProgramFound( luatype ch );

/**
 * Evento que indica un cambio de canal.
 * Si esta función se encuentra definida en lua, será llamada cuando ocurra un cambio de canal con el id del nuevo canal como parametro.
 * @param number_LuaParam_id Identificador del canal al cual se cambio.
 */
luatype zapperChannelChanged( luatype id );

/**
 * Indica si existen canales.
 * @return 1 si hay canales disponibles, 0 en caso contrario.
 */
luatype haveChannels();

/**
 * @return La cantidad de canales disponibles.
 */
luatype channelCount();

/**
 * Obtiene la lista de canales donde cada canal esta representado por una tabla con los siguientes campos:
 * - channelID: el id del canal.
 * - channel:   el número del canal.
 * - name:      el nombre del canal.
 * - oneSeg:    1 si el canal es on-seg, 0 en caso contrario.
 * - blocked:   1 si el canal esta bloqueado, 0 en caso contrario.
 * - favorite:  1 si el canal esta marcado como favorito, 0 en caso contrario.
 * .
 * @return Una tabla con todos los canales.
 */
luatype getAll();

/**
 * @param number_LuaParam_id Identificador del canal a retornar.
 * @return El canal identificado por @b id.
 * @see getAll para ver la estructura de un canal.
 */
luatype get( luatype id );

/**
 * Obtiene información extendida de un canal.
 * A demás de los campos indicados en @ref l_getAll añade:
 * - logo:            La imagen del logo del canal.
 * - parentalAge:     La edad de control parental del programa actual.
 * - parentalContent: El contenido de control parental del programa actual.
 * - showName:        El nombre del programa actual.
 * - showDescription: La descripción del programa actual.
 * .
 * @param number_LuaParam_id Identificador del canal a retornar.
 * @return El canal identificado por @b id con información extendida.
 */
luatype getInfo( luatype id );

/**
 * Obtiene una lista de programas correspondientes al canal identificado por @b id, que se muestran entre las
 * fechas @b from y @b to. Cada programa esta representado por una tabla con los siguientes campos:
 * - name:            El nombre del programa.
 * - description:     La descripción del programa.
 * - start:           La fecha y hora de comienzo del programa.
 * - duration:        La duración del programa.
 * - parentalAge:     La edad de control parental.
 * - parentalContent: El contenido de control parental.
 * - length:          La duración en minutos del programa.
 * - startDate:
 *   - time:          La hora de inicio del programa.
 *   - date:          La fecha de inicio del programa.
 * - stopDate:
 *   - time:          La hora de fin del programa.
 *   - date:          La fecha de fin del programa.
 * .
 * @param number_LuaParam_id El identificador del canal.
 * @param string_LuaParam_from Fecha/hora inicial.
 * @param string_LuaParam_to Fecha/hora final.
 * @return Una tabla con los programas del canal identificado por @b id
 * que se muestran entre las fechas especificadas en @b from y @b to.
 */
luatype getShowsBetween( luatype id, luatype from, luatype to );

/**
 * Muestra/oculta los canales one-seg.
 * @return 1 si se muestran los canales one-seg, 0 en caso contrario.
 */
luatype toggleOneSeg();

/**
 * @return 1 si actualmente se estan mostrando los canales one-seg, 0 en caso contrario.
 */
luatype showOneSeg();

/**
 * Borra el canal identificado por @b id.
 * @param number_LuaParam_id El identificador del canal.
 */
luatype remove( luatype id );

/**
 * @param number_LuaParam_id El identificador del canal.
 * @return 1 si el canal identificado por @b id se encuentra protegido mediante control parental, 0 en caso contrario.
 */
luatype isProtected( luatype id );

/**
 * @param number_LuaParam_id El identificador del canal.
 * @return 1 si el canal identificado por @b id se encuentra bloqueado, 0 en caso contrario.
 */
luatype isBlocked( luatype id );

/**
 * Bloquea/desbloquea el canala identificado por @b id.
 * @param number_LuaParam_id El identificador del canal.
 */
luatype toggleBlocked( luatype id );

/**
 * Desbloquea todos los canales.
 */
luatype unblockAll();

/**
 * Añade/eliminal al canal identificado por @b id de la lista de favoritos.
 * @param number_LuaParam_id Identificador del canal.
 */
luatype toggleFavorite( luatype id );

/**
 * Cambia al canal siguiente del indicado en @b first, avanzando @b factor canales.
 * @param number_LuaParam_first Indica el canal a partir del cual cambiar.
 * @param number_LuaParam_factor Indica la cantidad de canales a avanzar.
 * @return El identificador del canal actual.
 */
luatype next( luatype first, luatype factor );

/**
 * Cambia al canal siguiente del indicado en @b first de la lista de favoritos, avanzando @b factor canales.
 * @param number_LuaParam_first Indica el canal a partir del cual cambiar.
 * @param number_LuaParam_factor Indica la cantidad de canales a avanzar.
 * @return El identificador del canal actual.
 */
luatype nextFavorite( luatype first, luatype factor );

}
