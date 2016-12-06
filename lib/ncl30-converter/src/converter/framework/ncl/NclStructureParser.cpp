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

#include "../../../../include/framework/ncl/NclStructureParser.h"
#include "../../../../include/framework/ncl/NclComponentsParser.h"
#include "../../../../include/framework/ncl/NclPresentationControlParser.h"
#include "../../../../include/framework/ncl/NclInterfacesParser.h"
#include "../../../../include/framework/ncl/NclImportParser.h"
#include "../../../../include/framework/ncl/NclLayoutParser.h"
#include "../../../../include/framework/ncl/NclTransitionParser.h"
#include "../../../../include/framework/ncl/NclPresentationSpecificationParser.h"
#include "../../../../include/framework/ncl/NclConnectorsParser.h"
#include "../../../../include/framework/ncl/NclMetainformationParser.h"
#include <util/xml/nodehandler.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {

NclStructureParser::NclStructureParser( DocumentParser *documentParser )
	: ModuleParser( documentParser )
{
}

void *NclStructureParser::parseBody( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	void *parentObject = createBody( parentElement );
	if (parentObject == NULL) {
		return NULL;
	}

	void *elementObject;
	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node element, elementNodeList ) {
		const std::string &elementTagName = nodeHnd->tagName(element);
		if (elementTagName.compare( "media" ) == 0) {
			elementObject = getComponentsParser()->parseMedia( element );
			if (elementObject != NULL) {
				addMediaToBody( parentObject, elementObject );
			}
		} else if (elementTagName.compare( "context" ) == 0) {
			elementObject = getComponentsParser()->parseContext( element );
			if (elementObject != NULL) {
				addContextToBody( parentObject, elementObject );
			}
		} else if (elementTagName.compare( "switch" ) == 0) {
			elementObject = getPresentationControlParser()->parseSwitch( element );
			if (elementObject != NULL)
				addSwitchToBody( parentObject, elementObject );
		}
	}

	BOOST_FOREACH( util::xml::dom::Node node, elementNodeList ) {
		if (boost::iequals(nodeHnd->tagName(node), "property")) {
			elementObject = getInterfacesParser()->parseProperty(node);
			if (elementObject != NULL) {
				addPropertyToBody( parentObject, elementObject );
			}
		}
	}
	return parentObject;
}

void *NclStructureParser::posCompileBody( util::xml::dom::Node parentElement, void *parentObject ) {

	return getComponentsParser()->posCompileContext( parentElement, static_cast<bptnc::ContextNode*>(parentObject));
}

void *NclStructureParser::parseHead( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	void *parentObject = createHead( parentElement );
	if (parentObject == NULL) {
		return NULL;
	}

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node element, elementNodeList ) {
		const std::string &elementTagName = nodeHnd->tagName(element);
		void *elementObject = NULL;
		if (elementTagName.compare( "importedDocumentBase" ) == 0) {
			elementObject = getImportParser()->parseImportedDocumentBase( element );
			if (elementObject == NULL)
				return NULL;
		} else if (elementTagName.compare( "regionBase" ) == 0) {
			elementObject = getLayoutParser()->parseRegionBase( element );
			if (elementObject != NULL)
				addRegionBaseToHead( elementObject );
			else
				return NULL;
		} else if (elementTagName.compare( "ruleBase" ) == 0) {
			elementObject = getPresentationControlParser()->parseRuleBase( element );
			if (elementObject != NULL)
				addRuleBaseToHead( elementObject );
		} else if (elementTagName.compare( "transitionBase" ) == 0) {
			elementObject = getTransitionParser()->parseTransitionBase( element );
			if (elementObject != NULL)
				addTransitionBaseToHead( elementObject );
		} else if (elementTagName.compare( "descriptorBase" ) == 0) {
			elementObject = getPresentationSpecificationParser()->parseDescriptorBase( element );
			if (elementObject != NULL)
				addDescriptorBaseToHead( elementObject );
		} else if (elementTagName.compare( "connectorBase" ) == 0) {
			elementObject = getConnectorsParser()->parseConnectorBase( element );
			if (elementObject != NULL)
				addConnectorBaseToHead( elementObject );
		} else if (elementTagName.compare( "meta" ) == 0) {
			elementObject = getMetainformationParser()->parseMeta( element );
			if (elementObject != NULL)
				addMetaToHead( elementObject );
		} else if (elementTagName.compare( "metadata" ) == 0) {
			elementObject = getMetainformationParser()->parseMetadata();
			if (elementObject != NULL)
				addMetadataToHead( elementObject );
		}
	}
	return parentObject;
}

