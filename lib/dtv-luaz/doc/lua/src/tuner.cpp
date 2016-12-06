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
 * Provee funcionalidad relacionada al uso del sintonizador de canales.
 */
namespace tuner {

/**
 * Evento que indica el comienzo del escaneo de canales.
 * Se llamará a esta función de lua si la misma se encuentra definida.
 * @param number_LuaParam_count La cantidad de redes a escanear.
 */
luatype zapperBeginScan( luatype count );

/**
 * Evento que indica el comienzo de escaneo de canales en una red.
 * Se llamará a esta función de lua si la misma se encuentra definida.
 * @param string_LuaParam_net La red que se comenzará a escanear.
 */
luatype zapperScanNetwork( luatype net );

/**
 * Evento que indica la finalización del escaneo de canales.
 * Se llamará a esta función de lua si la misma se encuentra definida.
 */
luatype zapperEndScan();

/**
 * Lanza el escaneo de canales.
 */
luatype startScan();

/**
 * Detiene el escaneo de canales.
 */
luatype stopScan();

/**
 * @return La intensidad de la señal del sintonizador de canales.
 */
luatype getSignal();

}
