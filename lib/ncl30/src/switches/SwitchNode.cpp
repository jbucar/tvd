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

#include "../../include/switches/SwitchNode.h"
#include "../../include/switches/SwitchContent.h"
#include "../../include/switches/Rule.h"
#include "../../include/interfaces/InterfacePoint.h"
#include "../../include/interfaces/Port.h"
#include "../../include/interfaces/SwitchPort.h"
#include "../../include/components/Node.h"

namespace bptnc = ::br::pucrio::telemidia::ncl::components;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace switches {
SwitchNode::SwitchNode( const std::string &id ) :
		CompositeNode( id ) {
	content = new SwitchContent();
	CompositeNode::nodes = new std::vector<Node*>;
	ruleList = new std::vector<Rule*>;
	defaultNode = NULL;
	typeSet.insert( "SwitchNode" );
	typeSet.insert( "DocumentNode" );
}

bool SwitchNode::addNode( unsigned int index, Node* node, Rule* rule ) {
	if (node == NULL || rule == NULL || index > nodes->size())
		return false;

	if (index == nodes->size()) {
		nodes->push_back( node );
		ruleList->push_back( rule );
	} else {
		nodes->insert( nodes->begin() + index, node );
		ruleList->insert( ruleList->begin() + index, rule );
	}

	node->setParentComposition( this );
	return true;
}

bool SwitchNode::addNode( Node* node, Rule* rule ) {
	if (node == NULL || rule == NULL
	)
		return false;

	nodes->push_back( node );
	ruleList->push_back( rule );

	node->setParentComposition( this );
	return true;
}

bool SwitchNode::addNode( Node* node ) {
	if (!node->instanceOf( "DocumentNode" ))
		return false;

	setDefaultNode( node );
	return true;
}

bool SwitchNode::addSwitchPortMap( bptni::SwitchPort* switchPort, Node* node, bptni::InterfacePoint* interfacePoint ) {

	bptni::Port* port;

	if (getNode( node->getId() ) == NULL || getPort( switchPort->getId() ) == NULL) {

		return false;
	}

	port = new bptni::Port( switchPort->getId(), node, interfacePoint );
	return switchPort->addPort( port );
}

bool SwitchNode::addPort( bptni::Port* port ) {
	return addPort( portList->size(), port );
}

bool SwitchNode::addPort( unsigned int index, bptni::Port* port ) {
	if (!(port->instanceOf( "SwitchPort" ))) {
		return false;
	}

	return CompositeNode::addPort( index, port );
}

void SwitchNode::clearNodes() {
	while (nodes->size() > 0) {
		removeNode( nodes->size() - 1 );
	}
	setDefaultNode( NULL );
	clearAnchors();
	clearPorts();
}

void SwitchNode::exchangeNodesAndRules( unsigned int index1, unsigned int index2 ) {

	if (index1 >= nodes->size() || index2 >= nodes->size())
		return;

	Node* auxNode = (Node*) ((*nodes)[index1]);
	Rule* auxRule = (Rule*) ((*ruleList)[index1]);

	(*nodes)[index1] = (*nodes)[index2];
	(*nodes)[index2] = auxNode;

	(*ruleList)[index1] = (*ruleList)[index2];
	(*ruleList)[index2] = auxRule;
}

bptnc::Node* SwitchNode::getDefaultNode() {
	return defaultNode;
}

bptni::InterfacePoint *SwitchNode::getMapInterface( bptni::Port *port ) {
	if (port->instanceOf( "SwitchPort" )) {
		return port;
	} else {
		return CompositeNode::getMapInterface( port );
	}
}

bptnc::Node* SwitchNode::getNode( const std::string &nodeId ) {
	//verifica se o no' default possui identificador dado por nodeId
	if (defaultNode != NULL && defaultNode->getId() == nodeId) {
		return defaultNode;
	}
	return CompositeNode::getNode( nodeId );
}

bptnc::Node* SwitchNode::getNode( unsigned int index ) {
	if (index >= nodes->size())
		return NULL;

	return (Node*) ((*nodes)[index]);
}

bptnc::Node* SwitchNode::getNode( Rule* rule ) {
	unsigned int index;

	index = indexOfRule( rule );
	if (index > ruleList->size())
		return NULL;

	return static_cast<Node*>( (*nodes)[index] );
}

unsigned int SwitchNode::getNumRules() {
	return ruleList->size();
}

Rule* SwitchNode::getRule( unsigned int index ) {
	if (index >= ruleList->size())
		return NULL;

	return static_cast<Rule*>( (*ruleList)[index] );
}

unsigned int SwitchNode::indexOfRule( Rule* rule ) {
	unsigned int i = 0;
	std::vector<Rule*>::iterator j;

	for (j = ruleList->begin(); j != ruleList->end(); ++j) {
		if ((*j)->getId() == rule->getId())
			return i;
		i++;
	}
	return (ruleList->size() + 10);
}

bool SwitchNode::recursivelyContainsNode( const std::string &nodeId ) {
	if (recursivelyGetNode( nodeId ) != NULL
	)
		return true;
	else
		return false;
}

bptnc::Node* SwitchNode::recursivelyGetNode( const std::string &nodeId ) {
	if (defaultNode != NULL && defaultNode->getId() == nodeId)
		return defaultNode;

	return CompositeNode::recursivelyGetNode( nodeId );
}

bool SwitchNode::removeNode( Node *node ) {
	int i, size;
	Node *auxNode;

	size = CompositeNode::nodes->size();
	for (i = 0; i < size; i++) {
		auxNode = (Node*) (*CompositeNode::nodes)[i];
		if (auxNode->getId() == node->getId()) {
			return removeNode( i );
		}
	}
	return false;
}

bool SwitchNode::removeNode( unsigned int index ) {
	Node *node;

	if (/*index < 0 || */index >= CompositeNode::nodes->size())
		return false;

	node = (Node*) (*CompositeNode::nodes)[index];
	node->setParentComposition( NULL );
	(CompositeNode::nodes)->erase( CompositeNode::nodes->begin() + index );
	ruleList->erase( ruleList->begin() + index );
	return true;
}

bool SwitchNode::removeRule( Rule* rule ) {
	unsigned int index;

	index = indexOfRule( rule );
	if (index < ruleList->size()) {
		std::vector<Node*>::iterator iterNode;
		iterNode = nodes->begin();
		iterNode = iterNode + index;

		std::vector<Rule*>::iterator iterRule;
		iterRule = ruleList->begin();
		iterRule = iterRule + index;

		nodes->erase( iterNode );
		ruleList->erase( iterRule );
		return true;
	} else
		return false;
}

void SwitchNode::setDefaultNode( Node* node ) {
	defaultNode = node;
}
}
}
}
}
}
