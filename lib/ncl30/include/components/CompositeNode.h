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
#ifndef _COMPOSITENODE_H_
#define _COMPOSITENODE_H_

#include "NodeEntity.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace interfaces {
class InterfacePoint;
class Port;
class Anchor;
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
namespace components {

class Node;

class CompositeNode: public NodeEntity
{
protected:
	std::vector<Node*>* nodes;
	std::vector<bptni::Port*>* portList;

public:
	explicit CompositeNode( const std::string &id );
	virtual ~CompositeNode();
	bool addAnchor( int index, bptni::Anchor *anchor );
	bool addAnchor( bptni::Anchor *anchor );

	//virtual to all
	virtual bool addNode( Node* node )=0;

	//virtual to SwitchNode
	virtual bool addPort( unsigned int index, bptni::Port* port );

	//virtual to SwitchNode
	virtual bool addPort( bptni::Port* port );

	void clearPorts();
	unsigned int getNumPorts();
	bptni::Port* getPort( const std::string &portId );
	bptni::Port* getPort( unsigned int index );
	std::vector<bptni::Port*>* getPorts();
	unsigned int indexOfPort( bptni::Port* port );
	bool removePort( bptni::Port* port );

	//virtual to SwitchNode
	virtual bptni::InterfacePoint* getMapInterface( bptni::Port* port );

	//virtual to SwitchNode
	virtual void clearNodes();

	//virtual to SwitchNode
	virtual Node* getNode( const std::string &nodeId );

	std::vector<Node*> *getNodes();
	unsigned int getNumNodes();

	//virtual to SwitchNode
	virtual bool recursivelyContainsNode( const std::string &nodeId );

	bool recursivelyContainsNode( Node* node );

	//virtual to SwitchNode
	virtual Node* recursivelyGetNode( const std::string &nodeId );

	//virtual to SwitchNode
	virtual bool removeNode( Node* node );

};
}
}
}
}
}

#endif //_COMPOSITENODE_H_
