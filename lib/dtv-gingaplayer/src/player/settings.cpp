/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "settings.h"
#include "keys.h"
#include "lua/settings/settings.h"
#include <connector/gingaversion.h>
#include <canvas/color.h>
#include <util/log.h>
#include <util/string.h>
#include <util/cfg/cfg.h>
#include <util/cfg/configregistrator.h>
#include <boost/algorithm/string.hpp>
#include <stdio.h>

REGISTER_INIT_CONFIG(gingaPlayer) {
	root().addNode( "gingaPlayer" );
}

REGISTER_INIT_CONFIG(gingaPlayer_system) {
	std::string ver = "Ginga.ar " + connector::gingaVersion();

	util::cfg::PropertyNode &systemNode = root().addNode( "system" );
	systemNode.addValue( "language", "", "es" )
			.addValue( "caption", "", "es" )
			.addValue( "subtitle", "", "es" )
			.addValue( "returnBitRate(0)", "", "" )
			.addValue( "audioType", "", "stereo" )
			.addValue( "audioType(0)", "", "stereo" )
			.addValue( "devNumber(0)", "", "1" )
			.addValue( "classType", "", "active" )
			.addValue( "info(0)", "", "" )
			.addValue( "classNumber", "", "1" )
			.addValue( "CPU", "", "" )
			.addValue( "memory", "", "" )
			.addValue( "operatingSystem", "", "" )
			.addValue( "luaVersion", "", "5.1" )
			.addValue( "luaSupportedEventClasses", "", "tcp,user,ncl,key")
			.addValue( "nclVersion", "NCL language version", "3.0" )
			.addValue( "gingaNCLProfile", "Language profile supported by the receiver", "Enhanced" )
			.addValue( "gingaNCLVersion", "Ginga version", ver );
	systemNode.addNode("ncl").addValue( "version", "NCL language version", "3.0" );
	systemNode.addNode("GingaNCL").addValue( "version", "Ginga version", ver );
}

REGISTER_INIT_CONFIG(gingaPlayer_user) {
		root().addNode( "user" )
			.addValue( "age", "", "" )
			.addValue( "location", "", "" )
			.addValue( "genre", "", "" );
}

REGISTER_INIT_CONFIG(gingaPlayer_service) {
	root().addNode( "service" )
			.addValue( "currentFocus", "", 0 )
			.addValue( "currentKeyMaster", "", "" );
}

REGISTER_INIT_CONFIG(gingaPlayer_si) {
	root().addNode( "si" )
			.addValue( "numberOfServices", "", 0 )
			.addValue( "numberOfPartialServices", "", 0 )
			.addValue( "channelNumber", "", 0 );
}

REGISTER_INIT_CONFIG(gingaPlayer_channel) {
	root().addNode( "channel" )
		.addValue( "keyCapture", "", "" )
		.addValue( "virtualKeyboard", "", "" )
		.addValue( "keyboardBounds", "", "" );
}

static bool checkFloat( const std::string &value, float &f ) {
	std::string v = value;
	boost::trim( v );
	bool percentual = v.find( "%" ) == v.size() - 1;
	if (percentual) {
		boost::trim_right_if( v, boost::is_any_of( "%" ) );
	}
	try {
		f = boost::lexical_cast<float>( v );
	} catch (boost::bad_lexical_cast &) {
		return false;
	}
	f = percentual ? f/100 : f;
	return true;
}


