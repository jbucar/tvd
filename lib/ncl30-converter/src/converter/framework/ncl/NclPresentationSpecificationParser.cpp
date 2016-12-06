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

#include "../../../../include/framework/ncl/NclPresentationSpecificationParser.h"
#include "../../../../include/framework/ncl/NclPresentationControlParser.h"
#include "../../../../include/framework/ncl/NclImportParser.h"
#include <util/xml/nodehandler.h>
#include <boost/foreach.hpp>

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {

NclPresentationSpecificationParser::NclPresentationSpecificationParser( DocumentParser *documentParser )
	: ModuleParser( documentParser )
{
}

void *NclPresentationSpecificationParser::parseDescriptor( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	void *parentObject = createDescriptor( parentElement );
	if (parentObject == NULL) {
		return NULL;
	}

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node element, elementNodeList ) {
		if (nodeHnd->tagName(element).compare( "descriptorParam" ) == 0) {
			void *elementObject = parseDescriptorParam( element );
			if (elementObject != NULL) {
				addDescriptorParamToDescriptor( parentObject, elementObject );
			}
		}
	}
	return parentObject;
}

void *NclPresentationSpecificationParser::parseDescriptorBase( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	void *parentObject = createDescriptorBase( parentElement );
	if (parentObject == NULL) {
		return NULL;
	}

	void *elementObject;
	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node element, elementNodeList ) {
		const std::string &elementTagName = nodeHnd->tagName(element);
		if (elementTagName.compare( "importBase" ) == 0) {
			elementObject = element;
			if (elementObject != NULL) {
				addImportBaseToDescriptorBase( parentObject, elementObject );
			}
		} else if (elementTagName.compare( "descriptorSwitch" ) == 0) {
			elementObject = ((NclPresentationControlParser*) getPresentationControlParser())->parseDescriptorSwitch( element );
			if (elementObject != NULL) {
				addDescriptorSwitchToDescriptorBase( parentObject, elementObject );
			}
		} else if (elementTagName.compare( "descriptor" ) == 0) {
			elementObject = parseDescriptor( element );
			if (elementObject != NULL) {
				addDescriptorToDescriptorBase( parentObject, elementObject );
			}
		}
	}
	return parentObject;
}

void *NclPresentationSpecificationParser::parseDescriptorBind( util::xml::dom::Node parentElement ) {
	return createDescriptorBind( parentElement );
}

void *NclPresentationSpecificationParser::parseDescriptorParam( util::xml::dom::Node parentElement ) {
	return createDescriptorParam( parentElement );
}

NclImportParser *NclPresentationSpecificationParser::getImportParser() {
	return importParser;
}

void NclPresentationSpecificationParser::setImportParser( NclImportParser *importParser ) {

	this->importParser = importParser;
}

void *NclPresentationSpecificationParser::getPresentationControlParser() {
	return presentationControlParser;
}

void NclPresentationSpecificationParser::setPresentationControlParser( void *presentationControlParser ) {

	this->presentationControlParser = presentationControlParser;
}
}
}
}
}
}
}
