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

#include "../../../include/model/NodeNesting.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace components {
NodeNesting::NodeNesting() {
	initialize();
}

NodeNesting::NodeNesting( Node *node ) {
	initialize();
	insertAnchorNode( node );
}

NodeNesting::NodeNesting( NodeNesting *seq ) {
	initialize();
	append( seq );
}

NodeNesting::NodeNesting( vector<Node*> *seq ) {
	initialize();
	append( seq );
}

NodeNesting::~NodeNesting() {
	if (nodes != NULL) {
		delete nodes;
		nodes = NULL;
	}
}

void NodeNesting::initialize() {
	string type = "NodeNesting";
	this->nodes = new vector<Node*>;
	id = "";
	typeSet.insert( type );
}

bool NodeNesting::instanceOf( string s ) {
	if (!typeSet.empty()) {
		return (typeSet.find( s ) != typeSet.end());
	} else {
		return false;
	}
}

void NodeNesting::append( NodeNesting* otherSeq ) {
	int i, size;
	Node* node;

	size = otherSeq->getNumNodes();
	for (i = 0; i < size; i++) {
		node = otherSeq->getNode( i );
		insertAnchorNode( node );
	}
}

void NodeNesting::append( vector<Node*>* otherSeq ) {
	vector<Node*>::iterator i;

	for (i = otherSeq->begin(); i != otherSeq->end(); ++i) {
		insertAnchorNode( *i );
	}
}

Node* NodeNesting::getAnchorNode() {
	Node* node;

	if (nodes == NULL || nodes->empty()) {
		return NULL;

	} else if (nodes->size() == 1) {
		node = *(nodes->begin());
		return node;

	} else {
		node = *(nodes->end() - 1);
		return node;
	}
}

Node* NodeNesting::getHeadNode() {
	Node* node;

	if (nodes == NULL || nodes->empty()) {
		return NULL;
	} else {
		node = *(nodes->begin());
		return node;
	}
}

Node* NodeNesting::getNode( int index ) {
	Node* node;
	vector<Node*>::iterator i;

	if (nodes == NULL || nodes->empty() || index < 0 || index >= (int) (nodes->size())) {

		return NULL;
	}

	i = nodes->begin() + index;
	node = *i;
	return node;
}

int NodeNesting::getNumNodes() {
	int s;

	if (nodes == NULL) {
		s = 0;
	} else {
		s = nodes->size();
	}
	return s;
}

void NodeNesting::insertAnchorNode( Node* node ) {
	string nodeId;

	if (nodes == NULL) {
		return;
	}

	nodeId = node->getId();
	if (nodes->size() > 0) {
		id = id + "/" + nodeId;

	} else {
		id = nodeId;
	}

	nodes->push_back( node );
}

void NodeNesting::insertHeadNode( Node* node ) {
	if (nodes == NULL) {
		return;
	}

	if (nodes->size() > 0) {
		id = node->getId() + "/" + id;
	} else {
		id = node->getId();
	}
	nodes->insert( nodes->begin(), node );
}

bool NodeNesting::removeAnchorNode() {
	if (nodes == NULL || nodes->empty()) {
		return false;
	}

	nodes->erase( nodes->end() - 1 );

	if (id.find( "/" ) != std::string::npos) {
		id = id.substr( 0, id.find_last_of( "/" ) );
	}

	return true;
}

bool NodeNesting::removeHeadNode() {
	if (nodes == NULL || nodes->empty()) {
		return false;
	}

	nodes->erase( nodes->begin() );

	if (id.find( "/" ) != std::string::npos) {
		id = id.substr( 0, id.find_last_of( "/" ) );
	}

	return true;
}

bool NodeNesting::removeNode( Node *node ) {
	unsigned int i;
	for (i = 0; i < nodes->size(); i++) {
		if ((*nodes)[i] == node)
			break;
	}

	if (i >= nodes->size())
		return false;

	while (nodes->size() > i) {
		removeAnchorNode();
	}
	return true;
}

string NodeNesting::getId() {
	return id;
}

NodeNesting *NodeNesting::copy() {
	return new NodeNesting( this );
}

string NodeNesting::toString() {
	return id;
}
}
}
}
}
}
}
}
