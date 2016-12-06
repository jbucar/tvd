/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include <string>
#include <boost/function.hpp>
namespace canvas {

class Point;
class Size;
class System;
class Surface;

/**
 * Representa una superficie utilizada para renderizar contenido html.
 */
class WebViewer {
public:
	explicit WebViewer( Surface *surface );
	virtual ~WebViewer( void );

	/**
	 * Carga la pagina html indicada por @b uri.
	 * @param uri El nombre de dominio de la pagina html a cargar.
	 * @return true si pudo cargar la pagina correctamente, false en caso contrario.
	 */
	virtual bool load( const std::string &uri )=0;

	/**
	 * Detiene la carga en curso.
	 */
	virtual void stop()=0;

	/**
	 * Renderiza la página cargada previamente con load().
	 */
	virtual void draw()=0;

	//	Widget
	virtual void resize( const Size &size );
	virtual void move( const Point &point );

	//	Events
	typedef boost::function<void (void)> Callback;
	void setCallbackLoadFinished( const Callback &callback );

	//	Instance creation
	static WebViewer *create( System *sys, Surface *s );

protected:
	Surface *surface();
	void runCallbackLoadFinished();

private:
	Surface *_surface;
	Callback _callbackFinished;
	WebViewer() {}
};

}

