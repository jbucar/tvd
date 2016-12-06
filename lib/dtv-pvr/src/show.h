/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-pvr implementation.

    DTV-pvr is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-pvr is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-pvr.

    DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "types.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <string>

namespace pvr {

namespace bpt=boost::posix_time;

namespace parental {
	class Control;
}

/**
 * Representa un programa de un canal.
 */
class Show {
public:
	Show();
	Show( const std::string &name, const std::string &desc, const bpt::time_period &range );
	virtual ~Show();

	//	Getters
	const std::string &name() const;
	const std::string &description() const;
	const bpt::time_period &timeRange() const;

	//	Category
	category::Type category() const;
	void category( category::Type cat );

	//	Parental
	parental::Control *parentalControl() const;
	void parentalControl( parental::Control *parental );

	//	Methods
	int percentage( const bpt::ptime &time ) const;
	int percentage() const;

protected:
	void name( const std::string &name );
	void description( const std::string &desc );
	void timeRange( const bpt::time_period &range );

private:
	std::string _name;
	std::string _description;
	bpt::time_period _timeRange;
	category::Type _category;
	parental::Control *_parental;
};

}

