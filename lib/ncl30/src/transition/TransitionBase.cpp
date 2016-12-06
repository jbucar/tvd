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

#include "../../include/transition/TransitionBase.h"
#include "../../include/transition/Transition.h"
#include <iostream>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace transition {
  
/**
 * Constructor de la clase.
 * @param id Un string indicando el id.
 */
TransitionBase::TransitionBase( const std::string &id ) :
		Base( id ) {
}

/**
 * Agrega una transición.
 * @param transition Una instancia de @c Transition.
 * @return True en caso de haber sido agregada, false de lo contrario.
 */
bool TransitionBase::addTransition( Transition* transition ) {
	if (transition == NULL) {
		return false;
	}

	std::vector<Transition*>::iterator i;
	i = transitionSet.begin();
	while (i != transitionSet.end()) {
		if (*i == transition) {
			return false;
		}
		++i;
	}

	transitionSet.push_back( transition );
	return true;
}

bool TransitionBase::addBase( Base* base, const std::string &alias, const std::string &location ) {

	if (base->instanceOf( "TransitionBase" )) {
		return Base::addBase( base, alias, location );
	} else {
		return false;
	}
}

void TransitionBase::clear() {
	transitionSet.clear();
	Base::clear();
}

Transition* TransitionBase::getTransitionLocally( const std::string &transitionId ) {
	std::vector<Transition*>::iterator i;
	Transition* transition;

	i = transitionSet.begin();
	while (i != transitionSet.end()) {
		transition = *i;
		if (transition->getId() == transitionId) {
			return transition;
		}
		++i;
	}
	return NULL;
}

/**
 * @param transitionId Un string con el id de una transición.
 * @return Una instancia de @c Transition que corresponda con el id indicado.
 */
Transition* TransitionBase::getTransition( const std::string &transitionId ) {
	std::string::size_type index;
	std::string prefix, suffix;
	TransitionBase* base;

	index = transitionId.find_first_of( "#" );
	if (index == std::string::npos) {
		return getTransitionLocally( transitionId );
	}

	prefix = transitionId.substr( 0, index );
	index++;
	suffix = transitionId.substr( index, transitionId.length() - index );
	if (baseAliases->count( prefix ) != 0) {
		base = (TransitionBase*) ((*baseAliases)[prefix]);
		return base->getTransition( suffix );

	} else if (baseLocations->count( prefix ) != 0) {
		base = (TransitionBase*) ((*baseLocations)[prefix]);
		return base->getTransition( suffix );

	} else {
		return NULL;
	}
}

/**
 * Elimina una transición de la base.
 * @param transition Una instancia de @c Transition.
 * @return True en caso de haber sido eliminada, false de lo contrario.
 */
bool TransitionBase::removeTransition( Transition* transition ) {
	std::vector<Transition*>::iterator i;
	i = transitionSet.begin();
	while (i != transitionSet.end()) {
		if (*i == transition) {
			transitionSet.erase( i );
			return true;
		}
		++i;
	}

	return false;
}
}
}
}
}
}
