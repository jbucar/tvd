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

#include "layermanager.h"
#include "layer.h"
#include <util/log.h>
#include <util/assert.h>
#include <boost/foreach.hpp>

namespace canvas {

LayerManager::LayerManager()
{
}

LayerManager::~LayerManager()
{
	DTV_ASSERT(_layers.empty());
}

bool LayerManager::initialize() {
	bool ret = true;
	BOOST_FOREACH( Layer *layer, _layers ) {
		ret &= layer->initialize();
	}
	return ret;
}

void LayerManager::finalize() {
	BOOST_FOREACH( Layer *layer, _layers ) {
		layer->finalize();
		delete layer;
	}
	_layers.clear();
}

Layer *LayerManager::getLayer( size_t layer ) const {
	DTV_ASSERT( layer < _layers.size() );
	return _layers[layer];
}

void LayerManager::addLayer( Layer *layer ) {
	DTV_ASSERT( layer );
	_layers.push_back( layer );
}

bool LayerManager::setupLayers() {
	return true;
}

}

