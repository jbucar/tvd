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

#include "../../types.h"
#include <string>
#include <vector>

namespace tuner {

class DownloadInfo {
public:
	DownloadInfo( void );
	DownloadInfo( tuner::ID srvID, util::BYTE tag, util::DWORD downloadID, bool isCompulsory );
	virtual ~DownloadInfo( void );

	//	Getters
	bool compulsory() const;
	tuner::ID srvID() const;
	util::BYTE tag() const;
	util::DWORD downloadID() const;
	bool started() const;
	const std::vector<std::string> &files() const;
	ID pid() const;

	void start( ID pid );
	void onComplete( const std::vector<std::string> &files );
	bool isComplete() const;

private:
	tuner::ID   _srvID;
	util::BYTE  _tag;
	util::DWORD _downloadID;
	bool        _isCompulsory;
	ID          _pid;
	std::vector<std::string> _files;
};

}
