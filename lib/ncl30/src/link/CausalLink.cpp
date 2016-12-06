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

#include "../../include/link/CausalLink.h"
#include "../../include/link/Bind.h"
#include "../../include/components/Node.h"
#include "../../include/descriptor/GenericDescriptor.h"
#include "../../include/connectors/Connector.h"
#include "../../include/connectors/Role.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace link {
CausalLink::CausalLink( const std::string &uid, bptncon::Connector* connector ) :
		Link( uid, connector ) {

	typeSet.insert( "CausalLink" );
}

bool CausalLink::containsSourceNode( bptnc::Node *node, bptnd::GenericDescriptor* descriptor ) {

	bool contains;
	std::vector<Bind*>* conds;

	conds = getConditionBinds();
	contains = Link::containsNode( node, descriptor, conds );

	delete conds;
	return contains;
}

std::vector<Bind*>* CausalLink::getActionBinds() {
	std::vector<Bind*>* actionsVector;
	actionsVector = new std::vector<Bind*>;
	std::vector<Bind*>::iterator i;

	if (binds->empty()) {
		delete actionsVector;
		return NULL;
	}

	for (i = binds->begin(); i != binds->end(); ++i) {
		if (((*i)->getRole())->instanceOf( "SimpleAction" ))
			actionsVector->push_back( *i );
	}

	if (actionsVector->empty()) {
		delete actionsVector;
		return NULL;
	}

	return actionsVector;
}

std::vector<Bind*>* CausalLink::getConditionBinds() {
	if (binds->empty()) {
		return NULL;
	}

	std::vector<Bind*>* conditionsVector;
	conditionsVector = new std::vector<Bind*>;

	std::vector<Bind*>::iterator iterator;
	Bind* bind;

	for (iterator = binds->begin(); iterator != binds->end(); ++iterator) {
		bind = (Bind*) (*iterator);
		if ((bind->getRole())->instanceOf( "SimpleCondition" ) || (bind->getRole())->instanceOf( "AttributeAssessment" )) {

			conditionsVector->push_back( bind );
		}
	}

	if (conditionsVector->empty()) {
		delete conditionsVector;
		return NULL;
	}

	return conditionsVector;
}
}
}
}
}
}
