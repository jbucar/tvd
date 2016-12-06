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

#include "xmlparser.h"
#include "propertynode.h"
#include "propertyvalue.h"
#include "../log.h"
#include "../assert.h"
#include "../string.h"
#include "../xml/documenthandler.h"
#include "../xml/documentserializer.h"
#include "../xml/nodehandler.h"
#include <boost/foreach.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace util {
namespace cfg {

namespace impl {

bool pretendFilter( PropertyValue *p ) {
	return p->isModified() && p->isVisible();
}
bool defaultFilter( PropertyValue *p ) {
	return p->isVisible();
}
std::string pretendPrinter( PropertyValue *p ) {
	return p->asString();
}

std::string defaultPrinter( PropertyValue *p ) {
	return p->asStringDefault();
}

}

XmlParser::XmlParser()
{
	_currentNode = NULL;
	_document = xml::dom::DocumentHandler::create();
	if (!_document->initialize()) {
		throw std::runtime_error("[util::cfg::XmlParser] Fail in initialization");
	}
}

XmlParser::~XmlParser()
{
	if (_document) {
		_document->finalize();
		DEL(_document);
	}
}

void XmlParser::traverse( xml::dom::Node root, PropertyNode *tree ) {
	xml::dom::NodeHandler *nodeHnd = _document->handler();

	std::vector<xml::dom::Node> childrenList;
	nodeHnd->children(root, childrenList);

	BOOST_FOREACH( xml::dom::Node node, childrenList ) {
		const std::string &tagname = nodeHnd->tagName(node);
		if (nodeHnd->childrenCount(node) > 0) {
			// It is an internal node
			if (tree->hasChild( tagname )) {
				traverse( node, &( *tree )( tagname ) );
			} else {
				throw std::runtime_error( format("[util::cfg::XParser::traverse] Invalid node in configuration file: %s", tagname.c_str()));
			}
		} else {
			// It is a leaf
			if (tree->existsValue(tagname)) {
				tree->setStr( tagname, nodeHnd->textContent(node) );
			} else {
				throw std::runtime_error(format("[util::cfg::XParser::traverse] Invalid value in configuration file: %s", tagname.c_str()));
			}
		}
	}
}

bool XmlParser::pretend( std::ostream &output, PropertyNode *tree ) {
	_filter = &impl::pretendFilter;
	_printer = &impl::pretendPrinter;
	return print( output, tree );
}

bool XmlParser::defaults( std::ostream &output, PropertyNode *tree ) {
	_filter = &impl::defaultFilter;
	_printer = &impl::defaultPrinter;
	return print( output, tree );
}

void XmlParser::operator()( PropertyValue *val ) {
	if (( *_filter )( val )) {
		std::string tmp = val->name();
		if (tmp.find("(") != tmp.npos) {
			// Replace ilegal caracteres
			tmp.replace(tmp.find("("),1,"");
			tmp.replace(tmp.find(")"),1,"");
		}
		xml::dom::Node value = _document->createElement( tmp );
		_document->handler()->textContent( value, (*_printer)( val ) );
		_document->handler()->appendChild( _currentNode, value );
	}
}


void XmlParser::operator()( PropertyNode *node ) {
	xml::dom::NodeHandler *nodeHnd = _document->handler();

	if (!_currentNode) {
		_currentNode = _document->root();
	} else {
		xml::dom::Node tmpNode = _document->createElement(node->name());
		nodeHnd->appendChild(_currentNode, tmpNode);
		_currentNode = tmpNode;
	}

	// Visits its properties values
	node->visitValues( *this );

	// Visit its children nodes
	node->visitNodes( *this );

	if (nodeHnd->hasChildren(_currentNode)) {
		_currentNode = nodeHnd->parent( _currentNode );
	} else if (nodeHnd->hasParent( _currentNode )) {
		xml::dom::Node tmpNode = nodeHnd->parent( _currentNode );
		nodeHnd->remove(&_currentNode);
		_currentNode = tmpNode;
	}

}

bool XmlParser::print( std::ostream &output, PropertyNode *tree ) {
	_currentNode = NULL;
	_document->createDocument( tree->name() );

	// Traverse tree filling document with nodes and values
	( *this )( tree );

	// Serialize document to ostream in pre-order
	{
		xml::dom::StreamDocumentSerializer docSerializer( output );
		_document->traverse( docSerializer );
	}

	_document->close();

	return true;
}

void XmlParser::parse( const std::string &name, PropertyNode *tree ) {
	DTV_ASSERT(tree);

	if (_document->loadDocument(name)) {
		xml::dom::Node root = _document->root();
		if (tree->name() != _document->handler()->tagName(root)) {
			throw std::runtime_error( "Error occurred parsing xml file: The xml not match with de Config Tree" );
		}
		traverse( root, tree );
		_document->close();
	} else {
		throw std::runtime_error( "Error occurred parsing xml file" );
	}
}

}
}
