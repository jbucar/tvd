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

#include "../../../include/ncl/NclStructureConverter.h"
#include "../../../include/ncl/NclDocumentConverter.h"
#include "../../../include/ncl/NclComponentsConverter.h"
#include "../../../include/ncl/NclPresentationControlConverter.h"
#include "ncl30/NclDocument.h"
#include "ncl30/connectors/ConnectorBase.h"
#include "ncl30/reuse/ReferNode.h"
#include "ncl30/descriptor/DescriptorBase.h"
#include "ncl30/switches/RuleBase.h"
#include "ncl30/switches/SwitchNode.h"
#include "ncl30/transition/TransitionBase.h"
#include "ncl30/components/ContextNode.h"
#include "ncl30/metainformation/Meta.h"
#include "ncl30/metainformation/Metadata.h"
#include "ncl30/layout/RegionBase.h"
#include <util/log.h>
#include <util/xml/nodehandler.h>

namespace bptn = ::br::pucrio::telemidia::ncl;
namespace bptncon = ::br::pucrio::telemidia::ncl::connectors;
namespace bptnd = ::br::pucrio::telemidia::ncl::descriptor;
namespace bptns = ::br::pucrio::telemidia::ncl::switches;
namespace bptnr = ::br::pucrio::telemidia::ncl::reuse;
namespace bptnt = ::br::pucrio::telemidia::ncl::transition;
namespace bptnm = ::br::pucrio::telemidia::ncl::metainformation;
namespace bptnl = ::br::pucrio::telemidia::ncl::layout;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {

NclStructureConverter::NclStructureConverter( bptcf::DocumentParser *documentParser )
	: NclStructureParser( documentParser )
{
}

/**
 * Agrega un port al body.
 * @param parentObject El nodo que representa al body.
 * @param childObject El port a agregar.
 */
void NclStructureConverter::addPortToBody( void *parentObject, void *childObject ) {
	getComponentsParser()->addPortToContext( static_cast<bptnc::ContextNode*>(parentObject), childObject );
}

/**
 * Agrega una propiedad al body.
 * @param parentObject El nodo que representa al body.
 * @param childObject La propiedad a agregar.
 */
void NclStructureConverter::addPropertyToBody( void *parentObject, void *childObject ) {
	getComponentsParser()->addPropertyToContext( static_cast<bptnc::ContextNode*>(parentObject), childObject );
}

void NclStructureConverter::addContextToBody( void *parentObject, void *childObject ) {

	getComponentsParser()->addElementToContext( static_cast<bptnc::Node*>(parentObject), static_cast<bptnc::Node*>(childObject));
}

void NclStructureConverter::addSwitchToBody( void *parentObject, void *childObject ) {

	getComponentsParser()->addElementToContext( static_cast<bptnc::Node*>(parentObject), static_cast<bptnc::Node*>(childObject));
}

void NclStructureConverter::addDescriptorBaseToHead( void *childObject ) {

	bptn::NclDocument *document;
	document = (bptn::NclDocument*) getDocumentParser()->getObject( "return", "document" );

	document->setDescriptorBase( (bptnd::DescriptorBase*) childObject );
}

/**
 * Agrega un regionBase al head del documento.
 * @param childObject El regionbase a agregar.
 */
void NclStructureConverter::addRegionBaseToHead( void *childObject ) {

	bptn::NclDocument *document;
	document = (bptn::NclDocument*) getDocumentParser()->getObject( "return", "document" );

	document->addRegionBase( (bptnl::RegionBase*) childObject );
}

void NclStructureConverter::addTransitionBaseToHead( void* childObject ) {

	bptn::NclDocument* document;

	document = (bptn::NclDocument*) (getDocumentParser()->getObject( "return", "document" ));

	document->setTransitionBase( (bptnt::TransitionBase*) childObject );
}

/**
 * Agrega un link al body.
 * @param parentObject El nodo que representa al body.
 * @param childObject El link a agregar.
 */
void NclStructureConverter::addLinkToBody( void *parentObject, void *childObject ) {

	getComponentsParser()->addLinkToContext( static_cast<bptnc::ContextNode*>(parentObject), childObject );
}

void NclStructureConverter::addMediaToBody( void *parentObject, void *childObject ) {

	getComponentsParser()->addElementToContext( static_cast<bptnc::Node*>(parentObject), static_cast<bptnc::Node*>(childObject));
}

void NclStructureConverter::addRuleBaseToHead( void *childObject ) {

	bptn::NclDocument *document;
	document = (bptn::NclDocument*) getDocumentParser()->getObject( "return", "document" );

	document->setRuleBase( (bptns::RuleBase*) childObject );
}

void NclStructureConverter::addConnectorBaseToHead( void *childObject ) {

	bptn::NclDocument *document;
	document = (bptn::NclDocument*) getDocumentParser()->getObject( "return", "document" );

	document->setConnectorBase( (bptncon::ConnectorBase*) childObject );
}

void *NclStructureConverter::createBody( util::xml::dom::Node parentElement ) {
	// criar composicao a partir do elemento body do documento ncl fazer
	// uso do nome da composicao body que foi atribuido pelo compilador
	bptn::NclDocument *document = (bptn::NclDocument*) getDocumentParser()->getObject( "return", "document" );
	bptnc::ContextNode *context;

	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();
	if (!nodeHnd->hasAttribute(parentElement, "id")) {
		nodeHnd->setAttribute(parentElement, "id", document->getId());
		context = (bptnc::ContextNode*) ((NclComponentsConverter*) getComponentsParser())->createContext( parentElement );
		nodeHnd->removeAttribute( parentElement, "id");
	} else {
		context = (bptnc::ContextNode*) ((NclComponentsConverter*) getComponentsParser())->createContext( parentElement );

	}
	document->setBody( context );
	return context;
}

void NclStructureConverter::solveNodeReferences( bptnc::CompositeNode *composition ) {
	std::vector<bptnc::Node*>* nodes;
	bptnc::Node *node;
	bptnc::NodeEntity *nodeEntity;
	bptn::Entity *referredNode;

	if (composition->instanceOf( "SwitchNode" )) {
		nodes = ((NclPresentationControlConverter*) getPresentationControlParser())->getSwitchConstituents( (bptns::SwitchNode*) composition );

	} else {
		nodes = composition->getNodes();
	}

	if (nodes == NULL) {
		return;
	}

	std::vector<bptnc::Node*>::iterator it;
	for (it = nodes->begin(); it != nodes->end(); ++it) {
		node = *it;
		if (node != NULL) {
			if (node->instanceOf( "ReferNode" )) {
				referredNode = ((bptnr::ReferNode*) node)->getReferredEntity();
				if (referredNode != NULL) {
					if (referredNode->instanceOf( "ReferredNode" )) {
						nodeEntity = (bptnc::NodeEntity*) (((NclDocumentConverter*) getDocumentParser())->getNode( referredNode->getId() ));

						if (nodeEntity != NULL) {
							((bptnr::ReferNode*) node)->setReferredEntity( nodeEntity->getDataEntity() );

						} else {
							LERROR("NclStructureConverter", "The media element refers to %s object, which was not declared before", referredNode->getId().c_str());
						}
					}
				}

			} else if (node->instanceOf( "CompositeNode" )) {
				solveNodeReferences( (bptnc::CompositeNode*) node );
			}
		}
	}
}

void *NclStructureConverter::posCompileBody( util::xml::dom::Node parentElement, void *parentObject ) {

	// solve refer references before the second trace through the document
	solveNodeReferences( (bptnc::CompositeNode*) parentObject );
	return NclStructureParser::posCompileBody( parentElement, parentObject );
}

/**
 * Crea el head del documento.
 * @param parentElement Elemento DOM representando al head.
 * @return El elemento DOM pasada por parámetro.
 */
void *NclStructureConverter::createHead( util::xml::dom::Node parentElement ) {

	// nothing to do
	return parentElement;
}

/**
 * Crea el documento NCL.
 * @param parentElement El elemento DOM representando al documento.
 * @return Una nueva instancia de @c NclDocument.
 */
void *NclStructureConverter::createNcl( util::xml::dom::Node parentElement ) {
	bptn::NclDocument *document = NULL;
	const std::string &docName = getNodeHandler()->attribute(parentElement, "id");
	if (docName.empty()) {
		document = new bptn::NclDocument( "nclDocument" );
	} else {
		document = new bptn::NclDocument( docName );
	}

	getDocumentParser()->addObject( "return", "document", document );
	return document;
}

void NclStructureConverter::addMetaToHead( void* childObject ) {

	bptn::NclDocument* document;

	document = (bptn::NclDocument*) getDocumentParser()->getObject( "return", "document" );

	document->addMetainformation( (bptnm::Meta*) childObject );
}

void NclStructureConverter::addMetadataToHead( void* childObject ) {

	bptn::NclDocument* document;

	document = (bptn::NclDocument*) getDocumentParser()->getObject( "return", "document" );

	document->addMetadata( (bptnm::Metadata*) childObject );
}
}
}
}
}
}
