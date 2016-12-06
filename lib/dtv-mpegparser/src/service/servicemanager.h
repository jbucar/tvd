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

#include "types.h"
#include <boost/function.hpp>
#include <vector>

namespace util {
namespace task {
	class Dispatcher;
}
}

namespace tuner {

class Service;
class Extension;
class ResourceManager;
class PSIDemuxer;

namespace ca {
	class Conditional;
}

/**
 * Esta clase permite acceder a los servicios de un TS así también como administrar extensiones y filtros.
 */
class ServiceManager {
public:
	explicit ServiceManager( ResourceManager *resources );
	virtual ~ServiceManager();

	//	Extensions
	virtual void attach( Extension *extension );
	virtual void detach( Extension *extension );
	bool isAttached( Extension *ext ) const;

	//	Notifications
	typedef boost::function<void (void)> OnEndScan;
	void onEndScan( const OnEndScan &callback );

	//	Initialization/finalization
	virtual void initialize();
	virtual void finalize();

	//	Start/Stop service manager
	virtual void start( bool scan=false );
	virtual void stop();
	bool isRunning() const;

	//	Start/Stop services
	void startService( ID serviceID );
	void stopService( ID serviceID );
	void stopServices();
	bool isEnqueued( ID serviceID ) const;
	void removeQueued( ID serviceID );
	bool isRunning( ID serviceID ) const;
	Service *get( ID serviceID ) const;

	//	Getters
	ResourceManager *resMgr();
	ca::Conditional *ca() const;
	virtual bool isReady() const;

	/**
	 * @return El identificador del TS actual.
	 */
	virtual ID currentTS() const=0;

	/**
	 * @return El identificador de la NIT actual.
	 */
	virtual ID currentNit() const=0;

	/**
	 * @return El dispatcher utilizado por el ServiceManager.
	 */
	virtual util::task::Dispatcher *dispatcher()=0;

	//	Filters
	/**
	 * Crea un filtro a partir de @b demux y lo ejecuta, filtrando así las secciones correspondientes.
	 * @param demux Instancia de @c PSIDemuxer.
	 * @return true si pudo crear y ejecutar el filtro correctamente, false en caso contrario.
	 */
	virtual bool startFilter( PSIDemuxer *demux )=0;

	/**
	 * Crea un filtro a partir de los parametros indicados en @b params.
	 * @param params Estructura que contiene el identificador del flujo de datos, el tipo de PES, y el pipe para enviar los datos filtrados.
	 * @return true si pudo crear y ejecutar el filtro correctamente, false en caso contrario.
	 */
	virtual bool startFilter( pes::FilterParams *params )=0;

	/**
	 * Finaliza el filtro identificado por @b pid y lo destruye.
	 * @param pid Identificador del filtro a detener.
	 */
	virtual void stopFilter( ID pid )=0;

protected:
	//	Scan
	void notifyEndScan( bool allScanned=false );

	//	Ready state
	void expire();
	void setReady();

	//	Services
	typedef std::vector<Service *> Services;
	Services &services();
	void addService( Service *srv );
	Service *findService( ID service ) const;
	Service *findServiceByPID( ID pid );
	void startService( Service *srv );
	bool stopService( Service *srv );
	void setServiceReady( Service *srv );
	void expireService( Service *srv, bool needExpire=true );
	void serviceChanged( Service *srv );

private:
	ResourceManager *_resMgr;
	ca::Conditional *_ca;
	Services _services;
	std::vector<ID> _servicesQueued;
	std::vector<Extension *> _extensions;
	OnEndScan _onEndScan;
	bool _initialized;
	bool _running;
	bool _inScanMode;
	bool _inScan;
	bool _isReady;
};

}
