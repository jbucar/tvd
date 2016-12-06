/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#pragma once

#include "../../../../types.h"

namespace tuner {

class ResourceManager;

namespace dsmcc {

class ModuleData {
public:
	ModuleData(	DWORD size ) : _size( size ) {}
	virtual ~ModuleData( void ) {};

	//	Getters
	virtual bool canStart( ResourceManager *resMgr )=0;

	//	Operations
	virtual bool checkCRC32( DWORD crc )=0;
	virtual bool saveAs( const std::string &name )=0;
	virtual size_t read( size_t off, BYTE *buf, size_t len )=0;
	virtual size_t sendFile( FILE *of, size_t o_off, size_t i_off, size_t len )=0;	
	virtual size_t write( size_t off, BYTE *buf, size_t len )=0;

protected:
	DWORD size() const { return _size; }
	
private:
	DWORD _size;
};

}
}

