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

#include "../../include/components/NodeEntity.h"
#include "../../include/components/CompositeNode.h"
#include "../../include/components/Content.h"
#include "../../include/interfaces/Anchor.h"
#include "../../include/interfaces/LambdaAnchor.h"
#include "../../include/reuse/ReferNode.h"
#include "../../include/descriptor/GenericDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace components {
NodeEntity::NodeEntity( const std::string &uid, Content* someContent ) :
		Node( uid ),
		descriptor( NULL ),
		content( someContent ),
		instSameInstances( new std::set<bptnr::ReferNode*> ),
		gradSameInstances( new std::set<bptnr::ReferNode*> ) {

	anchorList->push_back( new bptni::LambdaAnchor( uid ) );
	typeSet.insert( "NodeEntity" );
}

NodeEntity::~NodeEntity() {
	std::vector<bptni::Anchor*>::iterator i;
	std::set<bptnr::ReferNode*>::iterator j;

	/*	if (descriptor != NULL) {
	 delete descriptor;
	 descriptor = NULL;
	 }*/

	if (content != NULL) {
		delete content;
		content = NULL;
	}

	if (instSameInstances != NULL) {
		for (j = instSameInstances->begin(); j != instSameInstances->end(); ++j) {

			delete (*j);
		}

		delete instSameInstances;
		instSameInstances = NULL;
	}

	if (gradSameInstances != NULL) {
		for (j = gradSameInstances->begin(); j != gradSameInstances->end(); ++j) {

			delete (*j);
		}

		delete gradSameInstances;
		gradSameInstances = NULL;
	}

	if (anchorList != NULL) {
		i = anchorList->begin();
		while (i != anchorList->end()) {
			delete (*i);
			++i;
		}

		delete anchorList;
		anchorList = NULL;
	}
}

bool NodeEntity::addAnchor( int index, bptni::Anchor* anchor ) {
	if (index == 0) {
		return false;
	}
	return Node::addAnchor( index, anchor );
}

void NodeEntity::clearAnchors() {
	bptni::LambdaAnchor* lambda = (bptni::LambdaAnchor*) ((*anchorList->begin()));
	Node::clearAnchors();
	anchorList->push_back( lambda );
}

bptni::LambdaAnchor* NodeEntity::getLambdaAnchor() {
	bptni::LambdaAnchor* lambda;
	lambda = static_cast<bptni::LambdaAnchor*>( *(anchorList->begin()));return
lambda	;
}

void NodeEntity::setId( const std::string &id ) {
	bptni::LambdaAnchor* anchor;

	Entity::setId( id );
	anchor = getLambdaAnchor();
	anchor->setId( id );
}

bool NodeEntity::removeAnchor( int index ) {
	if (index == 0) {
		return false;
	}
	return Node::removeAnchor( index );
}

bptnd::GenericDescriptor* NodeEntity::getDescriptor() {
	return descriptor;
}

void NodeEntity::setDescriptor( bptnd::GenericDescriptor* someDescriptor ) {
	descriptor = someDescriptor;
}

Content* NodeEntity::getContent() {
	return content;
}

void NodeEntity::setContent( Content* someContent ) {
	content = someContent;
}

bool NodeEntity::addAnchor( bptni::Anchor *anchor ) {
	return Node::addAnchor( anchor );
}

bool NodeEntity::removeAnchor( bptni::Anchor *anchor ) {
	return Node::removeAnchor( anchor );
}

std::set<bptnr::ReferNode*>* NodeEntity::getInstSameInstances() {
	if (instSameInstances->empty()) {
		return NULL;
	}

	return instSameInstances;
}

std::set<bptnr::ReferNode*>* NodeEntity::getGradSameInstances() {
	if (gradSameInstances->empty()) {
		return NULL;
	}

	return gradSameInstances;
}

bool NodeEntity::addSameInstance( bptnr::ReferNode* node ) {
	if (node->getInstanceType() == "instSame") {
		if (instSameInstances->count( node ) != 0) {
			return false;
		}

		instSameInstances->insert( node );

	} else if (node->getInstanceType() == "gradSame") {
		if (gradSameInstances->count( node ) != 0) {
			return false;
		}

		gradSameInstances->insert( node );

	} else {
		return false;
	}

	return true;
}

void NodeEntity::removeSameInstance( bptnr::ReferNode* node ) {
	std::set<bptnr::ReferNode*>::iterator i;

	i = gradSameInstances->find( node );
	if (i != gradSameInstances->end()) {
		gradSameInstances->erase( i );
	}

	i = instSameInstances->find( node );
	if (i != instSameInstances->end()) {
		instSameInstances->erase( i );
	}
}
}
}
}
}
}
