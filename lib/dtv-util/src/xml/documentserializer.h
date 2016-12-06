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
#include <iostream>
#include <fstream>
#include <vector>

namespace util {
namespace xml {
namespace dom {

class DocumentSerializer {
public:
	DocumentSerializer();
	virtual ~DocumentSerializer();

	void processDeclaration( const std::string &docDecl );
	void processNode( Node node, NodeHandler *handler, int level, bool going );

protected:
	std::string outputTag( Node node, NodeHandler *handler, int ident, bool isClose=true );

	virtual void put( const std::string &data )=0;
};

class StreamDocumentSerializer : public DocumentSerializer {
public:
	explicit StreamDocumentSerializer( std::ostream &output );
	StreamDocumentSerializer( const StreamDocumentSerializer &other ) : _output(other._output) {}
	virtual ~StreamDocumentSerializer();

protected:
	virtual void put( const std::string &data );

private:
	std::ostream &_output;
	StreamDocumentSerializer();
	StreamDocumentSerializer& operator=( const StreamDocumentSerializer &other );
};

class FileDocumentSerializer : public DocumentSerializer {
public:
	explicit FileDocumentSerializer( const std::string &filename );
	virtual ~FileDocumentSerializer();

protected:
	virtual void put( const std::string &data );

private:
	std::ofstream _file;
};

}
}
}

