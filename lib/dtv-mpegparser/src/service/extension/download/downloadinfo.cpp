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
#include "downloadinfo.h"
#include "../../../demuxer/ts.h"

namespace tuner {

DownloadInfo::DownloadInfo( void )
{
	_srvID        = 0;
	_pid          = TS_PID_NULL;
	_isCompulsory = false;
	_downloadID   = 0;
	_tag          = 0;
}

DownloadInfo::DownloadInfo( tuner::ID srvID, util::BYTE tag, util::DWORD downloadID, bool isCompulsory )
{
	_srvID        = srvID;
	_pid          = TS_PID_NULL;	
	_isCompulsory = isCompulsory;
	_downloadID   = downloadID;
	_tag          = tag;
}

DownloadInfo::~DownloadInfo( void )
{
}

bool DownloadInfo::compulsory() const {
	return _isCompulsory;
}

tuner::ID DownloadInfo::srvID() const {
	return _srvID;
}

util::BYTE DownloadInfo::tag() const {
	return _tag;
}

util::DWORD DownloadInfo::downloadID() const {
	return _downloadID;
}

bool DownloadInfo::started() const {
	return 	_pid != TS_PID_NULL;
}

void DownloadInfo::onComplete( const std::vector<std::string> &files ) {
	_files = files;
}

bool DownloadInfo::isComplete() const {
	return !_files.empty();
}

const std::vector<std::string> &DownloadInfo::files() const {
	return _files;
}

ID DownloadInfo::pid() const {
	return _pid;
}

void DownloadInfo::start( ID pid ) {
	_pid = pid;
}

}
