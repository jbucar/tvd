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

#ifndef _BIND_H_
#define _BIND_H_

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
class Parameter;

namespace descriptor {
class GenericDescriptor;
}
namespace connectors {
class Role;
}
namespace components {
class Node;
}
namespace interfaces {
class InterfacePoint;
}
}
}
}
}
namespace bptn = ::br::pucrio::telemidia::ncl;
namespace bptnd = ::br::pucrio::telemidia::ncl::descriptor;
namespace bptncon = ::br::pucrio::telemidia::ncl::connectors;
namespace bptnc = ::br::pucrio::telemidia::ncl::components;
namespace bptni = ::br::pucrio::telemidia::ncl::interfaces;

#include <string>
#include <map>
#include <vector>
#include <set>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace link {
class Bind
{
private:
	bptnc::Node* node;
	bptni::InterfacePoint* interfacePoint;
	bptnd::GenericDescriptor* descriptor;
	bptncon::Role* role;
	bool embed;

protected:
	std::map<std::string, bptn::Parameter*>* parameters;
	std::set<std::string> typeSet;

public:
	Bind( bptnc::Node* node, bptni::InterfacePoint* interfPt, bptnd::GenericDescriptor* desc, bptncon::Role* role );

	virtual ~Bind() {
	}
	;
	bool instanceOf( const std::string &s );
	bptnd::GenericDescriptor* getDescriptor();
	bptni::InterfacePoint* getInterfacePoint();
	bptnc::Node* getNode();
	bptncon::Role* getRole();
	void setInterfacePoint( bptni::InterfacePoint* interfPt );
	void setNode( bptnc::Node* node );
	void setRole( bptncon::Role* role );
	void setDescriptor( bptnd::GenericDescriptor* desc );
	bptn::Parameter *setParameterValue( const std::string &propertyLabel, bptn::Parameter *propertyValue );

	std::string toString();
	void addParameter( bptn::Parameter* parameter );
	std::vector<bptn::Parameter*>* getParameters();
	bptn::Parameter* getParameter( const std::string &name );
	bool removeParameter( bptn::Parameter *parameter );
	std::vector<bptnc::Node*>* getNodeNesting();
	bptni::InterfacePoint* getEndPointInterface();
};
}
}
}
}
}

#endif //_BIND_H_
