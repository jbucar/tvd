/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "serverimpl.h"
#include "../canvas.h"
#include "../surface.h"
#include "../system.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <util/types.h>

namespace canvas {
namespace remote {

ServerImpl::ServerImpl( canvas::System *sys )
{
	_sys = sys;
	_surface = NULL;
}

ServerImpl::~ServerImpl()
{
	DTV_ASSERT(!_surface);
}

bool ServerImpl::init( int zIndex ) {
	//	Destroy existing surface
	if (_surface) {
		_sys->canvas()->destroy( _surface );
	}

	//	Create a new surface
	_surface = _sys->canvas()->createSurface( size() );
	if (!_surface) {
		return false;
	}

	//	Setup surface
	_surface->autoFlush(true);
	_surface->setColor(canvas::Color(0,0,0,0)); // Black, fully transparent
	_surface->setZIndex( zIndex );
	_surface->setVisible( false );
	_surface->setCompositionMode( composition::source );

	return true;
}

void ServerImpl::fin() {
	if (_surface) {
		_sys->canvas()->destroy( _surface );
	}
}

util::task::Dispatcher *ServerImpl::disp() const {
	return _sys->dispatcher();
}

void ServerImpl::enable( bool val ) {
	DTV_ASSERT(_surface);
	_surface->setVisible( val );
	flush();
}

void ServerImpl::clear() {
	DTV_ASSERT(_surface);
	_surface->clear();
}

void ServerImpl::setOpacity( util::BYTE opacity ) {
	DTV_ASSERT(_surface);
	_surface->setOpacity( opacity );
}

void ServerImpl::flush() {
	_sys->canvas()->flush();
}

void ServerImpl::render( const std::vector<canvas::Rect> &dirtyRegions, void *mem, size_t memSize ) {
	DTV_ASSERT(_surface);
	LDEBUG("remote::ServerImpl", "Render: dirty regions=%d", dirtyRegions.size());

	canvas::ImageData img;
	memset(&img,0,sizeof(canvas::ImageData));

	//	Setup data
	img.size = size();
	img.length = memSize;
	img.data = static_cast<unsigned char*>(mem);
	img.bitsPerPixel = 32;
	img.bytesPerPixel = 4;
	img.stride = img.size.w*4;
	img.dataOffset = 0;

	//	Create surface from data
	canvas::Surface *sur=_sys->canvas()->createSurface( &img );

	//	Blit into surface the remote canvas
	BOOST_FOREACH( const canvas::Rect &rect, dirtyRegions ) {
		_surface->blit( canvas::Point(rect.x,rect.y), sur, rect );
	}

	flush();
	DEL( sur );
}

}
}

