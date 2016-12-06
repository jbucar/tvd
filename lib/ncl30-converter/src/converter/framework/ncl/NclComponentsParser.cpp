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

#include "../../../../include/framework/ncl/NclComponentsParser.h"
#include "../../../../include/framework/ncl/NclInterfacesParser.h"
#include "../../../../include/framework/ncl/NclPresentationControlParser.h"
#include "../../../../include/framework/ncl/NclLinkingParser.h"

#include "ncl30/components/Node.h"
#include "ncl30/components/ContentNode.h"
#include "ncl30/components/ContextNode.h"
#include "ncl30/reuse/ReferNode.h"
#include <util/xml/nodehandler.h>
#include <boost/foreach.hpp>

namespace bptnc = ::br::pucrio::telemidia::ncl::components;
namespace bptnr = ::br::pucrio::telemidia::ncl::reuse;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {
  
NclComponentsParser::NclComponentsParser( DocumentParser *documentParser )
	: ModuleParser( documentParser )
{
}

bptnc::Node *NclComponentsParser::parseMedia( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptnc::Node *mediaNode = createMedia(parentElement);
	if (mediaNode == NULL) {
		return NULL;
	}

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	NclInterfacesParser *interfParser = getInterfacesParser();
	BOOST_FOREACH( util::xml::dom::Node element, elementNodeList ) {
		const std::string &elementTagName = nodeHnd->tagName(element);
		if (elementTagName.compare( "area" ) == 0) {
			addElementToMedia( mediaNode, interfParser->parseArea(element));
		} else if (elementTagName.compare( "property" ) == 0) {
			addElementToMedia( mediaNode, interfParser->parseProperty( element));
		}
	}

	return mediaNode;
}

bptnc::Node *NclComponentsParser::parseContext( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptnc::Node *ctxNode = createContext(parentElement);
	if (ctxNode == NULL) {
		return NULL;
	}

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node element, elementNodeList ) {
		const std::string &elementTagName = nodeHnd->tagName(element);
		if (elementTagName.compare("media") == 0) {
			addElementToContext(ctxNode, parseMedia(element));
		} else if (elementTagName.compare("context") == 0) {
			addElementToContext(ctxNode, parseContext(element));
		} else if (elementTagName.compare("switch") == 0) {
			addElementToContext(ctxNode, static_cast<bptnc::Node*>(presentationControlParser->parseSwitch(element)));
		} else if (elementTagName.compare("property") == 0) {
			addPropertyToContext(ctxNode, interfacesParser->parseProperty(element));
		}
	}
	return ctxNode;
}

bptnc::ContextNode *NclComponentsParser::posCompileContext( util::xml::dom::Node parentElement, bptnc::ContextNode *parentObject ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);
	void *elementObject;

	BOOST_FOREACH( util::xml::dom::Node element, elementNodeList ) {
		const std::string &elementTagName = nodeHnd->tagName(element);
		if (elementTagName.compare( "link" ) == 0) {
			elementObject = linkingParser->parseLink( element, parentObject );
			if (elementObject != NULL) {
				addLinkToContext( parentObject, elementObject );
			}
		} else if (elementTagName.compare( "port" ) == 0) {
			elementObject = interfacesParser->parsePort(element, parentObject );
			if (elementObject != NULL) {
				addPortToContext( parentObject, elementObject );
			}
		}
	}
	return parentObject;
}

}}}}}}
