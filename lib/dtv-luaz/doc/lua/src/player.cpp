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
 * Provee la funcionalidad relacionada con el reproductor de audio.
 */
namespace player {

/**
 * Evento que indica la finalización de la reproducción de un media.
 * Se llamará a esta función de lua si la misma se encuentra definida.
 */
luatype mediaFileEnd();

/**
 * Habilita/deshabilita el volumen del reproductor.
 * @return 0 si el volumen del reproductor se encuentra habilitado, 1 en caso contrario.
 */
luatype toggleMute();

/**
 * @return 0 si el volumen del reproductor se encuentra habilitado, 1 en caso contrario.
 */
luatype isMuted();

/**
 * Cambia el nivel de volumen del reproductor.
 * @param number_LuaParam_vol El nivel de volumen a setear.
 */
luatype setVolume( luatype vol );

/**
 * Incrementa el volumen del reproductor.
 */
luatype volumeUp();

/**
 * Decrementa el volumen del reproductor.
 */
luatype volumeDown();

/**
 * @return El máximo nivel de volumen del reproductor.
 */
luatype maxVolume();

/**
 * @return El nivel de volumen actual del reproductor.
 */
luatype getVolume();

/**
 * @return [1] Tabla con los nombres de los canales de audio soportados por el reproductor.
 * @return [2] El número del canal de audio en uso actualmente.
 */
luatype getAudioChannels();

/**
 * @return [1] El número del canal de audio en uso actualmente.
 * @return [2] El nombre del canal de audio en uso actualmente.
 */
luatype getAudioChannel();

/**
 * Cambia el canal de audio al indicado en @b ch.
 * @param number_LuaParam_ch El número del canal de audio a setear.
 */
luatype setAudioChannel( ch );

/**
 * @return Una tabla con las imagenes (jpg y png) encontradas en el dispositivo usb.
 * De cada imagen se retorna el id, el nombre del archivo, y la ruta completa a la misma.
 */
luatype getImageFiles();

/**
 * @return Una tabla con los archivos de audio (mp3) encontrados en el dispositivo usb.
 * De cada archivo se retorna el id y el nombre del mismo.
 */
luatype getAudioFiles();

/**
 * Reproduce el archivo de audio indicado por @b id.
 * @param number_LuaParam_id El id del archivo de audio a reproducir.
 */
luatype playAudioFile( luatype id );

/**
 * @return Una tabla con los archivos de video encontrados en el dispositivo usb.
 * De cada archivo se retorna el id y el nombre del mismo.
 */
luatype getVideoFiles();

/**
 * Reproduce el archivo de video indicado por @b id.
 * @param number_LuaParam_id El id del archivo de video a reproducir.
 */
luatype playVideoFile( luatype id );

/**
 * Pausa/continua la reproducción del archivo multimedia actual.
 * @param number_LuaParam_pause Indica si se debe pausar o continuar la reproducción del archivo multimedia actual (1=pausar, 0=continuar).
 */
luatype pauseFile( luatype pause );

/**
 * Frena la reproducción del archivo multimedia actual.
 */
luatype stopFile();

}
