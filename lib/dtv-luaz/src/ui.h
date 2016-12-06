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

#pragma once

#include <string>
#include <vector>
#include <boost/function.hpp>
#include <boost/bind.hpp>


typedef struct lua_State lua_State;

namespace zapper {
	class Zapper;
}

namespace luaz {

namespace lua {
	template<typename T> void setGlobalT( lua_State *st, const std::string &name, T val );
}

class MainWindow;

/**
 * Permite ejecutar un script de lua
 */
class UI {
public:
	explicit UI( const std::string &name );
	virtual ~UI();

	int run( const std::string &script );

	/**
	 * Agrega una variable de entorno para que sea accesible desde lua.
	 * @param name El nombre de la variable de entorno a setear.
	 * @param value El valor asociado a la variable de entorno.
	 */
	template <typename T>
	void setEnvironmentVar( const std::string &name, T value ) {
		_environmentVars.push_back( boost::bind( &lua::setGlobalT<T>, _1, name, value )  );
	}

protected:
	//	Aux lua
	void runScript( const std::string &script );

	//	Initialize
	int initialize();
	int finalize();
	void loadEnvironmentVars();

private:
	std::string _name;
	zapper::Zapper *_zapper;
	MainWindow *_mainWindow;
	typedef boost::function<void ( lua_State *st )> Setter;
	std::vector<Setter> _environmentVars;
	lua_State *_lState;
};

}

