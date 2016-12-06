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

#include <util/id/ident.h>
#include <util/netlib/types.h>
#include <boost/function.hpp>

namespace util {
	class Buffer;
}

namespace zapper {

class Zapper;

namespace fetcher {	//	Package fetcher

struct InfoS {
	std::string fileName;
	std::string description;
};
typedef struct InfoS Info;

typedef boost::function<void (Info *)> OnUpdateReceived;
typedef boost::function<void (unsigned long long,unsigned long long)> OnProgress;

class Fetcher {
public:
	explicit Fetcher( Zapper *zapper );
	virtual ~Fetcher();

	bool check( int vendorID, int productID, int version );
	void cancel();

	void steps( int steps );
	void destDirectory( const std::string &dir );
	void onUpdateReceived( const OnUpdateReceived &callback );
	void onProgress( const OnProgress &callback );

protected:
	struct FetcherInfoS {
		Info basic;
		std::string url;
		std::string md5;
		unsigned long long totalSize;
		util::Buffer *headers;
	};
	typedef struct FetcherInfoS FetcherInfo;

	//	Aux for check update
	bool requestCheckUpdate( const std::string &url );
	void checkUpdateFinished( bool result );

	//	Aux for download update
	void requestFetchUpdate();
	void fetchUpdateFinished( bool result );
	void downloadProgress( unsigned long long cant );

	//	Pem verification
	bool verifyPem( const std::string &filename );
	const std::string pemFile() const;

	util::netlib::Manager *netlib() const;

private:
	Zapper *_zapper;
	std::string _destDirectory;
	int _steps;
	OnUpdateReceived _onUpdateReceived;
	OnProgress _onProgress;

	util::Buffer *_bufCheck;
	util::id::Ident _reqID;
	FetcherInfo _info;
};

}	//	namespace pu
}

