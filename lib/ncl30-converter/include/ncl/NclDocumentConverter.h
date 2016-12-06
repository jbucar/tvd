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

#ifndef NclDocumentConverter_H
#define NclDocumentConverter_H

#include "../IDocumentConverter.h"
#include "../framework/ncl/NclDocumentParser.h"
namespace bptcfn = ::br::pucrio::telemidia::converter::framework::ncl;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
class NclDocument;
class IPrivateBaseContext;

namespace components {
class Node;
}
}
}
}
}
namespace bptn = ::br::pucrio::telemidia::ncl;
namespace bptnc = ::br::pucrio::telemidia::ncl::components;

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
namespace ncl {
  
/**
 * Esta clase es la que inicia el parsing del documento. Desde aquí se van realizando llamadas a cada clase encargada de un bloque
 * en especial del documento NCL.
 */
class NclDocumentConverter: public IDocumentConverter, public bptcfn::NclDocumentParser {

private:
	bptn::IPrivateBaseContext* privateBaseContext;
	bool ownManager;

	bool parseEntityVar;
	void* parentObject;

public:
	NclDocumentConverter( bptn::IPrivateBaseContext* baseContext );

	virtual ~NclDocumentConverter();

protected:
	void checkManager();
	virtual void initialize();
	virtual void* parseRootElement( util::xml::dom::Node rootElement );

public:
	const std::string getAttribute( void* element, const std::string &attribute );
	bptnc::Node *getNode( const std::string &id );
	bool removeNode( bptnc::Node *node );
	bptn::NclDocument* importDocument( const std::string &docLocation );
	void* parseEntity( std::string &entityLocation, bptn::NclDocument* document, void* parent );

	void* getObject( const std::string &tableName, const std::string &key ) {
		return DocumentParser::getObject( tableName, key );
	}
	
	bptn::IPrivateBaseContext* getPrivateBaseContext(){
		return privateBaseContext;
	}

	/**
	 * Realiza el parsing de un elemento con la uri especificada.
	 * @param uri Un string con la ubicación del elemento.
	 */
	void* parse( std::string &uri ) {
		return DocumentParser::parse( uri );
	}
};
}
}
}
}
}

#endif
