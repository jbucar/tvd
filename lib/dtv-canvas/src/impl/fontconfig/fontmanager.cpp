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

#include "fontmanager.h"
#include <util/log.h>
#include <boost/foreach.hpp>
#include <fontconfig/fontconfig.h>
#include <string>

namespace canvas {
namespace fontconfig {

FontManager::FontManager()
{
	_config = NULL;
}

FontManager::~FontManager()
{
}

bool FontManager::initialize() {
	_config = FcInitLoadConfigAndFonts();
	if (!_config) {
		LERROR( "fontconfig", "Cannot load config fonfs" );
		return false;
	}

	if (FcConfigSetCurrent(_config) == FcFalse) {
		FcConfigDestroy( _config );
		_config = NULL;
		
		LERROR( "fontconfig", "Cannot set current configuration" );
		return false;
	}

	return true;
}

void FontManager::finalize() {
	FcConfigDestroy( _config );
	_config = NULL;
	
	FcFini();
}

bool FontManager::addFontDirImpl( const std::string &dir ) {
	LDEBUG( "fontconfig", "Add font directory: dir=%s", dir.c_str() );
	
	if (FcConfigAppFontAddDir( _config, (const FcChar8 *)dir.c_str() ) == FcFalse) {
		LERROR( "fontconfig", "Cannot add directory to config database: dir=%s", dir.c_str() );
		return false;
	}

	if (FcConfigSetCurrent(_config) == FcFalse) {
		LERROR( "fontconfig", "Cannot set current configuration" );
		return false;
	}

	if (FcConfigBuildFonts(_config) == FcFalse) {
		LERROR( "fontconfig", "Cannot build fonts database" );
		return false;
	}

	return true;
}

bool FontManager::clearFontDirImpl() {
	LDEBUG( "fontconfig", "Clear font directories" );
	
	FcConfigAppFontClear( _config );

	if (FcConfigSetCurrent(_config) == FcFalse) {
		LERROR( "fontconfig", "Cannot set current configuration" );
		return false;
	}

	if (FcConfigBuildFonts(_config) == FcFalse) {
		LERROR( "fontconfig", "Cannot build fonts database" );
		return false;
	}

	return true;	
}

std::string FontManager::findFontImpl( const FontInfo &fontInfo ) {
	std::string filename;
	FcPattern *pattern = FcPatternCreate();
	if (pattern) {
		FcPatternAddInteger( pattern, FC_WEIGHT, fontInfo.bold() ? FC_WEIGHT_BOLD : FC_WEIGHT_REGULAR );
		FcPatternAddInteger( pattern, FC_SLANT, fontInfo.italic() ? FC_SLANT_ITALIC : FC_SLANT_ROMAN );

		//	Add all families
		BOOST_FOREACH( const std::string &family, fontInfo.families() ) {
			FcPatternAddString( pattern, FC_FAMILY, (FcChar8*)family.c_str() );
		}

		FcConfigSubstitute( _config, pattern, FcMatchPattern );
		FcDefaultSubstitute( pattern );

		FcResult res;
		FcPattern *match = FcFontMatch( _config, pattern, &res );
		if (match) {
			FcValue value;
			if (!FcPatternGet( match, FC_FILE, 0, &value )) {
				filename = std::string( (char*)(value.u.s) );
				LTRACE("fontconfig", "Found font %s", filename.c_str());
			}

			FcPatternDestroy( match );
		}

		FcPatternDestroy( pattern );
	}

	return filename;
}

}
}
