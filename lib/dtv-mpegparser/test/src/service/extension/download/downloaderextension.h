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

#include "../extension.h"
#include "../../../../../src/service/extension/download/downloaderextension.h"
#include "../../../../../src/service/extension/download/downloadinfo.h"
#include "../../../../../src/demuxer/ts.h"

class DownloaderExtensionTest : public ServiceManagerTest {
public:
	DownloaderExtensionTest( void ) {}
	virtual ~DownloaderExtensionTest( void ) {}

	virtual void SetUp() {
		_info = NULL;
		_step = _total = 0;
		ServiceManagerTest::SetUp();
	}

	virtual void TearDown() {
		mgr()->ready( false );
		mgr()->stop();
	}

	virtual void start( tuner::DownloaderExtension *ext ) {
		ext->onDownload( boost::bind(&DownloaderExtensionTest::onDownload,this,_1) );
		ext->onProgressChanged( boost::bind(&DownloaderExtensionTest::onProgress,this,_1,_2) );
		mgr()->attach( ext );
		mgr()->initialize();
		mgr()->start();
		mgr()->ready( true );
	}

	void probe( tuner::ID pid, const char *file ) {
		mgr()->probe( pid, file );
	}

	void doServiceReady() {
		util::BYTE tag=0x10;
		tuner::ID ocPID = 0x102;
		tuner::ID srvID = 0xe764;
		tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );
		mgr()->readySrv( srv );
		delete srv;
	}

	void probeCarousel() {
		tuner::ID pid = 0x102;
		mgr()->probe( pid, "download/DII.sec" );
		mgr()->probe( pid, "download/upgrade.tar_000000.sec" );
	}

	void onProgress( int step, int total ) {
		_step = step;
		_total = total;
	}

	void onDownload( tuner::DownloadInfo *info ) {
		_info = info;
	}

	virtual OkServiceManager *createManager( tuner::ResourceManager *res );

protected:
	tuner::DownloadInfo *_info;
	int _step;
	int _total;
};


class DownloaderExtensionTest2 : public DownloaderExtensionTest {
public:	
	virtual OkServiceManager *createManager( tuner::ResourceManager *res );
};
