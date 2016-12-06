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
#include "errorhandler.h"
#include "../../../buffer.h"
#include "../../../log.h"
#include "../../../mcr.h"
#include "../../../assert.h"
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace util {
namespace xml {
namespace dom {
namespace xerces {

namespace bfs = boost::filesystem;

DocumentHandler::DocumentHandler()
{
	_domParser = NULL;
	_errHnd = NULL;
}

DocumentHandler::~DocumentHandler()
{
}

bool DocumentHandler::init() {
	try {
		xercesc::XMLPlatformUtils::Initialize();
	} catch (const xercesc::XMLException &toCatch) {
		char *message = xercesc::XMLString::transcode( toCatch.getMessage() );
		LERROR("DocumentHandler::xerces", "during initialization: %s", message );
		xercesc::XMLString::release( &message );
		return false;
	}
	_domParser = new xercesc::XercesDOMParser();
	_errHnd = new ErrorHandler();
	_domParser->setErrorHandler( _errHnd );

	return true;
}

void DocumentHandler::fin() {
	if (_domParser) {
		_domParser->resetDocumentPool();
		DEL(_domParser);
	}
	DEL(_errHnd);

	xercesc::XMLPlatformUtils::Terminate();
}

Document DocumentHandler::doLoad( const xercesc::InputSource &source ) {
	_domParser->parse( source );
	if (_domParser->getErrorCount() != 0) {
		LERROR( "DocumentHandler::xerces", "Error parsing xml file: Source file cannot be open" );
		return NULL;
	}
	return (Document)(_domParser->adoptDocument());
}

Document DocumentHandler::loadXMLImpl( const std::string &xml ) {
	xercesc::MemBufInputSource xmlBuf((const XMLByte*)xml.c_str(), xml.length(), "xml");
	return doLoad(xmlBuf);
}

Document DocumentHandler::loadXMLImpl( const Buffer *buf ) {
	xercesc::MemBufInputSource xmlBuf( (const XMLByte*)buf->data(), buf->length(), "xml");
	return doLoad(xmlBuf);
}

Document DocumentHandler::loadDocumentImpl( const std::string &file ) {
	if (!bfs::exists(file)) {
		LWARN( "DocumentHandler::xerces", "Fail to load document. File '%s' does not exist", file.c_str());
		return NULL;
	}
	XMLCh *str = xercesc::XMLString::transcode( file.c_str() );
	xercesc::LocalFileInputSource source( str );
	xercesc::XMLString::release( &str );

	return doLoad(source);
}

Document DocumentHandler::createDocumentImpl( const std::string &rootElement ) {
	XMLCh *core = xercesc::XMLString::transcode( "LS" );
	xercesc::DOMImplementation *domImpl = xercesc::DOMImplementationRegistry::getDOMImplementation( core );
	xercesc::XMLString::release(&core);

	XMLCh *tempStrNameRoot = xercesc::XMLString::transcode( rootElement.c_str() );
	xercesc::DOMDocument *xercesDoc = domImpl->createDocument( NULL, tempStrNameRoot, NULL );
	xercesc::XMLString::release(&tempStrNameRoot);
	return (Document) xercesDoc;
}

bool DocumentHandler::saveImpl( Document doc, const std::string &file ) {
	xercesc::DOMDocument *xercesDoc = static_cast<xercesc::DOMDocument*>(doc);

	XMLCh *core = xercesc::XMLString::transcode( "LS" );
	xercesc::DOMImplementation *domImpl = xercesc::DOMImplementationRegistry::getDOMImplementation( core );
	xercesc::XMLString::release(&core);

	xercesc::DOMLSSerializer *serializer = domImpl->createLSSerializer();
	XMLCh *lfSeq = xercesc::XMLString::transcode("\n");
	serializer->setNewLine(lfSeq);
	xercesc::XMLString::release(&lfSeq);
	serializer->getDomConfig()->setParameter( xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true );

	xercesDoc->normalizeDocument();
	XMLCh* filename = xercesc::XMLString::transcode(file.c_str());
	serializer->writeToURI(xercesDoc, filename);
	xercesc::XMLString::release(&filename);
	serializer->release();

	return true;
}

bool DocumentHandler::saveImpl( Document doc, Buffer *buf ) {
	xercesc::DOMDocument *xercesDoc = static_cast<xercesc::DOMDocument*>(doc);

	XMLCh *core = xercesc::XMLString::transcode( "LS" );
	xercesc::DOMImplementation *domImpl = xercesc::DOMImplementationRegistry::getDOMImplementation( core );
	xercesc::XMLString::release(&core);

	xercesc::DOMLSSerializer *serializer = domImpl->createLSSerializer();
	XMLCh *lfSeq = xercesc::XMLString::transcode("\n");
	serializer->setNewLine(lfSeq);
	xercesc::XMLString::release(&lfSeq);
	serializer->getDomConfig()->setParameter( xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true );

	bool result = false;
	xercesDoc->normalizeDocument();
	XMLCh *tmp = serializer->writeToString(xercesDoc);
	if (tmp) {
		char *tmpStrC = xercesc::XMLString::transcode( tmp );
		std::string tmpStr(tmpStrC);
		xercesc::XMLString::release(&tmpStrC);

		boost::replace_first(tmpStr, "UTF-16", "UTF-8");
		boost::replace_first(tmpStr, "no", "yes");
		boost::replace_first(tmpStr, "xmlns=\"\"", "");
		buf->copy(tmpStr.c_str(), tmpStr.length()+1);
		result = true;
	}
	xercesc::XMLString::release(&tmp);
	serializer->release();

	return result;
}

bool DocumentHandler::closeImpl( Document doc ) {
	static_cast<xercesc::DOMDocument*>(doc)->release();
	_domParser->resetDocumentPool();
	return true;
}

Node DocumentHandler::rootImpl( Document doc ) const {
	return (( xercesc::DOMDocument* )doc)->getDocumentElement();
}

Node DocumentHandler::createTextNodeImpl( Document doc, const std::string &text ) {
	xercesc::DOMDocument *xercesDoc = static_cast<xercesc::DOMDocument*>(doc);

	XMLCh *textContent = xercesc::XMLString::transcode(text.c_str());
	xercesc::DOMText *xercesTextNode = xercesDoc->createTextNode( textContent );
	xercesc::XMLString::release(&textContent);
	return (Node) xercesTextNode;
}

Node DocumentHandler::createElementImpl( Document doc, const std::string &tagname ) {
	xercesc::DOMDocument *xercesDoc = static_cast<xercesc::DOMDocument*>(doc);

	XMLCh *name = xercesc::XMLString::transcode(tagname.c_str());
	xercesc::DOMNode *xercesNode = xercesDoc->createElement( name );
	xercesc::XMLString::release(&name);
	return (Node) xercesNode;
}

}
}
}
}
