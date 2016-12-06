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
 * Provee funcionalidad relacionada con la pantalla.
 */
namespace display {

/**
 * @return[1] El índice del connector que está activo.
 * @return[2] Un string que describe al connector que está activo.
 */
luatype getConnector();

/**
 * @return[1] Una tabla con todos los conectores disponibles.
 * @return[2] El índice del conector activo.
 */
luatype getConnectors();

/**
 * @param number_LuaParam_idx El índice del conector.
 * @return[1] El índice del modo de video que esta activo para el conector indicado por @b idx.
 * @return[2] Un string que describe al modo de video que esta activo para el conector indicado por @b idx.
 */
luatype getVideoMode( luatype idx );

/**
 * @param number_LuaParam_idx El índice del conector.
 * @return[1] Una tabla con todos los modos de video disponibles para el conector indicado por @b idx.
 * @return[2] El índice del modo de video activo para el conector indicado por @b idx.
 */
luatype getVideoModes( luatype idx );

/**
 * @return[1] El índice del modo de video que esta activo para el conector actual.
 * @return[2] Un string que describe al modo de video que esta activo para el conector actual.
 */
luatype getCurVideoMode();

/**
 * @return[1] Una tabla con todos los modos de video disponibles para el conector actual.
 * @return[2] El índice del modo de video activo para el conector actual.
 */
luatype getCurVideoModes();

/**
 * Setea el modo de video indicado por @b modIdx para el conector indicado por @b conIdx.
 * @param number_LuaParam_conIdx El índice del conector.
 * @param number_LuaParam_modIdx El índice del modo de video.
 */
luatype setVideoMode( luatype conIdx, luatype modIdx );

/**
 * Cambia el nivel de transparencia del menu.
 * @param number_LuaParam_value El nivel de transparencia a setear.
 */
luatype setTransparency( luatype value );

/**
 * @return El nivel de transparencia actual del menu.
 */
luatype getTransparency();

/**
 * @return[1] El índice de la relación de aspecto que esta activa.
 * @return[2] Un string que describe la relación de aspecto que está activa.
 */
luatype getAspectMode();

/**
 * @return[1] Una tabla con todas las relaciónes de aspecto disponibles.
 * @return[2] El índice de la relación de aspecto activa.
 */
luatype getAspectModes();

/**
 * Cambia la relación de aspecto actual.
 * @param number_LuaParam_idx El índice de la relación de aspecto a activar.
 */
luatype setAspectMode( luatype idx );

/**
 * @return[1] El índice del modulador de video activo.
 * @return[2] Un string que describe el modulador de video activo.
 */
luatype getModulator();

/**
 * @return[1] Una tabla con todos los moduladores de video disponibles.
 * @return[2] El índice del modulador de video activo.
 */
luatype getModulators();

/**
 * Activa el modulador de video indicado en @b idx.
 * @param number_LuaParam_idx El índice del modulador de video a activar.
 */
luatype setModulator( luatype idx );

}
