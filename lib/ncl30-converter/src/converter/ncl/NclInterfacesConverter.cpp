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

#include "../../../include/ncl/NclInterfacesConverter.h"
#include "../../../include/ncl/NclDocumentConverter.h"
#include <ncl30/components/Node.h>
#include <ncl30/components/CompositeNode.h>
#include <ncl30/components/NodeEntity.h>
#include <ncl30/interfaces/PropertyAnchor.h>
#include <ncl30/interfaces/InterfacePoint.h>
#include <ncl30/interfaces/RectangleSpatialAnchor.h>
#include <ncl30/interfaces/SampleIntervalAnchor.h>
#include <ncl30/interfaces/TextAnchor.h>
#include <ncl30/interfaces/LabeledAnchor.h>
#include <ncl30/interfaces/Port.h>
#include <ncl30/interfaces/IntervalAnchor.h>
#include <ncl30/interfaces/RelativeTimeIntervalAnchor.h>
#include <ncl30/interfaces/ContentAnchor.h>
#include <ncl30/interfaces/SwitchPort.h>
#include <ncl30/switches/SwitchNode.h>
#include "ncl30/util/functions.h"
#include <util/types.h>
#include <util/log.h>
#include <util/xml/nodehandler.h>
#include <boost/lexical_cast.hpp>
#include <cstdio>

