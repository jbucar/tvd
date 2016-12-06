/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-pvr implementation.

    DTV-pvr is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-pvr is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-pvr.

    DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include <util/log.h>
#include <util/mcr.h>
#include <util/assert.h>
#include <boost/foreach.hpp>
#include <vector>

namespace pvr {
namespace service {

template<typename P>
class Service {
public:
	Service();
	virtual ~Service();

	//	Setup
	inline void add( P *p );

	//	Methods
	bool start();
	void stop();
	void resetConfig();
	bool isStarted() const;

	bool isEnabled() const;
	void enableAll( bool state );
	P *get( const std::string &name ) const;
	const std::vector<P *> &getAll() const;

	//	Wrappers
	bool enable( const std::string &name, bool state );
	bool isEnabled( const std::string &name ) const;
	const std::string &version( const std::string &name ) const;

protected:
	virtual bool init();
	virtual void fin();

private:
	std::vector<P *> _providers;
	bool _started;
};

template<class P>
inline Service<P>::Service()
{
	_started = false;
}

template<class P>
inline Service<P>::~Service()
{
	CLEAN_ALL( P *, _providers );
}

//	Setup
template<class P>
inline void Service<P>::add( P *p ) {
	_providers.push_back(p);
}

//	Methods
template<class P>
inline bool Service<P>::start() {
	DTV_ASSERT(!_started);

	if (!init()) {
		return false;
	}

	BOOST_FOREACH( P *p, _providers ) {
		if (!p->start()) {
			LWARN( "Service", "Cannot start provider: name=%s", p->name().c_str() );
		}
	}

	_started = true;

	return true;
}

template<class P>
inline bool Service<P>::init() {
	return true;
}

template<class P>
inline void Service<P>::stop() {
	if (_started) {
		BOOST_FOREACH( P *p, _providers ) {
			p->stop();
		}
		fin();

		_started = false;
	}
}

template<class P>
inline void Service<P>::fin() {
}

template<class P>
bool Service<P>::isStarted() const {
	return _started;
}

template<class P>
inline void Service<P>::resetConfig() {
	BOOST_FOREACH( P *p, _providers ) {
		p->resetConfig();
	}
}

template<class P>
inline bool Service<P>::isEnabled() const {
	bool enabled=false;
	BOOST_FOREACH( P *p, _providers ) {
		enabled |= p->isEnabled();
	}
	return enabled;
}

template<class P>
inline void Service<P>::enableAll( bool state ) {
	BOOST_FOREACH( P *p, _providers ) {
		p->enable(state);
	}
}

template<class P>
inline bool Service<P>::enable( const std::string &name, bool state ) {
	P *p = get( name );
	if (!p) {
		LWARN( "Service", "Cannot enable service: name=%s", name.c_str() );
		return false;
	}
	p->enable( state );
	return true;
}

template<class P>
inline bool Service<P>::isEnabled( const std::string &name ) const {
	P *p = get( name );
	if (!p) {
		LWARN( "Service", "Service not found: name=%s", name.c_str() );
		return false;
	}
	return p->isEnabled();
}

template<class P>
inline const std::string &Service<P>::version( const std::string &name ) const {
	P *p = get( name );
	if (!p) {
		static std::string defaultVersion = "0.0";
		LWARN( "Service", "Service not found: name=%s", name.c_str() );
		return defaultVersion;
	}
	return p->version();
}

template<class P>
inline P *Service<P>::get( const std::string &name ) const {
	typename std::vector<P *>::const_iterator it=std::find_if(
		_providers.begin(),
		_providers.end(),
		boost::bind(&P::name,_1) == name );
	if (it != _providers.end()) {
		return (*it);
	}
	return NULL;
}

template<class P>
inline const std::vector<P *> &Service<P>::getAll() const {
	return _providers;
}

}
}

