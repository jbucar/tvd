/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include <list>
#include <string>

namespace util {
namespace reg {

class Registrator;

namespace impl{
	typedef void (*addCallback)( Registrator *);
}

void init();
void fin();
void addfinCallbacks(Registrator * aReg);
void addinitCallbacks(Registrator * aReg);

class Registrator {
public:
	Registrator( const std::string &name, impl::addCallback aCall );
	virtual ~Registrator();

	virtual void operator()();
	virtual int priority() const;
	const std::string &name() const;

private:
	std::string _name;
};

#define BUILD_BASENAME( type, name, class, target ) class##type##name##target
#define BUILD_CLASSNAME( type, name, class ) BUILD_BASENAME( type, name, class, ClassImpl )
#define BUILD_INSTANCE( type, name, class ) BUILD_BASENAME( type, name, class, Instance )

#define BUILD_REGISTER( type, name ) \
class BUILD_CLASSNAME( type, name, Registrator ) : public util::reg::Registrator { \
public: \
	BUILD_CLASSNAME( type, name, Registrator )() : util::reg::Registrator( #name, util::reg::add##type##Callbacks ) {} \
	virtual void operator()(); \
}; \
static BUILD_CLASSNAME( type, name, Registrator ) BUILD_INSTANCE( type, name, Registrator ); \
void BUILD_CLASSNAME( type, name, Registrator )::operator()()

#define REGISTER_INIT( name ) BUILD_REGISTER( init, name )
#define REGISTER_FIN( name ) BUILD_REGISTER( fin, name )

}
}