namespace bptnc = ::br::pucrio::telemidia::ncl::components;
namespace bptni = ::br::pucrio::telemidia::ncl::interfaces;
namespace bptns = ::br::pucrio::telemidia::ncl::switches;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {

NclInterfacesConverter::NclInterfacesConverter( bptcf::DocumentParser *documentParser )
	: NclInterfacesParser( documentParser )
{
}

/**
 * Crea un port.
 * @param parentElement El elemento DOM que representa al port.
 * @param objGrandParent Un puntero a void que contiene el nodo en el que esta el port.
 * @return Una nueva instancia de @c Port.
 */
void *NclInterfacesConverter::createPort( util::xml::dom::Node parentElement, void *objGrandParent ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	std::string id = nodeHnd->attribute(parentElement, "id");
	if (id.empty()) {
		LERROR("NclInterfacesConverter", "a port element was declared without an id attribute.");
		return NULL;
	}

	bptnc::CompositeNode *context = (bptnc::CompositeNode*) objGrandParent;
	if (context->getPort( id ) != NULL) {
		LERROR("NclInterfacesConverter", "a port already exists with the same id=%s in context=%s", id.c_str(), context->getId().c_str());
		return NULL;
	}

	std::string attValue = nodeHnd->attribute(parentElement, "component");
	if (attValue.empty()) {
		LERROR("NclInterfacesConverter", "%s port must refer a context component using component attribute", id.c_str());
		return NULL;
	}
	
	bptnc::Node *portNode = context->getNode(attValue);
	if (portNode == NULL) {
		LERROR("NclInterfacesConverter", "Composition does not contain the referenced component=%s", attValue.c_str());
		return NULL;
	}

	bptnc::NodeEntity *portNodeEntity = static_cast<bptnc::NodeEntity*>(portNode->getDataEntity());
	bptni::InterfacePoint *portInterfacePoint = NULL;

// 	component = ;
	attValue = nodeHnd->attribute(parentElement, "interface");
	if (attValue.empty()) {
		// port element nao tem o atributo port, logo pegar a
		// ancora lambda do docment node->
		portInterfacePoint = portNodeEntity->getAnchor( 0 );

	} else {
		// tem-se o atributo port
		// tentar pegar a ancora do document node
		

		portInterfacePoint = portNodeEntity->getAnchor(attValue);
		if (portInterfacePoint == NULL && portNodeEntity->instanceOf( "CompositeNode" )) {

			// the interface may refer to a composition port and instead of
			// an anchor
			portInterfacePoint = ((bptnc::CompositeNode*) portNodeEntity)->getPort( attValue );
		}
	}

	if (portInterfacePoint == NULL) {
		LERROR("NclInterfacesConverter", "The referenced %s  component does not contain the referenced %s interface", portNode->getId().c_str(), attValue.c_str());
		return NULL;
	}
	
	bptni::Port *port = new bptni::Port( id, portNode, portInterfacePoint );
	return port;
}

bptni::SpatialAnchor *NclInterfacesConverter::createSpatialAnchor( util::xml::dom::Node areaElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptni::SpatialAnchor *anchor = NULL;

	std::string coords = nodeHnd->attribute(areaElement, "coords");
	if (!coords.empty()){
		std::string shape = nodeHnd->attribute(areaElement, "shape");
		if (shape.empty()) {
			shape = "rect";
		}

		if (shape == "rect" || shape == "default") {
			long int x1, y1, x2, y2;
			std::sscanf( coords.c_str(), "%ld,%ld,%ld,%ld", &x1, &y1, &x2, &y2 );
			anchor = new bptni::RectangleSpatialAnchor( nodeHnd->attribute(areaElement, "id"), x1, y1, x2 - x1, y2 - y1 );
		} else if (shape == "circle") {
			// TODO
		} else if (shape == "poly") {
			// TODO
		}
	}
	return anchor;
}

bptni::IntervalAnchor *NclInterfacesConverter::createTemporalAnchor( util::xml::dom::Node areaElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptni::IntervalAnchor *anchor = NULL;

	{
		if (nodeHnd->hasAttribute(areaElement, "begin") || nodeHnd->hasAttribute(areaElement, "end")) {
			double begVal = 0;
			double endVal = bptni::IntervalAnchor::OBJECT_DURATION;

			if (nodeHnd->hasAttribute(areaElement, "begin")) {
				util::DWORD duration;

				std::string begin = nodeHnd->attribute(areaElement, "begin");
				if( ncl_util::parseTimeString( begin, duration ) ) {
					begVal = duration;
				}
			}

			if (nodeHnd->hasAttribute(areaElement, "end" )) {
				util::DWORD duration;

				std::string end = nodeHnd->attribute(areaElement, "end");
				if (ncl_util::parseTimeString( end, duration )) {
					endVal = duration;
				}
			}

			LDEBUG("NclInterfacesConverter", "Creating RelativeTimeIntervalAnchor with begin=%f and end=%f", begVal, endVal);
			anchor = new bptni::RelativeTimeIntervalAnchor( nodeHnd->attribute(areaElement, "id"), begVal, endVal );
		}
	}

	{
		// region delimeted through sample identifications
		if (nodeHnd->hasAttribute(areaElement, "first" ) || nodeHnd->hasAttribute(areaElement, "last" )) {

			double begVal = 0;
			double endVal = bptni::IntervalAnchor::OBJECT_DURATION;
			short firstSyntax = bptni::ContentAnchor::CAT_NPT;
			short lastSyntax = bptni::ContentAnchor::CAT_NPT;
			std::string begin;

			if (nodeHnd->hasAttribute(areaElement, "first")) {
				begin = nodeHnd->attribute(areaElement, "first");

				if (begin.find( "s" ) != std::string::npos) {
					firstSyntax = bptni::ContentAnchor::CAT_SAMPLES;
					begVal = boost::lexical_cast<double>( begin.substr( 0, begin.length() - 1 ) );

				} else if (begin.find( "f" ) != std::string::npos) {
					firstSyntax = bptni::ContentAnchor::CAT_FRAMES;
					begVal = boost::lexical_cast<double>( begin.substr( 0, begin.length() - 1 ) );

				} else if (begin.find( "npt" ) != std::string::npos) {
					firstSyntax = bptni::ContentAnchor::CAT_NPT;
					begVal = boost::lexical_cast<double>( begin.substr( 0, begin.length() - 3 ) );
				}
			}

			if (nodeHnd->hasAttribute(areaElement, "last")) {
				std::string end = nodeHnd->attribute(areaElement, "last");

				if (begin.find( "s" ) != std::string::npos) {
					lastSyntax = bptni::ContentAnchor::CAT_SAMPLES;
					endVal = boost::lexical_cast<double>( end.substr( 0, end.length() - 1 ) );

				} else if (begin.find( "f" ) != std::string::npos) {
					lastSyntax = bptni::ContentAnchor::CAT_FRAMES;
					endVal = boost::lexical_cast<double>( end.substr( 0, end.length() - 1 ) );

				} else if (begin.find( "npt" ) != std::string::npos) {
					lastSyntax = bptni::ContentAnchor::CAT_NPT;
					endVal = boost::lexical_cast<double>( end.substr( 0, end.length() - 3 ) );
				}
			}

			LDEBUG("NclInterfacesConverter", "Creating SampleIntervalAnchor with first=%f and last=%f", begVal, endVal);
			anchor = new bptni::SampleIntervalAnchor( nodeHnd->attribute(areaElement, "id"), begVal, endVal );

			((bptni::SampleIntervalAnchor*) anchor)->setValueSyntax( firstSyntax, lastSyntax );

		}
	}
	return anchor;
}

/**
 * Crea una propiedad a partir de un elemento DOM.
 * @param parentElement El elemento DOM que tiene la información de la propiedad.
 * @return Una nueva instancia de @c PropertyAnchor.
 */
void *NclInterfacesConverter::createProperty( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	std::string attributeName = nodeHnd->attribute(parentElement, "name");
	if (attributeName.empty()) {
		LERROR("NclInterfacesConverter", "A property element=%s was declared without a name attribute", nodeHnd->tagName(parentElement).c_str());
		return NULL;
	}

	bptni::PropertyAnchor *anchor = new bptni::PropertyAnchor(attributeName);
	std::string attributeValue = nodeHnd->attribute(parentElement, "value");
	if (!attributeValue.empty()) {
		anchor->setPropertyValue(attributeValue);
	}
	return anchor;
}

void *NclInterfacesConverter::createArea( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptni::Anchor *anchor = NULL;

	std::string anchorId = nodeHnd->attribute(parentElement, "id");
	if (anchorId.empty()) {
		LERROR("NclInterfacesConverter", "A media interface element=%s was declared without an id attribute", nodeHnd->tagName(parentElement).c_str());
		return NULL;
	}

	if (nodeHnd->hasAttribute(parentElement, "begin") || nodeHnd->hasAttribute(parentElement, "end") ||
	    nodeHnd->hasAttribute(parentElement, "first") || nodeHnd->hasAttribute(parentElement, "last")) {

		anchor = createTemporalAnchor( parentElement );
		// ancora textual
	} else if (nodeHnd->hasAttribute(parentElement, "text")) {
		std::string position = nodeHnd->attribute(parentElement,  "position");
		anchor = new bptni::TextAnchor( anchorId, nodeHnd->attribute(parentElement,  "text" ), atoi(position.c_str()) );
	} else if (nodeHnd->hasAttribute(parentElement, "coords")) {
		anchor = createSpatialAnchor( parentElement );
	} else if (nodeHnd->hasAttribute(parentElement, "label")) {
		std::string anchorLabel = nodeHnd->attribute(parentElement,  "label");
		anchor = new bptni::LabeledAnchor( anchorId, anchorLabel );
	} else {
		anchor = new bptni::LabeledAnchor( anchorId, anchorId );
	}

	if (anchor == NULL) {
		LERROR("NclInterfacesConverter", "error on createArea");
		return NULL;
	}

	return anchor;
}

bptni::Port *NclInterfacesConverter::createMapping( util::xml::dom::Node parentElement, bptni::SwitchPort *objGrandParent ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	util::xml::dom::Node switchElement = nodeHnd->parent(nodeHnd->parent(parentElement));
	bptns::SwitchNode *switchNode = (bptns::SwitchNode*) ((NclDocumentConverter*) getDocumentParser())->getNode( nodeHnd->attribute(switchElement, "id") );

	std::string componentValue = nodeHnd->attribute(parentElement, "component");

	bptnc::Node *mappingNode = switchNode->getNode(componentValue);
	if (mappingNode == NULL) {
		LERROR("NclInterfacesConverter", "A mapping element points to a node (%s) not contained by the %s switch", componentValue.c_str(), switchNode->getId().c_str());
		return NULL;
	}

	bptnc::NodeEntity *mappingNodeEntity = (bptnc::NodeEntity*) mappingNode->getDataEntity();
	std::string interfaceValue = nodeHnd->attribute(parentElement, "interface");
	bptni::InterfacePoint *interfacePoint;
	if (! interfaceValue.empty()) {
		// tem-se o atributo, tentar pegar a ancora do document node
		interfacePoint = mappingNodeEntity->getAnchor(interfaceValue);
		if (interfacePoint == NULL) {
			// ou o document node era um terminal node e nao
			// possuia a ancora (erro), ou port indicava uma porta em uma
			// composicao
			if (mappingNodeEntity->instanceOf( "CompositeNode" )) {
				interfacePoint = ((bptnc::CompositeNode*) mappingNodeEntity)->getPort(interfaceValue);
			}
		}
	} else {
		// port element nao tem o atributo port, logo pegar a
		// ancora lambda do no->
		interfacePoint = mappingNodeEntity->getAnchor( 0 );
	}

	if (interfacePoint == NULL) {
		LERROR("NclInterfacesConverter", "a mapping element points to a node interface (%s) not contained by the %s node", interfaceValue.c_str(), mappingNode->getId().c_str());
		return NULL;
	}

	bptni::Port *port = new bptni::Port(objGrandParent->getId(), mappingNode, interfacePoint);
	return port;
}

bptni::SwitchPort *NclInterfacesConverter::createSwitchPort( util::xml::dom::Node parentElement, void *objGrandParent ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	std::string id = nodeHnd->attribute(parentElement, "id");
	if (id.empty()) {
		LERROR("NclInterfacesConverter", "the switch port element was declared without an id attribute.");
		return NULL;
	}

	bptns::SwitchNode *switchNode = (bptns::SwitchNode*) objGrandParent;
	if (switchNode->getPort( id ) != NULL) {
		LERROR("NclInterfacesConverter", "a port already exists with the same %s id in %s context", id.c_str(), switchNode->getId().c_str());
		return NULL;
	}

	bptni::SwitchPort *switchPort = new bptni::SwitchPort( id, switchNode );
	return switchPort;
}

/**
 * Agrega un port a un switchPort.
 * @param parentObject La instancia de @c SwitchPort a la que se agregará el port.
 * @param childObject La instancia de @c Port que se agregará.
 */
void NclInterfacesConverter::addMappingToSwitchPort(bptni::SwitchPort *parentObject, bptni::Port *childObject) {
	parentObject->addPort(childObject);
}

}
}
}
}
}
