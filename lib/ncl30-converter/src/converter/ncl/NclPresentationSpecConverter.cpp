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

#include "../../../include/ncl/NclPresentationSpecConverter.h"
#include "../../../include/ncl/NclImportConverter.h"
#include "../../../include/ncl/NclPresentationControlConverter.h"
#include "../../../include/ncl/NclDocumentConverter.h"
#include "../../../include/framework/ncl/NclLayoutParser.h"
#include "ncl30/Parameter.h"
#include "ncl30/NclDocument.h"
#include "ncl30/descriptor/Descriptor.h"
#include "ncl30/descriptor/DescriptorBase.h"
#include "ncl30/descriptor/GenericDescriptor.h"
#include "ncl30/switches/RuleBase.h"
#include "ncl30/navigation/FocusDecoration.h"
#include "ncl30/navigation/KeyNavigation.h"
#include "ncl30/transition/TransitionBase.h"
#include "ncl30/transition/Transition.h"
#include "ncl30/layout/RegionBase.h"
#include "ncl30/util/functions.h"
#include <util/types.h>
#include <util/log.h>
#include <util/xml/nodehandler.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace bptnd = ::br::pucrio::telemidia::ncl::descriptor;
namespace bptns = ::br::pucrio::telemidia::ncl::switches;
namespace bptnn = ::br::pucrio::telemidia::ncl::navigation;
namespace bptnt = ::br::pucrio::telemidia::ncl::transition;
namespace bptnl = ::br::pucrio::telemidia::ncl::layout;
namespace bptn = ::br::pucrio::telemidia::ncl;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {

NclPresentationSpecConverter::NclPresentationSpecConverter( bptcf::DocumentParser *documentParser )
	: NclPresentationSpecificationParser( documentParser )
{
}

void NclPresentationSpecConverter::addDescriptorToDescriptorBase( void *parentObject, void *childObject ) {
	((bptnd::DescriptorBase*) parentObject)->addDescriptor( (bptnd::GenericDescriptor*) childObject );
}

void NclPresentationSpecConverter::addDescriptorSwitchToDescriptorBase( void *parentObject, void *childObject ) {
	((bptnd::DescriptorBase*) parentObject)->addDescriptor( (bptnd::GenericDescriptor*) childObject );
}

void NclPresentationSpecConverter::addDescriptorParamToDescriptor( void *parentObject, void *childObject ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	// recuperar nome e valor da variavel
	util::xml::dom::Node param = static_cast<util::xml::dom::Node>(childObject);
	std::string paramName = nodeHnd->attribute(param, "name");
	std::string paramValue = nodeHnd->attribute(param, "value");

	// adicionar variavel ao descritor
	bptn::Parameter *descParam = new bptn::Parameter( paramName, paramValue );
	static_cast<bptnd::Descriptor*>(parentObject)->addParameter( descParam );
}

void NclPresentationSpecConverter::addImportBaseToDescriptorBase( void *parentObject, void *childObject ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	// get the external base alias and location
	std::string baseAlias = nodeHnd->attribute(static_cast<util::xml::dom::Node>(childObject), "alias");
	std::string baseLocation = nodeHnd->attribute(static_cast<util::xml::dom::Node>(childObject), "documentURI");

	NclDocumentConverter *compiler = (NclDocumentConverter*) getDocumentParser();
	bptn::NclDocument *importedDocument = compiler->importDocument( baseLocation );
	if (importedDocument == NULL) {
		return;
	}

	bptnd::DescriptorBase *descriptorBase = importedDocument->getDescriptorBase();
	if (descriptorBase == NULL) {
		return;
	}

	// insert the imported base into the document descriptor base
	try {
		((bptnd::DescriptorBase*) parentObject)->addBase( descriptorBase, baseAlias, baseLocation );

	} catch (...) {

	}

	// importing descriptor bases implies importing region, rule, and cost
	// function bases in order to maintain reference consistency
	bptn::NclDocument *thisDocument = (bptn::NclDocument*) getDocumentParser()->getObject( "return", "document" );

	std::map<int, bptnl::RegionBase*> *regionBases = importedDocument->getRegionBases();
	if (regionBases != NULL) {
		std::map<int, bptnl::RegionBase*>::iterator i = regionBases->begin();
		while (i != regionBases->end()) {
			bptnl::RegionBase *b = thisDocument->getRegionBase( 0 );
			if (b) {
				b->addBase( i->second, baseAlias, baseLocation );
			}
			else {
				LWARN( "NclPresentationSpecConverter", "Region base not found" );
			}
			++i;
		}
	}

	bptns::RuleBase *ruleBase = importedDocument->getRuleBase();
	if (ruleBase != NULL) {
		try {
			thisDocument->getRuleBase()->addBase( ruleBase, baseAlias, baseLocation );
		} catch (...) {}
	}
}

void *NclPresentationSpecConverter::createDescriptorParam( util::xml::dom::Node parentElement ) {
	return parentElement;
}

void *NclPresentationSpecConverter::createDescriptorBind( util::xml::dom::Node parentElement ) {

	// ver componentCompilers para comentarios
	return parentElement;
}

void *NclPresentationSpecConverter::createDescriptorBase( util::xml::dom::Node parentElement ) {
	LDEBUG("NclPresentationSpecConverter", "Create descriptorBase");
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();
	// criar nova base de conectores com id gerado a partir do nome de seu elemento
	bptnd::DescriptorBase *descBase = new bptnd::DescriptorBase( nodeHnd->attribute(parentElement, "id") );
	return descBase;
}

void* NclPresentationSpecConverter::createDescriptor( util::xml::dom::Node parentElement ) {
	LDEBUG("NclPresentationSpecConverter", "Create descriptor");
	bptnl::LayoutRegion* region;

	bptnn::KeyNavigation* keyNavigation;
	bptnn::FocusDecoration* focusDecoration;
	std::string attValue;
	std::vector<std::string> transIds;
	unsigned int i;
	bptnt::TransitionBase* transitionBase;
	bptnt::Transition* transition;
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	// cria descritor
	bptnd::Descriptor *descriptor = new bptnd::Descriptor( nodeHnd->attribute(parentElement, "id") );

	bptn::NclDocument *document = (bptn::NclDocument*) getDocumentParser()->getObject( "return", "document" );
	std::string tmpChar;

	// atributo region
	if (nodeHnd->hasAttribute(parentElement, "region" )) {
		tmpChar = nodeHnd->attribute(parentElement, "region");
		region = document->getRegion( tmpChar );
		LDEBUG("NclPresentationSpecConverter", "Add region %s to descriptor", tmpChar.c_str());
		if (region != NULL) {
			descriptor->setRegion( region );
		}
	}

	// atributo explicitDur
	if (nodeHnd->hasAttribute(parentElement, "explicitDur")) {
		std::string durStr = nodeHnd->attribute(parentElement, "explicitDur");
		LDEBUG("NclPresentationSpecConverter", "Add explicitDur %s to descriptor", durStr.c_str());
		util::DWORD duration;
		if (ncl_util::parseTimeString( durStr, duration )) {
			descriptor->setExplicitDuration( duration );
		}
	}

	if (nodeHnd->hasAttribute(parentElement, "freeze")) {
		std::string freeze = nodeHnd->attribute(parentElement, "freeze");
		LDEBUG("NclPresentationSpecConverter", "Add freeze %s to descriptor", freeze.c_str());
		if (freeze == "true") {
			descriptor->setFreeze( true );
		} else {
			descriptor->setFreeze( false );
		}
	}

	// atributo player
	if (nodeHnd->hasAttribute(parentElement, "player")) {
		tmpChar = nodeHnd->attribute(parentElement, "player");
		LDEBUG("NclPresentationSpecConverter", "Add player %s to descriptor", tmpChar.c_str());
		descriptor->setPlayerName( tmpChar );
	}

	// key navigation attributes
	keyNavigation = new bptnn::KeyNavigation();
	descriptor->setKeyNavigation( keyNavigation );
	if (nodeHnd->hasAttribute(parentElement, "focusIndex")) {
		tmpChar = nodeHnd->attribute(parentElement, "focusIndex");
		LDEBUG("NclPresentationSpecConverter", "Add focusIndex %s to descriptor", tmpChar.c_str());
		keyNavigation->setFocusIndex( tmpChar );
	}

	if (nodeHnd->hasAttribute(parentElement, "moveUp")) {
		tmpChar = nodeHnd->attribute(parentElement, "moveUp");
		LDEBUG("NclPresentationSpecConverter", "Add moveUp %s to descriptor", tmpChar.c_str());
		keyNavigation->setMoveUp( tmpChar );
	}

	if (nodeHnd->hasAttribute(parentElement, "moveDown")) {
		tmpChar = nodeHnd->attribute(parentElement, "moveDown");
		LDEBUG("NclPresentationSpecConverter", "Add moveDown %s to descriptor", tmpChar.c_str());
		keyNavigation->setMoveDown( tmpChar );
	}

	if (nodeHnd->hasAttribute(parentElement, "moveLeft")) {
		tmpChar = nodeHnd->attribute(parentElement, "moveLeft");
		LDEBUG("NclPresentationSpecConverter", "Add moveLeft %s to descriptor", tmpChar.c_str());
		keyNavigation->setMoveLeft( tmpChar );
	}

	if (nodeHnd->hasAttribute(parentElement, "moveRight")) {
		tmpChar = nodeHnd->attribute(parentElement, "moveRight");
		LDEBUG("NclPresentationSpecConverter", "Add moveRight %s to descriptor", tmpChar.c_str());
		keyNavigation->setMoveRight( tmpChar );
	}

	focusDecoration = new bptnn::FocusDecoration();
	descriptor->setFocusDecoration( focusDecoration );
	if (nodeHnd->hasAttribute(parentElement, "focusSrc")) {
		tmpChar = nodeHnd->attribute(parentElement, "focusSrc");
		focusDecoration->setFocusSrc( tmpChar );
		LDEBUG("NclPresentationSpecConverter", "Add focusSrc %s to descriptor", tmpChar.c_str());
	}

	if (nodeHnd->hasAttribute(parentElement, "focusBorderColor")) {
		tmpChar = nodeHnd->attribute(parentElement, "focusBorderColor");
		LDEBUG("NclPresentationSpecConverter", "Add focusBorderColor %s to descriptor", tmpChar.c_str());
		focusDecoration->setFocusBorderColor( tmpChar );
	}

	if (nodeHnd->hasAttribute(parentElement, "focusBorderWidth")) {
		tmpChar = nodeHnd->attribute(parentElement, "focusBorderWidth");
		LDEBUG("NclPresentationSpecConverter", "Add focusBorderWidth %s to descriptor", tmpChar.c_str());
		focusDecoration->setFocusBorderWidth( tmpChar );
	}

	if (nodeHnd->hasAttribute(parentElement, "focusBorderTransparency")) {
		tmpChar = nodeHnd->attribute(parentElement, "focusBorderTransparency");
		focusDecoration->setFocusBorderTransparency( tmpChar );
		LDEBUG("NclPresentationSpecConverter", "Add focusBorderTransparency %s to descriptor", tmpChar.c_str());
	}

	if (nodeHnd->hasAttribute(parentElement, "focusSelSrc")) {
		tmpChar = nodeHnd->attribute(parentElement, "focusSelSrc");
		LDEBUG("NclPresentationSpecConverter", "Add focusSelSrc %s to descriptor", tmpChar.c_str());
		focusDecoration->setFocusSelSrc( tmpChar );
	}

	if (nodeHnd->hasAttribute(parentElement, "selBorderColor")) {
		tmpChar = nodeHnd->attribute(parentElement, "selBorderColor");
		LDEBUG("NclPresentationSpecConverter", "Add selBorderColor %s to descriptor", tmpChar.c_str());
		focusDecoration ->setSelBorderColor( tmpChar );
	}

	if (nodeHnd->hasAttribute(parentElement, "transIn")) {
		transitionBase = document->getTransitionBase();
		if (transitionBase != NULL) {
			std::string value;
			attValue = nodeHnd->attribute(parentElement, "transIn");
			LDEBUG("NclPresentationSpecConverter", "Add transIn %s to descriptor", attValue.c_str());

			boost::split( transIds, attValue, boost::is_any_of( ";" ) );

			if (!transIds.empty()) {
				std::vector<std::string>::iterator i;
				i = transIds.begin();
				int j = 0;
				while (i != transIds.end()) {
					value = (*i);
					boost::trim( value );
					*i = value;
					transition = transitionBase->getTransition( value );
					if (transition != NULL) {
						descriptor->addInputTransition( transition, j );
					}
					++i;
					j++;
				}
			}

			transIds.clear();
		}
	}

	if (nodeHnd->hasAttribute(parentElement, "transOut")) {
		transitionBase = document->getTransitionBase();
		if (transitionBase != NULL) {
			std::string value;
			attValue = nodeHnd->attribute(parentElement, "transOut");
			LDEBUG("NclPresentationSpecConverter", "Add transOut %s to descriptor", attValue.c_str());

			boost::split( transIds, attValue, boost::is_any_of( ";" ) );

			if (!transIds.empty()) {
				for (i = 0; i < transIds.size(); i++) {
					value = transIds[i];
					boost::trim( value );
					transition = transitionBase->getTransition( value );
					if (transition != NULL) {
						descriptor->addOutputTransition( transition, i );
					}
				}
			}

			transIds.clear();
		}
	}

	return descriptor;
}

bptnd::GenericDescriptor* NclPresentationSpecConverter::createDummyDescriptorBase(){
	LDEBUG("NclPresentationSpecConverter", "Create dummy descriptorBase");
	bptn::NclDocument *document = static_cast<bptn::NclDocument*>( getDocumentParser()->getObject( "return", "document" ) );
	bptnd::GenericDescriptor *descriptor = document->getDescriptor( "dummyDescriptor" );

	if ( descriptor == NULL ) {
		bptnd::DescriptorBase *descBase = document->getDescriptorBase();
		if ( descBase == NULL ) {
			descBase = new bptnd::DescriptorBase( "dummyDescriptorBase" );
			document->setDescriptorBase( descBase );
		}

		bptnl::LayoutRegion *region = ((NclDocumentConverter*)getDocumentParser())->getLayoutParser()->createDummyRegBase();
		descriptor = new bptnd::Descriptor( "dummyDescriptor" );
		((bptnd::Descriptor*)descriptor)->setRegion( region );


		addDescriptorToDescriptorBase(descBase, descriptor);
	}

	return descriptor;
}

}
}
}
}
}
