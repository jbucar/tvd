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

#ifndef _LINK_H_
#define _LINK_H_

#include "../Entity.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {

class Parameter;
namespace components {
class Node;
}
namespace descriptor {
class GenericDescriptor;
}
namespace connectors {
class Connector;
class Role;
}
namespace interfaces {
class InterfacePoint;
}
}
}
}
}
namespace bptn = ::br::pucrio::telemidia::ncl;
namespace bptnc = ::br::pucrio::telemidia::ncl::components;
namespace bptnd = ::br::pucrio::telemidia::ncl::descriptor;
namespace bptncon = ::br::pucrio::telemidia::ncl::connectors;
namespace bptni = ::br::pucrio::telemidia::ncl::interfaces;

#include <map>
#include <vector>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace link {

class LinkComposition;
class Bind;

/**
 * Clase que representa a un Link de @c NCL.
 */
class Link: public Entity
{
protected:
	bptncon::Connector *connector;
	std::map<std::string, std::vector<Bind*>*> *roleBinds;
	std::vector<Bind*> *binds;
	std::map<std::string, bptn::Parameter*> *parameters;

private:
	LinkComposition* composition;

public:
	Link( const std::string &id, bptncon::Connector *newConnector );
	virtual ~Link();
	Bind* bind( bptnc::Node *node, bptni::InterfacePoint *interfPt, bptnd::GenericDescriptor *desc, const std::string &roleId );

	Bind* bind( bptnc::Node *node, bptni::InterfacePoint *interfPt, bptnd::GenericDescriptor *desc, bptncon::Role *role );

	bool isConsistent();
	Bind* getBind( bptnc::Node *node, bptni::InterfacePoint *interfPt, bptnd::GenericDescriptor *desc, bptncon::Role *role );

	std::vector<Bind*> *getBinds();
	bptncon::Connector *getConnector();
	void setParentComposition( LinkComposition* composition );
	LinkComposition* getParentComposition();
	unsigned int getNumBinds();
	unsigned int getNumRoleBinds( bptncon::Role *role );
	std::vector<Bind*> *getRoleBinds( bptncon::Role *role );
	bool isMultiPoint();
	std::string toString();
	void setConnector( bptncon::Connector *connector );
	bool unBind( Bind *bind );
	void addParameter( bptn::Parameter *parameter );
	std::vector<bptn::Parameter*> *getParameters();
	bptn::Parameter *getParameter( const std::string &name );
	void removeParameter( bptn::Parameter *parameter );
	void updateConnector( bptncon::Connector *newConnector );
	bool containsNode( bptnc::Node *node, bptnd::GenericDescriptor *descriptor );

protected:
	bool containsNode( bptnc::Node *node, bptnd::GenericDescriptor *descriptor, std::vector<Bind*> *binds );
};
}
}
}
}
}

#endif //_LINK_H_
