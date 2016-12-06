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

#include "networkprovider.h"
#include "../fetcher/fetcher.h"
#include "../zapper.h"
#include <pvr/channel.h>
#include <pvr/dvb/tuner.h>
#include <util/serializer/xml.h>
#include <util/fs.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/filesystem.hpp>

namespace zapper {

bool unzipFile( const std::string &parent, const std::string &recvZipFile );

namespace logos {
namespace network {

namespace fs = boost::filesystem;

class LogoInfo {
public:
	template<class Marshall>
		void serialize( Marshall &m ) {
		m.serialize( "name", name );
		m.serialize( "frequency", freq );
		m.serialize( "nitID", nitID );
		m.serialize( "tsID", tsID );
		m.serialize( "srvID", srvID );
		m.serialize( "channelID", channelID );
		m.serialize( "logo", logo );
	}

	std::string name;
	std::string freq;
	int nitID;
	int tsID;
	int srvID;
	std::string channelID;
	std::string logo;
};

struct FindLogo {
public:
	FindLogo( pvr::Channel *ch ) : _ch(ch) {}

	bool operator()( const LogoInfo *info ) {
		{	//	Find by dvb IDS
			pvr::dvb::id::ChannelID id;
			if (pvr::dvb::id::fromString( _ch->uri(), id ) &&
				(info->nitID == id.nitID && info->tsID == id.tsID && info->srvID == id.srvID))
			{
				return true;
			}
		}

		//	Find by name
		if (_ch->name().find( info->name ) != std::string::npos) {
			return true;
		}

		//	Find by #channel
		return _ch->channel() == info->channelID;
	}

	pvr::Channel *_ch;
};

class Provider::XmlInfo {
public:
	XmlInfo() {
		vendorID = 0;
		productID = 0;
		version = 0;
	}

	~XmlInfo() {
		reset();
	}

	void reset() {
		vendorID = 0;
		productID = 0;
		version = 0;
		CLEAN_ALL( LogoInfo *, logos );
	}

	template<class Marshall>
		void serialize( Marshall &m ) {
		m.serialize( "vendorID", vendorID );
		m.serialize( "productID", productID );
		m.serialize( "version", version );
		m.serialize( "channels", logos );
	}

	int vendorID;
	int productID;
	int version;
	std::vector<LogoInfo *> logos;
};

Provider::Provider( Zapper *zapper )
	: pvr::logos::Provider( "logos::Network", "1.0" ), _zapper( zapper )
{
	_update = NULL;
	_info = new Provider::XmlInfo();
}

Provider::~Provider()
{
	DEL(_info);
}

bool Provider::doStart() {
	LDEBUG( "Logos", "Start" );

	//	Parse logo info, if exists
	parse();

	//	Check for new logos
	_update = new fetcher::Fetcher( _zapper );
	_update->onUpdateReceived( boost::bind(&Provider::onUpdateReceived,this,_1) );
	if (!_update->check( _info->vendorID, _info->productID, _info->version )) {
		LWARN( "Logos", "Cannot check for new logos" );
		DEL(_update);
	}

	return true;
}

void Provider::doStop() {
	LDEBUG( "Logos", "Stop" );

	if (_update) {
		_update->cancel();
		DEL(_update);
	}

	_info->reset();
}

util::Settings *Provider::settings() const {
	return _zapper->settings();
}

bool Provider::find( pvr::Channel *ch, std::string &logoFile, int &priority ) {
	if (_info->version > 0) {
		priority = 10;

		std::vector<LogoInfo *>::const_iterator it=std::find_if(
			_info->logos.begin(),
			_info->logos.end(),
			FindLogo(ch)
		);
		if (it != _info->logos.end()) {
			logoFile = util::fs::make( _zapper->dbDir(), "logos", (*it)->logo );
			return true;
		}
	}
	return false;
}

const std::string Provider::cfgDir() const {
	return util::fs::make( _zapper->dbDir(), "logos" );
}

const std::string Provider::cfgFile() const {
	return util::fs::make( cfgDir(), "logos.xml" );
}

void Provider::onUpdateReceived( fetcher::Info *info ) {
	//	Setup temporary directory
	fs::path tmpDir( _zapper->dbDir() );
	tmpDir /= "logos_update_%%%%%%%%";
	tmpDir = fs::unique_path(tmpDir);

	//	Create directory
	if (!fs::create_directory( tmpDir )) {
		LWARN( "Logos", "Could not create temporary directory used for unzip file" );
		return;
	}

	//	unzip file
	if (!unzipFile( tmpDir.string(), info->fileName )) {
		return;
	}

	//	Old logos dir
	std::string logosDir = cfgDir();

	//	remove old logos directory
	if (!util::safeRemoveDirectory( logosDir )) {
		LWARN( "Logos", "Could not remove old logos directory" );
		return;
	}

	//	move to db directory
	fs::path newDir(tmpDir);
	newDir /= "logos";
	try {
		fs::rename( newDir, logosDir );
	} catch (...) {
		LWARN( "Logos", "Could not rename downloaded logos to db directory" );
		return;
	}

	//	cleanup info
	_info->reset();

	//	Parse logo info, if exists
	parse();

	//	Remove temporary directory
	if (!util::safeRemoveDirectory( tmpDir.string() )) {
		LWARN( "Logos", "Could not remove temporary directories" );
	}

	//	Remove downloaded file
	try {
		fs::remove( info->fileName );
	} catch (...) {
		LWARN( "Logos", "Could not remove downloaded file" );
	}
}

bool Provider::parse() {
	std::string cfg = cfgFile();
	if (fs::exists( cfg )) {
		util::serialize::XML ser( cfg );
		if (ser.load( *_info )) {
			LDEBUG( "Logos", "VendorID=%d, ProductID=%d, version=%d, logos=%d",
				_info->vendorID, _info->productID, _info->version, _info->logos.size() );
			return true;
		} else {
			LWARN( "Logos", "Fail to load logos from %s", cfg.c_str());
		}
	}
	return false;
}

}
}
}

