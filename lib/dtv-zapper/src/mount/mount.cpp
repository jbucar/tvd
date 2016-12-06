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

#include "mount.h"
#include <util/log.h>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <string.h>
#if defined(_WIN32) && ! defined(__MINGW32__)
#define strcasecmp _strcmpi
#endif

namespace zapper {
namespace mount {

namespace fs = boost::filesystem;

/**
 * Construye un objeto de la clase @c Mount.
 */
Mount::Mount()
{
	_maxDepth = 5;
}

/**
 * Destruye la instancia de la clase @c Mount.
 */
Mount::~Mount()
{
}

bool Mount::initialize() {
	reset();
	return init();
}

void Mount::finalize() {
	fin();
	reset();
}

bool Mount::init() {
	return true;
}

void Mount::fin() {
}

/**
 * @return Señal que se emite cuándo un punto de montaje es añadido o removido.
 */
Mount::MountSignal &Mount::onMount() {
	return _onMount;
}

/**
 * @return Colección con todos las rutas montadas.
 */
const std::vector<std::string> &Mount::mount() const {
	return _mount;
}

void Mount::maxDepth( int max ) {
	_maxDepth = max;
	reScan();
}

int Mount::maxDepth() const {
	return _maxDepth;
}

/**
 * Registra una extensión. Sólo los archivos de extensiones registradas son detectados cuándo se escanean archivos.
 */
void Mount::registerExtension( const std::string &ext ) {
	_extensions.push_back( ext );
	reScan();
}

void Mount::registerExtensions( const std::set<std::string> &exts ) {
	_extensions.insert( _extensions.end(), exts.begin(), exts.end() );
	reScan();
}

void Mount::reScan() {
	_files.clear();
	BOOST_FOREACH( const std::string &dir, _mount ) {
		scanFiles( dir, _maxDepth );
	}
}

/**
 * Retorna en el vector las rutas de los archivos cargados cuya extensión coincida con @em ext.
 * @param ext String con la extensión a buscar.
 * @param[out] files Variable donde es retornada la lista de archivos.
 */
void Mount::getFiles( const std::string &ext, std::vector<std::string> &files ) {
	BOOST_FOREACH( const std::string &file, _files ) {
		fs::path p(file);
		if (strcasecmp(p.extension().string().c_str(), ext.c_str()) == 0) {
			files.push_back( file );
		}
	}
}

void Mount::reset() {
	_mount.clear();
	_files.clear();
}

/**
 * Añade o remueve una ruta a la colección de rutas del objeto @c Mount.
 * @param mountPoint Ruta a agregar o remover.
 * @param isMounted Si es true y la ruta no ha sido agregada se escaneará, si es false y la ruta ya ha sido añadida
 * entonces se removerá de la colección de rutas.
 * @note Si se añade o se remueve una ruta se escaneará nuevamente.
 */
void Mount::addPath( const std::string &mountPoint, bool isMounted ) {
	bool changed=false;

	LINFO( "Mount", "Add dir: state=%d, path=%s", isMounted, mountPoint.c_str() );

	std::vector<std::string>::iterator it=std::find(
		_mount.begin(), _mount.end(), mountPoint );
	if (it == _mount.end()) {
		//	MountPoint not exist, must add?
		if (isMounted) {
			_mount.push_back( mountPoint );
			changed=true;
		}
	}
	else {
		//	MountPoint exist!!!, must remove?
		if (!isMounted) {
			_mount.erase( it );
			changed=true;
		}
	}

	//	If mount points changed?
	if (changed) {
		//	Scan files on all mount points. (Optimize!)
		reScan();

		_onMount( mountPoint, isMounted );
	}
}

struct CompareExtension {
	const std::string &_s2;
	CompareExtension(const std::string &s2):_s2(s2){}
	bool operator()(const std::string &s1) const {
		return strcasecmp(s1.c_str(), _s2.c_str()) == 0;
	}

private:
	CompareExtension &operator=(CompareExtension & /*ce*/ ) { return *this; }
};

void Mount::scanFiles( const std::string &path, int maxDepth ) {
	if (fs::exists( path )) {
		fs::directory_iterator end_itr;
		for (fs::directory_iterator itr( path ); itr != end_itr; ++itr ) {
			if (fs::is_directory( itr->status() ) && 0 < maxDepth) {
				scanFiles( itr->path().string(), maxDepth - 1 );
			}
			else if (fs::is_regular_file( itr->status() )) {
				fs::path file = itr->path();
				std::vector<std::string>::const_iterator it=std::find_if(
					_extensions.begin(),
					_extensions.end(),
					CompareExtension(file.extension().string()) );
				if (it != _extensions.end()) {
					LDEBUG( "Mount", "Add file: %s", file.string().c_str() );
					_files.push_back( file.string() );
				}
			}
		}
	}
}

}
}

