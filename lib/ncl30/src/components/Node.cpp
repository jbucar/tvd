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

#include "../../include/components/Node.h"
#include "../../include/components/CompositeNode.h"
#include "../../include/interfaces/Anchor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace components {
Node::Node( const std::string &id ) :
		Entity( id ),
		parentNode( NULL ),
		anchorList( new std::vector<bptni::Anchor*> ) {
	typeSet.insert( "Node" );

}

Node::~Node() {
	std::vector<bptni::Anchor*>::iterator i;

	parentNode = NULL;
	if (anchorList != NULL) {
		i = anchorList->begin();
		while (i != anchorList->end()) {
			delete *i;
			++i;
		}

		delete anchorList;
		anchorList = NULL;
	}
}

void* Node::getParentComposition() {
	return parentNode;
}

std::vector<Node*> *Node::getPerspective() {
	std::vector<Node*> *perspective;

	if (parentNode == NULL) {
		perspective = new std::vector<Node*>;
	} else {
		perspective = ((CompositeNode*) parentNode)->getPerspective();
	}
	perspective->push_back( (Node*) this );
	return perspective;
}

void Node::setParentComposition( void* composition ) {
	if (composition == NULL || ((CompositeNode*) composition)->getNode( getId() ) != NULL) {

		this->parentNode = (CompositeNode*) composition;
	}
}

bool Node::addAnchor( int index, bptni::Anchor *anchor ) {
	// anchor position must be in the correct range and anchor must exist
	if ((index < 0 || index > (int) anchorList->size()) || anchor == NULL) {
		return false;
	}

	// anchor id must be unique - conflicts with referredNode anchor ids
	// can only be solved at runtime, since anchors can be inserted after
	if (getAnchor( anchor->getId() ) != NULL) {
		return false;
	}

	if (index == (int) anchorList->size()) {
		anchorList->push_back( anchor );
	} else {
		anchorList->insert( anchorList->begin() + index, anchor );
	}

	return true;
}

bool Node::addAnchor( bptni::Anchor *anchor ) {
	return Node::addAnchor( anchorList->size(), anchor );
}

bptni::Anchor* Node::getAnchor( const std::string &anchorId ) {
	std::vector<bptni::Anchor*>::iterator it;
	bptni::Anchor *anchor;

	it = anchorList->begin();
	int i = 0;
	while (it != anchorList->end()) {
		anchor = (bptni::Anchor*) (*it);
		if (anchor == NULL) {
			return NULL;

		} else if (anchor->getId() != "" && anchor->getId() == anchorId) {
			return anchor;
		}
		++it;
		i++;
	}
	return NULL;
}

bptni::Anchor *Node::getAnchor( int index ) {
	if (index < 0 || index > ((int) anchorList->size() - 1)) {
		return NULL;
	} else {
		return (bptni::Anchor*) (*anchorList)[index];
	}
}

std::vector<bptni::Anchor*> *Node::getAnchors() {
	return anchorList;
}

int Node::getNumAnchors() {
	return anchorList->size();
}

int Node::indexOfAnchor( bptni::Anchor *anchor ) {
	std::vector<bptni::Anchor*>::iterator i;
	int n;
	n = 0;

	for (i = anchorList->begin(); i != anchorList->end(); ++i) {
		if (*i == anchor) {
			return n;
		}
		n++;
	}
	return anchorList->size() + 10;

}

bool Node::removeAnchor( int index ) {
	if (index < 0 || index >= (int) anchorList->size()) {
		return false;
	}

	anchorList->erase( anchorList->begin() + index );
	return true;
}

bool Node::removeAnchor( bptni::Anchor *anchor ) {
	return removeAnchor( indexOfAnchor( anchor ) );
}

void Node::clearAnchors() {
	anchorList->clear();
}
}
}
}
}
}
