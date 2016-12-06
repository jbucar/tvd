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

#include "../../include/descriptor/DescriptorBase.h"
#include "../../include/descriptor/GenericDescriptor.h"
#include <algorithm>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace descriptor {
  
/**
 * Constructor de la clase.
 * @param id Un string indicando el id.
 */
DescriptorBase::DescriptorBase( const std::string &id ) :
		Base( id ) {
	typeSet.insert( "DescriptorBase" );
}

/**
 * Destructor de la clase.
 * @note Elimina todas las referencias de las sub-bases.
 */
DescriptorBase::~DescriptorBase() {
	for (unsigned int i = 0; i < descriptorSet.size(); i++) {
		if (descriptorSet.at( i ) != NULL) {
			GenericDescriptor *desc = descriptorSet.at( i );
			delete desc;
			desc = NULL;
		}
	}
	Base::clear();
}

/**
 * Agrega un descriptor.
 * @param descriptor Una instancia de @c GenericDescriptor.
 * @return True en caso de haber sido agregado, false de lo contrario.
 */
bool DescriptorBase::addDescriptor( GenericDescriptor* descriptor ) {
	if (descriptor == NULL
	)
		return false;

	std::vector<GenericDescriptor*>::const_iterator i;

	i = std::find( descriptorSet.begin(), descriptorSet.end(), descriptor );
	if (i != descriptorSet.end())
		return false;
	descriptorSet.push_back( descriptor );
	return true;
}

/**
 * Agrega una sub-base
 * @param base La base a agregar.
 * @param alias Un string indicando el alias de la base.
 * @param location Un string indicando la ubicación de base.
 * @return True en caso de haber sido agregado, false de lo contrario.
 */
bool DescriptorBase::addBase( Base* base, const std::string &alias, const std::string &location ) {

	if (base->instanceOf( "DescriptorBase" )) {
		return Base::addBase( base, alias, location );
	}
	return false;
}

/**
 * Elimina las sub-bases.
 */
void DescriptorBase::clear() {
	descriptorSet.clear();
	Base::clear();
}

GenericDescriptor* DescriptorBase::getDescriptorLocally( const std::string &descriptorId ) {
	std::vector<GenericDescriptor*>::const_iterator descriptors;

	descriptors = descriptorSet.begin();
	while (descriptors != descriptorSet.end()) {
		if ((*descriptors)->getId() == descriptorId) {
			return (*descriptors);
		}
		++descriptors;
	}
	return NULL;
}

/**
 * @param descriptorId Un string con el id de un descriptor.
 * @return Una instancia de @c GenericDescriptor que corresponda con el id indicado.
 */
GenericDescriptor* DescriptorBase::getDescriptor( const std::string &descriptorId ) {
	std::string::size_type index;
	std::string prefix, suffix;
	DescriptorBase* base;

	index = descriptorId.find_first_of( "#" );
	if (index == std::string::npos) {
		return getDescriptorLocally( descriptorId );
	}
	prefix = descriptorId.substr( 0, index );
	index++;
	suffix = descriptorId.substr( index, descriptorId.length() - index );
	if (baseAliases->find( prefix ) != baseAliases->end()) {
		base = (DescriptorBase*) ((*baseAliases)[prefix]);
		return base->getDescriptor( suffix );

	} else if (baseLocations->find( prefix ) != baseLocations->end()) {
		base = (DescriptorBase*) ((*baseLocations)[prefix]);
		return base->getDescriptor( suffix );

	} else {
		return NULL;
	}
}

/**
 * @return Un vector con los descriptores de la base.
 */
std::vector<GenericDescriptor*>* DescriptorBase::getDescriptors() {
	return &descriptorSet;
}

/**
 * Elimina un descriptor de la base.
 * @param descriptor Una instancia de @c GenericDescriptor.
 * @return True en caso de haber sido eliminado, false de lo contrario.
 */
bool DescriptorBase::removeDescriptor( GenericDescriptor* descriptor ) {
	std::vector<GenericDescriptor*>::iterator i;
	i = std::find( descriptorSet.begin(), descriptorSet.end(), descriptor );
	if (i != descriptorSet.end()) {
		descriptorSet.erase( i );
		return true;
	}
	return false;
}
}
}
}
}
}
