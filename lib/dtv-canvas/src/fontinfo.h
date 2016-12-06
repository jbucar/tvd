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

#include <string>
#include <vector>

#define DEFAULT_FONT "Tiresias"

namespace canvas {

namespace font {
namespace family {
typedef std::vector<std::string> Type;
}
}

/**
 * Esta clase agrupa información de una fuente. Maneja parámetros como familia, tamaño, bold e italic.
 */
class FontInfo {
public:
	FontInfo();
	FontInfo( const font::family::Type &families, size_t size, bool bold=false, bool italic=false, bool smallCaps=false );
	FontInfo( const char *familySeparetedByComa, size_t size, bool bold=false, bool italic=false, bool smallCaps=false );
	FontInfo( const FontInfo &other );
	virtual ~FontInfo();
	
	void families( const font::family::Type &families );
	void families( const std::string &families );
	void addFamily( const std::string &family );
	std::string familiesAsString() const;
	const font::family::Type& families() const;

	void fixDejaVuFont();
	
	void size( size_t size );
	size_t size() const;
	
	void bold( bool enable );
	bool bold() const;
	
	void italic( bool enable );
	bool italic() const;
	
	void smallCaps( bool enable );
	bool smallCaps() const;

	bool operator==(const FontInfo &font) const;
	bool operator!=(const FontInfo &font) const;
	bool operator<(const FontInfo &font) const;

	void show() const;

private:
	font::family::Type _families;
	size_t _size;
	bool _bold;
	bool _italic;
	bool _smallCaps;
};

}

