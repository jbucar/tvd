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

#include "../include/Base.h"
#include <boost/foreach.hpp>
#include <algorithm>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
  
/**
 * Construye una instancia de la clase.
 * @param paramId Es un string con el id.
 */
Base::Base( const std::string & paramId ) :
		id( paramId ),
		baseAliases( new std::map<std::string, Base*> ),
		baseLocations( new std::map<std::string, Base*> ) {

	typeSet.insert( "Base" );
}

/**
 * Destructor de la clase.
 */
Base::~Base() {
	if (baseAliases != NULL) {
		baseAliases->clear();
		delete baseAliases;
		baseAliases = NULL;
	}
	if (baseLocations != NULL) {
		baseLocations->clear();
		delete baseLocations;
		baseLocations = NULL;
	}
//         if (baseSet != NULL){
//             baseSet->clear();
//             delete baseSet;
//             baseSet = NULL;
//         }
}

/**
 * Agrega una sub-base.
 * @param base El objeto representando la base a agregar.
 * @param alias Un string indicando un alias.
 * @param location Un string indicando la ubicación de la base.
 * @return True en caso de haberla agregado, false de lo contrario.
 */
bool Base::addBase( Base* base, const std::string & alias, const std::string & location ) {

	if (base == NULL || location == "") {
		return false;
	}

	std::vector<Base*>::const_iterator i;
	i = std::find( baseSet.begin(), baseSet.end(), base );
	if (i != baseSet.end()) {
		return false;
	}

	baseSet.push_back( base );

	if (alias != "") {
		(*baseAliases)[alias] = base;
	}

	(*baseLocations)[location] = base;
	return true;
}

/**
 * Elimina las referencias a las sub-bases.
 */
void Base::clear() {
	baseSet.clear();
	baseAliases->clear();
	baseLocations->clear();
}

/**
 * @param baseId String indicando el id de la base.
 * @return Un objeto base que se corresponde con el id pasado por parámetro.
 */
Base* Base::getBase( const std::string & baseId ) {
	BOOST_FOREACH( Base * tempBase, baseSet )
	{
		if (tempBase->getId() == baseId) {
			return tempBase;
		}
	}
	return NULL;
}

/**
 * @param base Una instancia de la clase @c Base.
 * @return El alias del objeto pasado por parámetro.
 */
std::string Base::getBaseAlias( Base* base ) {
	std::map<std::string, Base*>::iterator i;
	for (i = baseAliases->begin(); i != baseAliases->end(); ++i) {
		if (i->second == base) {
			return i->first;
		}
	}
	return "";
}

/**
 * @param base Una instancia de la clase @c Base.
 * @return Un String con la ubicación de la base pasada por parámetro.
 */
std::string Base::getBaseLocation( Base* base ) {
	std::map<std::string, Base*>::iterator i;
	for (i = baseLocations->begin(); i != baseLocations->end(); ++i) {
		if (i->second == base) {
			return i->first;
		}
	}
	return "";
}

/**
 * @return Un vector con las sub-bases.
 */
std::vector<Base*>* Base::getBases() {
	return &baseSet;
}

/**
 * Elimina una sub-base.
 * @param base Una instancia de la clase @c Base.
 * @return True en caso de haber sido eliminada, false de lo contrario.
 */
bool Base::removeBase( Base* base ) {
	std::string alias, location;

	alias = getBaseAlias( base );
	location = getBaseLocation( base );

	std::vector<Base*>::iterator i;
	i = std::find( baseSet.begin(), baseSet.end(), base );
	if (i != baseSet.end()) {
		baseSet.erase( i );
		if (alias != "") {
			baseAliases->erase( alias );
		}
		baseLocations->erase( location );
		return true;
	}
	return false;
}

/**
 * Cambia el alias a una sub-base.
 * @param base La instancia de la clase @c Base a la que se le cambiará el alias.
 * @param alias Un string indicando el nuevo alias.
 */
void Base::setBaseAlias( Base* base, const std::string & alias ) {
	std::string oldAlias;
	oldAlias = getBaseAlias( base );

	if (oldAlias != "") {
		baseAliases->erase( oldAlias );
	}

	if (alias != "") {
		(*baseAliases)[alias] = base;
	}
}

/**
 * Cambia la ubicación de una sub-base.
 * @param base La instancia de la clase @c Base a la que se le cambiará la ubicación.
 * @param alias Un string indicando la nueva ubicación.
 */
void Base::setBaseLocation( Base* base, const std::string & location ) {
	std::string oldLocation;

	oldLocation = getBaseLocation( base );
	if (oldLocation == "")
		return;

	baseLocations->erase( oldLocation );
	(*baseLocations)[location] = base;
}

/**
 * @return Un string con el id de la base.
 */
const std::string & Base::getId() {
	return id;
}

/**
 * Cambia el id de la base.
 * @param id String con el nuevo id.
 */
void Base::setId( const std::string & id ) {
	this->id = id;
}

/**
 * Indica si el objeto es una insancia de la clase indicada.
 * @param s Nombre de la clase a verificar.
 * @return True en caso de ser una instancia de s, false de lo contrario.
 */
bool Base::instanceOf( const std::string & s ) {
	if (!typeSet.empty()) {
		return (typeSet.find( s ) != typeSet.end());
	} else {
		return false;
	}
}
}
}
}
}
