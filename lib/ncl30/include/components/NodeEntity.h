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

#ifndef _NODEENTITY_H_
#define _NODEENTITY_H_

#include "Node.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace interfaces {
class Anchor;
class LambdaAnchor;
}
namespace reuse {
class ReferNode;
}
namespace descriptor {
class GenericDescriptor;
}
}
}
}
}
namespace bptni = ::br::pucrio::telemidia::ncl::interfaces;
namespace bptnr = ::br::pucrio::telemidia::ncl::reuse;
namespace bptnd = ::br::pucrio::telemidia::ncl::descriptor;

#include <string>
#include <set>
#include <vector>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace components {

class Content;

class NodeEntity: public Node
{
protected:
	bptnd::GenericDescriptor* descriptor;
	Content* content;

private:
	std::set<bptnr::ReferNode*>* instSameInstances;
	std::set<bptnr::ReferNode*>* gradSameInstances;

public:
	NodeEntity( const std::string &uid, Content* someContent );
	virtual ~NodeEntity();

	bptni::LambdaAnchor* getLambdaAnchor();
	bptnd::GenericDescriptor* getDescriptor();
	void setDescriptor( bptnd::GenericDescriptor* someDescriptor );
	Content* getContent();
	void setContent( Content* content );
	bool addAnchor( bptni::Anchor *anchor );
	bool addAnchor( int index, bptni::Anchor* anchor );
	void setId( const std::string &id );
	bool removeAnchor( int index );
	bool removeAnchor( bptni::Anchor *anchor );
	void clearAnchors();
	std::set<bptnr::ReferNode*>* getInstSameInstances();
	std::set<bptnr::ReferNode*>* getGradSameInstances();
	bool addSameInstance( bptnr::ReferNode* node );
	void removeSameInstance( bptnr::ReferNode* node );
};
}
}
}
}
}

#endif //_NODEENTITY_H_
