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

#include "window.h"
#include "system.h"
#include "surface.h"
#include "dfb.h"
#include "../../canvas.h"
#include <util/assert.h>
#include <util/mcr.h>
#include <util/log.h>
#include <util/cfg/cfg.h>
#include <boost/foreach.hpp>

namespace canvas {
namespace directfb {

Window::Window( IDirectFB *dfb )
	: _dfb( dfb )
{
	_layer = NULL;
	_surface = NULL;
	_drawBuffer = (DrawBuffer)util::cfg::getValue<unsigned long>("gui.directfb.drawBuffer");
	LINFO( "directfb::Window", "Using drawBuffer method: %08ld", _drawBuffer );
}

Window::~Window()
{
}

//	Initialization
bool Window::init() {
	//	Get layer
	DFBResult err = _dfb->GetDisplayLayer( _dfb, DLID_PRIMARY, &_layer );
	DFB_CHECK( "[canvas::directfb::Window] GetDisplayLayer" );

	//	Set cooperative level to layer
	err = _layer->SetCooperativeLevel(_layer, DLSCL_ADMINISTRATIVE);
	DFB_CHECK( "[canvas::directfb::Window] SetCooperativeLevel" );		

	//	Set background color
	Color colorKey = getColorKey();
	err = _layer->SetBackgroundColor(
		_layer,
		colorKey.r, colorKey.g, colorKey.b, colorKey.alpha
	);
	DFB_CHECK( "[canvas::directfb::Window] SetBackgroundColor" );

	//	Setup basic video
	initSize();
	if (!setUpConfig( size() )) {
		LERROR("directfb::Window", "Fail to setup window configuration");
		return false;
	}

	err = _layer->EnableCursor( _layer, 0 );
	DFB_CHECK( "[canvas::directfb::Window] EnableCursor" );

	return true;
}
 
void Window::fin() {
	DFB_DEL(_layer);
}

bool Window::setUpConfig( const Size &size ) {
	DFBDisplayLayerConfig cfg;
	DFBResult err = _layer->GetConfiguration( _layer, &cfg);
	DFB_CHECK( "[canvas::directfb::Window] GetConfiguration" );

	cfg.flags = (DFBDisplayLayerConfigFlags)(DLCONF_BUFFERMODE | DLCONF_WIDTH | DLCONF_HEIGHT);
	cfg.buffermode = DLBM_BACKVIDEO;
	cfg.width  = size.w;
	cfg.height = size.h;

	{	//	Setup alpa channel, opacity if supported
		DFBDisplayLayerDescription desc;
		err=_layer->GetDescription( _layer, &desc );
		DFB_CHECK( "[canvas::directfb::Window] GetDescription" );

		if (desc.caps & DLCAPS_ALPHACHANNEL) {
			cfg.flags = (DFBDisplayLayerConfigFlags)(cfg.flags | DLCONF_OPTIONS);
			cfg.options = (DFBDisplayLayerOptions)(cfg.options | DLOP_ALPHACHANNEL);
		}

		if (desc.caps & DLCAPS_OPACITY) {
			cfg.flags = (DFBDisplayLayerConfigFlags)(cfg.flags | DLCONF_OPTIONS);
			cfg.options = (DFBDisplayLayerOptions)(cfg.options | DLOP_OPACITY);
		}
	}

	err = _layer->SetConfiguration( _layer, &cfg );
	DFB_CHECK( "[canvas::directfb::Window] SetConfiguration" );

	return true;
}

//	Layer methods
bool Window::initLayer( canvas::Canvas *canvas ) {
	const Size &size = canvas->size();
	_surface = new Surface( (directfb::Canvas *)canvas, Rect(0,0,size.w,size.h), true );
	return true;
}

void Window::finLayer( canvas::Canvas * /*canvas*/ ) {
	DEL(_surface);
}

canvas::Surface *Window::lockLayer() {
	DTV_ASSERT(_surface);
	return _surface;
}

void Window::renderLayerImpl( canvas::Surface *surface, const std::vector<Rect> &dirtyRegions ) {
	DFBResult err;
	DTV_ASSERT( _surface == surface );
	UNUSED(surface);

	IDirectFBSurface *sur = _surface->getContent();
	BOOST_FOREACH( const Rect &rect, dirtyRegions ) {
		DFBRegion r;
		r.x1 = (rect.x<0) ? 0 : rect.x;
		r.y1 = (rect.y<0) ? 0 : rect.y;
		r.x2 = rect.x+rect.w-1;
		r.y2 = rect.y+rect.h-1;

		err=sur->Flip( sur, &r, (DFBSurfaceFlipFlags)(0) );
		DFB_CHECK_WARN( "[canvas::directfb::Window] Flip" );
	}

	if (_drawBuffer) {
		unsigned char *buffer;
		int pitch;
		const Size s=_surface->getSize();
		err=sur->Lock( sur, DSLF_READ, (void **)&buffer, &pitch );
		DFB_CHECK_WARN( "[canvas::directfb::Window] Cannot lock screen surface" );
		(*_drawBuffer)( buffer, pitch*s.h );
		sur->Unlock( sur );
	}
}

Size Window::screenSize() const {
	DFBResult err;
	IDirectFBScreen *screen;

	err = _layer->GetScreen( _layer, &screen );
	DFB_CHECK_WARN( "[canvas::directfb::Window] Cannot get screen size" );
	Size s;
	err = screen->GetSize( screen, &s.w, &s.h );
	DFB_CHECK_WARN( "[canvas::directfb::Window] Cannot get screen size" );
	return s;
}

void Window::setSizeImpl( const Size &size ) {
	if (!setUpConfig( size )) {
		LWARN( "directfb::Window", "Cannot set window size" );
	}
}

bool Window::supportFullScreen() const {
	return true;
}

void Window::fullScreenImpl( bool enable ) {
	DFBResult err;
	if (enable) {
		err = _dfb->SetCooperativeLevel( _dfb, DFSCL_FULLSCREEN );
		DFB_CHECK_WARN( "[canvas::directfb::Window] Cannot set fullScreen mode" );
	} else {
		err = _dfb->SetCooperativeLevel( _dfb, DFSCL_NORMAL );
		DFB_CHECK_WARN( "[canvas::directfb::Window] Cannot set normal mode" );
	}
}

}
}

