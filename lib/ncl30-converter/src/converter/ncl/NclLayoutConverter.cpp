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

#include "../../../include/ncl/NclLayoutConverter.h"
#include "../../../include/ncl/NclDocumentConverter.h"
#include <ncl30/NclDocument.h>
#include <ncl30/layout/RegionBase.h>
#include <ncl30/layout/LayoutRegion.h>
#include <ncl30/util/functions.h>
#include <util/log.h>
#include <util/xml/documenthandler.h>
#include <util/xml/nodehandler.h>
#include <boost/lexical_cast.hpp>

namespace bptn = ::br::pucrio::telemidia::ncl;

#define SET_ATTRIBUTE( name, valid )\
	if (nodeHnd->hasAttribute(parentElement, #name)) {\
		std::string value = nodeHnd->attribute(parentElement, #name);\
		valid &= region->setProperty( #name, value );\
	}\

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {

NclLayoutConverter::NclLayoutConverter( bptcf::DocumentParser *documentParser ) :
		NclLayoutParser( documentParser ) {
}

void NclLayoutConverter::addImportBaseToRegionBase( void *parentObject, void *childObject ) {

	std::map<int, bptnl::RegionBase*>* bases;
	std::map<int, bptnl::RegionBase*>::iterator i;
	std::string baseAlias, baseLocation;
	NclDocumentConverter *compiler;
	bptn::NclDocument *importedDocument;

	// get the external base alias and location
	util::xml::dom::DocumentHandler *docHnd = getDocumentParser()->getDocumentHandler();
	baseAlias = docHnd->handler()->attribute(static_cast<util::xml::dom::Node>(childObject), "alias");
	baseLocation = docHnd->handler()->attribute(static_cast<util::xml::dom::Node>(childObject), "documentURI");

	compiler = (NclDocumentConverter*) getDocumentParser();
	importedDocument = compiler->importDocument( baseLocation ); //TODO memory leak
	if (importedDocument == NULL) {
		return;
	}

	bases = importedDocument->getRegionBases();
	if (bases == NULL || bases->empty()) {
		return;
	}

	// insert the imported base into the document region base
	i = bases->begin();
	while (i != bases->end()) {
		((bptnl::RegionBase*) parentObject)->addBase( i->second, baseAlias, baseLocation );
		++i;
	}
}

/**
 * Agrega una nueva región dentro de otra.
 * @param parentObject La región a la que se agregará la subregión.
 * @param childObject Subregión a ser agregada.
 */
void NclLayoutConverter::addRegionToRegion( void *parentObject, void *childObject ) {

	((bptnl::LayoutRegion*) parentObject)->addRegion( (bptnl::LayoutRegion*) childObject );
}

/**
 * Agrega una región a un regionBase.
 * @param parentObject Instancia de @c RegionBase a la que se agregará la región.
 * @param childObject La región a ser agregada.
 */
void NclLayoutConverter::addRegionToRegionBase( void *parentObject, void *childObject ) {

	bptnl::RegionBase *layout = (bptnl::RegionBase*) parentObject;
	layout->addRegion( (bptnl::LayoutRegion*) childObject );
}

/**
 * Crea un regionBase.
 * @param parentElement El elemento DOM que representa a un regionBase.
 * @return Una nueva instancia de @c RegionBase.
 */
void *NclLayoutConverter::createRegionBase( util::xml::dom::Node parentElement ) {
	bptnl::RegionBase *layout;
	std::string mapId = "";

	layout = new bptnl::RegionBase(getDocumentHandler()->handler()->attribute(parentElement, "id").c_str());
	bptnl::Device device;

	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	// device attribute
	if (nodeHnd->hasAttribute(parentElement, "device")) {
		// region for output bit map attribute
		if (nodeHnd->hasAttribute(parentElement, "region" )) {
			mapId = nodeHnd->attribute(parentElement, "region");
		}

		std::string deviceStr = nodeHnd->attribute(parentElement, "device");
		if ( !isDeviceValid( deviceStr, device ) ){
			LERROR("NclLayoutConverter", "invalid device, value=%s", deviceStr.c_str());
			return NULL;
		}

	} else {
		device.name = "systemScreen(0)";
		device.number = 0;
	}
	layout->setDevice( device, mapId );

	return layout;
}

bool NclLayoutConverter::isDeviceValid(const std::string &deviceStr, bptnl::Device &device) {
	bool isValid;

	device.name = deviceStr;
	isValid = deviceStr.find("systemScreen(") != std::string::npos;
	isValid |= deviceStr.find("systemAudio(") != std::string::npos;
	try{
		int devNumber = boost::lexical_cast<int>(deviceStr.at(deviceStr.size() - 2));
		device.number = devNumber;
		if (devNumber < 0){
			isValid = false;
		}
	} catch (boost::bad_lexical_cast &) {
		isValid = false;
	}
	return isValid;
}

bptnl::LayoutRegion* NclLayoutConverter::createDummyRegBase(){

	bptn::NclDocument *document;
	bptnl::Device device = { "systemScreen(0)", 0};

	LDEBUG("NclLayoutConverter", "Creating dummy region");

	bptnl::LayoutRegion *region = new bptnl::LayoutRegion( "dummyRegion" );

	document = (bptn::NclDocument*) getDocumentParser()->getObject( "return", "document" );
	bptnl::RegionBase *aRegionBase = document->getRegionBase( 0 );

	if ( aRegionBase == NULL ) {
		aRegionBase = new bptnl::RegionBase( "dummyRegionBase" );
		aRegionBase->setDevice( device, "" );
		document->addRegionBase( aRegionBase );
	}

	addRegionToRegionBase(aRegionBase, region);

	return region;
}

/**
 * Crea una nueva región.
 * @param parentElement Elemento DOM representando a la región.
 * @param parent Un puntero a void con el elemento padre.
 */
void *NclLayoutConverter::createRegion( util::xml::dom::Node parentElement, void* parent ) {
	bool isValid = true;

	std::string id = getNodeHandler()->attribute(parentElement, "id");
	bptnl::LayoutRegion *region = new bptnl::LayoutRegion( id );

	region->setParent( (bptnl::LayoutRegion *) parent );

	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();
	SET_ATTRIBUTE( width, isValid );
	SET_ATTRIBUTE( left, isValid );
	SET_ATTRIBUTE( right, isValid );

	SET_ATTRIBUTE( height, isValid );
	SET_ATTRIBUTE( top, isValid );
	SET_ATTRIBUTE( bottom, isValid );

	SET_ATTRIBUTE( zIndex, isValid );
	SET_ATTRIBUTE( title, isValid );

	if (!isValid){
		region = NULL;
	}
	return region;
}

}
}
}
}
}
