/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include <canvas/size.h>
#include <canvas/point.h>
#include <canvas/rect.h>

namespace canvas {
	class Canvas;
}

namespace player {
namespace check {

template<typename T>
	inline bool always( const T & ) {
	return true;
}

template<typename T>
	inline bool never( const T & ) {
	return false;
}

template<typename T>
	inline bool range( const T &value, const T &min, const T &max ) {
	return value >= min && value <= max;
}

bool color( const std::string &color );
bool position( canvas::Canvas *canvas, const canvas::Point &point );
bool size( canvas::Canvas *canvas, const canvas::Size &size );
bool bounds( canvas::Canvas *canvas, const canvas::Rect &bounds );
bool fileExists( const std::string &file );
inline bool notEmpty( const std::string &str ) {
	return str.empty() ? false : true;
}

}
}

