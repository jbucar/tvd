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

#include "fetcher.h"
#include "ssl-cert-pem.h"
#include "../zapper.h"
#include <util/xml/nodehandler.h>
#include <util/xml/documenthandler.h>
#include <util/netlib/requestinfo.h>
#include <util/netlib/manager.h>
#include <util/cfg/configregistrator.h>
#include <util/cfg/cfg.h>
#include <util/functions.h>
#include <util/buffer.h>
#include <util/log.h>
#include <util/assert.h>
#include <util/mcr.h>
#include <boost/math/special_functions.hpp>
#include <boost/filesystem.hpp>
#include <stdio.h>
#include <zlib.h>

namespace zapper {
namespace fetcher {

REGISTER_INIT_CONFIG( zapper_fetcher ) {
	root().addNode( "updater" )
		.addValue( "url", "URL update", "https://tvd-git.dmz.tecnologia.lifia.info.unlp.edu.ar/fwupdater/available/" )
		.addValue( "verbose", "Do request verbose", false );
}

namespace fs = boost::filesystem;

Fetcher::Fetcher( Zapper *zapper )
	: _zapper(zapper), _destDirectory(zapper->ramDisk()), _steps(10)
{
	_bufCheck = new util::Buffer(256);
	_info.totalSize = 0;
	_info.headers = new util::Buffer(256);
}

Fetcher::~Fetcher()
{
	delete _bufCheck;
	delete _info.headers;
}

bool Fetcher::check( int makerID, int productID, int version ) {
	LINFO( "Update", "Checking for update: version=(%d,%d,%d)",
		makerID, productID, version );

	//	Setup URL
	std::string url = util::cfg::getValue<std::string>("zapper.updater.url");
	url += "/";
	url += boost::lexical_cast<std::string>(makerID);
	url += "/";
	url += boost::lexical_cast<std::string>(productID);
	url += "/";
	url += boost::lexical_cast<std::string>(version);
	url += "/";

	cancel();
	return requestCheckUpdate( url );
}

void Fetcher::cancel() {
	if (util::id::isValid(_reqID)) {
		netlib()->stop( _reqID );
	}
}

void Fetcher::steps( int steps ) {
	_steps = steps;
}

void Fetcher::destDirectory( const std::string &dir ) {
	_destDirectory = dir;
}

void Fetcher::onUpdateReceived( const OnUpdateReceived &callback ) {
	_onUpdateReceived = callback;
}

void Fetcher::onProgress( const OnProgress &callback ) {
	_onProgress = callback;
}

bool Fetcher::requestCheckUpdate( const std::string &url ) {
	//	Setup CA
	std::string pem = pemFile();
	if (!verifyPem( pem )) {
		LERROR("Update", "Cannot decompress CA certificate.");
		return false;
	}

	//	Setup request
	util::netlib::RequestInfo *request = new util::netlib::RequestInfo();
	request->url( url );
	request->verbose( util::cfg::getValue<bool>("zapper.updater.verbose") );
	request->verifyPeer( true );
	request->caCertificate( pem );
	request->onFinished( boost::bind(&Fetcher::checkUpdateFinished,this,_2) );

	//	Start request
	_reqID = netlib()->start( request, _bufCheck );
	return util::id::isValid(_reqID);
}

/*	Request info
	<root>
	    <makerId>208</makerId>
	    <firmware>https://tvd-git.dmz.tecnologia.lifia.info.unlp.edu.ar:4443/media/rootfs_5.tgz</firmware>
	    <version>558</version>
	    <has_update>True</has_update>
	    <modelId>2</modelId>
	    <md5>6059fc1d1af258bb7880cc4de5713087</md5>
	</root>

	<root>
		<makerId>208</makerId>
		<version>609</version>
		<has_update>False</has_update>
		<modelId>2</modelId>
	</root>
*/
void Fetcher::checkUpdateFinished( bool result ) {
	LDEBUG( "Update", "Check for update finished: result=%d", result );

	_reqID.reset();
	if (!result) {
		LWARN( "Update", "Request failed" );
		return;
	}

	//	Create DOM handler
	util::xml::dom::DocumentHandler *doc = util::xml::dom::DocumentHandler::create();
	DTV_ASSERT(doc);

	if (!doc->initialize()) {
		LWARN( "Update", "Cannot initialize DOM handler" );
		return;
	}

	//	Get handler
	util::xml::dom::NodeHandler *handler = doc->handler();
	DTV_ASSERT(handler);

	//	Load document
	if (!doc->loadXML( _bufCheck )) {
		LDEBUG( "Update", "Cannot parse xml response: xml=%s", _bufCheck->bytes() );
		return;
	}

	{	//	has update?
		util::xml::dom::Node node = handler->child( doc->root(), "has_update" );
		if (!node || handler->textContent( node ) != "True") {
			LDEBUG( "Update", "No firmware update!" );
			return;
		}
	}

	{	//	Firmware URL
		util::xml::dom::Node node = handler->child( doc->root(), "firmware" );
		if (!node) {
			LWARN( "Update", "There is a firmware available but firmware property is not present" );
			return;
		}
		_info.url = handler->textContent( node );
	}

	{	//	md5
		util::xml::dom::Node node = handler->child( doc->root(), "md5" );
		if (!node) {
			LWARN( "Update", "There is a firmware available but md5 property is not present" );
			return;
		}
		_info.md5 = handler->textContent( node );
	}

	{	//	size
		util::xml::dom::Node node = handler->child( doc->root(), "size" );
		if (!node) {
			LWARN( "Update", "There is a firmware available but size property is not present" );
			return;
		}
		_info.totalSize = boost::lexical_cast<unsigned long long>(handler->textContent( node ));
	}

	{	//	Description
		util::xml::dom::Node node = handler->child( doc->root(), "description" );
		if (!node) {
			LWARN( "Update", "There is a firmware available but description property is not present" );
			return;
		}
		_info.basic.description = handler->textContent( node );
	}

	requestFetchUpdate();
}

void Fetcher::requestFetchUpdate() {
	LDEBUG( "Update", "Request firmware update: url=%s", _info.url.c_str() );
	util::netlib::RequestInfo *request = new util::netlib::RequestInfo();

	//	Setup filename
	_info.basic.fileName = util::netlib::temporaryName( _zapper->ramDisk() );

	//	Setup request
	request->url( _info.url );
	request->verbose( util::cfg::getValue<bool>("zapper.updater.verbose") );
	request->verifyPeer( true );
	request->caCertificate( pemFile() );
	request->onFinished( boost::bind(&Fetcher::fetchUpdateFinished,this,_2) );
	if (!_onProgress.empty()) {
		double steps = (double)_info.totalSize/(double)_steps;
		request->onProgress( boost::math::iround(steps), boost::bind(&Fetcher::downloadProgress,this,_1) );
	}

	//	Start request
	_reqID = netlib()->start( request, _info.basic.fileName, _info.headers );
}

void Fetcher::fetchUpdateFinished( bool result ) {
	LDEBUG( "Update", "Fetch update finished: result=%d", result );

	_reqID.reset();
	if (!result) {
		LWARN( "Update", "Request failed" );
		return;
	}

	if (!util::check_md5( _info.basic.fileName, _info.md5 )) {
		LWARN( "Update", "Invalid md5 for fetched file: md5=%s", _info.md5.c_str() );
		return;
	}

	fs::path dstName = _destDirectory;

	{	//	Try to find filename on headers
		std::string tmp = util::netlib::resolveName( _info.url, _info.headers );
		if (!tmp.empty()) {
			dstName /= tmp;
		}
		else {	//	Use temporary filename
			dstName /= "util_curl_file%%%%%%%%";
			dstName = fs::unique_path(dstName).string();
		}
	}

	//	Move to update directory
	try {
		fs::rename( _info.basic.fileName, dstName );
		_info.basic.fileName = dstName.string();
	} catch (const fs::filesystem_error& ex) {
		LERROR( "Update", "Cannot move update file: %s", ex.what() );
		return;
	}

	//	Notify
	DTV_ASSERT(!_onUpdateReceived.empty());
	_onUpdateReceived( &_info.basic );
}

void Fetcher::downloadProgress( unsigned long long cant ) {
	DTV_ASSERT(!_onProgress.empty());
	_onProgress( cant, _info.totalSize );
}

const std::string Fetcher::pemFile() const {
	//	Setup PEM file
	fs::path tmp = _zapper->ramDisk();
	tmp /= "ssl-cert.pem";
	return tmp.string();
}

#define CHUNK 1024
bool Fetcher::verifyPem( const std::string &filename ) {
	int ret;
	unsigned have;
	z_stream strm;
	unsigned char out[CHUNK];
	FILE *file;
	bool err=false;

	if(fs::exists(filename)) {
		char md5[16*2+1];
		for(int i=0; i < 16; ++i) {
			sprintf(md5+2*i, "%02x", pem_md5[i]);
		}
		if(util::check_md5(filename, md5)) {
			return true;
		}
		LWARN("Update", "Corrupted pem file!");
	}

	//	Allocate inflate state
	strm.zalloc   = Z_NULL;
	strm.zfree    = Z_NULL;
	strm.opaque   = Z_NULL;
	strm.avail_in = 0;
	strm.next_in  = Z_NULL;

	ret = inflateInit2(&strm, 16+MAX_WBITS);
	if (ret != Z_OK) {
		LERROR( "Update", "Cannot initialize zlib." );
		return false;
	}

	file = fopen( filename.c_str(), "wb" );
	if (!file) {
		LERROR( "Update", "Cannot create file: %s.", filename.c_str() );
		return false;
	}

	//	Fix zipped_pem header
	zipped_pem[0] = 0x1F;
	zipped_pem[1] = 0x8B;

	strm.avail_in = sizeof(zipped_pem) / sizeof(zipped_pem[0]);
	strm.next_in = zipped_pem;

	//	run inflate() on input until output buffer not full
	do {
		strm.avail_out = CHUNK;
		strm.next_out = out;

		ret = inflate(&strm, Z_NO_FLUSH);
		DTV_ASSERT(ret != Z_STREAM_ERROR);  //	state not clobbered
		switch (ret) {
			case Z_NEED_DICT:
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				LWARN( "Update", "Inflate error: ret=%d", ret );
				err=true;
		}

		//	Write inflated data
		have = CHUNK - strm.avail_out;
		if (fwrite( out, 1, have, file ) != have) {
			LWARN( "Update", "write error: ret=%x", ret );
			err=true;
		}
	} while (strm.avail_out == 0);
	//	done when inflate() says it's done

	//	clean up
	(void)inflateEnd(&strm);
	fclose(file);
	return !err;
}

util::netlib::Manager *Fetcher::netlib() const {
	return _zapper->network();
}

}
}

