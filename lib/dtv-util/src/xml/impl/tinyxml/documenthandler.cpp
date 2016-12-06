/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "documenthandler.h"
#include "nodehandler.h"
#include "../../../log.h"
#include "../../../mcr.h"
#include "../../../assert.h"
#include "../../../buffer.h"
#include <tinyxml.h>

namespace util {
namespace xml {
namespace dom {
namespace tinyxml {

DocumentHandler::DocumentHandler()
{
}

DocumentHandler::~DocumentHandler()
{
}

Document DocumentHandler::loadXMLImpl( const std::string &xml ) {
	TiXmlDocument *doc = new TiXmlDocument();
	const char *pTest = doc->Parse( xml.c_str(), 0, TIXML_ENCODING_UTF8 );
	if (!pTest && doc->Error()) {
		LERROR("tinyxml::DocumentHandler", "Fail to load document from string. Reason: %s", doc->ErrorDesc());
		DEL(doc);
	}
	return (Document) doc;
}

Document DocumentHandler::loadXMLImpl( const Buffer *buf ) {
	TiXmlDocument *doc = new TiXmlDocument();
	const char *pTest = doc->Parse( (const char *)buf->data(), 0, TIXML_ENCODING_UTF8 );
	if (!pTest && doc->Error()) {
		LERROR("tinyxml::DocumentHandler", "Fail to load document from string. Reason: %s", doc->ErrorDesc());
		DEL(doc);
	}
	return (Document) doc;
}

Document DocumentHandler::loadDocumentImpl( const std::string &file ) {
	TiXmlDocument *doc = new TiXmlDocument( file.c_str() );
	if (!doc->LoadFile()) {
		LERROR("tinyxml::DocumentHandler", "Fail to load document from file: '%s:%d:%d'. Reason: %s", file.c_str(), doc->ErrorRow(), doc->ErrorCol(), doc->ErrorDesc());
		DEL(doc);
	}
	return (Document) doc;
}

Document DocumentHandler::createDocumentImpl( const std::string &rootElement ) {
	TiXmlDocument *doc = new TiXmlDocument();
	TiXmlDeclaration *decl = new TiXmlDeclaration( "1.0", "UTF-8", "yes" );
	doc->LinkEndChild( decl );
	TiXmlElement *element = new TiXmlElement( rootElement.c_str() );
	doc->LinkEndChild( element );
	return static_cast<Document>(doc);
}

bool DocumentHandler::saveImpl( Document doc, const std::string &file ) {
	LTRACE("tinyxml::DocumentHandler", "Save document to file: '%s'", file.c_str());
	return static_cast<TiXmlDocument*>(doc)->SaveFile( file.c_str() );
}

bool DocumentHandler::saveImpl( Document doc, Buffer *buf ) {
	LTRACE("tinyxml::DocumentHandler", "Save document to buffer");
	TiXmlPrinter printer;
	TiXmlDocument *tDoc = static_cast<TiXmlDocument*>(doc);
	tDoc->Accept(&printer);
	// Copy size+1 to include str ending null
	buf->copy(printer.CStr(), printer.Size()+1);
	return true;
}

bool DocumentHandler::closeImpl( Document doc ) {
	TiXmlDocument *tiDoc = static_cast<TiXmlDocument*>(doc);
	tiDoc->Clear();
	DEL(tiDoc);
	return true;
}

Node DocumentHandler::rootImpl( Document doc ) const {
	TiXmlElement *element = static_cast<TiXmlDocument*>(doc)->RootElement();
	return static_cast<Node>(element);
}

Node DocumentHandler::createTextNodeImpl( Document /*doc*/, const std::string &text ) {
	TiXmlText *textNode = new TiXmlText( text.c_str() );
	return static_cast<Node>(textNode);
}

Node DocumentHandler::createElementImpl( Document /*doc*/, const std::string &tagname ) {
	TiXmlElement *element = new TiXmlElement( tagname.c_str() );
	return static_cast<Node>(element);
}

}
}
}
}
