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

#include "../../../include/ncl/NclPresentationControlConverter.h"
#include "../../../include/ncl/NclDocumentConverter.h"
#include "../../../include/framework/ncl/NclComponentsParser.h"
#include "ncl30/Entity.h"
#include "ncl30/NclDocument.h"
#include "ncl30/components/ContextNode.h"
#include "ncl30/switches/CompositeRule.h"
#include "ncl30/switches/SimpleRule.h"
#include "ncl30/switches/DescriptorSwitch.h"
#include "ncl30/switches/SwitchNode.h"
#include "ncl30/switches/Rule.h"
#include "ncl30/switches/RuleBase.h"
#include "ncl30/interfaces/Port.h"
#include "ncl30/reuse/ReferredNode.h"
#include "ncl30/reuse/ReferNode.h"
#include "ncl30/descriptor/GenericDescriptor.h"
#include "ncl30/util/Comparator.h"
#include <util/log.h>
#include <util/xml/nodehandler.h>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

namespace bptn = ::br::pucrio::telemidia::ncl;
namespace bptni = ::br::pucrio::telemidia::ncl::interfaces;
namespace bptnr = ::br::pucrio::telemidia::ncl::reuse;
namespace bptnd = ::br::pucrio::telemidia::ncl::descriptor;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {

NclPresentationControlConverter::NclPresentationControlConverter( bptcf::DocumentParser *documentParser )
	: NclPresentationControlParser( documentParser )
{
	switchConstituents = new std::map<std::string, std::map<std::string, bptnc::NodeEntity*>*>;
}

NclPresentationControlConverter::~NclPresentationControlConverter() {
	if (switchConstituents != NULL) {
		delete switchConstituents;
		switchConstituents = NULL;
	}

}

std::vector<bptnc::Node*> *NclPresentationControlConverter::getSwitchConstituents( bptns::SwitchNode *switchNode ) {

	std::map<std::string, bptnc::NodeEntity*> *hTable;
	std::vector<bptnc::Node*> *ret = new std::vector<bptnc::Node*>;

	if (switchConstituents->count( switchNode->getId() ) == 1) {
		hTable = (*switchConstituents)[switchNode->getId()];
		std::map<std::string, bptnc::NodeEntity*>::iterator it;
		for (it = hTable->begin(); it != hTable->end(); ++it) {
			ret->push_back( (bptnc::Node*) it->second );
		}
	}

	return ret;
}

void NclPresentationControlConverter::addCompositeRuleToCompositeRule( void *parentObject, void *childObject ) {

	((bptns::CompositeRule*) parentObject)->addRule( (bptns::Rule*) childObject );
}

void NclPresentationControlConverter::addCompositeRuleToRuleBase( void *parentObject, void *childObject ) {

	((bptns::RuleBase*) parentObject)->addRule( (bptns::Rule*) childObject );
}

void NclPresentationControlConverter::addRuleToCompositeRule( void *parentObject, void *childObject ) {

	// adicionar regra
	((bptns::CompositeRule*) parentObject)->addRule( (bptns::Rule*) childObject );
}

void NclPresentationControlConverter::addRuleToRuleBase( void *parentObject, void *childObject ) {

	((bptns::RuleBase*) parentObject)->addRule( (bptns::Rule*) childObject );
}

void NclPresentationControlConverter::addSwitchPortToSwitch( void *parentObject, void *childObject ) {

	((bptns::SwitchNode*) parentObject)->addPort( (bptni::Port*) childObject );
}

void *NclPresentationControlConverter::createCompositeRule( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	short ruleOp = bptns::CompositeRule::OP_AND;
	if (nodeHnd->attribute(parentElement, "operator").compare("and") == 0) {
		ruleOp = bptns::CompositeRule::OP_AND;
	} else if (nodeHnd->attribute(parentElement, "operator").compare("or") == 0) {
		ruleOp = bptns::CompositeRule::OP_OR;
	}

	// cria regra composta
	bptns::CompositeRule *compositePresentationRule = new bptns::CompositeRule(nodeHnd->attribute(parentElement, "id"), ruleOp);
	return compositePresentationRule;
}

void *NclPresentationControlConverter::createSwitch( util::xml::dom::Node parentElement ) {
	bptn::NclDocument *document;
	bptns::SwitchNode *switchNode;
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	if (!nodeHnd->hasAttribute(parentElement, "id")) {
		LERROR("NclPresentationControlConverter", "A switch element was declared without an id attribute.");
		return NULL;
	}

	std::string id = nodeHnd->attribute(parentElement, "id");
	bptnc::Node *node = ((NclDocumentConverter*) getDocumentParser())->getNode( id );
	if (node != NULL) {
		LERROR("NclPresentationControlConverter", "there is another node element previously declared with the same %s id.", id.c_str());
		return NULL;
	}

	if (nodeHnd->hasAttribute(parentElement, "refer")) {
		std::string attValue = nodeHnd->attribute(parentElement, "refer");
		bptn::Entity *referNode=NULL;

		try {
			referNode = (bptns::SwitchNode*) ((NclDocumentConverter*) getDocumentParser())->getNode( attValue );

			if (referNode == NULL) {
				document = (bptn::NclDocument*) getDocumentParser()->getObject( "return", "document" );

				referNode = (bptns::SwitchNode*) document->getNode( attValue );
				if (referNode == NULL) {
					referNode = new bptnr::ReferredNode( attValue, (void*) parentElement );
				}
			}

		} catch (...) {
			LERROR("NclPresentationControlConverter", "the switch element refers to %s object, which is not a switch element", attValue.c_str());
			return NULL;
		}

		node = new bptnr::ReferNode( id );
		((bptnr::ReferNode*) node)->setReferredEntity( referNode );

		return node;
	}

	switchNode = new bptns::SwitchNode( id );
	(*switchConstituents)[switchNode->getId()] = new std::map<std::string, bptnc::NodeEntity*>;

	return switchNode;
}

void *NclPresentationControlConverter::createRuleBase( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptns::RuleBase *ruleBase = new bptns::RuleBase( nodeHnd->attribute(parentElement, "id") );
	return ruleBase;
}

void *NclPresentationControlConverter::createRule( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	std::string var = nodeHnd->attribute(parentElement, "var");
	std::string value = nodeHnd->attribute(parentElement, "value");
	boost::trim( var );
	boost::trim( value );

	short ruleOp = convertComparator( nodeHnd->attribute(parentElement, "comparator") );
	bptns::SimpleRule *simplePresentationRule = new bptns::SimpleRule( nodeHnd->attribute(parentElement, "id"), var.c_str(), ruleOp, value.c_str() );

	return simplePresentationRule;
}

void *NclPresentationControlConverter::createDescriptorSwitch( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptns::DescriptorSwitch *descriptorSwitch = new bptns::DescriptorSwitch( nodeHnd->attribute(parentElement, "id") );
	// vetores para conter componentes e regras do switch
	(*switchConstituents)[descriptorSwitch->getId()] = new std::map<std::string, bptnc::NodeEntity*>;

	return descriptorSwitch;
}

void NclPresentationControlConverter::addDescriptorToDescriptorSwitch( void *parentObject, void *childObject ) {
	try {
		if (switchConstituents->count( ((bptns::DescriptorSwitch*) parentObject)->getId() ) != 0) {
			std::map<std::string, bptnc::NodeEntity*> *descriptors =
				(*switchConstituents)[((bptns::DescriptorSwitch*) parentObject)->getId()];

			if (descriptors->count( ((bptnd::GenericDescriptor*) childObject)->getId() ) == 0) {
				(*descriptors)[((bptnd::GenericDescriptor*) childObject)->getId()] = (bptnc::NodeEntity*) childObject;
			}
		}
	} catch (...) {}
}

void NclPresentationControlConverter::addImportBaseToRuleBase( void *parentObject, void *childObject ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	// apanha o alias a localizacao da base
	std::string baseAlias = nodeHnd->attribute(static_cast<util::xml::dom::Node>(childObject), "alias");
	std::string baseLocation = nodeHnd->attribute(static_cast<util::xml::dom::Node>(childObject), "documentURI");

	NclDocumentConverter *compiler = (NclDocumentConverter*) getDocumentParser();
	bptn::NclDocument *importedDocument = compiler->importDocument( baseLocation );
	if (importedDocument == NULL) {
		return;
	}

	bptns::RuleBase *createdBase = importedDocument->getRuleBase();
	if (createdBase == NULL) {
		return;
	}

	// insere a base compilada na base do documento
	try {
		((bptns::RuleBase*) parentObject)->addBase( createdBase, baseAlias, baseLocation );
	} catch (...) {}
}

void NclPresentationControlConverter::addBindRuleToDescriptorSwitch( void *parentObject, void *childObject ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptns::DescriptorSwitch *descriptorSwitch = (bptns::DescriptorSwitch*) parentObject;
	util::xml::dom::Node bindRule = static_cast<util::xml::dom::Node>(childObject);

	if (switchConstituents->count( descriptorSwitch->getId() ) == 0) {
		return;
	}

	std::map<std::string, bptnc::NodeEntity*> *descriptors = (*switchConstituents)[descriptorSwitch->getId()];
	if (descriptors->count(nodeHnd->attribute(bindRule, "constituent")) == 0) {
		return;
	}

	bptnd::GenericDescriptor *descriptor = (bptnd::GenericDescriptor*) (*descriptors)[nodeHnd->attribute(bindRule, "constituent")];
	if (descriptor == NULL) {
		return;
	}

	bptn::NclDocument *document = (bptn::NclDocument*) getDocumentParser()->getObject( "return", "document" );
	bptns::Rule *ncmRule = document->getRule( nodeHnd->attribute(bindRule, "rule") );
	if (ncmRule == NULL) {
		return;
	}

	descriptorSwitch->addDescriptor( descriptor, ncmRule );
}

void NclPresentationControlConverter::addBindRuleToSwitch( void *parentObject, void *childObject ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptns::SwitchNode *switchNode = (bptns::SwitchNode*) parentObject;
	util::xml::dom::Node bindRule = static_cast<util::xml::dom::Node>(childObject);

	if (switchConstituents->count( switchNode->getId() ) == 0) {
		return;
	}

	std::map<std::string, bptnc::NodeEntity*> *nodes = (*switchConstituents)[switchNode->getId()];
	if (nodes->count(nodeHnd->attribute(bindRule, "constituent")) == 0) {
		return;
	}

	bptnc::Node *node = (bptnc::NodeEntity*) (*nodes)[nodeHnd->attribute(bindRule, "constituent")];
	if (node == NULL) {
		return;
	}

	bptn::NclDocument *document = (bptn::NclDocument*) getDocumentParser()->getObject( "return", "document" );
	bptns::Rule *ncmRule = document->getRule( nodeHnd->attribute(bindRule, "rule"));
	if (ncmRule == NULL) {
		return;
	}

	switchNode->addNode( node, ncmRule );
}

void NclPresentationControlConverter::addDefaultComponentToSwitch( void *parentObject, void *childObject ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptns::SwitchNode *switchNode = (bptns::SwitchNode*) parentObject;
	util::xml::dom::Node defaultComponent = static_cast<util::xml::dom::Node>(childObject);

	if (switchConstituents->count( switchNode->getId() ) == 0) {
		return;
	}

	std::map<std::string, bptnc::NodeEntity*> *nodes = (*switchConstituents)[switchNode->getId()];
	if (nodes->count(nodeHnd->attribute(defaultComponent, "component")) == 0) {
		return;
	}

	bptnc::NodeEntity *node = (bptnc::NodeEntity*) (*nodes)[nodeHnd->attribute(defaultComponent, "component")];
	if (node == NULL) {
		return;
	}

	switchNode->setDefaultNode( node );
}

void NclPresentationControlConverter::addDefaultDescriptorToDescriptorSwitch( void *parentObject, void *childObject ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptns::DescriptorSwitch *descriptorSwitch = (bptns::DescriptorSwitch*) parentObject;
	util::xml::dom::Node defaultDescriptor = static_cast<util::xml::dom::Node>(childObject);

	if (switchConstituents->count( descriptorSwitch->getId() ) == 0) {
		return;
	}

	std::map<std::string, bptnc::NodeEntity*> *descriptors = (*switchConstituents)[descriptorSwitch->getId()];
	if (descriptors->count(nodeHnd->attribute(defaultDescriptor, "descriptor")) == 0) {
		return;
	}

	bptnd::GenericDescriptor *descriptor = (bptnd::GenericDescriptor*) (*descriptors)[nodeHnd->attribute(defaultDescriptor, "descriptor")];
	if (descriptor == NULL) {
		return;
	}

	descriptorSwitch->setDefaultDescriptor( descriptor );
}

void NclPresentationControlConverter::addContextToSwitch( void *parentObject, void *childObject ) {
	addNodeToSwitch( (bptns::SwitchNode*) parentObject, (bptnc::NodeEntity*) childObject );
}

void NclPresentationControlConverter::addMediaToSwitch( void *parentObject, void *childObject ) {
	addNodeToSwitch( (bptns::SwitchNode*) parentObject, (bptnc::NodeEntity*) childObject );
}

void NclPresentationControlConverter::addSwitchToSwitch( void *parentObject, void *childObject ) {
	addNodeToSwitch( (bptns::SwitchNode*) parentObject, (bptnc::NodeEntity*) childObject );
}

void NclPresentationControlConverter::addNodeToSwitch( bptns::SwitchNode *switchNode, bptnc::NodeEntity *node ) {
	if (switchConstituents->count( switchNode->getId() ) == 0) {
		(*switchConstituents)[switchNode->getId()] = new std::map<std::string, bptnc::NodeEntity*>;
	}

	std::map<std::string, bptnc::NodeEntity*>* nodes = (*switchConstituents)[switchNode->getId()];
	if (nodes->count( node->getId() ) == 0) {
		(*nodes)[node->getId()] = node;
	}
}

void *NclPresentationControlConverter::createBindRule( util::xml::dom::Node parentElement ) {
	return parentElement;
}

void *NclPresentationControlConverter::createDefaultComponent( util::xml::dom::Node parentElement ) {
	return parentElement;
}

void *NclPresentationControlConverter::createDefaultDescriptor( util::xml::dom::Node parentElement ) {
	return parentElement;
}

void *NclPresentationControlConverter::posCompileSwitch( util::xml::dom::Node parentElement, void *parentObject ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node element, elementNodeList ) {
		const std::string &elementTagName = nodeHnd->tagName(element);
		bptnc::Node *elementObject;
		if (elementTagName.compare( "context" ) == 0) {
			std::string temp = nodeHnd->attribute( element, "id" );
			elementObject = static_cast<bptnc::Node*>( ((NclDocumentConverter*) getDocumentParser())->getNode( temp ));

			if ((elementObject != NULL) && (elementObject->instanceOf("ContextNode"))) {
				((bptcfn::NclComponentsParser*)NclPresentationControlParser::getComponentsParser())
					->posCompileContext(element,static_cast<bptnc::ContextNode*>(elementObject));
			}
		} else if (elementTagName.compare("switch") == 0) {
			elementObject = ((NclDocumentConverter*)getDocumentParser())->getNode(nodeHnd->attribute(element, "id"));
			if (elementObject->instanceOf("SwitchNode")) {
				posCompileSwitch(element, elementObject);
			}
		}
	}

	return NclPresentationControlParser::posCompileSwitch( parentElement, parentObject );
}

short NclPresentationControlConverter::convertComparator( const std::string &comparator ) {
	if (comparator == "eq")
		return ncl_util::Comparator::CMP_EQ;
	else if (comparator == "ne")
		return ncl_util::Comparator::CMP_NE;
	else if (comparator == "gt")
		return ncl_util::Comparator::CMP_GT;
	else if (comparator == "lt")
		return ncl_util::Comparator::CMP_LT;
	else if (comparator == "ge")
		return ncl_util::Comparator::CMP_GTE;
	else if (comparator == "le")
		return ncl_util::Comparator::CMP_LTE;
	return -1;
}

}
}
}
}
}
