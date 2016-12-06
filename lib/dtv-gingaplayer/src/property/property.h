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

#include <canvas/point.h>
#include <canvas/size.h>
#include <canvas/rect.h>
#include <util/types.h>
#include <util/keydefs.h>
#include <string>

namespace player {

/**
 * Cada contenido reproducible por un @c Player tiene una serie de propiedades, como bien pueden ser tamaño para 
 * contenidos gráficos o volumen en caso de contenidos audibles, éstas propiedades son implementadas en los Players por medio 
 * de la clase abstracta @c Property y su subclase @c PropertyImpl que es una clase template que representa el tipo 
 * de la propiedad.
 */
class Property {
public:
	Property( void );
	virtual ~Property();

	virtual bool assign( const bool &value );
	virtual bool assign( const int &value );
	virtual bool assign( const float &value );
	virtual bool assign( const util::DWORD &value );
	virtual bool assign( const std::string &value );
	bool assign( const char *value );		
	virtual bool assign( const canvas::Size &value );
	virtual bool assign( const canvas::Point &value );
	virtual bool assign( const canvas::Rect &value );
	virtual bool assign( const std::pair<std::string,std::string> &value );	
	virtual bool assign( const std::pair<util::key::type,bool> &value );

	virtual bool canStart() const;
	virtual bool needRefresh() const;
	virtual void markModified();
	
	/**
	 * Una propiedad sólo dinámica no puede ser asignada si el player no ha sido iniciado previamente.
	 * @return True si la propiedad es sólo dinámica, false caso contrario.
	 */
	virtual bool isOnlyDynamic(){
		return false;
	}
	bool apply();
	bool changed() const;

protected:
	virtual void applyChanges();

private:
	bool _changed;
};

}

