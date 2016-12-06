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
#include "generated/config.h"
#if CANVAS_FONTMANAGER_USE_FONTCONFIG
#	include "impl/fontconfig/fontmanager.h"
#endif
#include "impl/dummy/fontmanager.h"
#include <util/log.h>
#include <util/cfg/configregistrator.h>

namespace canvas {

REGISTER_INIT_CONFIG( gui_fontManager ) {
	root().addNode( "fontManager" )
#if CANVAS_FONTMANAGER_USE_FONTCONFIG
	.addValue( "use", "FontManager to instance", std::string("fontconfig") )
#else
	.addValue( "use", "FontManager to instance", std::string("dummy") )
#endif
	;
}

/**
 * Crea un @c FontManager utilizando la implementación seteada en el árbol de configuración (ver módulo dtv-util).
 * @return Una nueva instancia de @c FontManager.
 */
FontManager *FontManager::create() {
	const std::string &use = util::cfg::getValue<std::string>("gui.fontManager.use");

	LINFO("FontManager", "Using FontManager: %s", use.c_str());
#if CANVAS_FONTMANAGER_USE_FONTCONFIG
	if (use == "fontconfig") {
		return new fontconfig::FontManager();
	}
#endif
	return new dummy::FontManager();
}

/**
 * Constructor base.
 * Para crear instancias de FontManager utilizar create().
 */
FontManager::FontManager()
{
}

/**
 * Destructor base.
 */
FontManager::~FontManager()
{
}

/**
 * Inicializa el estado de la instancia.
 * Este método debe ser llamado antes de cualquier otro.
 * @return true si la inicialización finalizó correctamente, false en caso contrario.
 */
bool FontManager::initialize() {
	return true;
}

/**
 * Finaliza el estado de la instancia.
 * Este método debe ser llamado antes de destruir la misma.
 */
void FontManager::finalize() {
}

bool FontManager::addFontDirImpl( const std::string & /*dir*/ ) {
	return false;
}

/**
 * Agrega el directorio indicado en @b dir para buscar fuentes.
 * @param dir La ruta completa del directorio a utilizar para la busqueda de fuentes.
 */
void FontManager::addFontDirectory( const std::string &dir ) {
	if (addFontDirImpl( dir )) {
		//	Clean cache
		_cache.clear();
	}
	else {
		LWARN( "FontManager", "Font manager backend does not have support for add a custom directory" );
	}
}

bool FontManager::clearFontDirImpl() {
	return false;
}

/**
 * Deja de buscar fuentes en los directorios agregados mediante addFontDirectory().
 */
void FontManager::clearFontDirectories() {
	_cache.clear();
	if (!clearFontDirImpl()) {
		LWARN( "FontManager", "Font manager backend does not have support for clear custom directories" );
	}
}

/**
 * Busca el archivo de fuente que mejor mapee con los valores indicados en @b font.
 * @param fontInfo Especificación de la fuente a buscar.
 * @return La ruta absoluta al archivo de fuente, o un string vacío si no encontro una fuente apropiada.
 * @see FontInfo
 */
const std::string &FontManager::findFont( const FontInfo &fontInfo ) {
	//	Try insert a new key with invalid data
	std::pair<FontCache::iterator,bool> res = _cache.insert( FontCache::value_type(fontInfo, "") );
	std::string &value = res.first->second;

	//	If key not found -> find in backend
	if (res.second) {
		value = findFontImpl( fontInfo );
	}

	//	Check if empty
	if (value.empty()) {
		LWARN("FontManager", "Couldn't find any font: familes=%s",
			fontInfo.familiesAsString().c_str() );
	}

	return value;
}

}
