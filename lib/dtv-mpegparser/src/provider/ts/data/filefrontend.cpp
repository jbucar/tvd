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
#include "filefrontend.h"
#include "../../../demuxer/ts/demuxerimpl.h"
#include "generated/config.h"
#include <util/buffer.h>
#include <util/cfg/configregistrator.h>
#include <util/fs.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <stdio.h>

REGISTER_INIT_CONFIG( tuner_provider_tsdata_file ) {
	boost::filesystem::path path( util::fs::home() );
	path /= "ts";

	root().addNode("file")
		.addValue("dir", "TS file provider dir", path.string() )
		.addValue("bitrate", "Bit rate", 29958294UL);
}

namespace tuner {
namespace ts {

namespace fs = boost::filesystem;

FileFrontend::FileFrontend( demuxer::ts::DemuxerImpl *demux )
	: Frontend( demux )
{
	const std::string &dir = util::cfg::getValue<std::string>("tuner.provider.tsdata.file.dir");
	scanFiles( dir, _files );
	LDEBUG( "ts::FileFrontend", "Scan files: %d, dir=%s", _files.size(), dir.c_str() );

	_exit = false;
}

FileFrontend::~FileFrontend()
{
}

//	Set current network
size_t FileFrontend::getCount() const {
	return _files.size();
}

std::string FileFrontend::getName( size_t nIndex ) const {
	util::log::flush();
	DTV_ASSERT(nIndex<_files.size());
	return _files[nIndex];
}

size_t FileFrontend::find( const std::string &name ) const {
	std::vector<std::string>::const_iterator it = _files.begin();
	size_t i = 0;
	for ( ; it != _files.end(); it++, i++ ) {
		if ((*it)==name) {
			break;
		}
	}
	return i;
}

std::string FileFrontend::startImpl( size_t nIndex ) {
	DTV_ASSERT(nIndex<_files.size());
	_exit = false;
	return _files[nIndex];
}

void FileFrontend::stopImpl() {
	_exit = true;
}

//	Running in provider thread
void FileFrontend::reader( const std::string net ) {
	util::Buffer *buf=NULL;

	LDEBUG( "ts::FileFrontend", "file thread started: net=%s", net.c_str() );

	FILE *file = fopen( net.c_str(), "rb" );
	if (!file) {
		LDEBUG( "ts::FileFrontend", "Warning, cannot open network: net=%s, errno=%d", net.c_str(), errno );
		return;
	}

	while (!_exit) {
		boost::posix_time::ptime time(boost::posix_time::microsec_clock::local_time());

		//  Get a buffer
		if (!buf) {
			buf = demux()->allocBuffer();
			DTV_ASSERT(buf);
		}

		//  Read from network
		int bytes = ::fread( buf->data(), 1, buf->capacity(), file );
		if (bytes <= 0) {
			::fseek( file, 0, SEEK_SET );
			continue;
		}
		else if (bytes) {
			//	Enqueue buffer into demuxer
			buf->resize( bytes );
			demux()->pushData( buf );
			buf = NULL;

			waitForBitrate(time, bytes);
		}
		else {
			break;
		}
	}

	if (buf) {
		demux()->freeBuffer( buf );
	}
	fclose( file );

	LDEBUG( "ts::FileFrontend", "file thread terminated: net=%s", net.c_str() );
}

void FileFrontend::scanFiles( const std::string &root, std::vector<std::string> &files ) {
	if (fs::exists( root )) {
		fs::directory_iterator end_itr;
		for (fs::directory_iterator itr( root ); itr != end_itr; ++itr) {
			if (fs::is_directory( itr->status() )) {
				scanFiles( itr->path().string(), files );
			}
			else if (fs::is_regular_file( itr->status() )) {
				fs::path file = itr->path();
				if (file.extension() == ".ts") {
					files.push_back( file.string() );
				}
			}
		}
	}
}

void FileFrontend::waitForBitrate(boost::posix_time::ptime &time, unsigned bytes) {
	boost::posix_time::ptime endtime(boost::posix_time::microsec_clock::local_time());
	boost::posix_time::time_duration td = endtime - time;
	unsigned long totalTime = (unsigned long) td.total_milliseconds();

	unsigned long bitrate = util::cfg::getValue<unsigned long>("tuner.provider.tsdata.file.bitrate");
	unsigned long millisecondsNeded = (bytes*1000)/(bitrate/8);
	if ( totalTime < millisecondsNeded ) {
		unsigned long ms = millisecondsNeded-totalTime;
		boost::this_thread::sleep(boost::posix_time::milliseconds(ms));
	}
}

}
}