void *NclStructureParser::parseNcl( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	void *parentObject = createNcl( parentElement );
	if (parentObject == NULL) {
		return NULL;
	}

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node node, elementNodeList ) {
		if (boost::iequals(nodeHnd->tagName(node), "head")) {
			void *elementObject = parseHead( node );
			if (elementObject != NULL) {
				break;
			} else {
				return NULL;
			}
		}
	}

	BOOST_FOREACH( util::xml::dom::Node node, elementNodeList ) {
		if (boost::iequals(nodeHnd->tagName(node), "body")) {
			void *elementObject = parseBody( node );
			if (elementObject != NULL) {
				posCompileBody( node, elementObject );
				break;
			} else {
				return NULL;
			}
		}
	}
	return parentObject;
}

NclTransitionParser* NclStructureParser::getTransitionParser() {
	return transitionParser;
}

void NclStructureParser::setTransitionParser( NclTransitionParser* transitionParser ) {

	this->transitionParser = transitionParser;
}

NclPresentationSpecificationParser *NclStructureParser::getPresentationSpecificationParser() {

	return presentationSpecificationParser;
}

void NclStructureParser::setPresentationSpecificationParser( NclPresentationSpecificationParser * presentationSpecificationParser ) {

	this->presentationSpecificationParser = presentationSpecificationParser;
}

NclComponentsParser *NclStructureParser::getComponentsParser() {
	return componentsParser;
}

void NclStructureParser::setComponentsParser( NclComponentsParser *componentsParser ) {

	this->componentsParser = componentsParser;
}

NclLinkingParser *NclStructureParser::getLinkingParser() {
	return linkingParser;
}

void NclStructureParser::setLinkingParser( NclLinkingParser *linkingParser ) {

	this->linkingParser = linkingParser;
}

NclLayoutParser *NclStructureParser::getLayoutParser() {
	return layoutParser;
}

void NclStructureParser::setLayoutParser( NclLayoutParser *layoutParser ) {

	this->layoutParser = layoutParser;
}

NclMetainformationParser*
NclStructureParser::getMetainformationParser() {

	return metainformationParser;
}

NclInterfacesParser *NclStructureParser::getInterfacesParser() {
	return interfacesParser;
}

void NclStructureParser::setInterfacesParser( NclInterfacesParser *interfacesParser ) {

	this->interfacesParser = interfacesParser;
}

NclPresentationControlParser *NclStructureParser::getPresentationControlParser() {

	return presentationControlParser;
}

void NclStructureParser::setPresentationControlParser( NclPresentationControlParser *presentationControlParser ) {

	this->presentationControlParser = presentationControlParser;
}

NclConnectorsParser *NclStructureParser::getConnectorsParser() {
	return connectorsParser;
}

void NclStructureParser::setConnectorsParser( NclConnectorsParser *connectorsParser ) {

	this->connectorsParser = connectorsParser;
}

NclImportParser *NclStructureParser::getImportParser() {
	return importParser;
}

void NclStructureParser::setImportParser( NclImportParser *importParser ) {

	this->importParser = importParser;
}

void NclStructureParser::setMetainformationParser( NclMetainformationParser* metainformationParser ) {

	this->metainformationParser = metainformationParser;
}
}
}
}
}
}
}
