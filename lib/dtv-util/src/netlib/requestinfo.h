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

#include "types.h"
#include <vector>

namespace util {
namespace netlib {

class RequestInfo {
public:
	RequestInfo();
	~RequestInfo();

	//	Callback
	void onFinished( const OnFinished &callback );
	void onProgress( unsigned int lowMarker, const ProgressCallback &callback );

	//	Getters/setters
	void url( const std::string &url );
	const std::string &url() const;

	void verbose( bool verbose );
	bool verbose() const;

	void verifyPeer( bool verify );
	bool verifyPeer() const;

	void verifyHost( bool verify );
	bool verifyHost() const;

	void caCertificate( const std::string &ca_in_form_of_pem );
	const std::string &caCertificate() const;

	void headers( const std::vector<std::string> &headers );
	const std::vector<std::string> &headers() const;

	void writeHeaders( bool write );
	bool writeHeaders() const;

	void method( const std::string &req );
	const std::string method() const;

	void postFields( const std::string &fields );
	const std::string &postFields() const;

	void connectTimeout( int msTimeout );
	int connectTimeout() const;

	bool needProgress() const;
	unsigned int progressLowMarker() const;
	const ProgressCallback &progressCallback() const;

	//	Methods
	bool isValid() const;
	void dispatchFinished( const id::Ident &id, bool result );

private:
	std::string _url;
	std::string _ca;
	std::string _method;
	std::string _fields;
	int _timeout;
	bool _verbose;
	bool _writeHeaders;
	bool _verifyPeer;
	bool _verifyHost;
	std::vector<std::string> _headers;
	OnFinished _onFinished;

	ProgressCallback _progress;
	unsigned int _progressLowMarker;
};

}
}

