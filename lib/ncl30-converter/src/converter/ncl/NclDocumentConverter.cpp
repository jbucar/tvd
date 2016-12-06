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

#include "../../../include/ncl/NclDocumentConverter.h"
#include "../../../include/ncl/NclConnectorsConverter.h"
#include "../../../include/ncl/NclImportConverter.h"
#include "../../../include/ncl/NclPresentationControlConverter.h"
#include "../../../include/ncl/NclComponentsConverter.h"
#include "../../../include/ncl/NclStructureConverter.h"
#include "../../../include/ncl/NclPresentationSpecConverter.h"
#include "../../../include/ncl/NclLayoutConverter.h"
#include "../../../include/ncl/NclInterfacesConverter.h"
#include "../../../include/ncl/NclLinkingConverter.h"
#include "../../../include/ncl/NclTransitionConverter.h"
#include "../../../include/ncl/NclMetainformationConverter.h"

#include "ncl30/NclDocument.h"
#include "ncl30/IPrivateBaseContext.h"
#include <util/xml/nodehandler.h>
#include <util/xml/documenthandler.h>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {

NclDocumentConverter::NclDocumentConverter( bptn::IPrivateBaseContext* baseContext ) :
		NclDocumentParser( ) {

	this->privateBaseContext = baseContext;
	this->ownManager = false;
	this->parseEntityVar = false;

	initialize();
	setDependencies();
}

NclDocumentConverter::~NclDocumentConverter() {
	if (structureParser != NULL) {
		delete structureParser;
		structureParser = NULL;
	}

	if (componentsParser != NULL) {
		delete componentsParser;
		componentsParser = NULL;
	}

	if (connectorsParser != NULL) {
		delete connectorsParser;
		connectorsParser = NULL;
	}

	if (linkingParser != NULL) {
		delete linkingParser;
		linkingParser = NULL;
	}

	if (interfacesParser != NULL) {
		delete interfacesParser;
		interfacesParser = NULL;
	}

	if (layoutParser != NULL) {
		delete layoutParser;
		layoutParser = NULL;
	}

	if (transitionParser != NULL) {
		delete transitionParser;
		transitionParser = NULL;
	}

	if (presentationControlParser != NULL) {
		delete presentationControlParser;
		presentationControlParser = NULL;
	}

	if (importParser != NULL) {
		delete importParser;
		importParser = NULL;
	}

	if (metainformationParser != NULL) {
		delete metainformationParser;
		metainformationParser = NULL;
	}
}

void NclDocumentConverter::initialize() {
	presentationSpecificationParser = new NclPresentationSpecConverter( this );

	structureParser = new NclStructureConverter( this );
	componentsParser = new NclComponentsConverter( this );
	connectorsParser = new NclConnectorsConverter( this );
	linkingParser = new NclLinkingConverter( this );

	interfacesParser = new NclInterfacesConverter( this );
	layoutParser = new NclLayoutConverter( this );
	transitionParser = new NclTransitionConverter( this );
	presentationControlParser = new NclPresentationControlConverter( this );
	importParser = new NclImportConverter( this );
	metainformationParser = new NclMetainformationConverter( this );
}

const std::string NclDocumentConverter::getAttribute( void* element, const std::string &attribute ) {
	return getDocumentHandler()->handler()->attribute( static_cast<util::xml::dom::Node>(element), attribute );
}

bptnc::Node *NclDocumentConverter::getNode( const std::string &nodeId ) {
	
	bptn::NclDocument *document = (bptn::NclDocument*) NclDocumentParser::getObject( "return", "document" );
	return document->getNode( nodeId );
}

bool NclDocumentConverter::removeNode( bptnc::Node* /*node*/ ) {
	
	/*bptn::NclDocument *document = (bptn::NclDocument*) */NclDocumentParser::getObject( "return", "document" );
	//return document->removeDocument()
	return true;
}

/**
 * Agrega un documento NCL al privateBase.
 * @param docLocation Un string con la ubicación del documento.
 */
bptn::NclDocument* NclDocumentConverter::importDocument( const std::string &docLocation ) {
	std::string uri;

	fs::path url( docLocation );

	if (!url.is_complete()) {
		url = getDocumentPath() / url;
	}

	return (bptn::NclDocument*) (privateBaseContext->addVisibleDocument( url.string() ));
}


