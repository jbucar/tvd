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

#include "xml.h"
#include "../log.h"
#include "../assert.h"
#include <boost/filesystem.hpp>

namespace util {
namespace serialize {

namespace fs = boost::filesystem;

XML::XML( const std::string &file )
	: _filename(file), _buf(NULL)
{
	_document = xml::dom::DocumentHandler::create();
	if (!_document) {
		LERROR("xml", "Cannot create XML DOM provider");
		throw std::runtime_error( "Cannot create XML DOM provider" );
	}

	_currentNode = NULL;
	_save = false;
}

XML::XML( Buffer *buffer )
	: _filename(""), _buf(buffer)
{
	_document = xml::dom::DocumentHandler::create();
	if (!_document) {
		LERROR("xml", "Cannot create XML DOM provider");
		throw std::runtime_error( "Cannot create XML DOM provider" );		
	}

	_currentNode = NULL;
	_save = false;
}

XML::~XML()
{
	DEL(_document);
}

bool XML::create() {
	if (!_document->initialize()) {
		LERROR("xml", "Cannot initialize XML DOM provider");
		return false;
	}
	
	_document->createDocument("root");
	_currentNode = _document->root();
	return true;
}

bool XML::open() {
	if (!_document->initialize()) {
		LERROR("xml", "Cannot initialize XML DOM provider");
		return false;
	}

	bool result = false;
	if (_buf) {
		result = _document->loadXML(_buf);
	}
	else if (!_filename.empty()) {
		result = _document->loadDocument(_filename);
	}

	if (result) {
		_currentNode = _document->root();
		result = true;
	} else {
		_document->finalize();
	}

	return result;
}

bool XML::save( const std::vector<std::string> &data ) {
	bool result = false;
	if (create()) {
		_save = true;
		marshall( "Items", data );
		result = saveImpl();
	}
	return result;
}

bool XML::saveImpl() {
	bool result=false;
	if (_buf) {
		result = _document->save(_buf);
	}
	else if (!_filename.empty()) {
		result = _document->save(_filename);
	}
	_document->finalize();
	return result;
}

bool XML::load( std::vector<std::string> &data ) {
	bool result = false;
	if (open()) {
		_save = false;
		unmarshall( "Items", data );
		_document->finalize();
		result = true;
	}
	return result;
}

//	strings
void XML::marshall( const std::string &key, const std::string &data ) {
	util::xml::dom::Node node = _document->createElement( key );
	_document->handler()->appendChild( _currentNode, node );
	_document->handler()->textContent(node, data);	
}

void XML::unmarshall( const std::string &key, std::string &data ) {
	util::xml::dom::Node node = _document->handler()->child(_currentNode, key);
	if (node) {
		data = _document->handler()->textContent(node);
	}
}

//	unsigned char
void XML::marshall( const std::string &key, const unsigned char data ) {
	marshall( key, (int)data );
}

void XML::unmarshall( const std::string &key, unsigned char &data ) {
	int tmp=0;
	unmarshall( key, tmp );
	data = static_cast<unsigned char>(tmp);
}

//	char
void XML::marshall( const std::string &key, const char data ) {
	marshall( key, (int)data );
}

void XML::unmarshall( const std::string &key, char &data ) {
	int tmp=0;
	unmarshall( key, tmp );
	data = static_cast<char>(tmp);	
}

//	bool
void XML::marshall( const std::string &key, const bool &data ) {
	std::string val = data ? "true" : "false";
	marshall( key, val );
}

void XML::unmarshall( const std::string &key, bool &data ) {
	std::string val;
	unmarshall( key, val );
	data = (val == "true");
}

// vector<std::string>
void XML::marshall( const std::string &key, const std::vector<std::string> &data ) {
	util::xml::dom::Node parent = _currentNode;
	_currentNode = _document->createElement( key );
	_document->handler()->appendChild( parent, _currentNode );

	//	Add each item
	for (size_t i=0; i<data.size(); i++) {
		marshall("item", data[i]);
	}
	_currentNode = parent;
}

void XML::unmarshall( const std::string &key, std::vector<std::string> &data ) {
	data.clear();
	util::xml::dom::Node vecNode = _document->handler()->child(_currentNode, key);
	if (vecNode) {
		std::vector<xml::dom::Node> children;
		_document->handler()->children(vecNode, children);
		for (size_t i=0; i<children.size(); i++) {
			data.push_back( _document->handler()->textContent(children[i]) );
		}
	} else {
		LWARN("serializer::XML", "Fail to unmarshall vector. Key %s does not exist", key.c_str());
	}
}

}
}
