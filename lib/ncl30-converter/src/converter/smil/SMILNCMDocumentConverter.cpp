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

#include "../../../include/smil/SMILNCMDocumentConverter.h"
#include <ncl30/NclDocument.h>
#include <ncl30/connectors/SimpleCondition.h>
#include <ncl30/connectors/SimpleAction.h>
#include <ncl30/descriptor/DescriptorBase.h>
#include <ncl30/descriptor/Descriptor.h>
#include <ncl30/connectors/CausalConnector.h>
#include <ncl30/connectors/ConnectorBase.h>
#include <ncl30/connectors/ConditionExpression.h>
#include <ncl30/connectors/Role.h>
#include <ncl30/connectors/Action.h>
#include <ncl30/components/ContextNode.h>
#include <ncl30/components/ContentNode.h>
#include <ncl30/interfaces/Port.h>
#include <ncl30/interfaces/InterfacePoint.h>
#include <ncl30/interfaces/Anchor.h>
#include <ncl30/link/CausalLink.h>
#include <ncl30/layout/RegionBase.h>
#include <ncl30/layout/LayoutRegion.h>
#include <util/log.h>
#include <util/xml/nodehandler.h>
#include <util/xml/documenthandler.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace bptn = ::br::pucrio::telemidia::ncl;
namespace bptnd = ::br::pucrio::telemidia::ncl::descriptor;
namespace bptncon = ::br::pucrio::telemidia::ncl::connectors;
namespace bptnc = ::br::pucrio::telemidia::ncl::components;
namespace bptni = ::br::pucrio::telemidia::ncl::interfaces;
namespace bptnli = ::br::pucrio::telemidia::ncl::link;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace smil {

SmilNcmDocumentConverter::SmilNcmDocumentConverter()
{
	this->autoId = 1;
}

std::string SmilNcmDocumentConverter::getNextId() {
	std::string id = "_autoId", num;
	int n = autoId++;

	while (n) {
		num = (char) ('0' + n % 10) + num;
		n /= 10;
	}

	return id + num;
}

void *SmilNcmDocumentConverter::parseRootElement( util::xml::dom::Node rootElement ) {
	const std::string &tagName = getDocumentHandler()->handler()->tagName( rootElement );
	if (tagName == "smil") {
		return parseSmil( rootElement );
	} else {
		LERROR("SmilNcmDocumentParser", "Trying to parse a non NCL document. rootElement->getTagName = '%s'", tagName.c_str());
		return NULL;
	}
}

void *SmilNcmDocumentConverter::parseSmil( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getDocumentHandler()->handler();

	void *parentObject = NULL;
	void *elementObject = NULL;
	bool parseHeadOk = false;

	parentObject = createSmil( parentElement );
	if (parentObject == NULL) {
		return NULL;
	}

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node node, elementNodeList ) {
		if (boost::iequals(nodeHnd->tagName(node), "head")) {
			elementObject = parseHead();
			parseHeadOk = true;
			if (elementObject != NULL) {
				//addHeadToSmil(parentObject, elementObject);
				break;
			}
		}
	}

	if (!parseHeadOk) {
		parseHead();
	}

	BOOST_FOREACH( util::xml::dom::Node node, elementNodeList ) {
		if (boost::iequals(nodeHnd->tagName(node), "body")) {
			elementObject = parseBody( node );
			if (elementObject != NULL) {
				//posCompileBody((DOMElement*)node, elementObject);
				//addBodyToSmil(parentObject, elementObject);
				break;
			}
		}
	}

	return parentObject;
}

void *SmilNcmDocumentConverter::createSmil( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getDocumentHandler()->handler();
	std::string docName;

	if (nodeHnd->hasAttribute(parentElement, "id")) {
		docName = nodeHnd->attribute(parentElement, "id");
	}

	if (docName == "") {
		docName = "smilDocument";
	}

	bptn::NclDocument *document = new bptn::NclDocument( docName );
	addObject( "return", "document", document );
	return document;
}

