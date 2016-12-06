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

#include "../../../include/ncl/NclTransitionConverter.h"
#include "../../../include/ncl/NclDocumentConverter.h"
#include "ncl30/NclDocument.h"
#include "ncl30/transition/Transition.h"
#include "ncl30/transition/TransitionBase.h"
#include "ncl30/transition/TransitionUtil.h"
#include <util/log.h>
#include <util/xml/nodehandler.h>
#include <boost/lexical_cast.hpp>

namespace bptn = ::br::pucrio::telemidia::ncl;
namespace bptnt = ::br::pucrio::telemidia::ncl::transition;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {

NclTransitionConverter::NclTransitionConverter( bptcf::DocumentParser *documentParser )
	: NclTransitionParser( documentParser )
{
}

void NclTransitionConverter::addTransitionToTransitionBase( void* parentObject, void* childObject ) {
	((bptnt::TransitionBase*) parentObject)->addTransition( (bptnt::Transition*) childObject );
}

void* NclTransitionConverter::createTransitionBase( util::xml::dom::Node parentElement ) {
	return new bptnt::TransitionBase( getNodeHandler()->attribute(parentElement, "id") );
}

void* NclTransitionConverter::createTransition( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	if (!nodeHnd->hasAttribute(parentElement, "id")) {
		return NULL;
	}
	std::string id = nodeHnd->attribute(parentElement, "id");

	int type;
	if (nodeHnd->hasAttribute(parentElement, "type")) {
		type = bptnt::TransitionUtil::getTypeCode( nodeHnd->attribute(parentElement, "type") );
		if (type < 0) {
			return NULL;
		}
	} else {
		// type must be defined
		LERROR("NclTransitionConverter", "createTransition, transition type must be defined");
		return NULL;
	}

	bptnt::Transition *transition = new bptnt::Transition( id, type );

	if (nodeHnd->hasAttribute(parentElement, "subtype")) {
		int subtype = bptnt::TransitionUtil::getSubtypeCode( type, nodeHnd->attribute(parentElement, "subtype") );
		if (subtype >= 0) {
			transition->setSubtype( subtype );
		}
	}

	if (nodeHnd->hasAttribute(parentElement, "dur")) {
		std::string attValue = nodeHnd->attribute(parentElement, "dur");
		double dur = boost::lexical_cast<float>( attValue.substr( 0, attValue.length() - 1 ) );
		transition->setDur( dur * 1000 );
	}

	if (nodeHnd->hasAttribute(parentElement, "startProgress")) {
		transition->setStartProgress( boost::lexical_cast<float>( nodeHnd->attribute(parentElement, "startProgress") ) );
	}

	if (nodeHnd->hasAttribute(parentElement, "endProgress")) {
		transition->setEndProgress( boost::lexical_cast<float>( nodeHnd->attribute(parentElement, "endProgress") ) );
	}

	if (nodeHnd->hasAttribute(parentElement, "direction")) {
		short direction = bptnt::TransitionUtil::getDirectionCode( nodeHnd->attribute(parentElement, "direction") );
		if (direction >= 0) {
			transition->setDirection( direction );
		}
	}

	if (nodeHnd->hasAttribute(parentElement, "fadeColor")) {
		transition->setFadeColor( nodeHnd->attribute(parentElement, "fadeColor") );
	}

	if (nodeHnd->hasAttribute(parentElement, "horzRepeat")) {
		transition->setHorzRepeat( boost::lexical_cast<int>( nodeHnd->attribute(parentElement, "horzRepeat") ) );
	}

	if (nodeHnd->hasAttribute(parentElement, "vertRepeat")) {
		transition->setVertRepeat( boost::lexical_cast<int>( nodeHnd->attribute(parentElement, "vertRepeat") ) );
	}

	if (nodeHnd->hasAttribute(parentElement, "borderWidth")) {
		transition->setBorderWidth( boost::lexical_cast<int>( nodeHnd->attribute(parentElement, "borderWidth") ) );
	}

	if (nodeHnd->hasAttribute(parentElement, "borderColor")) {
		transition->setBorderColor( nodeHnd->attribute(parentElement, "borderColor") );
	}

	return transition;
}

void NclTransitionConverter::addImportBaseToTransitionBase( void* parentObject, void* childObject ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	// get the external base alias and location
	std::string baseAlias = nodeHnd->attribute(static_cast<util::xml::dom::Node>(childObject), "alias");
	std::string baseLocation = nodeHnd->attribute(static_cast<util::xml::dom::Node>(childObject), "documentURI");

	NclDocumentConverter *compiler = (NclDocumentConverter*) getDocumentParser();
	bptn::NclDocument *importedDocument = compiler->importDocument( baseLocation );
	if (importedDocument == NULL) {
		return;
	}

	bptnt::TransitionBase *createdBase = importedDocument->getTransitionBase();
	if (createdBase == NULL) {
		return;
	}

	// insert the imported base into the document region base
	try {
		((bptnt::TransitionBase*) parentObject)->addBase( createdBase, baseAlias, baseLocation );
	} catch (std::exception* exc) {
		LERROR("NclTransitionConverter", "addImportBaseToTransitionBase, exception '%s'", exc->what());
	}
}
}
}
}
}
}
