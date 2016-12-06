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

#include "../../include/switches/RuleBase.h"
#include "../../include/switches/Rule.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace switches {
  
/**
 * Constructor de la clase.
 * @param id Un string indicando el id.
 */
RuleBase::RuleBase( const std::string &id ) :
		Base( id ) {
	ruleSet = new std::vector<Rule*>;
	typeSet.insert( "RuleBase" );
}

/**
 * Agrega una regla.
 * @param rule Una instancia de @c Rule.
 * @return True en caso de haber sido agregada, false de lo contrario.
 */
bool RuleBase::addRule( Rule* rule ) {
	if (rule == NULL
	)
		return false;

	std::vector<Rule*>::iterator i;
	for (i = ruleSet->begin(); i != ruleSet->end(); ++i) {
		if (*i == rule)
			return false;
	}

	ruleSet->push_back( rule );
	return true;
}

bool RuleBase::addBase( Base* base, const std::string &alias, const std::string &location ) {

	if (base->instanceOf( "RuleBase" )) {
		return Base::addBase( base, alias, location );
	}
	return false;
}

void RuleBase::clear() {
	ruleSet->clear();
	Base::clear();
}

Rule* RuleBase::getRuleLocally( const std::string &ruleId ) {
	std::vector<Rule*>::iterator rules;

	rules = ruleSet->begin();
	while (rules != ruleSet->end()) {
		if ((*rules)->getId() == ruleId) {
			return (*rules);
		}
		++rules;
	}
	return NULL;
}

/**
 * @param ruleId Un string con el id de una regla..
 * @return Una instancia de @c Rule que corresponda con el id indicado.
 */
Rule* RuleBase::getRule( const std::string &ruleId ) {
	std::string::size_type index;
	std::string prefix, suffix;
	RuleBase* base;

	index = ruleId.find_first_of( "#" );
	if (index == std::string::npos) {
		return getRuleLocally( ruleId );
	}
	prefix = ruleId.substr( 0, index );
	index++;
	suffix = ruleId.substr( index, ruleId.length() - index );
	if (baseAliases->find( prefix ) != baseAliases->end()) {
		base = (RuleBase*) ((*baseAliases)[prefix]);
		return base->getRule( suffix );

	} else if (baseLocations->find( prefix ) != baseLocations->end()) {
		base = (RuleBase*) ((*baseLocations)[prefix]);
		return base->getRule( suffix );

	} else {
		return NULL;
	}
}

/**
 * @return Un vector con las reglas que estan contenidas dentro de la base.
 */
std::vector<Rule*>* RuleBase::getRules() {
	return ruleSet;
}

/**
 * Elimina una regla de la base.
 * @param rule Una instancia de @c Rule.
 * @return True en caso de haber sido eliminada, false de lo contrario.
 */
bool RuleBase::removeRule( Rule* rule ) {
	std::vector<Rule*>::iterator i;
	for (i = ruleSet->begin(); i != ruleSet->end(); ++i) {
		if (*i == rule) {
			ruleSet->erase( i );
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
