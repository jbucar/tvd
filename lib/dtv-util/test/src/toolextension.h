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

#include <iostream>
#include <gtest/gtest.h>
#include "../../src/registrator.h"
#include "../../src/tool.h"
#include "../../src/cfg/cmd/commandline.h"
#include "../../src/cfg/propertynode.h"


class ToolExtension : public util::Tool {
public:
	ToolExtension();
	virtual ~ToolExtension();

	void setup( const std::string &name, const std::string &desc, const std::string &version = "1.0.0", const std::string &build = "0" );
	void pretend();
	void defaults();
	int exe( int argc, char *argv[] );
	util::cfg::PropertyTree &getCfg();
	std::string getBufAsString() const;

protected:
	virtual void registerProperties( util::cfg::cmd::CommandLine &cmd );

	template <typename T>
	void registerProp( const std::string &prop,const char /*sname*/, const std::string &desc, T val, util::cfg::cmd::CommandLine /*&cmd*/ );

	template <typename T>
	void registerProp( const std::string &prop, const std::string &desc, T val, util::cfg::cmd::CommandLine /*&cmd*/ );

	private:
	std::stringbuf *_buf;
	std::ostream *_anOstream;
	util::cfg::PropertyTree _cfg;

};

template <typename T>
void ToolExtension::registerProp( const std::string &prop, const std::string &desc, T val, util::cfg::cmd::CommandLine /*&cmd*/ ) {
	getCfg().addValue(prop, desc, val);
}

template <typename T>
void ToolExtension::registerProp( const std::string &prop,const char /*sname*/, const std::string &desc, T val, util::cfg::cmd::CommandLine /*&cmd*/ ) {
	getCfg().addValue(prop, desc, val);
}

class ToolExtensionTest: public testing::Test{
public:
	ToolExtensionTest();
	virtual ~ToolExtensionTest();
protected:
	virtual void SetUp( void );
	virtual void TearDown( void );

	char **_params;
	std::list<util::reg::Registrator *> _list;
	ToolExtension *_tool;
};