void* NclDocumentConverter::parseEntity( std::string &entityLocation, bptn::NclDocument* document, void* parent ) {

	void* entity;

	parseEntityVar = true;
	parentObject = parent;
	addObject( "return", "document", document );
	entity = parse( entityLocation );
	parseEntityVar = false;
	return entity;
}

void* NclDocumentConverter::parseRootElement( util::xml::dom::Node rootElement ) {
	void* object;

	if (parseEntityVar) {
		const std::string &elementName = getDocumentHandler()->handler()->tagName( rootElement );
		if (elementName == "region") {
			return getLayoutParser()->parseRegion( rootElement );

		} else if (elementName == "regionBase") {
			return getLayoutParser()->parseRegionBase( rootElement );

		} else if (elementName == "transition") {
			return getTransitionParser()->parseTransition( rootElement );

		} else if (elementName == "transitionBase") {
			return getTransitionParser()->parseTransitionBase( rootElement );

		} else if (elementName == "rule") {
			return getPresentationControlParser()->parseRule( rootElement );

		} else if (elementName == "compositeRule") {
			return getPresentationControlParser()->parseRule( rootElement );

		} else if (elementName == "ruleBase") {
			return getPresentationControlParser()->parseRuleBase( rootElement );

		} else if (elementName == "causalConnector") {
			return getConnectorsParser()->parseCausalConnector( rootElement );

		} else if (elementName == "connectorBase") {
			return getConnectorsParser()->parseConnectorBase( rootElement );

		} else if (elementName == "descriptor") {
			return getPresentationSpecificationParser()->parseDescriptor( rootElement );

		} else if (elementName == "descriptorSwitch") {
			return getPresentationControlParser()->parseDescriptorSwitch( rootElement );

		} else if (elementName == "descriptorBase") {
			return getPresentationSpecificationParser()->parseDescriptorBase( rootElement );

		} else if (elementName == "importBase") {
			// return the root element
			return rootElement;

		} else if (elementName == "importedDocumentBase") {
			// return the root element
			return getImportParser()->parseImportedDocumentBase( rootElement );

		} else if (elementName == "importNCL") {
			// return the root element
			return rootElement;

		} else if (elementName == "media") {
			return getComponentsParser()->parseMedia( rootElement );

		} else if (elementName == "context") {
			object = getComponentsParser()->parseContext( rootElement );

			getComponentsParser()->posCompileContext( rootElement, static_cast<bptnc::ContextNode*>(object));
			return object;

		} else if (elementName == "switch") {
			object = getPresentationControlParser()->parseSwitch( rootElement );

			getPresentationControlParser()->posCompileSwitch( rootElement, object );

			return object;

		} else if (elementName == "link") {
			return getLinkingParser()->parseLink( rootElement, parentObject );

		} else if (elementName == "area") {
			return getInterfacesParser()->parseArea( rootElement );

		} else if (elementName == "property") {
			return getInterfacesParser()->parseProperty( rootElement );

		} else if (elementName == "port") {
			return getInterfacesParser()->parsePort( rootElement, parentObject );

		} else if (elementName == "switchPort") {
			return getInterfacesParser()->parseSwitchPort( rootElement, parentObject );

		} else {
			return NULL;
		}

	} else {
		return NclDocumentParser::parseRootElement( rootElement );
	}
}

/*
 void *NclDocumentConverter::parseRootElement(DOMElement *rootElement){
 if ( XMLString::compareIString( rootElement->getTagName(),
 XMLString::transcode("ncl") ) == 0) {

 return getStructureParser()->parseNcl(rootElement, NULL);
 }

 return NULL;
 }*/
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::converter::IDocumentConverter*
createNclDocumentConverter( bptn::IPrivateBaseContext* pbc ) {

	return new ::br::pucrio::telemidia::converter::ncl::NclDocumentConverter( pbc );
}

extern "C" void destroyNclDocumentConverter( ::br::pucrio::telemidia::converter::IDocumentConverter* doc ) {

	delete doc;
}
