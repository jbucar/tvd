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
#include <util/log.h>
#include <util/mcr.h>
#include <boost/math/special_functions.hpp>
#include <QRawFont>

namespace canvas {
namespace qt {

Font::Font( const std::string &filename, size_t size )
	: canvas::Font(filename, size)
{
	_rawFont = NULL;
}

Font::~Font()
{
	DEL(_rawFont);
}

bool Font::initialize() {
	_rawFont = new QRawFont(QString(filename().c_str()), size());
	ascent( boost::math::iround(_rawFont->ascent()) );
	descent( boost::math::iround(_rawFont->descent())+1 );
	height( boost::math::iround(_rawFont->ascent()+_rawFont->descent()+std::abs(_rawFont->leading()))+1 );

	for (int i=0; i<256; i++) {
		quint32 idx;
		int numGlyphs;
		char c = (char)(i&0xFF);
		QChar qc(c);
		if (_rawFont->glyphIndexesForChars(&qc, 1, &idx, &numGlyphs)) {
			QPointF adv;
			if (!_rawFont->advancesForGlyphIndexes(&idx, &adv, 1)) {
				LWARN("qt::Font", "Fail to get glyph advance");
			}
			putGlyph(c, new Glyph(idx, boost::math::iround(adv.x())));
		} else {
			LTRACE( "qt::Font", "Glyph %d not present in charset", i );
		}
	}
	return true;
}

QRawFont *Font::qtFont() const {
	return _rawFont;
}

}
}
