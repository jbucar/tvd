/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

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

#include "configregistrator.h"
#include "../../../src/cfg/configregistrator.h"
#include "../../../src/cfg/cfg.h"
#include <string>

REGISTER_INIT_CONFIG( tuner_provider ) {
	root().addNode( "provider" );
}

REGISTER_INIT_CONFIG( tuner_dep ) {
	std::string ip("192.168.0.66");
	root().addNode( "dep" ).addValue("ip","desc",ip);
}

REGISTER_INIT_CONFIG( tuner_provider_dvb ) {
	std::string ip("192.168.0.224");
	root().addNode( "dvb" ).addValue("ip","desc",ip);
}

REGISTER_INIT_CONFIG( tuner_provider_dvb_dep ) {
	root().addNode( "dep" ).addValue("on","desc",true);
}

REGISTER_INIT_CONFIG( tuner ) {
	root().addNode("tuner");
}

REGISTER_INIT_CONFIG( lib ) {
	root().addNode("lib").addValue("val","desc",false);
}

REGISTER_INIT_CONFIG( config ) {
	root().addNode("config").addNode("lib").addValue("val","desc",false);
	root()("config").addValue("configval","desc",false);
}

ConfigRegistrator::ConfigRegistrator()
{
}

ConfigRegistrator::~ConfigRegistrator() {
}

void ConfigRegistrator::SetUp( void ) {
	util::reg::init();
}

void ConfigRegistrator::TearDown( void ) {
	util::reg::fin();
}

TEST_F( ConfigRegistrator, register_node ) {
	ASSERT_TRUE( util::cfg::get().hasChild("tuner") );
}

TEST_F( ConfigRegistrator, register_fin ) {
	util::reg::fin();
	ASSERT_FALSE(util::cfg::get().hasChildren());
}

TEST_F( ConfigRegistrator, register_node_and_value ) {
	ASSERT_TRUE( util::cfg::get().hasChild("lib") );
	ASSERT_TRUE( util::cfg::get("lib").existsValue("val") );
}

TEST_F( ConfigRegistrator, with_deps ) {
	ASSERT_TRUE( util::cfg::get("tuner").hasChild("provider") );
	ASSERT_TRUE( util::cfg::get("tuner").hasChild("dep") );
	ASSERT_TRUE( util::cfg::getValue<std::string> ("tuner.dep.ip") == "192.168.0.66" );
	ASSERT_TRUE( util::cfg::get("tuner.provider").hasChild("dvb") );
	ASSERT_TRUE( util::cfg::getValue<std::string> ("tuner.provider.dvb.ip") == "192.168.0.224" );
	ASSERT_TRUE( util::cfg::getValue<bool> ("tuner.provider.dvb.dep.on") );
}

TEST_F(ConfigRegistrator, initAgain){
	ASSERT_THROW(util::reg::init(), std::runtime_error);
}
