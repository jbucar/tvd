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

#include "../psifilter.h"
#include "../../../types.h"
#include <boost/function.hpp>

namespace tuner {

class ResourceManager;

namespace dsmcc {

class DSI;
class DII;
class Module;
class DSMCCDemuxer;

class DSMCCFilter : public PSIFilter {
public:
	explicit DSMCCFilter( DSMCCDemuxer *demux );
	virtual ~DSMCCFilter();

	//	Methods
	void filterDSI();
	void filterDII();
	void filterModules( const boost::shared_ptr<DII> &dii );

	//	Getters
	ResourceManager *resourceMgr();

	//	On DSMCC progress changed
	typedef boost::function<void ( int step, int totalSteps )> OnProgressChanged;
	void onProgressChanged( const OnProgressChanged &callback );

protected:
	//	Tables virtual methods
	virtual void processDSI( const boost::shared_ptr<DSI> &dsi );
	virtual void processDII( const boost::shared_ptr<DII> &dii );	
	virtual void processModule( const boost::shared_ptr<Module> &module );

	//	Aux
	void notify( int step );
	void onDSI( const boost::shared_ptr<DSI> &dsi );
	void onDII( const boost::shared_ptr<DII> &dii );	
	void onModule( const boost::shared_ptr<Module> &module );
	void onBlock( size_t bytes );	
	DSMCCDemuxer *demux() const;

private:
	DSMCCDemuxer *_demux;
	OnProgressChanged _onProgress;
	int _totalSize;
	int _progress;
};

}
}

