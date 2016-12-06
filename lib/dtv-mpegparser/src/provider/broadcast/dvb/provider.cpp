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
#include "provider.h"
#include "frontend.h"
#include "pesfilter.h"
#include "sectionfilter.h"
#include "../../../stream/pipe.h"
#include "generated/config.h"
#include <util/cfg/configregistrator.h>
#include <util/io/dispatcher.h>
#include <util/task/dispatcher.h>
#include <util/string.h>
#include <util/buffer.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

namespace tuner {
namespace dvb {

REGISTER_INIT_CONFIG( tuner_provider_dvb ) {
	root().addNode( "dvb" )
		.addValue( "max_filters", "Maximum section filters", 32 )
		.addValue( "useSet_v5", "Use set frequency API version >= (5,5)", true )
		.addValue( "useStats_v5", "Use statistics using API version >= (5,10)", true )
		.addValue( "adapter", "Adapter to use", 0 )
		.addValue( "frontend", "Frontend to use", 0 )
		.addValue( "demux", "Demux to use", 0 )
		.addValue( "names", "Use default names", true )
		.addValue( "genericPES", "Use generic PES type", true )
		.addValue( "diseqcTimeout", "Diseqc timeout (in ms)", 15 )
		.addValue( "buffer", "Buffer size for PES filters", 0 );
}

Provider::Provider( void )
{
	_io = util::io::Dispatcher::create();

	util::cfg::PropertyNode &dvbNode = util::cfg::get("tuner.provider.dvb");
	_maxFilters = dvbNode.get<int>("max_filters");

	{	//	Create devices name
		int adapter = dvbNode.get<int>("adapter");
		int frontend = dvbNode.get<int>("frontend");
		int demux = dvbNode.get<int>("demux");

		if (dvbNode.get<bool>("names")) {
			_frontend = util::format( "/dev/dvb/adapter%d/frontend%d", adapter, frontend );
			_demux    = util::format( "/dev/dvb/adapter%d/demux%d", adapter, demux );
		}
		else {
			_frontend = util::format( "/dev/dvb%d.frontend%d", adapter, frontend );
			_demux    = util::format( "/dev/dvb%d.demux%d", adapter, demux );
		}

		LINFO( "Provider", "Using devices: frontend=%s, demux=%s", _frontend.c_str(), _demux.c_str() );
	}
}

Provider::~Provider( void )
{
	delete _io;
}

//	Filter aux
tuner::Filter *Provider::createFilter( PSIDemuxer *demux ) {
	return new SectionFilter( demux, this );
}

tuner::Filter *Provider::createFilter( pes::FilterParams *params ) {
	return new PesFilter( params, this );
}

bool Provider::checkCRC() const {
	return false;
}

int Provider::maxFilters() const {
	return _maxFilters;
}

//	Getters
const std::string &Provider::device() {
	return _demux;
}

void Provider::startReadSections( int fd, ID pid ) {
	LTRACE( "Provider", "Start section io: fd=%d", fd );
	IOCallback fnc = boost::bind(&Provider::readSection,this,fd,pid);
	_io->dispatcher()->post( this, boost::bind(&Provider::startIO,this,fd,fnc) );
}

void Provider::startStream( int fd, stream::Pipe *pipe ) {
	LTRACE( "Provider", "Start stream io: fd=%d", fd );
	IOCallback fnc = boost::bind(&Provider::readStream,this,fd,pipe);
	_io->dispatcher()->post( this, boost::bind(&Provider::startIO,this,fd,fnc) );
}

void Provider::rmWatcher( int fd ) {
	_io->dispatcher()->post( this, boost::bind(&Provider::stopIO,this,fd) );
}

void Provider::startIO( int fd, const IOCallback &fnc ) {
	LTRACE( "Provider", "Start io: fd=%d", fd );
	util::id::Ident id = _io->addIO( fd, fnc );
	_handles[fd] = id;
}

void Provider::stopIO( int fd ) {
	Handles::iterator it = _handles.find( fd );
	if (it != _handles.end()) {
		util::id::Ident id = (*it).second;
		_io->stopIO( id );
		_handles.erase(it);
	}
	else {
		LWARN( "Provider", "Cannot found IO fd: fd=%d", fd );
	}
}

//	Aux filters engine
bool Provider::startEngineFilter( void ) {
	LDEBUG( "Provider", "Start filter engine" );
	if (!_io->initialize()) {
		LERROR( "Provider", "Cannot initialize io engine\n" );
		return false;
	}

	//	Start io thread
	_thread = boost::thread( boost::bind(&Provider::ioThread,this) );

	//	Register this on task dispatcher
	_io->dispatcher()->registerTarget( this, "dvb::Provider" );

	return true;
}

void Provider::stopEngineFilter( void ) {
	LDEBUG( "Provider", "Stop filter engine" );

	//	Exit from io
	_io->dispatcher()->post( this, boost::bind(&util::io::Dispatcher::exit,_io) );
	_thread.join();

	//	Finalize io
	_io->dispatcher()->unregisterTarget( this );
	_io->finalize();
}

void Provider::ioThread() {
	_io->run();
}

void Provider::readSection( int fd, ID pid ) {
	bool send=true;
	util::Buffer *buf = getNetworkBuffer();
	DTV_ASSERT(buf);

	//	Try read
	ssize_t len=read( fd, buf->data(), buf->capacity() );
	if (len > 0) {
		//	Resize content
		buf->resize( len );
	}
	else {
		freeNetworkBuffer( buf );
		buf = NULL;
		send = (len < 0 && errno == ETIMEDOUT);
		if (send) {
			//restartFilter( pid );	//	HACK: When timeout, it does not read again!
		}
	}

	if (send) {
		enqueue( pid, buf );
	}
}

void Provider::readStream( int fd, stream::Pipe *pipe ) {
	util::Buffer *buf = pipe->alloc();
	int dataSize = buf->capacity();
	util::BYTE *data = buf->bytes();
	ssize_t offset = 0;

	while (dataSize > offset) {
		ssize_t len = read( fd, data+offset, dataSize-offset );
		if (len > 0) {
			offset += len;
		}
		else {
			break;
		}
	}

	if (offset > 0) {
		buf->resize( offset );
		pipe->push( buf );
	}
	else {
		pipe->free( buf );
	}
}

//	Tuner methods
tuner::Frontend *Provider::createFrontend() const {
	return new Frontend(_frontend);
}

}
}

