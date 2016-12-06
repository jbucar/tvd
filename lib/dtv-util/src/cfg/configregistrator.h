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

#include "../registrator.h"
#include "cfg.h"
#include <list>

namespace util {
namespace cfg {

class ConfigRegistrator : public reg::Registrator {
public:
	ConfigRegistrator( const std::string &name, reg::impl::addCallback aCall );
	virtual ~ConfigRegistrator();
	
protected:
	virtual PropertyTree &root();
};

#define BUILD_CONFIG_REGISTER( type, name ) \
class BUILD_CLASSNAME( type, name, ConfigRegistrator ) : public util::cfg::ConfigRegistrator { \
public: \
	BUILD_CLASSNAME( type, name, ConfigRegistrator )() : ConfigRegistrator( #name, util::reg::add##type##Callbacks ) {} \
	virtual void operator()(); \
}; \
static BUILD_CLASSNAME( type, name, ConfigRegistrator ) BUILD_INSTANCE( type, name, ConfigRegistrator ); \
void BUILD_CLASSNAME( type, name, ConfigRegistrator )::operator()()


#define REGISTER_INIT_CONFIG( name ) BUILD_CONFIG_REGISTER( init, name )
#define REGISTER_FIN_CONFIG( name ) BUILD_CONFIG_REGISTER( fin, name )

}
}

