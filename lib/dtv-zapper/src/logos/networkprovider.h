/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include <pvr/logos/provider.h>

namespace zapper {

class Zapper;

namespace fetcher {
	struct InfoS;
	typedef struct InfoS Info;
	class Fetcher;
}

namespace logos {
namespace network {

class Provider : public pvr::logos::Provider {
public:
	explicit Provider( Zapper *zapper );
	virtual ~Provider();

	virtual bool find( pvr::Channel *ch, std::string &logoFile, int &priority );

protected:
	virtual bool doStart();
	virtual void doStop();
	virtual util::Settings *settings() const;

	//	Implementation
	class XmlInfo;

	bool parse();
	const std::string cfgFile() const;
	const std::string cfgDir() const;
	void onUpdateReceived( fetcher::Info *info );

private:
	Zapper *_zapper;
	fetcher::Fetcher *_update;
	XmlInfo *_info;
};

}
}
}


