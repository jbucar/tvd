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

#include "../../include/connectors/Connector.h"
#include "../../include/connectors/Role.h"
#include "../../include/Parameter.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
Connector::Connector( const std::string &id ) :
		Entity( id ) {
	parameters = new std::map<std::string, Parameter*>;
	typeSet.insert( "Connector" );
}

int Connector::getNumRoles() {
	int numOfRoles;

	std::vector<Role*>* childRoles;
	childRoles = getRoles();
	numOfRoles = childRoles->size();
	delete childRoles;

	return numOfRoles;
}

Role* Connector::getRole( const std::string &roleId ) {
	Role* wRole;
	std::vector<Role*> *roles = getRoles();
	if (roles == NULL) {
		return NULL;
	}

	std::vector<Role*>::iterator i;
	for (i = roles->begin(); i != roles->end(); ++i) {
		wRole = *i;
		if (wRole->getLabel() == roleId) {
			delete roles;
			return wRole;
		}
	}
	delete roles;
	return NULL;
}

void Connector::addParameter( bptn::Parameter* parameter ) {
	if (parameter == NULL
	)
		return;

	std::map<std::string, bptn::Parameter*>::iterator i;
	for (i = parameters->begin(); i != parameters->end(); ++i)
		if (i->first == parameter->getName())
			return;

	(*parameters)[parameter->getName()] = parameter;
}

std::vector<bptn::Parameter*>* Connector::getParameters() {
	if (parameters->empty())
		return NULL;

	std::vector<bptn::Parameter*>* params;
	params = new std::vector<bptn::Parameter*>;
	std::map<std::string, bptn::Parameter*>::iterator i;
	for (i = parameters->begin(); i != parameters->end(); ++i)
		params->push_back( i->second );

	return params;
}

bptn::Parameter* Connector::getParameter( const std::string &name ) {
	if (parameters->empty())
		return NULL;

	std::map<std::string, bptn::Parameter*>::iterator i;
	for (i = parameters->begin(); i != parameters->end(); ++i)
		if (i->first == name)
			return (bptn::Parameter*) (i->second);

	return NULL;
}

bool Connector::removeParameter( const std::string &name ) {
	if (parameters->empty())
		return false;

	std::map<std::string, bptn::Parameter*>::iterator i;
	for (i = parameters->begin(); i != parameters->end(); ++i) {
		if (i->first == name) {
			parameters->erase( i );
			return true;
		}
	}

	return false;
}
}
}
}
}
}
