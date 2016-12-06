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
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <vector>
#include <set>

namespace util {
	class Buffer;

	namespace pool {
		template<typename T>
		class CircularPool;
	}

	namespace task {
		class Dispatcher;
	}

}

namespace tuner {

namespace psi {
	class Cache;
}

class Frontend;
class Filter;
class PSIDemuxer;

namespace impl {
	class Timer;
	namespace pt = boost::posix_time;

	typedef struct {
		util::Buffer *buf;
		ID pid;
	} SectionBuffer;

	typedef struct {
		boost::function<void (void)> callback;
		ID pid;
	} SectionNotification;

	namespace state {
		enum type {
			off,      //  Tuner is turned off
			fail,     //  Tuner with error
			idle,     //  Tuner is currenly idle
			tuned     //  Tuner is receiving signal from a valid signal
		};
	}
}

/**
 * Provider permite acceder a las distintas secciones de control de un TS (PAT, PMT, NIT, etc) y a sus flujos de audio, video y datos.
 */
class Provider {
public:
	Provider();
	virtual ~Provider();

	//	Start/Stop provider
	bool start( util::DWORD networkBlocks=1000 );
	void stop();

	//	Generic network methods
	virtual std::string name() const;
	size_t getNetworksCount() const;
	std::string getNetworkName( size_t nIndex ) const;
	size_t findNetwork( const std::string &name ) const;

	//	Change network
	size_t currentNetwork() const;
	bool setNetwork( size_t nIndex );
	void stopCurrent();

	//	Network status
	typedef boost::function<void (void)> OnLockChanged;
	typedef boost::function<void (void)> OnLockTimeout;
	void onLockChanged( const OnLockChanged &callback );
	void onLockTimeout( const OnLockTimeout &callback );
	void status( status::Type &st ) const;
	bool isLocked() const;

	//	Filters methods
	bool startFilter( PSIDemuxer *demux );
	bool startFilter( pes::FilterParams *params );
	void stopFilter( ID pid, bool destroy=true );

	//	PSI demuxer implementation
	void setDispatcher( util::task::Dispatcher *taskDispatcher );
	util::task::Dispatcher *dispatcher() const;

	//	Instance creation
	static Provider* create();

protected:
	//	Types
	typedef std::list<impl::Timer *> TimerList;
	typedef std::list<Filter *> FilterList;

	typedef util::pool::CircularPool<impl::SectionBuffer> SectionsPool;
	typedef std::list<impl::SectionNotification> NotificationList;

	typedef std::list<PSIDemuxer *> DemuxerList;

	//	Frontend methods
	virtual Frontend *createFrontend() const=0;
	virtual void stopNetwork();

	//	Status/signal methods
	void state( impl::state::type newState );
	impl::state::type getState() const;
	void updateStatus( const status::Type &st, bool start, bool post=false );
	void checkStatus();
	virtual int lockTimeout() const;
	virtual int lockLatency() const;
	virtual int statusPollTimeout() const;

	//	Aux filters
	virtual Filter *createFilter( PSIDemuxer *demux )=0;
	virtual Filter *createFilter( pes::FilterParams *params )=0;
	virtual bool startEngineFilter();
	virtual void stopEngineFilter();
	virtual bool checkCRC() const;
	virtual bool startFilter( Filter *filter );
	virtual void stopFilter( Filter *filter, bool destroy );
	virtual int maxFilters() const;
	virtual bool haveTimers() const;
	void restartFilter( ID pid );
	Filter *removeFilter( ID pid );
	bool canAllocFilter( bool isSection, ID pid );

	//	Demuxers
	void timeout( ID pid );
	void enqueue( ID pid, util::BYTE *data, util::DWORD size );
	void enqueue( ID pid, util::Buffer *buf );
	void processSection( impl::SectionBuffer &section );
	virtual void processSectionBuffer( PSIDemuxer *demux, util::Buffer *buf );
	void onNotification( ID pid, const boost::function<void (void)> &callback );
	void postNotification();
	void clear( ID pid );

	//	Aux Network buffer
	util::Buffer *getNetworkBuffer();
	void freeNetworkBuffer( util::Buffer *buf );

	//	Aux timers
	void addTimer( ID pid, int ms );
	void rmTimer( ID pid );
	void resetTimer( ID pid );
	int checkTimers();

	//	Aux thread
	void providerThread();

private:
	util::task::Dispatcher *_disp;
	boost::thread _thread;

	//	Frontend
	Frontend *_frontend;
	size_t _current;

	//	Status
	impl::state::type _state;
	status::Type _st;
	impl::pt::ptime _lockTimer;
	impl::pt::ptime _lockLatency;	
	OnLockChanged _onLockChanged;
	OnLockTimeout _onLockTimeout;

	//	Sections
	SectionsPool *_pool;
	psi::Cache *_cache;

	//	Demuxers
	boost::mutex _mDemuxers;
	DemuxerList	_demuxers;

	//	Notifications
	boost::mutex _mNotifications;
	NotificationList _notifications;

	//	Filters
	boost::mutex _mFilters;
	FilterList _filters;

	//	Timers
	boost::shared_mutex  _mTimers;
	TimerList _timers;
};

}

//	Pool implementation
namespace util {
namespace pool {
namespace impl {
	inline Buffer *getBuffer( tuner::impl::SectionBuffer &item ) { return item.buf; }
}
}
}

