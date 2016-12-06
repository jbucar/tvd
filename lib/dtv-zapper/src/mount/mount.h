/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include <boost/signals2.hpp>
#include <set>

namespace zapper {
namespace mount {

/**
 * Clase que tiene como objetivo detectar todos los archivos que hay en determinadas rutas.
 */
class Mount {
public:
	Mount();
	virtual ~Mount();

	//	Start methods
	bool initialize();
	void finalize();

	//	Types
	typedef boost::signals2::signal<void (const std::string &, bool)> MountSignal;

	//	Methods
	MountSignal &onMount();
	const std::vector<std::string> &mount() const;
	void registerExtension( const std::string &ext );
	void registerExtensions( const std::set<std::string> &exts );
	void getFiles( const std::string &ext, std::vector<std::string> &files );
	void maxDepth( int max );
	int maxDepth() const;

	void addPath( const std::string &mountPoint, bool isMounted );
	void reScan();

protected:
	virtual bool init();
	virtual void fin();

	void scanFiles( const std::string &path, int maxDepth );

private:
	int _maxDepth;
	std::vector<std::string> _mount;
	std::vector<std::string> _files;
	std::vector<std::string> _extensions;
	MountSignal _onMount;

	void reset();
};

}
}

