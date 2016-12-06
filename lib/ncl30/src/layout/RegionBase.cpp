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

#include "../../include/layout/RegionBase.h"
#include "../../include/layout/LayoutRegion.h"
#include <boost/lexical_cast.hpp>
#include <util/log.h>
#include <iostream>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace layout {
  
/**
 * Constructor de la clase.
 * @param id Un string indicando el id.
 */
RegionBase::RegionBase( const std::string &id ) :
		Base( id ) {
	this->regions = new std::map<std::string, LayoutRegion*>;
	this->outputMapRegionId = "";
	this->deviceRegion = NULL;

	typeSet.insert( "RegionBase" );
}

/**
 * Destructor de la clase.
 * @note Se eliminarán todas las regiones que contiene.
 */
RegionBase::~RegionBase() {
	if (regions != NULL) {
		regions->clear();
		delete regions;
		regions = NULL;
	}
	if (deviceRegion != NULL) {
		delete deviceRegion;
		deviceRegion = NULL;
	}

}

/**
 * Agrega una región.
 * @param region Una instancia de @c LayoutRegion.
 * @return True en caso de haber sido agregado, false de lo contrario.
 */
bool RegionBase::addRegion( LayoutRegion* region ) {
	std::map<std::string, LayoutRegion*>::iterator i;
	std::string regId;

	regId = region->getId();

	i = regions->find( regId );
	if (i != regions->end()) {
		LWARN("RegionBase", "Trying to add a region with id '%s' twice ", regId.c_str());
		return false;

	} else {
		deviceRegion->addRegion( region );
		region->setParent( deviceRegion );
		(*regions)[regId] = region;
		return true;
	}
}

/**
 * Agrega una sub-base
 * @param base La base a agregar.
 * @param alias Un string indicando el alias de la base.
 * @param location Un string indicando la ubicación de base.
 * @return True en caso de haber sido agregado, false de lo contrario.
 */
bool RegionBase::addBase( Base* base, const std::string &alias, const std::string &location ) {

	if (base->instanceOf( "RegionBase" )) {
		return Base::addBase( base, alias, location );
	}
	return false;
}

LayoutRegion* RegionBase::getRegionLocally( const std::string &regionId ) {
	std::map<std::string, LayoutRegion*>::iterator childRegions;
	LayoutRegion *region;
	LayoutRegion *auxRegion;

	childRegions = regions->begin();
	while (childRegions != regions->end()) {
		region = childRegions->second;
		if (region->getId() == regionId) {
			return region;
		}
		auxRegion = region->getRegionRecursively( regionId );
		if (auxRegion != NULL) {
			return auxRegion;
		}
		++childRegions;
	}
	return NULL;
}

const std::string &RegionBase::getOutputMapRegionId() {
	return outputMapRegionId;
}

/**
 * @param regionId Un string con el id de una región.
 * @return Una instancia de @c LayoutRegion que corresponda con el id indicado.
 */
LayoutRegion* RegionBase::getRegion( const std::string &regionId ) {
	std::string::size_type index;
	std::string prefix, suffix;
	RegionBase* base;

	index = regionId.find_first_of( "#" );
	if (index == std::string::npos) {
		return getRegionLocally( regionId );
	}
	prefix = regionId.substr( 0, index );
	index++;
	suffix = regionId.substr( index, regionId.length() - index );
	if (baseAliases->find( prefix ) != baseAliases->end()) {
		base = (RegionBase*) ((*baseAliases)[prefix]);
		return base->getRegion( suffix );

	} else if (baseLocations->find( prefix ) != baseLocations->end()) {
		base = (RegionBase*) ((*baseLocations)[prefix]);
		return base->getRegion( suffix );

	} else {
		return NULL;
	}
}

/**
 * @return Una instancia de @c LayoutRegion que representa el layout.
 */
LayoutRegion* RegionBase::getLayout() const {
	return deviceRegion;
}

/**
 * @return Un vector con las regiones que contiene la base.
 */
std::vector<LayoutRegion*>* RegionBase::getRegions() {
	std::map<std::string, LayoutRegion*>::iterator i;
	std::vector<LayoutRegion*>* childRegions;
	childRegions = new std::vector<LayoutRegion*>;
	for (i = regions->begin(); i != regions->end(); ++i) {
		childRegions->push_back( i->second );
	}
	return childRegions;
}

/**
 * Elimina una región de la base.
 * @param region Una instancia de @c LayoutRegion.
 * @return True en caso de haber sido eliminada, false de lo contrario.
 */
bool RegionBase::removeRegion( LayoutRegion* region ) {
	std::map<std::string, LayoutRegion*>::iterator i;
	for (i = regions->begin(); i != regions->end(); ++i) {
		if (i->second == region) {
			regions->erase( i );
			return true;
		}
	}
	return false;
}

/**
 * Setea el device.
 * @param device Una instancia de @c Device.
 * @param regionId Un string con el id de la región a usar como outputMap.
 */
void RegionBase::setDevice( Device device, const std::string &regionId ) {
	_device = device;
	outputMapRegionId = regionId;

	if (deviceRegion != NULL) {
		delete deviceRegion;
		deviceRegion = NULL;
	}
	deviceRegion = new LayoutRegion( _device.name );
	deviceRegion->setDevice( _device );
}

/**
 * @return El device class.
 */
int RegionBase::getDeviceClass() {
	return _device.number;
}

/**
 * Elimina las regiones contenidas en la base.
 */
void RegionBase::clear() {
	regions->clear();
	Base::clear();
}
}
}
}
}
}
