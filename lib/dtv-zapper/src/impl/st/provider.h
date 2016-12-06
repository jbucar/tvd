/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "st.h"
#include <mpegparser/provider/provider.h>
#include <util/types.h>
#include <boost/thread.hpp>	

namespace st {

class Filter;

class Provider : public tuner::Provider
{
public:
	Provider( void );
	virtual ~Provider();

	STPTI_Handle_t pti() const;
	STPTI_Signal_t signal() const;	
	void addPID( STPTI_Buffer_t buf, Filter * );
	void rmPID( STPTI_Buffer_t buf );

protected:
	//	Types
	typedef std::map<STPTI_Buffer_t,Filter *> Filters;
	friend class PSIFilter;
	friend class PESFilter;

	//	Tuner methods
	virtual tuner::Frontend *createFrontend() const;

	//	Aux filters
	virtual tuner::Filter *createFilter( tuner::PSIDemuxer *sectionDemux );
	virtual tuner::Filter *createFilter( tuner::pes::FilterParams *params );
	virtual bool startEngineFilter( void );
	virtual void stopEngineFilter( void );
	virtual int maxFilters() const;
	virtual bool haveTimers() const;	

	//	Filters aux
	void reader();
	
private:
	boost::thread _thread;
	boost::mutex  _mutex;
	Filters _pids;
	STPTI_Handle_t _ptiHandle;
	STPTI_Signal_t _ptiSignal;
};

}
