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

#include "captionviewer.h"
#include <canvas/surface.h>
#include <canvas/canvas.h>
#include <canvas/system.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/bind.hpp>

#define CAPTION_LAYER 100

namespace zapper {

CaptionViewer::CaptionViewer( canvas::System *sys, int layer )
	: _sys(sys),
	  _bgColor(0,0,0,255),
	  _textColor(255,255,255,255),
	  _origin(150,450),
	  _size(420,75),
	  _layer(CAPTION_LAYER+layer)
{
	_surface = NULL;
}

CaptionViewer::~CaptionViewer()
{
	DTV_ASSERT(!_surface);
}

void CaptionViewer::enable( bool state) {
	LDEBUG( "zapper", "Enable CC: state=%d", state );
	if (state) {
		_surface = _sys->canvas()->createSurface(canvas::Rect(_origin, _size));
		_surface->setZIndex( _layer );
		_surface->setVisible(false);
		_surface->setColor(_bgColor);
		_surface->fillRect(canvas::Rect(canvas::Point(0,0), _size));
		_surface->setFont(canvas::FontInfo("Tiresias", 16));
		_surface->autoFlush(true);
	}
	else {
		_sys->canvas()->destroy(_surface);
		_sys->canvas()->flush();
	}
}

void CaptionViewer::show( const std::string &data ) {
	LDEBUG("zapper", "CaptionViewer show data: %s", data.c_str() );
	_surface->setVisible( true );
	_surface->clear();
	_surface->setColor(_textColor);
	_surface->drawText(canvas::Rect(5,5,410,65), data, canvas::wrapByWord );
	_sys->canvas()->flush();
}

} //	namespace zapper

