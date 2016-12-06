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

#include "layer.h"
#include "serverimpl.h"
#include <util/mcr.h>

namespace canvas {
namespace remote {

Layer::Layer( System *sys, int id, int zIndex )
	: canvas::Layer( id, zIndex, false )
{
	_render = new ServerImpl( sys );
}

Layer::~Layer()
{
	delete _render;
}

void Layer::finalize() {
	_render->finalize();
}

void Layer::mode( mode::type mode ) {
	//	Save size
	Mode &modeInfo = mode::get( mode );
	const Size size = Size(modeInfo.width,modeInfo.height);
	_render->initialize( size, zIndex() );
}

const std::vector<canvas::mode::type> Layer::supportedModes() const {
	std::vector<mode::type> tmp;
	tmp.push_back( mode::m576p_50 );	//	SD
	return tmp;
}

void Layer::setEnable( bool state ) {
	_render->enable( state );
}

void Layer::clear() {
	_render->clear();
}

void Layer::flush() {
	_render->flush();
}

bool Layer::setAlpha( util::BYTE alpha ) {
	_render->setOpacity( alpha );
	return true;
}

}
}
