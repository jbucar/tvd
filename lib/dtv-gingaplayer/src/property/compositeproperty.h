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

#include "property.h"
#include <vector>

namespace player {

class CompositeProperty : public Property {
public:
	CompositeProperty();
	virtual ~CompositeProperty();

	virtual bool assign( const bool &value ) { return loopAssign( value ); }
	virtual bool assign( const int &value )  { return loopAssign( value ); }
	virtual bool assign( const float &value )  { return loopAssign( value ); }
	virtual bool assign( const std::string &value ) { return loopAssign( value ); }
	virtual bool assign( const canvas::Size &value )  { return loopAssign( value ); }
	virtual bool assign( const canvas::Point &value ) { return loopAssign( value ); }
	virtual bool assign( const canvas::Rect &value ) { return loopAssign( value ); }	
	virtual bool assign( const std::pair<std::string,std::string> &value ) { return loopAssign( value ); }
	virtual bool canStart() const;

	void add( Property *prop );

protected:
	virtual bool needRefresh() const;
	virtual void applyChanges();
	
	template<typename T>
	bool loopAssign( const T &value ) {
		std::vector<Property *>::const_iterator it=_properties.begin();
		while (it != _properties.end()) {
			Property *prop = (*it);
			if (!prop->assign( value )) {
				return false;
			}
			if (prop->changed()) {
				markModified();
			}
			it++;
		}
		return true;
	}

private:
	std::vector<Property *> _properties;
};

}

