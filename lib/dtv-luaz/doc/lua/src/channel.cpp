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
namespace channel {

/**
 * Indica si el canal actual está bloqueado.
 * @return True si el canal actual está bloqueado, False en caso contrario.
 */
luatype blocked();

/**
 * @return El número del canal actual.
 */
luatype current();

/**
 * Cambia el canal al indicado en @b channelNumber.
 * @param number_LuaParam_channelNumber El número del canal al que se quiere cambiar.
 */
luatype change( luatype channelNumber );

/**
 * Pone el video del canal actual en pantalla completa (Relativo a la ventana principal).
 */
luatype setFullScreen();

/**
 * Cambia el tamaño/ubicación del video del canal actual.
 * @param number_LuaParam_x Posición horizontal de la esquina superior izquierda.
 * @param number_LuaParam_y Posición vertical de la esquina superior izquierda.
 * @param number_LuaParam_w Ancho.
 * @param number_LuaParam_h Alto.
 */
luatype resize( luatype x, luatype y, luatype w, luatype h );

/**
 * Habilita/deshabilita el audio del canal actual.
 * @param number_LuaParam_toggle Indica si se debe habilitar o deshabilitar el audio del canal actual (1=habilitar, 0=deshabilitar).
 */
luatype mute( luatype toggle );

/**
 * Cambia el volumen del audio del canal actual.
 * @param number_LuaParam_vol El nivel de volumen a setear.
 */
luatype volume( luatype vol );

/**
 * Cambia al siguiente stream de video.
 * @return El id del stream de video que se está reproduciendo actualmente.
 */
luatype nextVideo();

/**
 * Cambia al siguiente stream de audio.
 * @return El id del stream de audio que se está reproduciendo actualmente.
 */
luatype nextAudio();

/**
 * Cambia al siguiente subtítulo.
 * @return El id del subtítulo usado actualmente.
 */
luatype nextSubtitle();

/**
 * Indica si el video actual tiene subtítulos.
 * @return El string "CC" si el video actual tiene subtítulos, un string vacío en caso contrario.
 */
luatype haveSubtitle() ;

/**
 * Indica la resolución del reproductor.
 * @return Un string indicando la resolución del reproductor.
 */
luatype getResolution();

/**
 * Indica la relación de aspecto del reproductor.
 * @return Un string indicando la relación de aspecto del reproductor.
 */
luatype getAspect();

/**
 * Indica el tipo de audio del reproductor.
 * @return Un string indicando el tipo de audio del reproductor.
 */
luatype getAudio();

/**
 * Obtiene información de un stream de audio.
 * @param number_LuaParam_id El id del stream de audio.
 * @return Un string con la información del stream de audio indicado por @b id.
 */
luatype getAudioInfo( luatype id );

}
