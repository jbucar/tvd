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

#include "types.h"
#include "../buffer.h"
#include "../log.h"
#include "../assert.h"
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>

namespace util {
namespace netlib {

namespace fs = boost::filesystem;

void parseHeaders( Buffer *headers, Headers &result ) {
	DTV_ASSERT(headers);

	if (headers->length() == 0) {
		return;
	}

	//	Null terminated string ...
	std::string header( (const char *)headers->data(), headers->length() );

	boost::char_separator<char> sepa("\r\n");
	typedef boost::tokenizer<boost::char_separator<char> > LineTokenizer;
	LineTokenizer tok( header, sepa );
	for (LineTokenizer::iterator it=tok.begin(); it!=tok.end(); ++it) {
		std::string line = *it;
		size_t pos = line.find(":");
		if (pos != std::string::npos) {
			std::string field( line, 0, pos );
			boost::trim(field);

			std::string value = line.substr(pos+1);
			boost::trim(value);

			result[field] = value;
			LTRACE( "netlib", "line=%s\n\tfield=%s\n\tvalue=%s\n", line.c_str(), field.c_str(), value.c_str() );
		}
		else {
			//	TODO: multiline header .... or response code
		}
	}
}

std::string resolveName( const std::string &url, Buffer *buf ) {
	DTV_ASSERT(buf);
	std::string name;
	Headers headers;
	parseHeaders( buf, headers );
	if (!headers.empty() && headers.find( "Content-Disposition" ) != headers.end()) {
		//	From disposition header
		const std::string &field = headers["Content-Disposition"];
		size_t start = field.find("filename=\"");
		if (start != std::string::npos) {
			size_t end = field.rfind("\"");
			if (end != std::string::npos) {
				std::string tmp = field.substr(start+10,end-start-10);

				//	Handle escape character
				tmp.erase( std::remove(tmp.begin(), tmp.end(), '\\' ), tmp.end());

				if (fs::native(tmp)) {
					fs::path nativeFilename(tmp);
					name = nativeFilename.filename().string();
				}
			}
		}
	}

	if (name.empty()) {
		//	From url
		size_t start = url.find("://");
		start = (start != std::string::npos) ? start+3 : 0;
		std::string filename=url.substr(start);

		size_t end = filename.rfind("/"/*, url.size()-start*/);
		if (end != std::string::npos) {
			++end;
			if (end < filename.size()) {
				name = filename.substr(end);
			}
		}
	}

	return name;
}

std::string temporaryName( const std::string &temporaryDirectory /* ="" */ ) {
	fs::path fsTemp(temporaryDirectory);
 	fsTemp /= "util_netlib_file%%%%%%%%";

	//	Add temporary extension
	return fs::unique_path(fsTemp).string();
}

}
}
