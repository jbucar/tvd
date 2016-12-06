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

#ifndef _SWITCHNODE_H_
#define _SWITCHNODE_H_

#include "../components/CompositeNode.h"
namespace bptnc = ::br::pucrio::telemidia::ncl::components;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace interfaces {
class InterfacePoint;
class Port;
class SwitchPort;
}
}
}
}
}
namespace bptni = ::br::pucrio::telemidia::ncl::interfaces;

#include <vector>
#include <string>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace switches {

class Rule;

class SwitchNode: public bptnc::CompositeNode
{
private:
	Node* defaultNode;
	std::vector<Rule*>* ruleList;

public:
	explicit SwitchNode( const std::string &id );
	virtual ~SwitchNode() {
	}
	;
	bool addNode( unsigned int index, Node* node, Rule* rule );
	bool addNode( Node* node, Rule* rule );

	//virtual from CompositeNode
	bool addNode( Node* node );

	bool addSwitchPortMap( bptni::SwitchPort* switchPort, Node* node, bptni::InterfacePoint* interfacePoint );

	//virtual from CompositeNode
	bool addPort( bptni::Port* port );

	//virtual from CompositeNode
	bool addPort( unsigned int index, bptni::Port* port );

	//virtual from CompositeNode
	void clearNodes();

	void exchangeNodesAndRules( unsigned int index1, unsigned int index2 );

	Node* getDefaultNode();

	//virtual from CompositeNode
	interfaces::InterfacePoint *getMapInterface( bptni::Port *port );

	//virtual from CompositeNode
	Node* getNode( const std::string &nodeId );

	Node* getNode( unsigned int index );
	Node* getNode( Rule* rule );
	unsigned int getNumRules();
	Rule* getRule( unsigned int index );
	unsigned int indexOfRule( Rule* rule );

	//virtual from CompositeNode
	bool recursivelyContainsNode( const std::string &nodeId );

	//virtual from CompositeNode
	Node* recursivelyGetNode( const std::string &nodeId );

	//virtual from CompositeNode
	bool removeNode( Node* node );

	bool removeNode( unsigned int index );
	bool removeRule( Rule* rule );
	void setDefaultNode( Node* node );
};
}
}
}
}
}

#endif //_SWITCHNODE_H_
