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
 * Provee funcionalidad relacionada con el sistema y la ventana principal.
 */
namespace mainWindow {

/**
 * Ejecuta el loop principal de lua.
 */
luatype run();

/**
 * Finaliza el loop principal.
 * @param number_LuaParam_retCode Codigo de error, 0 indica que no hubo errores.
 */
luatype stop( luatype retCode );

/**
 * Indica si lua debe escuchar los eventos de teclado/control.
 * @param number_LuaParam_state Si es 1 los eventos de teclado llegaran a lua, si es 0 no.
 */
luatype lockKeys( luatype state );

/**
 * Resetea la configuración del sistema a sus valores por defecto.
 */
luatype resetConfig();

/**
 * Información sobre el tiempo actual (dias, horas) con el formato indicado en @b format.
 * @param string_LuaParam_format Indica el formato a usar (ej: '%Y-%m-%d %H:%M')
 * @return String formateado segun @b format de el tiempo actual.
 */
luatype getTime( luatype format );

/**
 * @param string_LuaParam_format Indica el formato a usar (ej: '%Y-%m-%d').
 * @param number_LuaParam_offset Indica los días a sumar a partir de hoy.
 * @return String formateado segun @b format de la fecha actual mas los días indicados por @b offset.
 */
luatype getDatePlus( luatype format, luatype offset );

/**
 * Cancela la ejecución de un timer.
 * @param number_LuaParam_id El id del timer a cancelar.
 */
luatype cancelTimer( luatype id );

/**
 * Registra un timer con la duración indicada por @b ms.
 * Una vez transcurridos @ms milisegundos se llama a la función de lua OnTimerEvent con el @b id del timer.
 * @param number_LuaParam_id El identificador del timer.
 * @param number_LuaParam_ms La duración del timer en milisegundos.
 */
luatype registerTimer( luatype id, luatype ms );

/**
 * Muestra/oculta el fondo de la ventana.
 * @param number_LuaParam_needShow Indica si se debe ocultar o mostrar el fondo de la ventana (1=mostrar, 0=ocultar).
 */
luatype showBackground( luatype needShow );

/**
 * Ejecuta un web browser si esta disponible.
 */
luatype launchBrowser();

/**
 * Cambia entre modo pantalla completa y ventana.
 * @param number_LuaParam_state Si es 0 cambia a modo ventana, cualquier otro valor pone la ventana en modo pantalla completa.
 */
luatype fullscreen( luatype state );

/**
 * Indica el modo en que se encuentra la ventana.
 * @return True si se esta en moodo pantalla completa, False en caso contrario.
 */
luatype isFullscreen();

/**
 * Minimiza/restaura la ventana.
 * @param number_LuaParam_state Si es 0 restaura la ventana, cualquier otro valor la minimiza a la bandeja del sistema.
 */
luatype iconify( luatype state );

}
