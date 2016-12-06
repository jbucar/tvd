/******************************************************************************
 Este arquivo eh parte da implementacao do ambiente declarativo do middleware
 Ginga (Ginga-NCL).

 Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

 Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
 os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
 Software Foundation.

 Este programa eh distribuido na expectativa de que seja util, porem, SEM
 NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
 ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
 GNU versao 2 para mais detalhes.

 Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
 com este programa; se nao, escreva para a Free Software Foundation, Inc., no
 endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

 Para maiores informacoes:
 ncl @ telemidia.puc-rio.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://www.telemidia.puc-rio.br
 ******************************************************************************
 This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

 Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.
 Copyright 2010 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata


 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
 details.

 You should have received a copy of the GNU General Public License version 2
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

 For further information contact:
 ncl @ telemidia.puc-rio.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://www.telemidia.puc-rio.br
 *******************************************************************************/

#include "../../include/components/CompositeNode.h"
#include "../../include/interfaces/Port.h"
#include "../../include/interfaces/InterfacePoint.h"
#include "../../include/interfaces/Anchor.h"
#include "../../include/reuse/ReferNode.h"
#include <util/log.h>
#include <iostream>

namespace bptnr = ::br::pucrio::telemidia::ncl::reuse;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace components {
CompositeNode::CompositeNode( const std::string &id ) :
		NodeEntity( id, NULL ),
		nodes( NULL ),
		portList( new std::vector<bptni::Port*> ) {
	typeSet.insert( "CompositeNode" );
}

CompositeNode::~CompositeNode() {
	if (nodes != NULL) {
		delete nodes;
		nodes = NULL;
	}

	if (portList != NULL) {
		for (unsigned int i = 0; i < portList->size(); i++) {
			bptni::Port* tempPort = portList->at( i );
			if (tempPort != NULL) {
				delete tempPort;
				tempPort = NULL;
			}
		}
		delete portList;
		portList = NULL;
	}
}

bool CompositeNode::addAnchor( int index, bptni::Anchor *anchor ) {
	if (anchor == NULL) {
		LWARN("CompositeNode", "Trying to add a NULL anchor");
		return false;
	}

	std::string anchorId;
	anchorId = anchor->getId();
	if (getPort( anchorId ) != NULL) {
		return false;
	}

	return NodeEntity::addAnchor( index, anchor );
}

bool CompositeNode::addAnchor( bptni::Anchor *anchor ) {
	return CompositeNode::addAnchor( anchorList->size(), anchor );
}

bool CompositeNode::addPort( unsigned int index, bptni::Port* port ) {
	if (index > portList->size() || port == NULL || NodeEntity::getAnchor( port->getId() ) != NULL || getPort( port->getId() ) != NULL
	)
		return false;

	if (index == portList->size())
		portList->push_back( port );
	else
		portList->insert( portList->begin() + index, port );

	return true;
}

bool CompositeNode::addPort( bptni::Port* port ) {
	return addPort( portList->size(), port );
}

void CompositeNode::clearPorts() {
	portList->clear();
}

unsigned int CompositeNode::getNumPorts() {
	return portList->size();
}

bptni::Port* CompositeNode::getPort( const std::string &portId ) {
	if (portId == "") {
		return NULL;
	}

	std::vector<bptni::Port*>::iterator i;

	for (i = portList->begin(); i != portList->end(); ++i) {
		if ((*i)->getId() == portId) {
			return (*i);
		}
	}
	return NULL;
}

bptni::Port* CompositeNode::getPort( unsigned int index ) {
	if (index >= portList->size())
		return NULL;

	return (*portList)[index];
}

std::vector<bptni::Port*>* CompositeNode::getPorts() {
	if (portList->empty())
		return NULL;

	return portList;
}

unsigned int CompositeNode::indexOfPort( bptni::Port* port ) {
	unsigned int i = 0;
	std::vector<bptni::Port*>::iterator it;

	for (it = portList->begin(); it != portList->end(); it++) {
		if ((*it)->getId() == port->getId()) {
			return i;
		}
		i++;
	}
	return portList->size() + 10;
}

bool CompositeNode::removePort( bptni::Port* port ) {
	std::vector<bptni::Port*>::iterator it;
	for (portList->begin(); it != portList->end(); it++) {
		if (*it == port) {
			portList->erase( it );
			return true;
		}
	}
	return false;
}

bptni::InterfacePoint* CompositeNode::getMapInterface( bptni::Port* port ) {
	Node *node;
	CompositeNode *compositeNode;
	bptni::InterfacePoint *interfacePoint;

	node = port->getNode();
	interfacePoint = port->getInterfacePoint();
	if (interfacePoint->instanceOf( "Port" )) {
		compositeNode = (CompositeNode*) node->getDataEntity();
		return compositeNode->getMapInterface( (bptni::Port*) interfacePoint );
	} else {
		return (bptni::Anchor*) interfacePoint;
	}
}

Node* CompositeNode::getNode( const std::string &nodeId ) {
	if (nodes == NULL) {
		return NULL;
	}

	std::vector<Node*>::iterator i;

	for (i = nodes->begin(); i != nodes->end(); ++i) {
		if ((*i)->getId() == nodeId) {
			return (*i);
		}
	}
	return NULL;
}

std::vector<Node*> *CompositeNode::getNodes() {
	return this->nodes;
}

unsigned int CompositeNode::getNumNodes() {
	if (nodes == NULL) {
		return 0;
	}

	return nodes->size();
}

bool CompositeNode::recursivelyContainsNode( const std::string &nodeId ) {
	if (recursivelyGetNode( nodeId ) != NULL
	)
		return true;
	else
		return false;
}

bool CompositeNode::recursivelyContainsNode( Node* node ) {
	if (nodes == NULL) {
		return false;
	}

	std::vector<Node*>::iterator it;
	Node *childNode;
	CompositeNode *compositeNode;
	unsigned int i;

	for (i = 0; i < nodes->size(); i++) {
		childNode = (*nodes)[i];
		if (childNode == node) {
			return true;
		}
	}

	for (it = nodes->begin(); it != nodes->end(); ++it) {
		childNode = (Node*) *it;
		if (childNode->instanceOf( "CompositeNode" )) {
			compositeNode = (CompositeNode*) childNode;
			if (compositeNode->recursivelyContainsNode( node )) {
				return true;
			}

		} else if (childNode->instanceOf( "ReferNode" )) {
			childNode = (Node*) (((bptnr::ReferNode*) childNode)->getReferredEntity());

			if (childNode == node) {
				return true;

			} else if (childNode->instanceOf( "CompositeNode" )) {
				compositeNode = (CompositeNode*) childNode;
				if (compositeNode->recursivelyContainsNode( node )) {
					return true;
				}
			}
		}
	}
	return false;
}

Node* CompositeNode::recursivelyGetNode( const std::string &nodeId ) {
	if (nodes == NULL) {
		return NULL;
	}

	std::vector<Node*>::iterator i;

	for (i = nodes->begin(); i != nodes->end(); ++i) {
		if (((*i)->getId()).compare( nodeId ) == 0) {
			return (*i);
		}
		if ((*i)->instanceOf( "CompositeNode" )) {
			CompositeNode* compositeNode = (CompositeNode*) (*i);
			Node* node = compositeNode->recursivelyGetNode( nodeId );
			if (node != NULL) {
				return node;
			}
		}
	}
	return NULL;
}

bool CompositeNode::removeNode( Node* node ) {
	if (nodes == NULL) {
		return false;
	}

	std::vector<Node*>::iterator it;

	for (it = nodes->begin(); it != nodes->end(); ++it) {
		if (*it == node) {
			break;
		}
	}

	if (it != nodes->end()) {
		node->setParentComposition( NULL );
		nodes->erase( it );
		return true;
	} else {
		return false;
	}
}

void CompositeNode::clearNodes() {
	if (nodes == NULL) {
		return;
	}

	std::vector<Node*>::iterator it;
	NodeEntity *node;

	for (it = nodes->begin(); it != nodes->end(); ++it) {
		node = (NodeEntity*) (*it);
		node->setParentComposition( NULL );
	}
	nodes->clear();
}

}
}
}
}
}
