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

#include "documentserializer.h"
#include "documenthandler.h"
#include "nodehandler.h"
#include "../assert.h"
#include <boost/foreach.hpp>
#include <sstream>

namespace util {
namespace xml {
namespace dom {

/**
 * Constructor base.
 */
DocumentSerializer::DocumentSerializer()
{
}

/**
 * Destructor base.
 */
DocumentSerializer::~DocumentSerializer()
{
}

/**
 * Procesa la declaración del documento xml.
 * @param docDecl La declaración del documento xml.
 */
void DocumentSerializer::processDeclaration( const std::string &docDecl ) {
	put( docDecl );
}

/**
 * Procesa un nodo de un documento xml.
 * @param node El nodo a procesar.
 * @param handler El handler para acceder al contenido del nodo.
 * @param level El nivel del nodo en el documento.
 * @param going Indica si se encuentra en el recorrido de ida (true) o de vuelta (false).
 */
void DocumentSerializer::processNode( Node node, NodeHandler *handler, int level, bool going ) {
	if (going) {
		// Open tag
		put( outputTag(node, handler, level*2, false) );

		// if is a leaf put text content else put "\n" for serializing children in next level
		const std::string &text = handler->hasChildren(node) ? "\n" : handler->textContent(node);
		put( text );
	} else {
		// Close tag
		int ident = handler->hasChildren(node) ? level*2 : 0;
		put( outputTag(node, handler, ident) );
	}
}

std::string DocumentSerializer::outputTag( Node node, NodeHandler *handler, int ident, bool isClose/*=true*/ ) {
	std::stringstream line;
	for (int i=0; i<ident; i++) {
		line << " ";
	}
	line << "<";
	if (isClose) {
		line << "/";
	}

	line << handler->tagName(node);

	if (!isClose && handler->hasAttributes(node)) {
		std::vector<Attribute> attrs;
		handler->attributes( node, attrs );
		line << " ";
		BOOST_FOREACH( Attribute attr, attrs ) {
			line << attr.name << "=\"" << attr.value << "\" ";
		}
	}

	line << ">";
	if (isClose) {
		line << std::endl;
	}
	return line.str();
}

StreamDocumentSerializer::StreamDocumentSerializer( std::ostream &out )
	: _output(out)
{
}

StreamDocumentSerializer::~StreamDocumentSerializer()
{
	_output.flush();
}

FileDocumentSerializer::FileDocumentSerializer( const std::string &filename )
{
	_file.open( filename.c_str(), std::ofstream::out );
}

FileDocumentSerializer::~FileDocumentSerializer()
{
	_file.flush();
	_file.close();
}

void StreamDocumentSerializer::put( const std::string &data ) {
	_output << data;
}

void FileDocumentSerializer::put( const std::string &data ) {
	_file << data;
}

}
}
}
