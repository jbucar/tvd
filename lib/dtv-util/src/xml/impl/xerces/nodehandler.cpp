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

#include "nodehandler.h"
#include "errorhandler.h"
#include "../../../log.h"
#include "../../../assert.h"
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <stdexcept>

namespace util {
namespace xml {
namespace dom {
namespace xerces {

NodeHandler::NodeHandler()
{
}

NodeHandler::~NodeHandler()
{
}

void NodeHandler::children( Node node, std::vector<Node> &children ) {
	xercesc::DOMNodeList *domList = static_cast<xercesc::DOMNode*>(node)->getChildNodes();
	XMLSize_t size = domList->getLength();
	for (XMLSize_t i = 0; i<size; ++i) {
		xercesc::DOMNode *currentNode = domList->item( i );
		if (currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			children.push_back( domList->item(i) );
		}
	}
}

void NodeHandler::appendChild( Node node, Node child ) {
	try {
		((xercesc::DOMElement*)node)->appendChild( (xercesc::DOMElement*)child );
	} catch( const xercesc::DOMException &except ) {
		LERROR("NodeHandler::xerces", "Can't append child: %s", stdString(except.msg).c_str() );
	}
}

void NodeHandler::remove( Node *node ) {
	xercesc::DOMNode *child = static_cast<xercesc::DOMNode*>(*node);
	xercesc::DOMNode *parent = child->getParentNode();
	try {
		parent->removeChild(child)->release();
		*node = NULL;
	}
	catch( const xercesc::DOMException &except ) {
		LERROR("NodeHandler::xerces", "Can't remove node: %s", stdString(except.msg).c_str() );
	}
}

bool NodeHandler::hasParent( Node node ) {
	return parent( node ) != NULL;
}

Node NodeHandler::parent( Node node ) {
	xercesc::DOMElement *xercesNode = static_cast<xercesc::DOMElement*>(node);
	xercesc::DOMNode *parent = xercesNode->getParentNode();
	if (parent && (parent->getNodeType()!=xercesc::DOMNode::ELEMENT_NODE)) {
		parent = NULL;
	}
	return (Node) parent;
}

const std::string &NodeHandler::tagName( Node node ) {
	return stdString( static_cast<xercesc::DOMElement*>(node)->getTagName() );
}

const std::string &NodeHandler::textContent( Node node ) {
	return stdString( static_cast<xercesc::DOMNode*>(node)->getTextContent() );
}

void NodeHandler::textContent( Node node, const std::string &text ) {
	XMLCh *tmp = xercesc::XMLString::transcode(text.c_str());
	static_cast<xercesc::DOMNode*>(node)->setTextContent(tmp);
	xercesc::XMLString::release(&tmp);
}

bool NodeHandler::hasAttributes( Node node ) {
	return static_cast<xercesc::DOMNode*>(node)->hasAttributes();
}

void NodeHandler::attributes( Node node, std::vector<Attribute> &attribs ) {
	xercesc::DOMNamedNodeMap *attrs = static_cast<xercesc::DOMNode*>(node)->getAttributes();
	if (attrs) {
		XMLSize_t length = attrs->getLength();
		for (XMLSize_t pos=0; pos<length; pos++) {
			xercesc::DOMAttr *attr = static_cast<xercesc::DOMAttr*>(attrs->item(pos));
			Attribute tmpAt( stdString(attr->getName()), stdString(attr->getValue()) );
			attribs.push_back( tmpAt );
		}
	}
}

bool NodeHandler::hasAttribute( Node node, const std::string &attribute ) {
	XMLCh *xstr = xercesc::XMLString::transcode(attribute.c_str());
	bool res = static_cast<xercesc::DOMElement*>(node)->hasAttribute(xstr);
	xercesc::XMLString::release(&xstr);
	return res;
}

const std::string &NodeHandler::attribute( Node node, const std::string &name ) {
	XMLCh *xstr = xercesc::XMLString::transcode(name.c_str());
	const XMLCh *attr = static_cast<xercesc::DOMElement*>(node)->getAttribute(xstr);
	xercesc::XMLString::release(&xstr);
	const std::string &tmp = stdString(attr);
	return tmp;
}

void NodeHandler::setAttribute( Node node, const std::string &name, const std::string &value ) {
	XMLCh *xstrName = xercesc::XMLString::transcode(name.c_str());
	XMLCh *xstrVal = xercesc::XMLString::transcode(value.c_str());
	static_cast<xercesc::DOMElement*>(node)->setAttribute( xstrName, xstrVal );
	xercesc::XMLString::release(&xstrName);
	xercesc::XMLString::release(&xstrVal);
}

void NodeHandler::removeAttribute( Node node, const std::string &name ) {
	XMLCh *xstr = xercesc::XMLString::transcode(name.c_str());
	static_cast<xercesc::DOMElement*>(node)->removeAttribute( xstr );
	xercesc::XMLString::release(&xstr);
}

std::string &NodeHandler::stdString( const XMLCh *key ) {
	std::pair<StrCache::iterator,bool> res = _strCache.insert( StrCache::value_type(key, "") );
	std::string &value = res.first->second;

	//	If tagName not found -> transcode it
	if (res.second) {	
		char *tagStr = xercesc::XMLString::transcode( key );
		value = std::string( tagStr );
		xercesc::XMLString::release( &tagStr );
	}
	return value;
}

void NodeHandler::clearCache() {
	_strCache.clear();
}

}
}
}
}