void *SmilNcmDocumentConverter::parseHead() {
	bptn::NclDocument *document;
	bptnl::RegionBase *rBase;
	bptnl::LayoutRegion *region;

	document = (bptn::NclDocument*) getObject( "return", "document" );

	rBase = new bptnl::RegionBase( "__rgBase" );
	region = new bptnl::LayoutRegion( "_fullScreen" );
	region->setProperty( "left", "0");
	region->setProperty( "top", "0");
	region->setProperty( "width", "100%");
	region->setProperty( "height", "100%");
	rBase->addRegion( region );
	document->addRegionBase( rBase );

	bptnd::DescriptorBase *dBase;
	bptnd::Descriptor *descriptor;
	dBase = new bptnd::DescriptorBase( "_dsBase" );
	descriptor = new bptnd::Descriptor( "_dsFullScreen" );
	descriptor->setRegion( region );
	dBase->addDescriptor( descriptor );
	document->setDescriptorBase( dBase );

	bptncon::ConnectorBase *connBase;
	bptncon::CausalConnector *conn;

	connBase = new bptncon::ConnectorBase( "_connBase" );

	bptncon::ConditionExpression *onEnd = new bptncon::SimpleCondition( "onEnd" );
	bptncon::Action *start = new bptncon::SimpleAction( "start" );
	((bptncon::Role*) start)->setMaxCon( bptncon::Role::UNBOUNDED );
	conn = new bptncon::CausalConnector( "onEndStart", onEnd, start );
	connBase->addConnector( conn );

	bptncon::ConditionExpression *onBegin = new bptncon::SimpleCondition( "onBegin" );
	;
	start = new bptncon::SimpleAction( "start" );
	((bptncon::Role*) start)->setMaxCon( bptncon::Role::UNBOUNDED );
	conn = new bptncon::CausalConnector( "onBeginStart", onBegin, start );
	connBase->addConnector( conn );
	document->setConnectorBase( connBase );

	return NULL;
}

void *SmilNcmDocumentConverter::createHead( util::xml::dom::Node parentElement ) {
	return parentElement;
}

void *SmilNcmDocumentConverter::parseBody( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getDocumentHandler()->handler();
	bool firstChildNode = true;

	void *parentObject = createBody( parentElement );
	if (parentObject == NULL) {
		return NULL;
	}

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node node, elementNodeList ) {
		void *elementObject = NULL;
		const std::string &tagName = nodeHnd->tagName(node);
		if (boost::iequals( tagName, "seq")) {
			elementObject = parseSeq( node );
		} else if (boost::iequals( tagName, "par")) {
			elementObject = parsePar( node );
		} else if (boost::iequals( tagName, "text")
		        || boost::iequals( tagName, "img")
		        || boost::iequals( tagName, "audio")
		        || boost::iequals( tagName, "video")
		        || boost::iequals( tagName, "ref")
		        || boost::iequals( tagName, "textstream")
		        || boost::iequals( tagName, "animation")) {

			elementObject = parseMedia( node );
		}

		if (elementObject != NULL) {
			((bptnc::ContextNode*) parentObject)->addNode( (bptnc::Node*) elementObject );

			if (firstChildNode) {
				std::string pId = "p" + ((bptnc::ContextNode*) parentObject)->getId();
				bptnc::Node *portNode = (bptnc::Node*) elementObject;

				bptnc::NodeEntity *portNodeEntity;
				portNodeEntity = (bptnc::NodeEntity*) portNode->getDataEntity();

				bptni::Port *port = new bptni::Port( pId, portNode, portNodeEntity->getAnchor( 0 ) );
				((bptnc::ContextNode*) parentObject)->addPort( port );
				firstChildNode = false;
			}
		}
	}

	return posCompileBody( parentObject );
}

void *SmilNcmDocumentConverter::createBody( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getDocumentHandler()->handler();

	// criar composicao a partir do elemento body do documento ncl
	// fazer uso do nome da composicao body que foi atribuido pelo
	// compilador
	bptn::NclDocument *document = (bptn::NclDocument*) getObject( "return", "document" );
	bptnc::ContextNode *context;

	if (!nodeHnd->hasAttribute(parentElement, "id")) {
		nodeHnd->setAttribute(parentElement, "id", document->getId());
		context = (bptnc::ContextNode*) createContext( parentElement );
		nodeHnd->removeAttribute(parentElement, "id");
	} else {
		context = (bptnc::ContextNode*) createContext( parentElement );
	}
	return context;
}

