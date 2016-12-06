/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-mpegparser implementation.

    DTV-mpegparser is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-mpegparser is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-mpegparser.

    DTV-mpegparser es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-mpegparser se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "downloadfilter.h"
#include "dsmccdemuxer.h"
#include "module.h"
#include "../../../resourcemanager.h"
#include <util/log.h>
#include <boost/filesystem.hpp>


namespace tuner {
namespace dsmcc {

namespace fs = boost::filesystem;

DownloadFilter::DownloadFilter( util::DWORD specifier, util::DWORD downloadID, DSMCCDemuxer *demux )
	: DSMCCFilter( demux ), _specifier(specifier), _downloadID(downloadID)
{
	_filesCount = 0;
	_files = new Download();

	//	Filter DII table
	filterDII();
}

DownloadFilter::~DownloadFilter()
{
	DEL(_files);
}

void DownloadFilter::onDownload( const OnDownload &callback ) {
	_onDownload = callback;
}

void DownloadFilter::processDII( const boost::shared_ptr<dsmcc::DII> &dii ) {
	//	Check if downloadID is correct
	if (_downloadID == dii->downloadID()) {
		//	Check if DII is for my download
		bool found=false;
		BOOST_FOREACH( const dsmcc::compatibility::Descriptor &desc, dii->compatibilities() ) {
			if (desc.specifier == _specifier) {
				//	Filter all modules in DII
				filterModules( dii );

				//	Set the amount of modules to be downloaded!!!
				_filesCount = dii->modules().size();

				found = true;
			}
		}

		if (!found) {
			LWARN( "DownloadFilter", "Ignoring DII becouse specifier not found: downloadID=%08x", dii->downloadID() );			
		}
	}
	else {
		LWARN( "DownloadFilter", "Ignoring DII: downloadID=%08x not referenced", dii->downloadID() );
	}
}

void DownloadFilter::processModule( const boost::shared_ptr<dsmcc::Module> &module ) {
	dsmcc::module::Descriptors descs;
	dsmcc::module::Descriptors::const_iterator it;
	std::string fileName;

	//	Check downloadID
	if (_downloadID == module->downloadID()) {
		//	Parse Module Info as descriptors
		dsmcc::module::parseDescriptors( module->info(), descs );
		LDEBUG( "DownloadFilter", "Module received: descriptors=%d, moduleSize=%d",
		        descs.size(), module->size() );

		if (descs.size()) {
			//	Check type descriptor exist
			it=descs.find( MODULE_DESC_TYPE );
			if (it == descs.end()) {
				LWARN( "DownloadFilter", "Module descriptor type not found" );
				return;
			}
			const std::string &type = (*it).second.get<std::string>();

			//	Check type = application/x-download
			if (type != "application/x-download") {
				LWARN( "DownloadFilter", "Module descriptor type invalid (%s)", type.c_str() );
				return;
			}
			
			//	Check name descriptor
			it=descs.find( MODULE_DESC_NAME );
			if (it == descs.end()) {
				LWARN( "DownloadFilter", "Module descriptor name not found" );
			}
			else {
				fileName = (*it).second.get<std::string>();
			}

			//	Check CRC descriptor
			it=descs.find( MODULE_DESC_CRC32 );
			if (it != descs.end()) {
				if (!module->checkCRC32( (*it).second.get<util::DWORD>() )) {
					LWARN( "DownloadFilter", "Module CRC32 failed" );
					return;
				}
				else {
					LDEBUG( "DownloadFilter", "CRC 32 check on module ok!" );
				}
			}
		}

		//	If filename not processed ...
		if (fileName.empty()) {
			//	Create random file name
			fileName = resourceMgr()->mkTempFileName( "module_%%%%%%%%", false );
		}

		//	Build path directory
		fs::path path = resourceMgr()->downloadPath();
		path /= fileName;

		//	Save file as
		if (module->saveAs( path.string() )) {
			LDEBUG( "DownloadFilter", "File downloaded: name=%s, size=%d", path.string().c_str(), module->size() );

			//	Add file to download
			_files->push_back( path.string() );

			//	If update is complete
			if (_filesCount == static_cast<int>(_files->size())) {
				demux()->dispatchNotify<Download>( _onDownload, _files );
				_files = new Download();
			}
		}
		else {
			LWARN( "DownloadFilter", "Module cannot be saved: %s", path.string().c_str() );
		}
	}
}

}
}

