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

#include "../../include/focus/FormatterFocusManager.h"
#include <gingaplayer/player/settings.h>
#include <gingaplayer/player/keys.h>
#include <gingaplayer/system.h>
#include "generated/config.h"
#include "../../include/emconverter/FormatterConverter.h"
#include <util/log.h>
#include <util/keydefs.h>

using namespace ::br::pucrio::telemidia::ginga::ncl::emconverter;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace focus {

namespace impl {
static FormatterFocusManager::KeyPressCallback _onKey;
}

FormatterFocusManager::FormatterFocusManager( PlayerAdapterManager* playerManager, emconverter::FormatterConverter* converter, player::System *system ) {
	currentFocus = "";
	objectToSelect = "";
	isHandler = false;
	selectedObject = NULL;
	this->playerManager = playerManager;
	this->converter = converter;
	_sys = system;
}

FormatterFocusManager::~FormatterFocusManager() {
	unregister();
	selectedObject = NULL;
	playerManager = NULL;
	converter = NULL;
	_sys = NULL;
}

bool FormatterFocusManager::setKeyHandler( bool isHandler ) {
	if (this->isHandler == isHandler) {
		return false;
	}

	this->isHandler = isHandler;
	setHandlingObjects( isHandler );

	return isHandler;
}

void FormatterFocusManager::setHandlingObjects( bool isHandling ) {
	converter->setHandlingStatus( isHandling );
}

ExecutionObject* FormatterFocusManager::getObjectFromFocusIndex( const std::string &focusIndex ) {

	map<string, set<ExecutionObject*>*>::iterator i;
	set<ExecutionObject*>::iterator j;
	CascadingDescriptor* desc;
	bool visible;

	i = focusTable.find( focusIndex );
	if (i == focusTable.end()) {
		return NULL;
	}

	j = i->second->begin();
	while (j != i->second->end()) {
		desc = (*j)->getDescriptor();
		if (desc != NULL && desc->getFormatterRegion() != NULL) {
			visible = desc->getFormatterRegion()->isVisible();
			if (visible) {
				return *j;
			}
		}
		++j;
	}

	delete i->second;
	focusTable.erase( i );

	return NULL;
}

void FormatterFocusManager::insertObject( ExecutionObject* obj, const std::string &focusIndex ) {

	string auxIndex;
	map<string, set<ExecutionObject*>*>::iterator i;
	vector<string>::iterator j;
	set<ExecutionObject*>* objs;

	i = focusTable.find( focusIndex );
	if (i != focusTable.end()) {
		objs = i->second;

	} else {
		objs = new set<ExecutionObject*>;
		focusTable[focusIndex] = objs;
	}

	objs->insert( obj );

	// put new index in alphabetical order
	for (j = focusSequence.begin(); j != focusSequence.end(); ++j) {
		auxIndex = *j;
		if (focusIndex.compare( auxIndex ) < 0) {
			break;

		} else if (focusIndex.compare( auxIndex ) == 0) {
			return;
		}
	}

	focusSequence.insert( j, focusIndex );
}

void FormatterFocusManager::removeObject( ExecutionObject* obj, const std::string &focusIndex ) {

	map<string, set<ExecutionObject*>*>::iterator i;
	set<ExecutionObject*>::iterator j;
	vector<string>::iterator k;
	set<ExecutionObject*>* objs;

	i = focusTable.find( focusIndex );
	if (i != focusTable.end()) {
		objs = i->second;
		j = objs->find( obj );
		if (j != objs->end()) {
			objs->erase( j );
		}
		if (objs->empty()) {
			delete objs;
			focusTable.erase( i );
		}
	}

	k = focusSequence.begin();
	while (k != focusSequence.end()) {
		if (*k == focusIndex) {
			focusSequence.erase( k );
			k = focusSequence.begin();

		} else {
			++k;
		}
	}
}

void FormatterFocusManager::setKeyMaster( const std::string &focusIndex ) {
	ExecutionObject* nextObject = NULL;
	ExecutionObject* currentObject = NULL;
	CascadingDescriptor* currentDescriptor = NULL;
	CascadingDescriptor* nextDescriptor = NULL;
	bool isId = false;

	if (focusIndex == "" && selectedObject != NULL) {
		objectToSelect = "";
		selectedObject->setHandler( false );
		selectedObject->getDescriptor()->getFormatterRegion()->setSelection( false );

		recoveryDefaultState( selectedObject );

		exitSelection( selectedObject );
		return;
	}

	nextObject = getObjectFromFocusIndex( focusIndex );
	if (nextObject == NULL) {
		nextObject = converter->getObjectFromNodeId( focusIndex );

		if (nextObject == NULL) {
			LWARN("FormatterFocusManager", "setKeyMaster: can't set '%s' as master: object is not available", focusIndex.c_str());
			objectToSelect = focusIndex;
			return;
		}

		isId = true;
	}

	nextDescriptor = nextObject->getDescriptor();
	if (nextDescriptor == NULL) {
		LWARN("FormatterFocusManager", "setKeyMaster: can't set '%s' as master: NULL descriptor", focusIndex.c_str());

		if (isId) {
			objectToSelect = focusIndex;
		}

		return;
	}

	currentObject = getObjectFromFocusIndex( currentFocus );
	if (currentObject != NULL && currentObject != nextObject) {
		//recovering focused object
		currentDescriptor = currentObject->getDescriptor();
		if (currentDescriptor != NULL) {
			//LayoutRegion* ncmRegion = currentDescriptor->getRegion();
			if (currentDescriptor->getFormatterRegion() != NULL) {
				currentDescriptor->getFormatterRegion()->setFocus( false );
			}
		}
		recoveryDefaultState( currentObject );

	} else {
	}

	if (!isId) {
		currentFocus = focusIndex;

	} else {
		currentFocus = nextDescriptor->getFocusIndex();
	}

	if (nextDescriptor->getFormatterRegion() != NULL) {
		if (nextDescriptor->getFormatterRegion()->isVisible() && nextDescriptor->getFormatterRegion()->setSelection( true )) {

			//recovering old selected object
			if (selectedObject != NULL && selectedObject != nextObject) {
				selectedObject->setHandler( false );
				selectedObject->getDescriptor()->getFormatterRegion()->setSelection( false );

				recoveryDefaultState( selectedObject );
			}

			//selecting new object
			selectedObject = nextObject;
			selectedObject->setHandler( true );

			enterSelection( selectedObject );
			nextObject->selectionEvent( ::util::key::null, playerManager->uptime( selectedObject ) );

		} else {
			LWARN("FormatterFocusManager", "setKeyMaster: can't select '%s'", currentFocus.c_str());
			if (isId) {
				objectToSelect = focusIndex;
			}
		}

	} else {
		LWARN("FormatterFocusManager", "setKeyMaster: can't select '%s' region == NULL", focusIndex.c_str());
		if (isId) {
			objectToSelect = focusIndex;
		}
	}
}

void FormatterFocusManager::setFocus( const std::string &focusIndex ) {
	ExecutionObject* nextObject = NULL;
	ExecutionObject* currentObject = NULL;
	CascadingDescriptor* currentDescriptor = NULL;
	CascadingDescriptor* nextDescriptor = NULL;

	if (focusIndex == currentFocus && currentFocus != "") {
		LDEBUG("FormatterFocusManager", "setFocus: unnecessary currentFocus='%s' focusIndex='%s'", currentFocus.c_str(), focusIndex.c_str());
		return;
	}

	nextObject = getObjectFromFocusIndex( focusIndex );
	if (nextObject == NULL) {
		LWARN("FormatterFocusManager", "setFocus: index '%s' is not visible", focusIndex.c_str());
		return;
	}

	currentObject = getObjectFromFocusIndex( currentFocus );
	if (currentObject != NULL) {
		currentDescriptor = currentObject->getDescriptor();
	}

	currentFocus = focusIndex;
	player::settings::setProperty( "service.currentFocus", currentFocus );

	if (currentDescriptor != NULL) {
		recoveryDefaultState( currentObject );
		currentDescriptor->getFormatterRegion()->setFocus( false );

	} else if (currentObject != NULL) {
		LWARN("FormatterFocusManager", "setFocus: can't recover '%s'", currentObject->getId().c_str());
	}

	nextDescriptor = nextObject->getDescriptor();
	if (nextDescriptor != NULL) {
		nextDescriptor->getFormatterRegion()->setFocus( true );
	}
}

void FormatterFocusManager::recoveryDefaultState( ExecutionObject* object ) {
	if (object == NULL || object->getDescriptor() == NULL || object->getDescriptor()->getFormatterRegion() == NULL) {
		return;
	}
}

void FormatterFocusManager::showObject( ExecutionObject* object ) {
	CascadingDescriptor* descriptor;
	string focusIndex, auxIndex;
	string paramValue;
	vector<string>::iterator i;
	bool isId = false;

	if (object == NULL) {
		LWARN("FormatterFocusManager", "showObject: object is null");
		return;
	}

	descriptor = object->getDescriptor();
	if (descriptor == NULL) {
		LWARN("FormatterFocusManager", "showObject: descriptor is null");
		return;
	}

	focusIndex = descriptor->getFocusIndex();
	if (focusIndex == "") {
		isId = true;
		focusIndex = object->getDataObject()->getDataEntity()->getId();

	} else {
		insertObject( object, focusIndex );
	}

	if (currentFocus == "") {


		paramValue = player::settings::getProperty( "service.currentKeyMaster" );

		if (paramValue == focusIndex || focusIndex == objectToSelect) {
			objectToSelect = "";
			setKeyMaster( focusIndex );

		} else if (!isId) {
			setFocus( focusIndex );
		}

	} else {
		paramValue = player::settings::getProperty( "service.currentFocus" );

		if (paramValue != "" && paramValue == focusIndex && !isId && descriptor->getFormatterRegion()->isVisible()) {

			/*if (focusTable->count(currentFocus) != 0) {
			 currentObject = (*focusTable)[currentFocus];
			 currentObject->getDescriptor()->
			 getFormatterRegion()->setFocus(false);

			 recoveryDefaultState(currentObject);
			 }

			 currentFocus = focusIndex;*/
			setFocus( focusIndex );
		}

		paramValue = player::settings::getProperty( "service.currentKeyMaster" );

		if ((paramValue == focusIndex || focusIndex == objectToSelect) && descriptor->getFormatterRegion()->isVisible()) {

			// first set as current focus
			/*if (focusTable->count(currentFocus) != 0) {
			 currentObject = (*focusTable)[currentFocus];
			 currentObject->getDescriptor()->
			 getFormatterRegion()->setFocus(false);

			 recoveryDefaultState(currentObject);
			 }

			 currentFocus = focusIndex;
			 PresentationContext::getInstance()->setPropertyValue(
			 "service.currentFocus", currentFocus);*/

			objectToSelect = "";

			if (!isId) {
				setFocus( focusIndex );
			}

			// then set as selected
			if (descriptor->getFormatterRegion()->setSelection( true )) {
				// unselect the previous selected object, if exists
				if (selectedObject != NULL) {
					selectedObject->setHandler( false );
					selectedObject->getDescriptor()->getFormatterRegion()->setSelection( false );

					recoveryDefaultState( selectedObject );
				}

				selectedObject = object;
				selectedObject->setHandler( true );
				enterSelection( selectedObject );
			}
		}
	}

}

void FormatterFocusManager::hideObject( ExecutionObject* object ) {
	string focusIndex;
	FormatterRegion* region;
	map<string, set<ExecutionObject*>*>::iterator i;

	if (object == NULL || object->getDescriptor() == NULL || object->getDescriptor()->getFormatterRegion() == NULL) {

		return;
	}

	region = object->getDescriptor()->getFormatterRegion();
	focusIndex = object->getDescriptor()->getFocusIndex();
	if (focusIndex != "") {
		removeObject( object, focusIndex );

		if (region->isSelected() && selectedObject == object) {

			exitSelection( selectedObject );
			//region->setSelection(false);
			//recoveryDefaultState(selectedObject);
			selectedObject = NULL;
		}

		if (currentFocus == focusIndex) {
			//region->setFocus(false);
			//recoveryDefaultState(object);

			if (focusSequence.empty()) {
				currentFocus = "";

			} else {
				string ix = *(focusSequence.begin());
				i = focusTable.find( ix );
				while (i == focusTable.end()) {
					focusSequence.erase( focusSequence.begin() );
					if (focusSequence.empty()) {
						currentFocus = "";
						return;
					}
					ix = *(focusSequence.begin());
					i = focusTable.find( ix );
				}
				setFocus( ix );
			}
		}
	}

}

bool FormatterFocusManager::keyCodeOk( ExecutionObject* currentObject ) {
	bool isHandling = false;

	if (currentObject != selectedObject) {
		if (selectedObject != NULL) {
			LWARN("FormatterFocusManager", "keyCodeOk: selecting an object with another selected");
			selectedObject->setHandler( false );
		}
	}

	selectedObject = currentObject;
	selectedObject->setHandler( true );

	changeSettingState( "service.currentKeyMaster", "start" );
	isHandling = enterSelection( selectedObject );

	if (selectedObject != NULL) {
		selectedObject->selectionEvent( ::util::key::null, playerManager->uptime( selectedObject ) );
	}

	changeSettingState( "service.currentKeyMaster", "stop" );

	return isHandling;
}

bool FormatterFocusManager::keyCodeBack() {
	CascadingDescriptor* selectedDescriptor;
	FormatterRegion* fr;
	string ix;

	if (selectedObject == NULL) {
		LDEBUG("FormatterFocusManager", "keyCodeBack: NULL selObject");
		return false;
	}

	selectedObject->setHandler( false );
	selectedDescriptor = selectedObject->getDescriptor();
	if (selectedDescriptor == NULL) {
		LDEBUG("FormatterFocusManager", "keyCodeBack: NULL selDescriptor");
		return false;
	}

	ix = selectedDescriptor->getFocusIndex();
	fr = selectedDescriptor->getFormatterRegion();
	if (fr == NULL) {
		LDEBUG("FormatterFocusManager", "keyCodeBack: NULL formatterRegion");
		return false;
	}

	fr->setSelection( false );

	if (ix == currentFocus) {
		fr->setFocus( true );
	}

	changeSettingState( "service.currentKeyMaster", "start" );
	exitSelection( selectedObject );
	changeSettingState( "service.currentKeyMaster", "stop" );
	selectedObject = NULL;

	return false;
}

bool FormatterFocusManager::enterSelection( ExecutionObject* object ) {
	bool newHandler = false;
	string keyMaster;

	if (selectedObject != NULL) {
		keyMaster = (selectedObject->getDataObject()->getDataEntity()->getId());
		player::settings::setProperty( "service.currentKeyMaster", keyMaster );
		newHandler = playerManager->setKeyHandler( object, true );

	}

	return newHandler;
}

void FormatterFocusManager::exitSelection( ExecutionObject* object ) {
	if (playerManager->setKeyHandler( object, false )) {
		player::settings::setProperty( "service.currentKeyMaster", "" );
	}
}

void FormatterFocusManager::registerNavigationKeys() {
	player::keys::onReservationChanged().connect( boost::bind(&FormatterFocusManager::onReserveKeys,this,_1) );
	onReserveKeys( player::keys::reservedKeys() );
}

void FormatterFocusManager::onReserveKeys( util::key::Keys keys ) {
	canvas::input::Callback fnc = boost::bind( &FormatterFocusManager::userEventReceived, this, _1, _2 );
	_sys->addInputListener( this, fnc, keys);
}

void FormatterFocusManager::unregister() {
	_sys->delInputListener( this );
}

void FormatterFocusManager::changeSettingState( const std::string &name, const std::string &act ) {
	set<ExecutionObject*>* settingObjects;
	set<ExecutionObject*>::iterator i;
	vector<FormatterEvent*>* events;
	vector<FormatterEvent*>::iterator j;
	PropertyAnchor* property;
	string pName, keyM;

	settingObjects = converter->getSettingNodeObjects();

	LDEBUG("FormatterFocusManager", "changeSettingState: number of settings objects: '%i'", settingObjects->size());

	i = settingObjects->begin();
	while (i != settingObjects->end()) {
		events = (*i)->getEvents();
		if (events != NULL) {
			j = events->begin();
			while (j != events->end()) {
				if ((*j)->instanceOf( "AttributionEvent" )) {
					property = ((AttributionEvent*) (*j))->getAnchor();
					pName = property->getPropertyName();
					if (pName == name) {
						if (act == "start") {
							(*j)->start();

						} else if (act == "stop") {
							if (pName == "service.currentFocus") {
								((AttributionEvent*) (*j))->setValue( currentFocus );

							} else if (pName == "service.currentKeyMaster") {

								if (selectedObject != NULL) {
									keyM = (selectedObject->getDataObject()->getDataEntity()->getId());

									((AttributionEvent*) (*j))->setValue( keyM );
								}
							}

							(*j)->stop();
						}
					}
				}
				++j;
			}
			delete events;
		}
		++i;
	}

	delete settingObjects;

}

bool FormatterFocusManager::userEventReceived( ::util::key::type key, bool isUp ) {
	ExecutionObject* currentObject;
	CascadingDescriptor* currentDescriptor;
	FormatterRegion* fr;
	string nextIndex;
	map<string, set<ExecutionObject*>*>::iterator i;

	if (!isHandler) {
		return true;
	}

	if (isUp) {
		return false;
	}

	i = focusTable.find( currentFocus );
	if (i == focusTable.end()) {
		if (currentFocus != "") {
			LWARN("FormatterFocusManager", "userEventReceived: currentFocus not found which is '%s'", currentFocus.c_str());
		}

		if (!focusSequence.empty()) {
			nextIndex = focusSequence[0];
			setFocus( nextIndex );

		} else {
		}

		return true;
	}

	currentObject = getObjectFromFocusIndex( currentFocus );
	if (currentObject == NULL) {
		LWARN("FormatterFocusManager", "userEventReceived: object == NULL");
		return true;
	}

	currentDescriptor = currentObject->getDescriptor();
	if (currentDescriptor == NULL) {
		LWARN("FormatterFocusManager", "userEventReceived: descriptor == NULL");
		return true;
	}

	nextIndex = "";
	bool result = true;
	if (selectedObject != NULL) {
		if (key == ::util::key::backspace || key == ::util::key::back) {
			result = keyCodeBack();
		}
	} else if (key == ::util::key::cursor_up) {
		nextIndex = currentDescriptor->getMoveUp();
	} else if (key == ::util::key::cursor_down) {
		nextIndex = currentDescriptor->getMoveDown();
	} else if (key == ::util::key::cursor_left) {
		nextIndex = currentDescriptor->getMoveLeft();
	} else if (key == ::util::key::cursor_right) {
		nextIndex = currentDescriptor->getMoveRight();
	} else if (key == ::util::key::enter || key == ::util::key::ok) {
		fr = currentDescriptor->getFormatterRegion();
		if (fr != NULL && fr->isVisible() && fr->setSelection( true )) {
			if (!keyCodeOk( currentObject )) {
				_sys->dispatchKey( ::util::key::back, false );
			}
		} else {
			LWARN("FormatterFocusManager", "userEventReceived: cant select '%s'", currentFocus.c_str());
		}

		result = false;
	}

	if (nextIndex != "") {
		changeSettingState( "service.currentFocus", "start" );
		setFocus( nextIndex );
		changeSettingState( "service.currentFocus", "stop" );
	}

	if (!impl::_onKey.empty()) {
		impl::_onKey( key );
	}

	return result;
}

void FormatterFocusManager::onKeyPress( const FormatterFocusManager::KeyPressCallback &callback ) {
	impl::_onKey = callback;
}
}
}
}
}
}
}
