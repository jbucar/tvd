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

#include "../../include/connectors/CompoundAction.h"
#include "../../include/connectors/SimpleAction.h"
#include "../../include/connectors/Role.h"
#include <util/log.h>
#include <iostream>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
CompoundAction::CompoundAction() :
		Action() {
	actions = new std::vector<Action*>;
	typeSet.insert( "CompoundAction" );
}

CompoundAction::CompoundAction( Action* a1, Action* a2, short op ) :
		Action() {

	actions = new std::vector<Action*>;
	actions->push_back( a1 );
	actions->push_back( a2 );
	myOperator = op;
	typeSet.insert( "CompoundAction" );
}

void CompoundAction::setOperator( short op ) {
	myOperator = op;
}

short CompoundAction::getOperator() {
	return myOperator;
}

std::vector<Action*>* CompoundAction::getActions() {
	if (actions->begin() == actions->end())
		return NULL;

	return actions;
}

void CompoundAction::addAction( Action* action ) {
	std::vector<Action*>::iterator i;

	i = actions->begin();
	while (i != actions->end()) {
		if (action == *i) {
			LWARN("CompoundAction", "Trying to add the action twice");
			return;
		}
		++i;
	}
	actions->push_back( action );
}

void CompoundAction::removeAction( Action* action ) {
	std::vector<Action*>::iterator iterator;
	std::vector<Action*>::iterator i;

	iterator = actions->begin();
	while (iterator != actions->end()) {
		if ((*iterator) == action) {
			i = actions->erase( iterator );
			if (i == actions->end())
				return;
		}
		++iterator;
	}
}

std::vector<Role*>* CompoundAction::getRoles() {
	std::vector<Role*>* roles;
	int i, size;
	Action* action;
	std::vector<Role*>* childRoles;

	roles = new std::vector<Role*>;
	size = actions->size();
	for (i = 0; i < size; i++) {
		action = (Action*) ((*actions)[i]);
		if (action->instanceOf( "SimpleAction" )) {
			roles->push_back( (SimpleAction*) action );
		} else {
			childRoles = ((CompoundAction*) action)->getRoles();
			std::vector<Role*>::iterator it;
			for (it = childRoles->begin(); it != childRoles->end(); ++it) {
				roles->push_back( *it );
			}

			delete childRoles;
		}
	}
	return roles;
}

std::string CompoundAction::toString() {
	std::vector<Action*>::iterator i;
	Action* action;
	std::string actionStr;

	actionStr = "";
	for (i = actions->begin(); i != actions->end(); i++) {
		action = (*i);
		actionStr += (action->toString());
	}
	return actionStr;
}
}
}
}
}
}
