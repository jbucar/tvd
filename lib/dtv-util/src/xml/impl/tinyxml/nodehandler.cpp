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
#include "../../../log.h"
#include <tinyxml.h>
#include <boost/foreach.hpp>

namespace util {
namespace xml {
namespace dom {
namespace tinyxml {
  
NodeHandler::NodeHandler()
{
}

NodeHandler::~NodeHandler()
{
}

void NodeHandler::children( Node node, std::vector<Node> &childrens ) {
	TiXmlNode *tiNode = static_cast<TiXmlNode*>(node);
	for( TiXmlNode *child = tiNode->FirstChildElement(); child; child = child->NextSiblingElement() ) {
		childrens.push_back( static_cast<Node>(child) );
	}
}

void NodeHandler::appendChild( Node node, Node child ) {
	static_cast<TiXmlNode*>(node)->LinkEndChild( static_cast<TiXmlNode*>(child) );
}

void NodeHandler::remove( Node *node ) {
	TiXmlNode *child = static_cast<TiXmlNode*>(*node);
	TiXmlNode *parent = child->Parent();

	if (!parent->RemoveChild(child)) {
		LERROR("tinyxml::NodeHandler", "Fail to remove node");
	}
	*node = NULL;
}

const std::string &NodeHandler::tagName( Node node ) {
	return static_cast<TiXmlNode*>(node)->ValueStr();
}

const std::string &NodeHandler::textContent( Node node ) {
	TiXmlNode *tiNode = static_cast<TiXmlNode*>(node);
	const char *text=NULL;
	if (tiNode->Type() == TiXmlNode::TINYXML_ELEMENT) {
		text = static_cast<TiXmlElement*>(tiNode)->GetText();
	}
	else if (tiNode->Type() == TiXmlNode::TINYXML_TEXT) {
		text = tiNode->Value();
	}
	else if (tiNode->Type() == TiXmlNode::TINYXML_COMMENT) {
		text = tiNode->Value();
	}
	else {
		LERROR("tinyxml::NodeHandler", "Fail to get text content of node. Invalid node type");
	}
	if (!text) {
		text = "";
	}
	return stdString(text);
}

void NodeHandler::textContent( Node node, const std::string &text ) {
	TiXmlNode *tiNode = static_cast<TiXmlNode*>(node);
	std::vector<TiXmlNode*> toRemove;
	if (tiNode->Type() == TiXmlNode::TINYXML_ELEMENT) {
		for (TiXmlNode *child = tiNode->FirstChild(); child; child = child->NextSibling() ) {
			if (child->Type() == TiXmlNode::TINYXML_TEXT) {
				toRemove.push_back(child);
			}
		}
		BOOST_FOREACH( TiXmlNode *tmp, toRemove ) {
			tiNode->RemoveChild(tmp);
		}
		tiNode->LinkEndChild( new TiXmlText(text) );
	}
	else {
		tiNode->SetValue( text );
	}
}

bool NodeHandler::hasAttributes( Node node ) {
	return static_cast<TiXmlElement*>(node)->FirstAttribute() != NULL;
}

void NodeHandler::attributes( Node node, std::vector<Attribute> &attribs ) {
	TiXmlElement *tiNode = static_cast<TiXmlElement*>(node);
	for (const TiXmlAttribute *attr=tiNode->FirstAttribute(); attr; attr=attr->Next()) {
		Attribute tmpAt(
			const_cast<std::string&>(stdString(attr->Name())),
			const_cast<std::string&>(attr->ValueStr())
		);
		attribs.push_back( tmpAt );
	}
}

bool NodeHandler::hasAttribute( Node node, const std::string &attribute ) {
	if (static_cast<TiXmlNode*>(node)->Type() == TiXmlNode::TINYXML_ELEMENT) {
		return static_cast<TiXmlElement*>(node)->Attribute( attribute.c_str() ) != NULL;
	} else {
		LERROR("tinyxml::NodeHandler", "Cannot query attribute on a node that is not of type element");
		return false;
	}
}

const std::string &NodeHandler::attribute( Node node, const std::string &name ) {
	const char *attribValue = NULL;
	if (static_cast<TiXmlNode*>(node)->Type() == TiXmlNode::TINYXML_ELEMENT) {
		attribValue = static_cast<TiXmlElement*>(node)->Attribute( name.c_str() );
		if (!attribValue) {
			LTRACE("tinyxml::NodeHandler", "Attribute named '%s' does not exist!", name.c_str());
		}
	} else {
		LERROR("tinyxml::NodeHandler", "Cannot query attribute on a node that is not of type element");
	}
	if (!attribValue) {
		attribValue = "";
	}
	return stdString(attribValue);
}


void NodeHandler::setAttribute( Node node, const std::string &name, const std::string &value ) {
	TiXmlNode *tiNode = static_cast<TiXmlNode*>(node);
	if (tiNode->Type() == TiXmlNode::TINYXML_ELEMENT) {
		static_cast<TiXmlElement*>(tiNode)->SetAttribute( name, value );
	} else {
		LERROR("tinyxml::NodeHandler", "Cannot set attribute on a node that is not of type element");
	}
}

void NodeHandler::removeAttribute( Node node, const std::string &name ) {
	TiXmlNode *tiNode = static_cast<TiXmlNode*>(node);
	if (tiNode->Type() == TiXmlNode::TINYXML_ELEMENT) {
		static_cast<TiXmlElement*>(tiNode)->RemoveAttribute( name );
	} else {
		LERROR("tinyxml::NodeHandler", "Cannot remove attribute from a node that is not of type element");
	}
}

bool NodeHandler::hasParent( Node node ) {
	TiXmlNode *parent = static_cast<TiXmlNode*>(node)->Parent();
	if (!parent) {
		return false;
	}
	return parent->Type() == TiXmlNode::TINYXML_ELEMENT;
}

Node NodeHandler::parent( Node node ) {
	TiXmlNode *parent = static_cast<TiXmlNode*>(node)->Parent();
	if (parent && (parent->Type() != TiXmlNode::TINYXML_ELEMENT)) {
		return NULL;
	}
	return parent;
}

const std::string &NodeHandler::stdString( const char *key ) {
	std::pair<StrCache::iterator,bool> res = _strCache.insert(std::string(key));
	return *res.first;
}

void NodeHandler::clearCache() {
	_strCache.clear();
}

}
}
}
}
