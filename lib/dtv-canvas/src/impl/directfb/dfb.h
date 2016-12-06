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

#pragma once

#include "../../color.h"
#include <directfb.h>

#define DFB_CHECK_EXT( err, str ) if (err != DFB_OK) { LWARN( "directfb", "%s: %s", str, DirectFBErrorString( err ) ); return false; }
#define DFB_CHECK_WARN( str ) if (err != DFB_OK) { LWARN( "directfb", "%s: %s", str, DirectFBErrorString( err ) ); }
#define DFB_CHECK_THROW( str ) if (err != DFB_OK) { LERROR( "directfb", "%s: %s", str, DirectFBErrorString(err) ); throw std::runtime_error( str ); }
#define DFB_CHECK( str ) DFB_CHECK_EXT( err, str )

#define DFB_DEL( obj ) 	if (obj) { obj->Release(obj); obj = NULL; }
#define DFB_DEL_SURFACE( s ) s->ReleaseSource(s); DFB_DEL( s )
#define DFB_DEL_FONT(f) if (f) { f->Dispose(f); f = NULL; }

namespace canvas {
namespace directfb {

Color getColorKey();
DFBSurfacePixelFormat getFormat();

}
}
