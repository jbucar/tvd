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

#include "font.h"
#include "dfb.h"
#include <util/log.h>
#include <cstdlib>

namespace canvas {
namespace directfb {

Font::Font( const std::string &filename, size_t size, IDirectFB *dfb )
	: canvas::Font(filename, size)
{
	_dfb = dfb;
	_dfbFont = NULL;
}

Font::~Font() {
	if (_dfbFont) {
		DFB_DEL_FONT(_dfbFont);
	}
	_dfb = NULL;
}

bool Font::initialize() {
	DFBFontDescription fDesc;
	fDesc.flags = (DFBFontDescriptionFlags)(DFDESC_HEIGHT);
	fDesc.height = size();
	
	DFBResult result = _dfb->CreateFont(_dfb, filename().c_str(), &fDesc, &_dfbFont );
	if (result==DFB_OK) {
		LTRACE("directfb::Font", "Using font %s", filename().c_str());
	}

	int asc, desc, h;
	_dfbFont->GetAscender( _dfbFont, &asc);
	_dfbFont->GetDescender( _dfbFont, &desc);
	_dfbFont->GetHeight(_dfbFont, &h);
	ascent(asc);
	descent(abs(desc));
	height(h);

	for (int i=0; i<256; i++) {
		unsigned int c = (unsigned int)(i&0xFF);
		int	adv;
		DFBRectangle rect;
		if (_dfbFont->GetGlyphExtents( _dfbFont, c, &rect, &adv ) == DFB_OK) {
			Glyph *glyph = new Glyph(0, adv, (unsigned char)c);
			putGlyph((unsigned char)c, glyph);
		} else {
			LTRACE( "directfb::Font", "Glyph %d not present in charset", i );
		}
	}

	return true;
}

IDirectFBFont *Font::dfbFont() const {
	return _dfbFont;
}

}
}
