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

#ifndef NCLSTRUCTUREPARSER_H_
#define NCLSTRUCTUREPARSER_H_

#include "../ModuleParser.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
class DocumentParser;
}
}
}
}
}
namespace bptcf = ::br::pucrio::telemidia::converter::framework;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {

class NclTransitionParser;
class NclPresentationSpecificationParser;
class NclComponentsParser;
class NclLinkingParser;
class NclLayoutParser;
class NclInterfacesParser;
class NclPresentationControlParser;
class NclConnectorsParser;
class NclImportParser;
class NclMetainformationParser;

/**
 * Clase encargada de parsear el body y el head.
 */
class NclStructureParser: public ModuleParser
{
protected:
	NclTransitionParser* transitionParser;
	NclPresentationSpecificationParser* presentationSpecificationParser;

	NclComponentsParser *componentsParser;
	NclLinkingParser *linkingParser;
	NclLayoutParser *layoutParser;
	NclInterfacesParser *interfacesParser;
	NclPresentationControlParser *presentationControlParser;
	NclConnectorsParser *connectorsParser;
	NclImportParser *importParser;
	NclMetainformationParser* metainformationParser;

public:
	explicit NclStructureParser( bptcf::DocumentParser *documentParser );

	virtual void* parseBody( util::xml::dom::Node parentElement );

	virtual void* posCompileBody( util::xml::dom::Node parentElement, void *parentObject );

	virtual void* createBody( util::xml::dom::Node parentElement )=0;

	virtual void addPropertyToBody( void *parentObject, void *childObject )=0;

	virtual void addPortToBody( void *parentObject, void *childObject )=0;

	virtual void addMediaToBody( void *parentObject, void *childObject )=0;

	virtual void addContextToBody( void *parentObject, void *childObject )=0;

	virtual void addSwitchToBody( void *parentObject, void *childObject )=0;

	virtual void addLinkToBody( void *parentObject, void *childObject )=0;

	virtual void* parseHead( util::xml::dom::Node parentElement );

	virtual void* createHead( util::xml::dom::Node parentElement )=0;

	virtual void addRegionBaseToHead( void *childObject )=0;

	virtual void addDescriptorBaseToHead( void *childObject )=0;

	virtual void addTransitionBaseToHead( void* childObject )=0;

	virtual void addRuleBaseToHead( void *childObject )=0;

	virtual void addConnectorBaseToHead( void *childObject )=0;

	virtual void addMetaToHead( void *childObject )=0;

	virtual void addMetadataToHead( void *childObject )=0;

	virtual void* parseNcl( util::xml::dom::Node parentElement );

	virtual void* createNcl( util::xml::dom::Node parentElement )=0;

	NclTransitionParser* getTransitionParser();
	void setTransitionParser( NclTransitionParser* transitionParser );

	NclPresentationSpecificationParser*
	getPresentationSpecificationParser();

	void setPresentationSpecificationParser( NclPresentationSpecificationParser* presentationSpecificationParser );

	NclComponentsParser* getComponentsParser();
	void setComponentsParser( NclComponentsParser *componentsParser );
	NclLinkingParser* getLinkingParser();
	void setLinkingParser( NclLinkingParser *linkingParser );
	NclLayoutParser* getLayoutParser();
	void setLayoutParser( NclLayoutParser *layoutParser );
	NclMetainformationParser* getMetainformationParser();
	NclInterfacesParser* getInterfacesParser();
	void setInterfacesParser( NclInterfacesParser *interfacesParser );
	NclPresentationControlParser* getPresentationControlParser();

	void setPresentationControlParser( NclPresentationControlParser *presentationControlParser );

	NclConnectorsParser* getConnectorsParser();
	void setConnectorsParser( NclConnectorsParser *connectorsParser );
	NclImportParser* getImportParser();
	void setImportParser( NclImportParser *importParser );
	void setMetainformationParser( NclMetainformationParser* metainformationParser );
};
}
}
}
}
}
}

#endif /*NCLSTRUCTUREPARSER_H_*/
