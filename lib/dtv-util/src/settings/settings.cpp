/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "settings.h"
#include "../log.h"
#include "../assert.h"
#include "../cfg/configregistrator.h"
#include "generated/config.h"
#if SETTINGS_USE_BDB
#	include "bdb/settings.h"
#endif
#if SETTINGS_USE_XML
#	include "xml/settings.h"
#endif
#if SETTINGS_USE_MEMORY
#	include "memory/settings.h"
#endif

namespace util {

REGISTER_INIT_CONFIG( settings ) {
	root().addNode( "settings" )
#if SETTINGS_USE_XML
		.addValue( "use", "Settings to use", std::string("xml") )
#elif SETTINGS_USE_BDB
		.addValue( "use", "Settings to use", std::string("bdb") )
#elif SETTINGS_USE_MEMORY
		.addValue( "use", "Settings to use", std::string("memory") )
#endif
	;
}

Settings *Settings::create( const std::string &name, const std::string &useParam ) {
	Settings *settings = NULL;
	const std::string &use = useParam.empty() ? util::cfg::getValue<std::string>("settings.use") : useParam;
	LINFO( "Settings", "Using settings: use=%s", use.c_str() );

#if SETTINGS_USE_BDB
	if (use == "bdb") {
		settings = new bdb::Settings(name);
	}
#endif
#if SETTINGS_USE_XML
	if (use == "xml") {
		settings = new xml::Settings(name);
	}
#endif
#if SETTINGS_USE_MEMORY
	if (use == "memory") {
		settings = new memory::Settings(name);
	}
#endif
	DTV_ASSERT(settings);
	return settings;
}

Settings::Settings( const std::string &name )
	: _name(name)
{
}

Settings::~Settings()
{
}

//	Initialization/finalization
bool Settings::initialize() {
	return true;
}

void Settings::finalize() {
}

bool Settings::commit() {
	return true;
}

const std::string &Settings::name() const {
	return _name;
}

void Settings::put( const std::string &key, const char *value ) {
	put( key, std::string(value) );
}

}