namespace player {
namespace settings {

typedef struct {
	prefix::type type;
	const char *name;
} PrefixType;

#define DO_ENUM_PREFIX_NAMES(t, s) { prefix::t, s },
static PrefixType prefixes[] = {
	{prefix::unknown, "unknown"},
	PREFIX_LIST(DO_ENUM_PREFIX_NAMES)
	{prefix::LAST, NULL}
};
#undef DO_ENUM_PREFIX_NAMES

void load( void ) {
	util::cfg::PropertyNode &gpNode = util::cfg::get("gingaPlayer");

	gpNode.addNode( "global" );
	gpNode.addNode( "shared" );

	char tmp[50];

	snprintf( tmp, sizeof(tmp), "(%d, %d)",
			util::cfg::getValue<int>("gui.window.size.width"),
			util::cfg::getValue<int>("gui.window.size.height") );
	std::string windowSize = std::string( tmp, 10 );

	snprintf( tmp, sizeof(tmp), "(%d, %d)",
				util::cfg::getValue<int>("gui.canvas.size.width"),
				util::cfg::getValue<int>("gui.canvas.size.height") );
	std::string canvasSize = std::string( tmp, 10 );

	util::cfg::PropertyNode &sys = util::cfg::get( "gingaPlayer.system" );
	sys.addValue( "screenSize", "", windowSize );
	sys.addValue( "screenSize(0)", "", windowSize );
	sys.addValue( "screenGraphicSize", "", canvasSize );
	sys.addValue( "screenGraphicSize(0)", "", canvasSize );

	util::cfg::PropertyNode &dft = gpNode.addNode("default");
	dft.addValue( "focusBorderColor", "", "white" );
	dft.addValue( "selBorderColor", "", "white" );
	dft.addValue( "focusBorderWidth", "", -3 );
	dft.addValue( "focusBorderTransparency", "", "0" );

	gpNode.addNode( "settings" ).addValue( "focusBorderTransparencyAsFloat", "", 0.0f );

	// Reserve all keys by default. If app define channel.keyCapture this will be overridden.
	player::keys::reserveKeys( "interactive_keys, numeric_keys" );
}

void unload( void ) {
	util::cfg::PropertyNode &gpNode = util::cfg::get("gingaPlayer");
	gpNode.removeNode( "global" );
	gpNode.removeNode( "shared" );
	util::cfg::PropertyNode &sys = util::cfg::get( "gingaPlayer.system" );
	sys.removeProp( "screenSize" );
	sys.removeProp( "screenSize(0)" );
	sys.removeProp( "screenGraphicSize" );
	sys.removeProp( "screenGraphicSize(0)" );
	gpNode.removeNode("default");
	gpNode.removeNode( "settings" );
}

typedef struct
{
	prefix::type prefix;
	std::string prefixName;
	std::string varName;
} Property;

Property property( const std::string &value ) {

	Property p;
	p.prefix = prefix::unknown;
	p.varName = "";

	std::vector<std::string> tokens;
	boost::split( tokens, value, boost::is_any_of( "." ) );

	p.prefixName = tokens[0];
	if (tokens.size() == 2) {
		p.varName = tokens[1];
	}

	int i = 0;
	while (prefixes[i].name) {
		if (tokens[0] == prefixes[i].name) {
			p.prefix = prefixes[i].type;
		}
		i++;
	}
	return p;
}

namespace impl {
static std::vector<Module *> luaListeners;
}

void addListener( Module *module ) {
	impl::luaListeners.push_back( module );
}

void delListener( Module *module ) {
	std::vector<Module *>::iterator it = std::find( impl::luaListeners.begin(), impl::luaListeners.end(), module );
	if (it != impl::luaListeners.end()) {
		impl::luaListeners.erase( it );
	}
}

void updateListeners() {
	BOOST_FOREACH( Module *m, impl::luaListeners ) {
		m->update();
	}
}

void setProperty( const std::string &name, const std::string &value, bool init /*=false*/) {

	LDEBUG("Player::settings", "set property, name=%s, value='%s'", name.c_str(), value.c_str());

	Property p = property( name );

	switch ( p.prefix ) {
		case prefix::unknown: {
			if (init) {
				util::cfg::get( "gingaPlayer.global" ).addValue( name, "", value );
			} else {
				util::cfg::get( "gingaPlayer.global" ).set( name, value );
			}
			break;
		}
		case prefix::system:
		case prefix::user:
		case prefix::si: {
			if (value != "") {
				LERROR("Player::settings", "%s is a read only group of environment variables", p.prefixName.c_str());
			}
			break;
		}
		case prefix::shared: {
			if (init) {
				if (!p.varName.empty()) {
					util::cfg::get( "gingaPlayer.shared" ).addValue( p.varName, "", value );
				}
			} else {
				util::cfg::setValue( std::string("gingaPlayer.")+name, value );
			}
			updateListeners();
			break;
		}
		case prefix::defaults: {
			if (!value.empty()) {
				if (p.varName == "focusBorderColor" || p.varName == "selBorderColor") {
					canvas::Color tmp;
					if (canvas::color::get( value.c_str(), tmp )) {
						util::cfg::setValue( std::string("gingaPlayer.")+name, value );
					} else {
						LWARN("Player::settings", "invalid %s, value=%s", p.varName.c_str(), value.c_str());
					}
				} else if (p.varName == "focusBorderWidth") {
					util::cfg::get().setStr( std::string("gingaPlayer.")+name, value );
				} else if (p.varName == "focusBorderTransparency") {
					float tmp = 0.0;
					if (checkFloat( value, tmp )) {
						util::cfg::setValue( "gingaPlayer.settings.focusBorderTransparencyAsFloat", tmp );
						util::cfg::setValue( std::string("gingaPlayer.")+name, value );
					}
				}
				updateListeners();
			}
			break;
		}
		case prefix::service: {
			if (!value.empty()) {
				if (p.varName == "currentFocus") {
					util::cfg::setValue( std::string("gingaPlayer.")+name, boost::lexical_cast<int>( value ) );
				} else {
					util::cfg::setValue( std::string("gingaPlayer.")+name, value );
				}
				updateListeners();
			}
			break;
		}
		case prefix::channel: {
			if (!value.empty()) {
				if (p.varName == "keyCapture") {
					keys::reserveKeys( value );
				}
				util::cfg::setValue( std::string("gingaPlayer.")+name, value );
				updateListeners();
			}
			break;
		}
		default:
			break;
	}
}

const std::string getProperty( const std::string &name ) {

	Property p = property( name );
	std::string value = "";
	switch ( p.prefix ) {
		case prefix::unknown: {
			value = util::cfg::get( "gingaPlayer.global" ).get<std::string>( name );
			break;
		}
		default: {
			value = util::cfg::get().asString( std::string("gingaPlayer.")+name );
			break;
		}
	};

	LDEBUG("Player::settings", "get var, name=%s, value=%s", name.c_str(), value.c_str());
	return value;
}


}
}
