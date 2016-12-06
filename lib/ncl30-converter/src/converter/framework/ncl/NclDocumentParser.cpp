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

#include "../../../../include/framework/ncl/NclDocumentParser.h"
#include "../../../../include/framework/ncl/NclPresentationControlParser.h"
#include "../../../../include/framework/ncl/NclPresentationSpecificationParser.h"
#include "../../../../include/framework/ncl/NclConnectorsParser.h"
#include "../../../../include/framework/ncl/NclComponentsParser.h"
#include "../../../../include/framework/ncl/NclStructureParser.h"
#include "../../../../include/framework/ncl/NclLayoutParser.h"
#include "../../../../include/framework/ncl/NclTransitionParser.h"
#include "../../../../include/smil/SMILNCMDocumentConverter.h"
#include <ncl30/NclDocument.h>
#include <util/log.h>
#include <util/xml/nodehandler.h>
#include <util/xml/documenthandler.h>

namespace bptcs = ::br::pucrio::telemidia::converter::smil;
namespace bptn = ::br::pucrio::telemidia::ncl;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {

NclDocumentParser::NclDocumentParser()
	: DocumentParser()
{
	this->presentationSpecificationParser = NULL;
	this->structureParser = NULL;
	this->componentsParser = NULL;
	this->connectorsParser = NULL;
	this->linkingParser = NULL;
	this->interfacesParser = NULL;
	this->layoutParser = NULL;
	this->presentationControlParser = NULL;
	this->importParser = NULL;
}

NclDocumentParser::~NclDocumentParser()
{
	if (presentationControlParser != NULL) {
		delete presentationControlParser;
		presentationControlParser = NULL;
	}

	if (presentationSpecificationParser != NULL) {
		delete presentationSpecificationParser;
		presentationSpecificationParser = NULL;
	}

}

void NclDocumentParser::setDependencies() {
	getConnectorsParser()->setImportParser( importParser );
	getPresentationControlParser()->setPresentationSpecificationParser( presentationSpecificationParser );

	getPresentationControlParser()->setComponentsParser( componentsParser );
	getPresentationControlParser()->setInterfacesParser( interfacesParser );
	getPresentationControlParser()->setImportParser( importParser );
// 	getComponentsParser()->setPresentationSpecificationParser( presentationSpecificationParser );

	getComponentsParser()->setLinkingParser( linkingParser );
	getComponentsParser()->setInterfacesParser( interfacesParser );
	getComponentsParser()->setPresentationControlParser( presentationControlParser );

	getStructureParser()->setPresentationSpecificationParser( presentationSpecificationParser );

	getStructureParser()->setComponentsParser( componentsParser );
	getStructureParser()->setLinkingParser( linkingParser );
	getStructureParser()->setLayoutParser( layoutParser );
	getStructureParser()->setInterfacesParser( interfacesParser );
	getStructureParser()->setPresentationControlParser( presentationControlParser );

	getStructureParser()->setConnectorsParser( connectorsParser );
	getStructureParser()->setImportParser( importParser );
	getStructureParser()->setTransitionParser( transitionParser );
	getStructureParser()->setMetainformationParser( metainformationParser );
	getPresentationSpecificationParser()->setImportParser( importParser );
	getPresentationSpecificationParser()->setPresentationControlParser( presentationControlParser );

	getLayoutParser()->setImportParser( importParser );
	getTransitionParser()->setImportParser( importParser );
}

void* NclDocumentParser::parseRootElement( util::xml::dom::Node rootElement ) {
	const std::string &tagName = getDocumentHandler()->handler()->tagName(rootElement);
	if (tagName.compare( "ncl" ) == 0) {
		void * parsed = getStructureParser()->parseNcl( rootElement );
		if (parsed == NULL) {
			removeObject( "return", "document" );
		}
		return parsed;
	} else if (tagName.compare( "smil" ) == 0) {
		bptcs::SmilNcmDocumentConverter *smilTiny = new bptcs::SmilNcmDocumentConverter();
		void *ret = smilTiny->parseRootElement( rootElement );
		bptn::NclDocument *doc = (bptn::NclDocument *) smilTiny->getObject( "return", "document" );
		if (doc == NULL){
			LERROR("NclDocumentParser", "parseRootElement smilDoc NULL");
		}
		addObject( "return", "document", smilTiny->getObject( "return", "document" ) );
		return ret;
	} else {
		LERROR("NclDocumentParser", "Trying to parse a non NCL document. tagName=%s", tagName.c_str());
		return NULL;
	}
}
}
}
}
}
}
}
