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
#include "downloaderextension.h"
#include "downloadinfo.h"
#include "../../service.h"
#include "../../serviceprovider.h"
#include "../../../provider/provider.h"
#include "../../../demuxer/ts.h"
#include "../../../demuxer/psi/psi.h"
#include "../../../demuxer/psi/download/sdttdemuxer.h"
#include "../../../demuxer/psi/dsmcc/downloadfilter.h"
#include "../../../demuxer/psi/dsmcc/dsmccdemuxer.h"
#include "../../../demuxer/psi/dsmcc/datacarouselhelper.h"
#include <util/assert.h>
#include <util/log.h>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

namespace tuner {

namespace fs = boost::filesystem;

struct FindDII {
	FindDII( DWORD downloadID ) { _downloadID = downloadID; }
	bool operator()( const DownloadInfo *info ) const {
		return info->downloadID() == _downloadID;
	}
	DWORD _downloadID;
};

DownloaderExtension::DownloaderExtension( BYTE maker, BYTE model, WORD version )
{
	_makerID   = maker;
	_modelID   = model;
	_version   = version;
	_dc = new dsmcc::DataCarouselHelper();
}

DownloaderExtension::~DownloaderExtension()
{
	delete _dc;
}

void DownloaderExtension::onDownload( const OnDownload &callback ) {
	_onDownload = callback;
}

void DownloaderExtension::onProgressChanged( const OnProgressChanged &callback ) {
	_onProgress = callback;
}

//	Notifications
void DownloaderExtension::onReady( bool isReady ) {
	if (isReady) {
		startFilters();
	}
	else {
		stopFilters();
	}
}

static bool collectDSMCC( BYTE streamType ) {
	return streamType == PSI_ST_TYPE_B || streamType == PSI_ST_TYPE_D;
}

void DownloaderExtension::onServiceReady( Service *srv, bool ready ) {
	if (ready) {
		//	Try clean old downloads
		cleanDownloads();

		//	Get TAGs from all DSMCC streams ...
		bool find=_dc->collectTags( srv, &collectDSMCC );
		if (find) {
			tryDownload();
		}
	}
}

void DownloaderExtension::startFilters() {
	PSIDemuxer *demux;

	LINFO("DownloaderExtension", "Filter Low/High SDTT profiles");
	DTV_ASSERT(!_onDownload.empty());

	//	Low profile
	demux = createDemuxer<SDTTDemuxer,Sdtt>(
		TS_PID_SDTT_LOW,
		boost::bind(&DownloaderExtension::onSdtt,this,_1) );
	srvMgr()->startFilter( demux );

	//	High profile
	demux = createDemuxer<SDTTDemuxer,Sdtt>(
		TS_PID_SDTT_HIGH,
		boost::bind(&DownloaderExtension::onSdtt,this,_1) );
	srvMgr()->startFilter( demux );
}

void DownloaderExtension::stopFilters() {
	LINFO("DownloaderExtension", "Stop Low/High SDTT profiles: downlods=%d", _downloads.size() );

	srvMgr()->stopFilter( TS_PID_SDTT_LOW );
	srvMgr()->stopFilter( TS_PID_SDTT_HIGH );

	//	Clear any carousel and downloader data
	_dc->clearTags();
	BOOST_FOREACH( DownloadInfo *info, _downloads ) {
		stop( info );
	}
	_downloads.clear();
}

void DownloaderExtension::cleanDownloads() {
	LDEBUG("DownloaderExtension", "Clean old downloads: downloads=%d", _downloads.size() );

	std::vector<DownloadInfo *>::iterator it = _downloads.begin();
	while (it != _downloads.end() ) {
		if ((*it)->isComplete()) {
			stop( (*it) );
			it = _downloads.erase( it );
		}
		else {
			it++;
		}
	}
}

void DownloaderExtension::stop( DownloadInfo *info ) {
	LINFO("DownloaderExtension", "Stop download: downloadID=%08x", info->downloadID());
	if (info->started()) {
		srvMgr()->stopFilter( info->pid() );
	}
	delete info;
}

//	Tables
void DownloaderExtension::onSdtt( const boost::shared_ptr<Sdtt> &sdtt ) {
	ID model = sdtt->modelID();

	//	First, try clean all old downloads
	cleanDownloads();

	sdtt->show();

	//	Check if SDTT is for my model, current network and current TS ...
	if (MAKER_ID(model) == _makerID && MODEL_ID(model) == _modelID &&
		checkTS( sdtt->tsID() ) && checkNit( sdtt->nitID() ))
	{
		bool find=false;
		const std::vector<Sdtt::ElementaryInfo> &elements = sdtt->elements();

		//	Iterate throw SDTT elements
		BOOST_FOREACH( const Sdtt::ElementaryInfo &elem, elements ) {
			bool apply = false;

			//	Is compulsory?
			bool isCompulsory = (elem.downloadLevel == 0x01) ? true : false;

			//	Check what version is the target
			switch (elem.versionIndicator) {
				case 0x00:	//	All versions are targeted
					apply = true;
					break;
				case 0x01:	//	Version(s) specified or later are targeted
					apply = (_version >= elem.targetVersion);
					break;
				case 0x02:	//	Version(s) specified or earlier are targeted
					apply = (_version <= elem.targetVersion);
					break;
				case 0x03:	//	Only specified version is targeted
					apply = (_version == elem.targetVersion);
					break;
			};

			LINFO("DownloaderExtension", "downloadLevel=%02x, version=%04x, indicator=%02x, target version=%04x, apply=%d",
				elem.downloadLevel, _version, elem.versionIndicator, elem.targetVersion, apply);

			//	Ok, apply, add download ...
			if (apply) {
				BOOST_FOREACH( const Sdtt::DownloadContentDescriptor &desc, elem.contents ) {
					//	If we already have the same downloadID
					std::vector<DownloadInfo *>::iterator it = std::find_if( _downloads.begin(), _downloads.end(), FindDII(desc.downloadID) );
					if (it != _downloads.end()) {
						LWARN( "DownloaderExtension", "Same downloadID in another update. Remove old update and add new: downloadID=%08x", desc.downloadID );
						stop( (*it) );
						_downloads.erase (it );
					}

					LINFO("DownloaderExtension", "Add download: tag=%02x, downloadID=%08x, isCompulsory=%d",
						desc.componentTag, desc.downloadID, isCompulsory);

					_downloads.push_back( new DownloadInfo( sdtt->serviceID(), desc.componentTag, desc.downloadID, isCompulsory ) );
					find = true;
				}
			}
		}

		if (find) {
			tryDownload();
		}
	} else {
		LINFO("DownloaderExtension", "Ignoring SDTT: stb(%02x,%02x), update=(%02x,%02x)",
			_makerID, _modelID, MAKER_ID(model), MODEL_ID(model));
	}
}

void DownloaderExtension::tryDownload() {
	//	For each downloadInfo ...
	BOOST_FOREACH( DownloadInfo *info, _downloads ) {
		if (!info->started()) {
			LINFO("DownloaderExtension", "Try download: tag=%02x, downloadID=%x", info->tag(), info->downloadID());

			//	Try find association tag in DSMCC tags stored
			ID pid=_dc->findTag( info->srvID(), info->tag() );
			if (pid != TS_PID_NULL) {
				//	Create demuxer
				dsmcc::DSMCCDemuxer *demux = new dsmcc::DSMCCDemuxer( pid, resMgr() );

				//	Create and setup downloader filter
				dsmcc::DownloadFilter *df = new dsmcc::DownloadFilter( DOWNLOAD_SPECIFIER, info->downloadID(), demux );
				df->onDownload( boost::bind(&DownloaderExtension::onDownload,this,info,_1) );
				df->onProgressChanged( _onProgress );

				//	Start filter
				srvMgr()->startFilter( demux );

				//	Mark download started
				info->start( pid );
			}
			else {
				LWARN("DownloaderExtension", "tag not found: tag=%02x, downloadID=%08x", info->tag(), info->downloadID());
			}
		}
	}
}

void DownloaderExtension::onDownload( DownloadInfo *info, const boost::shared_ptr<dsmcc::Download> &download ) {
	info->onComplete( *download );
	_onDownload( info );
}

}
