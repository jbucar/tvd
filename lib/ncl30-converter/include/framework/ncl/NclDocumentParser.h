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

#ifndef NCLDOCUMENTPARSER_H_
#define NCLDOCUMENTPARSER_H_

#include "../DocumentParser.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {

class NclConnectorsParser;
class NclImportParser;
class NclTransitionParser;
class NclPresentationControlParser;
class NclComponentsParser;
class NclStructureParser;
class NclPresentationSpecificationParser;
class NclLayoutParser;
class NclInterfacesParser;
class NclLinkingParser;
class NclMetainformationParser;

/**
 * Clase encargada de parsear el documento NCL.
 * Contiene referencias a todas las clases encargadas de parsear cada bloque específico del documento NCL.
 */
class NclDocumentParser: public DocumentParser
{
protected:
	NclConnectorsParser *connectorsParser;
	NclImportParser *importParser;
	NclTransitionParser* transitionParser;
	NclPresentationControlParser *presentationControlParser;
	NclComponentsParser *componentsParser;
	NclStructureParser *structureParser;
	NclPresentationSpecificationParser *presentationSpecificationParser;
	NclLayoutParser *layoutParser;
	NclInterfacesParser *interfacesParser;
	NclLinkingParser *linkingParser;
	NclMetainformationParser* metainformationParser;

public:
	explicit NclDocumentParser();
	virtual ~NclDocumentParser();

protected:
	void setDependencies();
	
	virtual void* parseRootElement( util::xml::dom::Node rootElement );
	virtual void initialize()=0;
	
public:
	NclTransitionParser* getTransitionParser(){
		return transitionParser;
	}
	
	NclConnectorsParser *getConnectorsParser(){
		return connectorsParser;
	}
	
	void setConnectorsParser( NclConnectorsParser *connectorsParser ){
		this->connectorsParser = connectorsParser;
	}
	
	NclImportParser *getImportParser(){
		return importParser;
	}
	
	void setBaseReuseParser( NclImportParser *importParser ){
		this->importParser = importParser;
	}
	
	NclPresentationControlParser *getPresentationControlParser(){
		return presentationControlParser;
	}
	
	void setPresentationControlParser( NclPresentationControlParser *presentationControlParser ){
		this->presentationControlParser = presentationControlParser;
	}

	NclComponentsParser *getComponentsParser(){
		return componentsParser;
	}
	
	void setComponentsParser( NclComponentsParser *componentsParser ){
		this->componentsParser = componentsParser;
	}
	
	NclStructureParser *getStructureParser(){
		return structureParser;
	}
	void setStructureParser( NclStructureParser *structureParser ){
		this->structureParser = structureParser;
	}
	
	NclPresentationSpecificationParser* getPresentationSpecificationParser(){
		return presentationSpecificationParser;
	}

	void setPresentationSpecificationParser( NclPresentationSpecificationParser* presentationSpecificationParser ){
		this->presentationSpecificationParser = presentationSpecificationParser;
	}

	NclLayoutParser *getLayoutParser(){
		return layoutParser;
	}
	
	void setLayoutParser( NclLayoutParser *layoutParser ){
		this->layoutParser = layoutParser;
	}
	
	NclInterfacesParser *getInterfacesParser(){
		return interfacesParser;
	}
	
	void setInterfacesParser( NclInterfacesParser *interfacesParser ){
		this->interfacesParser = interfacesParser;
	}
	
	NclMetainformationParser* getMetainformationParser(){
		return metainformationParser;
	}
	
	void setMetainformationParser( NclMetainformationParser* metainformationParser ){
		this->metainformationParser = metainformationParser;
	}

	NclLinkingParser *getLinkingParser(){
		return linkingParser;
	}
	
	void setLinkingParser( NclLinkingParser *linkingParser ){
		this->linkingParser = linkingParser;
	}
};
}
}
}
}
}
}

#endif /*NCLDOCUMENTPARSER_H_*/
