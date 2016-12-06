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

#include "../../src/cfg/xmlparser.h"
#include "../../src/cfg/cfg.h"
#include "toolextension.h"
#include "functions.h"
#include "../../src/cfg/cfg.h"

ToolExtension::ToolExtension() : Tool()
{
	_buf = new std::stringbuf;
	_anOstream = new std::ostream(_buf);
}

ToolExtension::~ToolExtension(){
	delete _anOstream;
	delete _buf;
}

void ToolExtension::setup( const std::string &name, const std::string &desc, const std::string &version, const std::string &build ) {
	util::ToolConfig cfg;
	cfg.name = name;
	cfg.description = desc;
	cfg.version = version;
	cfg.repo_version = build;
	initialize(cfg);
}

void ToolExtension::registerProperties( util::cfg::cmd::CommandLine &cmd ) {
	getCfg().setName( name() );

	registerProp("help", 'h', "Display this message", false, cmd);

	registerProp("version", 'v', "Display tool version", false, cmd);
	registerProp("build", 'b', "Display build information", false, cmd);
	registerProp("defaults", "Show in xml format the configuration default values", false, cmd);
	registerProp("pretend", "Show in xml format the configuration pretend values", false, cmd);
}

void ToolExtension::pretend()  {
	util::cfg::XmlParser parser;
	parser.pretend(*_anOstream, &getCfg());
}

void ToolExtension::defaults() {
	util::cfg::XmlParser parser;
	parser.defaults(*_anOstream, &getCfg());
}

int ToolExtension::exe( int argc, char *argv[] ) {
	util::cfg::cmd::CommandLine cmd( argc, argv );
	registerProperties( cmd );
	return 1;
}

util::cfg::PropertyTree &ToolExtension::getCfg(){
	return _cfg;
}

std::string ToolExtension::getBufAsString() const{
	return _buf->str();
}
ToolExtensionTest::ToolExtensionTest()
{
}

ToolExtensionTest::~ToolExtensionTest()
{
}

void ToolExtensionTest::SetUp( void ){
	_tool = new ToolExtension();
	_tool->setup("genericTool","description","2.0.0","0.0.1");
	const char *argv[1] = {"programName"};
	_params = util::makeParams( argv, 1 );
	_tool->exe( 1, _params );
}

void ToolExtensionTest::TearDown( void ){
	util::cfg::get().removeProp("help");
	util::cfg::get().removeProp("version");
	util::cfg::get().removeProp("build");
	util::cfg::get().removeProp("defaults");
	util::cfg::get().removeProp("pretend");
	util::deleteParams( _params );
	delete (_tool);
}


TEST_F(ToolExtensionTest, defaults){
	std::string print;
	std::string mustPrint("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n<genericTool>\n  <help>false</help>\n  <version>false</version>\n  <build>false</build>\n  <defaults>false</defaults>\n  <pretend>false</pretend>\n</genericTool>\n");

	_tool->defaults();
	print = _tool->getBufAsString();
	ASSERT_TRUE(print.size() > 0);
	ASSERT_TRUE(mustPrint.compare(print) == 0);

}
TEST_F(ToolExtensionTest, pretend){
	std::string print;
	std::string mustPrint("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n<genericTool>\n  <pretend>true</pretend>\n</genericTool>\n");
	_tool->getCfg().setStr("pretend","true");
	_tool->pretend();
	print = _tool->getBufAsString();
	ASSERT_TRUE(print.size() > 0);
	ASSERT_TRUE(mustPrint.compare(print) == 0);
}
