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

#include "fontinfo.h"
#include <string>
#include <map>

namespace canvas {

/**
 * La clase FontManager provee funcionalidad para la busqueda de fuentes en el sistema.
 */
class FontManager {
public:
	FontManager();
	virtual ~FontManager();

	virtual bool initialize();
	virtual void finalize();

	//	Add/clear search directories
	void addFontDirectory( const std::string &dir );
	void clearFontDirectories();
	
	const std::string &findFont( const FontInfo &font );

	static FontManager *create();

protected:
	typedef std::map<FontInfo, std::string> FontCache;

	virtual bool addFontDirImpl( const std::string &dir );
	virtual bool clearFontDirImpl();
	virtual std::string findFontImpl( const FontInfo &font )=0;

private:
	FontCache _cache;
};

}
