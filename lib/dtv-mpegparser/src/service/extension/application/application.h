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

#include "applicationid.h"
#include "../../../language.h"
#include <util/keydefs.h>
#include <util/buffer.h>
#include <boost/function.hpp>
#include <vector>
#include <map>

namespace tuner {

namespace dsmcc {
	class Event;
	class ObjectCarousel;
	class DSMCCDemuxer;
	class StreamEventDemuxer;
	typedef std::vector<Event *> Events;
}

namespace app {

class Controller;
class Extension;
class RunTask;

class Application {
public:
	Application( Extension *ext, const ApplicationID &id );
	Application( Extension *ext, const ApplicationID &id, const std::string &path );
	virtual ~Application();

	bool operator==( const ApplicationID &id );

	//	Start/Stop
	bool start();
	void stop( bool kill=false );
	void starting();
	void started();
	void queue();
	virtual int exitTimeout() const;

	//	Status
	bool isRunning() const;
	status::type status() const;

	virtual void show() const;

	void componentTag( util::BYTE tag );
	util::BYTE componentTag() const;

	virtual void mount( const std::string &path );
	virtual void mount( const std::string &path, const dsmcc::Events &events );
	void unmount();
	bool isMounted() const;
	const std::string &path() const;

	const ApplicationID &appID() const;
	const Language &language() const;
	const std::string &name() const;
	void name( const std::string &n, const Language &lang="" );

	bool autoStart() const;
	void autoStart( bool var );

	//	Download
	bool needDownload() const;
	bool autoDownload() const;
	void autoDownload( bool var );
	bool startDownload();
	void stopDownload();

	bool isBoundToService() const;
	bool isBoundToService( ID serviceID ) const;
	ID service() const;
	void service( ID serviceID );

	bool readOnly() const;
	void readOnly( bool var );

	visibility::type visibility() const;
	void visibility( visibility::type var );

	int priority() const;
	void priority( int var );

	void addVideoMode( video::mode::type mode );
	const std::vector<video::mode::type> &supportedModes() const;

	void addIcon( const std::string &icon );
	void addIcons( const std::vector<std::string> &icons );
	const std::vector<std::string> &icons() const;

	typedef boost::function<void (const util::Buffer &buf)> EventCallback;
	bool registerEvent( const std::string &url, const std::string &eventName, const EventCallback &callback );
	void unregisterEvent( const std::string &url, const std::string &eventName );

	bool dispatchKey( util::key::type key, bool isUp );

protected:
	//	Keys
	void reserveKeys( const std::vector<util::key::type> &keys );
	virtual void dispatchKeyImpl( util::key::type key, bool isUp );

	//	Status changes
	virtual void onStarting( RunTask *task );
	virtual void onStarted();
	virtual void onStopping();
	virtual void onStopped();
	bool canChange( status::type st ) const;
	void startImpl();

	//	Object Carousel
	bool startObjectCarousel( ID pid, ID tag );
	void onMounted( const boost::shared_ptr<dsmcc::ObjectCarousel> &oc );
	void onDownloadChanged( int step, int total );

	//	Stream Events
	typedef std::map<util::WORD,EventCallback> StreamEventCallback;
	typedef std::pair<dsmcc::StreamEventDemuxer *,int> StreamEventType;
	typedef std::pair<std::string,StreamEventType> StreamEventItem;
	typedef std::map<std::string,StreamEventType> StreamEventFilters;
	void stopStreamEvents();
	void stopStreamEvent( ID pid );
	void onEvent( util::WORD eventID, util::Buffer *streamEvent );
	template<class T>
	inline void loopEvents( T &finder );

	//	Getters
	Controller *ctrl() const;
	Extension *extension() const;

private:
	void status( status::type st );

	std::vector<util::key::type> _keys;
	Extension       *_extension;
	dsmcc::Events    _events;
	status::type     _status;
	ApplicationID    _appID;
	std::string      _name;
	Language         _language;
	std::string      _path;
	bool             _autoStart;
	bool             _autoDownload;
	bool             _readOnly;
	ID               _service;
	visibility::type _visibility;
	int              _priority;
	util::BYTE       _componentTag;
	std::vector<ID>  _downloads;
	std::vector<video::mode::type> _videoModes;
	std::vector<std::string> _icons;
	StreamEventFilters  _streams;
	StreamEventCallback _streamCallbacks;
	RunTask *_runTask;
};

template<class T>
inline void Application::loopEvents( T &iter ) {
	dsmcc::Events::const_iterator it = _events.begin();
	while (it != _events.end()) {
		iter( (*it) );
		it++;
	}
}

}
}
