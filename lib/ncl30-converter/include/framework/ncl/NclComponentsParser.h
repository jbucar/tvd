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

#ifndef NCLCOMPONENTSPARSER_H_
#define NCLCOMPONENTSPARSER_H_

#include "../ModuleParser.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace components {
	class ContextNode;
	class Node;
}
namespace reuse {
	class ReferNode;
}}}}}

namespace bptnc = ::br::pucrio::telemidia::ncl::components;
namespace bptnr = ::br::pucrio::telemidia::ncl::reuse;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
	class DocumentParser;
}}}}}

namespace bptcf = ::br::pucrio::telemidia::converter::framework;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {

class NclPresentationSpecificationParser;
class NclLinkingParser;
class NclInterfacesParser;
class NclPresentationControlParser;

/**
 * Clase encargada de parsear componentes que están en el body. Como pueden ser media o context.
 */
class NclComponentsParser: public ModuleParser {
	//Ref Classes
private:
// 	NclPresentationSpecificationParser *presentationSpecificationParser;
	NclLinkingParser *linkingParser;
	NclInterfacesParser *interfacesParser;
	NclPresentationControlParser *presentationControlParser;

public:
	explicit NclComponentsParser( bptcf::DocumentParser *documentParser );

	bptnc::Node *parseMedia( util::xml::dom::Node parentElement );
	bptnc::Node *parseContext( util::xml::dom::Node parentElement );
	
	virtual bptnc::ContextNode *posCompileContext( util::xml::dom::Node parentElement, bptnc::ContextNode *parentObject);
	
	virtual bptnc::Node* createMedia( util::xml::dom::Node parentElement )=0;
	virtual void addElementToMedia( bptnc::Node *parentObject, void *childObject )=0;
	
	virtual bptnc::Node *createContext( util::xml::dom::Node parentElement )=0;
	virtual void addPortToContext( bptnc::ContextNode * parentObject, void *childObject )=0;
	virtual void addPropertyToContext(bptnc::Node *parentObject, void *childObject )=0;
	virtual void addLinkToContext( bptnc::ContextNode *parentObject, void *childObject )=0;
	virtual void addElementToContext( bptnc::Node *parentObject, bptnc::Node *childObject )=0;

// 	NclPresentationSpecificationParser* getPresentationSpecificationParser(){
// 		return presentationSpecificationParser;
// 	}
// 
// 	void setPresentationSpecificationParser( NclPresentationSpecificationParser* presentSpecParser){
// 		presentationSpecificationParser = presentSpecParser;
// 	}

	NclLinkingParser *getLinkingParser(){
		return linkingParser;
	}
	
	void setLinkingParser( NclLinkingParser *linkingParser ){
		this->linkingParser = linkingParser;
	}
	
	NclInterfacesParser *getInterfacesParser(){
		return interfacesParser;
	}
	
	void setInterfacesParser( NclInterfacesParser *interfacesParser ){
		this->interfacesParser = interfacesParser;
	}
	
	NclPresentationControlParser *getPresentationControlParser(){
		return presentationControlParser;
	}
	
	void setPresentationControlParser( NclPresentationControlParser *presentCtrlParser ){
		presentationControlParser = presentCtrlParser;
	}
};
}
}
}
}
}
}

#endif /*NCLCOMPONENTSPARSER_H_*/
