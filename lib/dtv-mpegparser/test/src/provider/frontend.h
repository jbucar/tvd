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
#include "../util.h"
#include "../../../src/provider/frontend.h"
#include "../../../src/provider/provider.h"
#include "../../../src/provider/filter.h"
#include <util/task/dispatcherimpl.h>
#include <boost/lexical_cast.hpp>

class TestFrontend : public tuner::Frontend {
public:
	TestFrontend() {
		_isLocked = false;
	}

	virtual ~TestFrontend() {}

	virtual bool initialize() {
		_isLocked = false;
		return true;
	}

	virtual void finalize() {}

	//	Network getters
	virtual size_t getCount() const {
		return 10;
	}

	virtual std::string getName( size_t nIndex ) const {
		return boost::lexical_cast<std::string>(nIndex);
	};

	virtual size_t find( const std::string &name ) const {
		int i = boost::lexical_cast<int>(name);
		BOOST_ASSERT(i>=0 && i<10);
		return i;
	};

	//	Network operations
	virtual bool start( size_t nIndex ) {
		if (nIndex<10) {
			_isLocked = true;
			return true;
		} else {
			return false;
		}
	}

	virtual void stop() {
		_isLocked = false;
	}

	//	Status/signal
	virtual bool getStatus( tuner::status::Type &st ) const {
		st.isLocked = _isLocked;
		st.snr = 0;
		st.signal = 0;
		return true;
	}

private:
	bool _isLocked;
};

class TestProvider : public tuner::Provider {
public:
	TestProvider() {
		_frontend=NULL;
		_myDisp = new util::task::DispatcherImpl();
		setDispatcher( _myDisp );
	}

	virtual ~TestProvider() {
		delete _myDisp;
	}

	tuner::Frontend * frontend() {
		return _frontend;
	}

protected:
	virtual tuner::Frontend *createFrontend() const {
		_frontend = new TestFrontend();
		return _frontend;
	}

	virtual tuner::Filter *createFilter( tuner::pes::FilterParams * /*params*/ ) { return NULL; }
	virtual tuner::Filter *createFilter( tuner::PSIDemuxer * /*demux*/ ) { return NULL; }

private:
	util::task::DispatcherImpl *_myDisp;
	static tuner::Frontend *_frontend;
};

tuner::Frontend *TestProvider::_frontend;