void *SmilNcmDocumentConverter::posCompileBody( void *parentObject ) {
	bptn::NclDocument *document;
	document = (bptn::NclDocument*) getObject( "return", "document" );

	document->setBody( (bptnc::ContextNode*) parentObject );

	std::vector<bptnc::Node*> *nodes = ((bptnc::CompositeNode*) parentObject)->getNodes();

	bptncon::Connector *conn = document->getConnectorBase()->getConnector( "onEndStart" );

	for (int i = 0; i < (int) nodes->size() - 1; i++) {
		bptnc::Node *cond = (*nodes)[i];
		bptnc::Node *act = (*nodes)[i + 1];
		bptnli::CausalLink *link = new bptnli::CausalLink( getNextId(), conn );

		/*
		 anchorNodeEntity = (NodeEntity*)cond->getDataEntity();
		 InterfacePoint *interfCond = anchorNodeEntity->getAnchor(0);
		 link->bind(cond, interfCond, NULL, "onEnd");

		 anchorNodeEntity = (NodeEntity*)act->getDataEntity();
		 InterfacePoint *interfAct = anchorNodeEntity->getAnchor(0);
		 link->bind(act, interfAct, NULL, "start");
		 */

		bptni::InterfacePoint *interfCond = cond->getAnchor( 0 );
		link->bind( cond, interfCond, NULL, "onEnd" );

		bptni::InterfacePoint *interfAct = act->getAnchor( 0 );
		link->bind( act, interfAct, NULL, "start" );

		((bptnc::ContextNode*) parentObject)->addLink( link );
	}

	return parentObject;
}

void *SmilNcmDocumentConverter::createContext( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getDocumentHandler()->handler();

	std::string id;

	if (!nodeHnd->hasAttribute(parentElement, "id")) {
		/*
		 for (i = 0; i < size; i++) {
		 node = elementNodeList->item(i);
		 if (node->getNodeType()==DOMNode::ELEMENT_NODE &&
		 boost::iequals(((DOMElement*)node)->
		 getTagName(), XMLString::transcode("body") )==0) {

		 elementObject = parseBody((DOMElement*)node, parentObject);
		 if (elementObject != NULL) {
		 //posCompileBody((DOMElement*)node, elementObject);
		 //addBodyToSmil(parentObject, elementObject);
		 break;
		 }
		 }
		 }                          void *objGrandParent) {
		 string id;
		 ContentNode *media;



		 return NULL;
		 */
		id = getNextId();
	} else {
		id = nodeHnd->attribute(parentElement, "id");
	}

	/*
	 node = ((NclDocumentConverter*)getDocumentParser())->getNode(id);
	 if (node != NULL) {
	 return NULL;
	 }
	 */

	//retornar nova composicao ncm a partir do elemento xml que a
	//representa em NCL
	bptnc::ContextNode *context = new bptnc::ContextNode( id );

	return context;
}

void *SmilNcmDocumentConverter::parseMedia( util::xml::dom::Node parentElement ) {
	void *parentObject = NULL;

	parentObject = createMedia( parentElement );
	return parentObject;
}

void *SmilNcmDocumentConverter::createMedia( util::xml::dom::Node parentElement ) {
	std::string id;
	bptnc::ContentNode *media;
	bptnd::GenericDescriptor *descriptor;
	bptn::NclDocument *document;
	util::xml::dom::NodeHandler *nodeHnd = getDocumentHandler()->handler();

	if (!nodeHnd->hasAttribute(parentElement, "id")) {
		id = getNextId();
	} else {
		id = nodeHnd->attribute(parentElement, "id");
	}
	media = new bptnc::ContentNode( id, NULL );

	if (nodeHnd->hasAttribute(parentElement, "src")) {
		((bptnc::ContentNode*) media)->setUrl( getDocumentPath(), nodeHnd->attribute(parentElement, "src") );
	}

	if (nodeHnd->tagName(parentElement).compare( "audio" ) == 0) {
		document = (bptn::NclDocument*) getObject( "return", "document" );
		descriptor = document->getDescriptor( "_dsFullScreen" );
		media->setDescriptor( descriptor );
	}
	return media;
}

