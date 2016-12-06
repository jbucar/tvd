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

#include "../../../include/ncl/NclComponentsConverter.h"
#include "../../../include/ncl/NclDocumentConverter.h"
#include "../../../include/framework/ncl/NclPresentationControlParser.h"
#include "../../../include/framework/ncl/NclPresentationSpecificationParser.h"

#include <ncl30/interfaces/Port.h>
#include <ncl30/interfaces/Anchor.h>
#include <ncl30/reuse/ReferNode.h>
#include <ncl30/reuse/ReferredNode.h>
#include <ncl30/connectors/Connector.h>
#include <ncl30/connectors/Role.h>
#include <ncl30/link/Link.h>
#include <ncl30/NclDocument.h>
#include <ncl30/Entity.h>
#include <ncl30/descriptor/GenericDescriptor.h>
#include <ncl30/components/ContentNode.h>
#include <ncl30/components/Node.h>
#include <ncl30/components/ContextNode.h>
#include <util/log.h>
#include <util/xml/nodehandler.h>
#include <boost/foreach.hpp>

namespace bptn = ::br::pucrio::telemidia::ncl;
namespace bptnli = ::br::pucrio::telemidia::ncl::link;
namespace bptnconn = ::br::pucrio::telemidia::ncl::connectors;
namespace bptnd = ::br::pucrio::telemidia::ncl::descriptor;
namespace bptcf = ::br::pucrio::telemidia::converter::framework;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {
NclComponentsConverter::NclComponentsConverter( bptcf::DocumentParser *documentParser ) :
		NclComponentsParser( documentParser ) {
}

void NclComponentsConverter::addPortToContext( bptnc::ContextNode *parentObject, void *childObject ) {

	parentObject->addPort( (bptni::Port*) childObject );
}

void NclComponentsConverter::addPropertyToContext(bptnc::Node *parentObject, void *childObject ) {

	if(childObject != NULL){
		parentObject->addAnchor( (bptni::Anchor*) childObject );
	}
}

/**
 * Agrega un elemento al contexto.
 * @param parentObject El contexto al que se va a agregar el elemento.
 * @param childObject El nodo a agregar.
 */
void NclComponentsConverter::addElementToContext(bptnc::Node *parentObject, bptnc::Node *childObject ) {

	if(childObject != NULL){
			//adicionar composicao aa composicao
		(static_cast<bptnc::ContextNode*>(parentObject))->addNode(childObject);
	}
}

/**
 * Agrega un elemento a un media.
 * @param parentObject El media al que se va a agregar el elemento.
 * @param childObject El elemento a agregar.
 */
void NclComponentsConverter::addElementToMedia( bptnc::Node *parentObject, void *childObject ) {

	if(childObject != NULL){
		addAnchorToMedia( (bptnc::ContentNode*) parentObject, (bptni::Anchor*) childObject );
	}
}

void NclComponentsConverter::addAnchorToMedia( bptnc::ContentNode *contentNode, bptni::Anchor *anchor ) {

	if (contentNode->getAnchor( anchor->getId() ) != NULL) {
		LWARN("NclComponentsConverter", "There is another interface with the same id=%s", anchor->getId().c_str() );
	} else {
		contentNode->addAnchor( anchor );
	}
}

void NclComponentsConverter::addLinkToContext( bptnc::ContextNode *parentObject, void *childObject ) {

	int min;
	int max;
	bptnconn::Role* role;
	std::vector<bptnconn::Role*>::iterator i;

	std::vector<bptnconn::Role*>* roles;

	roles = ((bptnli::Link*) childObject)->getConnector()->getRoles();
	if (roles != NULL) {
		i = roles->begin();
		while (i != roles->end()) {
			role = *i;
			min = role->getMinCon();
			max = role->getMaxCon();

			if (((bptnli::Link*) childObject)->getNumRoleBinds( role ) < (unsigned int) min) {
				LWARN("NclComponentsConverter", "role %s with less than minimum binds", role->getLabel().c_str());
				return;

			} else if (max > 0 && ((bptnli::Link*) childObject)->getNumRoleBinds( role ) > (unsigned int) max) {
				LWARN("NclComponentsConverter", "role %s with more than maximum binds", role->getLabel().c_str());
				delete roles;
				return;
			}
			++i;
		}
		delete roles;
	}
	parentObject->addLink( (bptnli::Link*) childObject );
}

bool NclComponentsConverter::hasValidId( util::xml::dom::Node parentElement ){
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	if (!nodeHnd->hasAttribute(parentElement, "id")) {
		LWARN("NclComponentsConverter", "An element was declared without an id attribute");
		return false;
	}

	std::string id = nodeHnd->attribute(parentElement, "id");
	bptnc::Node *node = ((NclDocumentConverter*) getDocumentParser())->getNode(id);
	if (node != NULL) {
		LWARN("NclComponentsConverter", "There is another node element previously declared with the same %s id.", id.c_str());
		return false;
	}
	return true;
}

bptnc::Node *NclComponentsConverter::createContext( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();
	if ( hasValidId(parentElement)){
		if (nodeHnd->hasAttribute(parentElement, "refer")) {
			return static_cast<bptnc::Node*>(createReferNodeContext(parentElement));
		} else{
			return static_cast<bptnc::Node*>(createStandardContext(parentElement));
		}
	} else {
		return NULL;
	}
}

bptnr::ReferNode *NclComponentsConverter::createReferNodeContext( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();
	
	bptn::Entity *referNode;
	std::string attValue = nodeHnd->attribute(parentElement, "refer");

	try {
		referNode = (bptnc::ContextNode*) ((NclDocumentConverter*) getDocumentParser())->getNode( attValue );

		if (referNode == NULL) {
			bptn::NclDocument *document = (bptn::NclDocument*) (getDocumentParser()->getObject( "return", "document" ));

			referNode = (bptnc::ContextNode*) (document->getNode( attValue ));
			if (referNode == NULL) {
				referNode = (bptn::Entity*) (new bptnr::ReferredNode( attValue, (void*) parentElement ));
			}
		}
	} catch (...) {
		LWARN("NclComponentsConverter", "The context element refers to %s object, which is not a context element", attValue.c_str());
		return NULL;
	}

	bptnr::ReferNode* node = new bptnr::ReferNode( nodeHnd->attribute(parentElement, "id") );
	node->setReferredEntity( referNode );

	return node;
}

bptnc::ContextNode *NclComponentsConverter::createStandardContext( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	std::string id = nodeHnd->attribute(parentElement, "id");
	// retornar nova composicao ncm a partir do elemento xml que a representa em NCL
	bptnc::ContextNode *context = new bptnc::ContextNode( id );

	if (nodeHnd->hasAttribute(parentElement, "descriptor")) {
		// adicionar um descritor a um objeto de midia
		std::string attValue = nodeHnd->attribute(parentElement, "descriptor");
		bptn::NclDocument *document = (bptn::NclDocument*) (getDocumentParser()->getObject( "return", "document" ));
		bptnd::GenericDescriptor *descriptor = document->getDescriptor( attValue );
		if (descriptor != NULL) {
			context->setDescriptor( descriptor );
		} else {
			LWARN("NclComponentsConverter", "The context element with id=%s attribute refers to a descriptor=%s that does not exist.", id.c_str(), attValue.c_str());
			return NULL;
		}
	}

	return context;
}

/**
 * Crea un media.
 * @param parentElement El elemento DOM que tiene la información del media.
 * @return Retorna una nueva instancia de un media.
 */
bptnc::Node *NclComponentsConverter::createMedia( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	if ( hasValidId(parentElement)){
		if (nodeHnd->hasAttribute(parentElement, "refer")) {
			return static_cast<bptnc::Node*>(createReferNodeMedia(parentElement));
		} else{
			return static_cast<bptnc::Node*>(createStandardMedia(parentElement));
		}
	} else {
		return NULL;
	}
}

bptnr::ReferNode *NclComponentsConverter::createReferNodeMedia( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptn::Entity *referNode;
	std::string attValue = nodeHnd->attribute(parentElement, "refer");

	try {
		referNode = (bptnc::ContentNode*) ((NclDocumentConverter*) getDocumentParser())->getNode( attValue );

		if (referNode == NULL) {
			//TODO: verificar se faz a mesma coisa da linha anterior
			bptn::NclDocument *document = (bptn::NclDocument*) (getDocumentParser()->getObject( "return", "document" ));

			referNode = (bptnc::ContentNode*) document->getNode( attValue );
			if (referNode == NULL) {
				referNode = new bptnr::ReferredNode( attValue, (void*) parentElement );
			}
		}
	} catch (...) {
		LWARN("NclComponentsConverter", "The media element refers to %s object, which is not a media element.", attValue.c_str());
		return NULL;
	}
	std::string id = nodeHnd->attribute(parentElement, "id");
	
	bptnr::ReferNode* node = new bptnr::ReferNode( id );
	node->setReferredEntity( referNode );
	if (nodeHnd->hasAttribute(parentElement, "instance")) {
		node->setInstanceType( nodeHnd->attribute(parentElement, "instance") );
	}

	return node;
}

bptnc::ContentNode *NclComponentsConverter::createStandardMedia( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	std::string id = nodeHnd->attribute(parentElement, "id");
	bptnc::ContentNode *node = new bptnc::ContentNode( id, NULL );
	bptnd::GenericDescriptor *descriptor=NULL;

	std::string attValue;

	// type of media object
	if (nodeHnd->hasAttribute(parentElement, "type")) {
		attValue = nodeHnd->attribute(parentElement,  "type");
		node->setNodeType( attValue );
	}

	if (nodeHnd->hasAttribute(parentElement, "src")) {
		node->setUrl( getDocumentParser()->getDocumentPath(), nodeHnd->attribute(parentElement, "src") );
	}

	if (nodeHnd->hasAttribute(parentElement, "descriptor")) {
		// adicionar um descritor a um objeto de midia
		attValue = nodeHnd->attribute(parentElement, "descriptor");
		bptn::NclDocument *document = (bptn::NclDocument*) getDocumentParser()->getObject( "return", "document" );
		descriptor = document->getDescriptor( attValue );
	} else {
		descriptor = ((bptcfn::NclDocumentParser*)getDocumentParser())->getPresentationSpecificationParser()->createDummyDescriptorBase();
	}

	if (descriptor != NULL) {
		node->setDescriptor( descriptor );
	} else {
		LWARN("NclComponentsConverter", "The media element with %s id attribute refers to a descriptor ( %s ) that does not exist", id.c_str(), attValue.c_str());
		delete node;
		node = NULL;
		return NULL;
	}

	//retornar no' de midia
	return node;
}


bptnc::ContextNode *NclComponentsConverter::posCompileContext( util::xml::dom::Node parentElement, bptnc::ContextNode *parentObject ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node element, elementNodeList ) {
		const std::string &elementTagName = nodeHnd->tagName(element);
		bptnc::Node *elementObject;
		if (elementTagName.compare( "context" ) == 0) {
			elementObject = ((NclDocumentConverter*) getDocumentParser())->getNode( nodeHnd->attribute(element, "id") );
//			try {
				if (elementObject->instanceOf("ContextNode")) {
					posCompileContext(element, static_cast<bptnc::ContextNode*>(elementObject));
				}
//			} catch (...) {
//				//treating error with the <NodeEntity*> casting
//			}
		} else if (elementTagName.compare( "switch" ) == 0) {
			elementObject = ((NclDocumentConverter*) getDocumentParser())->getNode( nodeHnd->attribute(element, "id") );
//			try {
				if (elementObject->instanceOf("SwitchNode")) {
					getPresentationControlParser()->posCompileSwitch( element, elementObject );
				}
// 			} catch (...) {
// 				//treating error with the <NodeEntity*> casting
// 			}
		}
	}
	return NclComponentsParser::posCompileContext( parentElement, static_cast<bptnc::ContextNode*>(parentObject));
}
}
}
}
}
}
