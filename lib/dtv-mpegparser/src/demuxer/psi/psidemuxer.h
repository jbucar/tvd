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

#include "../types.h"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <vector>

namespace util {
	class Buffer;
}

namespace tuner {

namespace psi {
	class Cache;
}

class TableInfo;
class PSIFilter;

/**
 * Esta es la clase base encargada de demultiplexar las tablas psi.
 * Deberá ser subclasificada para cada tabla en particular.
 */
class PSIDemuxer {
public:
	explicit PSIDemuxer( ID pid );
	virtual ~PSIDemuxer();

	//	Getters/Setters
	ID pid() const;
	virtual ID tableID() const;
	virtual bool singleTableID() const;
	virtual bool filterTableID( util::BYTE &tID, util::BYTE &mask ) const;
	virtual util::WORD maxSectionBytes() const;
	virtual util::DWORD timeout() const;
	virtual util::DWORD frequency() const;
	void checkCRC( bool enableCheck );

	//	Filters
	void addFilter( PSIFilter *filter );

	//  Data operation
	virtual void push( util::BYTE *tsPayload, size_t len, bool start );
	virtual void process( util::Buffer *buf );

	//	Cache methods
	void setCache( psi::Cache *cache );
	virtual bool supportCache() const;

	//  Notifications
	typedef boost::function<void (void)> DispatcherTask;
	typedef boost::function<void ( ID pid, const DispatcherTask &)> DispatcherCallback;
	void setDispatcher( const DispatcherCallback &callback );

	typedef boost::function<void ( ID pid )> ExpiredCallback;
	void onExpired( const ExpiredCallback &callback );

	typedef boost::function<void ( ID pid )> TimeoutCallback;
	void onTimeout( const TimeoutCallback &callback );

	//	Notification (Implementation)
	void dispatchNotify( const boost::function<void (void)> &callback );
	template<typename T>
	inline void dispatchNotify( const boost::function<void (const boost::shared_ptr<T> &)> &callback, T *ptr );

protected:
	//	Getters
	virtual bool useStandardVersion() const;
	virtual bool supportMultipleSections() const;

	//	Syntax
	virtual bool syntax() const;
	virtual bool checkSyntax() const;

	//	TableID
	virtual bool checkTableID( ID tID ) const;

	//	Timeout
	virtual void timeoutExpired();

	//	Parsing
	virtual void onSection( util::BYTE *section, size_t len );
	virtual void onSection( TableInfo *ver, util::BYTE *section, size_t len );
	virtual void onComplete( TableInfo *ver, util::BYTE *section, size_t len );
	void parseSection( TableInfo *ver, util::BYTE *section, size_t len );
	bool checkCRC( util::BYTE *payload, int packetLen );

	//  Version handling
	virtual TableInfo *getTable( util::BYTE *section );
	virtual void onVersionChanged();

	//	Cache support
	void cacheSection( BYTE *section, size_t len );

	//	Notification
	bool directDispatch() const;
	void notify( const boost::function<void (void)> &callback );
	template<typename T>
	inline void notify( const boost::function<void (const boost::shared_ptr<T> &)> &callback, T *ptr );
	void notify( const boost::function<void (void)> &callback, bool direct );

	//	Aux
	size_t startHeader( util::BYTE *payload, size_t len );
	size_t endSection( util::BYTE *payload, size_t len );
	util::Buffer *getBuffer();
	bool checkPID( ID pid, ID ignore=0xFFFF );

private:
	ID _pid;
	DispatcherCallback _dispatcher;
	ExpiredCallback _onExpired;
	TimeoutCallback _onTimeout;
	std::vector<PSIFilter *> _filters;
	util::Buffer *_buffer;
	bool _error;
	bool _stuffingByteFlag;
	bool _enableCRC;
	bool _filterTID;
	int _psiTimeoutBase;
	psi::Cache *_cache;
};

template<typename T>
inline void PSIDemuxer::notify( const boost::function<void (const boost::shared_ptr<T> &)> &callback, T *ptr ) {
	boost::shared_ptr<T> tPtr(ptr);
	notify( boost::bind(callback,tPtr) );
}

template<typename T>
inline void PSIDemuxer::dispatchNotify( const boost::function<void (const boost::shared_ptr<T> &)> &callback, T *ptr ) {
	boost::shared_ptr<T> tPtr(ptr);
	dispatchNotify( boost::bind(callback,tPtr) );
}

template<class Demuxer, typename T>
inline PSIDemuxer *createDemuxer(
	ID pid,
	const typename Demuxer::ParsedCallback &onParsed )
{
	Demuxer *demux = new Demuxer( pid );
	demux->onParsed( onParsed );
	return demux;
}

template<class Demuxer, typename T>
	inline PSIDemuxer *createDemuxer(
	ID pid,
	const typename Demuxer::ParsedCallback &onParsed,
	const typename Demuxer::ExpiredCallback &onExpired )
{
	Demuxer *demux = new Demuxer( pid );
	demux->onParsed( onParsed );
	demux->onExpired( onExpired );
	return demux;
}

template<class Demuxer, typename T>
inline PSIDemuxer *createDemuxer(
	ID pid,
	const typename Demuxer::ParsedCallback &onParsed,
	const typename Demuxer::ExpiredCallback &onExpired,
	const typename Demuxer::TimeoutCallback &onTimeout )
{
	Demuxer *demux = new Demuxer( pid );
	demux->onParsed( onParsed );
	demux->onExpired( onExpired );
	demux->onTimeout( onTimeout );
	return demux;
}

}