void *SmilNcmDocumentConverter::parseSeq( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getDocumentHandler()->handler();
	bool firstChildNode = true;

	void *parentObject = createContext( parentElement );
	if (parentObject == NULL) {
		return NULL;
	}

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node node, elementNodeList ) {
		void *elementObject = NULL;
		const std::string &tagName = nodeHnd->tagName(node);
		if (boost::iequals(tagName, "seq")) {
			elementObject = parseSeq( node );
		} else if (boost::iequals(tagName, "par")) {
			elementObject = parsePar( node );
		} else if (boost::iequals(tagName, "text")
		        || boost::iequals(tagName, "img")
		        || boost::iequals(tagName, "audio")
		        || boost::iequals(tagName, "video")
		        || boost::iequals(tagName, "ref")
		        || boost::iequals(tagName, "textstream")
		        || boost::iequals(tagName, "animation")) {

			elementObject = parseMedia( node );
		}
		if (elementObject != NULL) {
			((bptnc::ContextNode*) parentObject)->addNode( (bptnc::Node*) elementObject );

			if (firstChildNode) {
				std::string pId = "p" + ((bptnc::ContextNode*) parentObject)->getId();
				bptnc::Node *portNode = (bptnc::Node*) elementObject;

				bptnc::NodeEntity *portNodeEntity;
				portNodeEntity = (bptnc::NodeEntity*) portNode->getDataEntity();

				bptni::Port *port = new bptni::Port( pId, portNode, portNodeEntity->getAnchor( 0 ) );
				((bptnc::ContextNode*) parentObject)->addPort( port );
				firstChildNode = false;
			}
		}
	}

	//posCompile: adding links
	std::vector<bptnc::Node*> *nodes = ((bptnc::CompositeNode*) parentObject)->getNodes();
	bptn::NclDocument *document = (bptn::NclDocument*) getObject( "return", "document" );
	bptncon::Connector *conn = document->getConnectorBase()->getConnector( "onEndStart" );

	for (int i = 0; i < (int) nodes->size() - 1; i++) {
		bptnc::Node *cond = (*nodes)[i];
		bptnc::Node *act = (*nodes)[i + 1];
		bptnli::CausalLink *link = new bptnli::CausalLink( getNextId(), conn );

		bptni::InterfacePoint *interfCond = cond->getAnchor( 0 );
		link->bind( cond, interfCond, NULL, "onEnd" );

		bptni::InterfacePoint *interfAct = act->getAnchor( 0 );
		link->bind( act, interfAct, NULL, "start" );

		((bptnc::ContextNode*) parentObject)->addLink( link );
	}

	return parentObject;

}

void *SmilNcmDocumentConverter::parsePar( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getDocumentHandler()->handler();
	int pCount = 1;

	void *parentObject = createContext( parentElement );
	if (parentObject == NULL) {
		return NULL;
	}

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node node, elementNodeList ) {
		void *elementObject = NULL;
		const std::string &tagName = nodeHnd->tagName(node);
		if (boost::iequals( tagName, "seq" )) {
			elementObject = parseSeq( node );
		} else if (boost::iequals( tagName, "par" )) {
			elementObject = parsePar( node );
		} else if (boost::iequals( tagName, "text" )
		        || boost::iequals( tagName, "img" )
		        || boost::iequals( tagName, "audio" )
		        || boost::iequals( tagName, "video" )
		        || boost::iequals( tagName, "ref" )
		        || boost::iequals( tagName, "textstream" )
		        || boost::iequals( tagName, "animation" )) {

			elementObject = parseMedia( node );
		}
		if (elementObject != NULL) {
			((bptnc::ContextNode*) parentObject)->addNode( (bptnc::Node*) elementObject );

			std::string nCount;
			int n = pCount++;
			while (n) {
				nCount = (char) (n % 10 + '0') + nCount;
				n /= 10;
			}

			std::string pId = "p" + ((bptnc::ContextNode*) parentObject)->getId() + nCount;
			bptnc::Node *portNode = (bptnc::Node*) elementObject;

			bptnc::NodeEntity *portNodeEntity;
			portNodeEntity = (bptnc::NodeEntity*) portNode->getDataEntity();

			bptni::Port *port = new bptni::Port( pId, portNode, portNodeEntity->getAnchor( 0 ) );
			((bptnc::ContextNode*) parentObject)->addPort( port );

		}
	}

	return parentObject;
}

}
}
}
}
}
