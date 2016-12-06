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

#include "servicemanager.h"
#include "service.h"
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>

namespace tuner {

//	Tables
class Pat;
class Pmt;
class Sdt;
class Nit;
class Provider;

class ServiceProvider : public ServiceManager {
public:
	ServiceProvider( Provider *prov, ResourceManager *resources );
	virtual ~ServiceProvider();

	//	Start/Stop
	virtual void start( bool scan=false );
	virtual void stop();

	//	Filters
	virtual bool startFilter( PSIDemuxer *demux );
	virtual bool startFilter( pes::FilterParams *params );
	virtual void stopFilter( ID pid );

	//	Getters
	virtual util::task::Dispatcher *dispatcher();
	virtual ID currentTS() const;
	virtual ID currentNit() const;

protected:
	//	Services
	void killServices();

	//	State
	void setFlags( util::DWORD flag, bool set );
	void stopNit();

	//	Tables callback
	void onPat( const boost::shared_ptr<Pat> &pat );
	void onPatExpired();
	void onPatTimeout();
	void onPmt( const boost::shared_ptr<Pmt> &pmt, ID pid );
	void onPmtExpired( ID pid );
	void onPmtTimeout( ID pid );
	void onNit( const boost::shared_ptr<Nit> &nit );
	void onSdt( const boost::shared_ptr<Sdt> &sdt );
	void onExpired( util::DWORD flags, service::desc::type tableIndex );
	void onTimeout( util::DWORD falgs );
	template<class T> inline void updateServices( const boost::shared_ptr<T> &ptr );
	template<class T> inline void updateService( Service *srv, const boost::shared_ptr<T> &ptr );

private:
	util::DWORD _readyMask;
	ID _tsID;
	ID _nitID;
	ID _nitPID;
	Provider *_provider;
};

template<class T>
inline void ServiceProvider::updateService( Service *srv, const boost::shared_ptr<T> &ptr ) {
	//	Process table and check if changed
	if (srv->process( ptr )) {
		//	Notify of service changed
		serviceChanged( srv );
	}
}

template<class T>
inline void ServiceProvider::updateServices( const boost::shared_ptr<T> &ptr ) {
	ptr->show();

	BOOST_FOREACH( Service *srv, services() ) {
		updateService( srv, ptr );
	}

	//	Notify end scan
	notifyEndScan();
}

}
