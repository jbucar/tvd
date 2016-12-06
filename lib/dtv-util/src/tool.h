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

#pragma once

#include "main.h"
#include "cfg/cfg.h"
#include "cfg/cmd/commandline.h"
#include <string>

namespace util {

/**
 * La responsabilidad de esta clase es iniciar la ejecución de la aplicación usando la configuración leída del archivo XML.
 * Contiene información como la versión, una descripción y nombre. Se deberá sub-clasificar y re implementar el método run.
 */
class Tool {
public:
	Tool();
	virtual ~Tool();

	void initialize( const ToolConfig &cfg );
	int execute(int argc, char *argv[]);

	const std::string &name() const;
	const std::string &version() const;
	const std::string &description() const;
	const std::string &build() const;
	const std::string &config() const;

	void pretend() const;
	void defaults() const;

protected:
	virtual int execute( cfg::cmd::CommandLine &cmd );
	virtual int run( cfg::cmd::CommandLine &cmd );

	//	Configuration hook methods
	virtual void init();
	virtual void registerProperties( cfg::cmd::CommandLine &cmd );

	template<typename T>
	const T &getCfg( const std::string &property );

	template<typename T>
	void registerProp( const std::string &prop, const char sname,
			   const std::string &desc, T val, cfg::cmd::CommandLine &cmd, bool visible = true);
	//	Register property without a short name
	template<typename T>
	void registerProp( const std::string &prop, const std::string &desc, T val, 
		cfg::cmd::CommandLine &cmd, bool visible = true );
	template<typename T>
	void registerCfg( const std::string &prop, const std::string &desc, T val, bool visible );

	void setVersion( const std::string &ver );
	virtual void printVersion() const;
	virtual void printBuild() const;
	virtual void printConfig() const;
	virtual void set() const;

private:
	std::string _name;
	std::string _desc;
	std::string _version;
	std::string _build;
	std::string _config;
};

namespace impl {
void registerOpt( cfg::cmd::CommandLine &cmd, const std::string &owner, const std::string &prop, bool visible, char sname = 0 );

template<typename T>
void registerCfg( const std::string &owner, const std::string &prop, const std::string &desc, T val, bool visible ) {
	cfg::get(owner).addValue(prop, desc, val);
	if (!visible) {
		cfg::get(owner).makeInvisible(prop);
	}
}

template<typename T>
void registerProp( const std::string &owner, const std::string &prop, const char sname, const std::string &desc, T val, cfg::cmd::CommandLine &cmd, bool visible ) {
	registerCfg(owner, prop, desc, val, visible);
	registerOpt(cmd, owner, prop, visible, sname);
}

template<typename T>
void registerProp( const std::string &owner, const std::string &prop, const std::string &desc, T val, cfg::cmd::CommandLine &cmd, bool visible ) {
	registerCfg(owner, prop, desc, val, visible);
	registerOpt(cmd, owner, prop, visible);
}
}

template<typename T>
void Tool::registerProp( const std::string &prop, const char sname, const std::string &desc, T val, cfg::cmd::CommandLine &cmd, bool visible ) {
	impl::registerProp( "tool." + name(), prop, sname, desc, val, cmd, visible );
}

template<typename T>
void Tool::registerProp( const std::string &prop, const std::string &desc, T val, cfg::cmd::CommandLine &cmd, bool visible ) {
	impl::registerProp( "tool." + name(), prop, desc, val, cmd, visible );
}

template<typename T>
void Tool::registerCfg( const std::string &prop, const std::string &desc, T val, bool visible ) {
	impl::registerCfg( "tool." + name(), prop, desc, val, visible );
}

template<typename T>
const T &Tool::getCfg( const std::string &property ) {
	return cfg::getValue<T> ( "tool." + name() + "." + property );
}

}
