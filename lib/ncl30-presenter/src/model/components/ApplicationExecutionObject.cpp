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

#include "../../../include/model/ApplicationExecutionObject.h"
#include <util/log.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace components {
ApplicationExecutionObject::ApplicationExecutionObject( string id, Node* node, bool handling ) :
	ExecutionObject( id, node, handling ) {

	initializeApplicationObject();
}

ApplicationExecutionObject::ApplicationExecutionObject( string id, Node* node, GenericDescriptor* descriptor, bool handling ) :
	ExecutionObject( id, node, descriptor, handling ) {

	initializeApplicationObject();
}

ApplicationExecutionObject::ApplicationExecutionObject( string id, Node* node, CascadingDescriptor* descriptor, bool handling ) :
	ExecutionObject( id, node, descriptor, handling ) {

	initializeApplicationObject();
}

ApplicationExecutionObject::~ApplicationExecutionObject() {

}

void ApplicationExecutionObject::initializeApplicationObject() {
	typeSet.insert( "ApplicationExecutionObject" );
	currentEvent = NULL;
	preparedEvents = new map<string, FormatterEvent*> ;
}

void ApplicationExecutionObject::setCurrentEvent( FormatterEvent* event ) {
	if (!containsEvent( event )) {
		currentEvent = NULL;
	} else {
		currentEvent = event;
	}
}

bool ApplicationExecutionObject::prepare( FormatterEvent* event, double offsetTime ) {

	CompositeExecutionObject* parentObject;
	int size;
	map<Node*, void*>::iterator i;
	double startTime = 0;
	ContentAnchor* contentAnchor;
	FormatterEvent* auxEvent;
	AttributionEvent* attributeEvent;
	PropertyAnchor* attributeAnchor;
	int j;

	ExecutionObject::prepare( event, offsetTime );

	if (event->getCurrentState() != EventUtil::ST_SLEEPING) {
		return false;
	}

	if (event->instanceOf( "AnchorEvent" )) {
		contentAnchor = ((AnchorEvent*) event)->getAnchor();
		if (contentAnchor != NULL && contentAnchor->instanceOf( "LabeledAnchor" )) {

			i = parentTable->begin();
			while (i != parentTable->end()) {
				parentObject = (CompositeExecutionObject*) (i->second);
				// register parent as a mainEvent listener
				event->addEventListener( parentObject );
				++i;
			}
			(*preparedEvents)[event->getId()] = event;
			return true;
		}
	}

	if (event->instanceOf( "PresentationEvent" )) {
		startTime = ((PresentationEvent*) event)->getBegin() + offsetTime;
		if (startTime > ((PresentationEvent*) event)->getEnd()) {
			return false;
		}
	}

	i = parentTable->begin();
	while (i != parentTable->end()) {
		parentObject = (CompositeExecutionObject*) (i->second);
		// register parent as a currentEvent listener
		event->addEventListener( parentObject );
		++i;
	}

	transMan->prepare( event == wholeContent, startTime, ContentAnchor::CAT_TIME );

	if (otherEvents != NULL) {
		size = otherEvents->size();
		for (j = 0; j < size; j++) {
			auxEvent = (*otherEvents)[j];
			if (auxEvent->instanceOf( "AttributionEvent" )) {
				attributeEvent = (AttributionEvent*) auxEvent;
				attributeAnchor = attributeEvent->getAnchor();
				if (attributeAnchor->getPropertyValue() != "") {
					attributeEvent->setValue( attributeAnchor->getPropertyValue() );
				}
			}
		}
	}

	this->offsetTime = startTime;
	(*preparedEvents)[event->getId()] = event;
	return true;
}

bool ApplicationExecutionObject::canStart() {
	if (currentEvent == NULL || preparedEvents->count( currentEvent->getId() ) == 0) {
		return false;
	}
	/*
	 * TODO: follow the event state machine or start instruction behavior
	 * if (currentEvent->getCurrentState() == EventUtil::ST_PAUSED) {
	 return resume();
	 }*/
	if (currentEvent->getCurrentState() != EventUtil::ST_SLEEPING) {
		return false;
	}

	return true;
}

void ApplicationExecutionObject::start() {
	if (currentEvent->instanceOf( "AnchorEvent" )) {
		ContentAnchor* contentAnchor = ((AnchorEvent*) currentEvent)->getAnchor();
		if (contentAnchor != NULL && contentAnchor->instanceOf( "LabeledAnchor" )) {
			currentEvent->start();
		}
	}
	transMan->start( offsetTime );
}

bool ApplicationExecutionObject::stop() {
	ContentAnchor* contentAnchor;
	double endTime;
	LDEBUG("ApplicationExecutionObject", "stop(%s)", id.c_str());

	if (currentEvent == NULL || currentEvent->getCurrentState() == EventUtil::ST_SLEEPING || preparedEvents->count( currentEvent->getId() ) == 0) {

		return false;
	}

	if (currentEvent->instanceOf( "PresentationEvent" )) {
		endTime = ((PresentationEvent*) currentEvent)->getEnd();
		transMan->stop( endTime );

	} else if (currentEvent->instanceOf( "AnchorEvent" )) {
		contentAnchor = ((AnchorEvent*) currentEvent)->getAnchor();
		if (contentAnchor != NULL && contentAnchor->instanceOf( "LabeledAnchor" )) {

			currentEvent->stop();
		}
	}

	transMan->resetTimeIndex();
	pauseCount = 0;
	return true;
}

bool ApplicationExecutionObject::abort() {
	vector<FormatterEvent*>* evs;
	vector<FormatterEvent*>::iterator i;

	if (currentEvent == NULL || currentEvent->getCurrentState() == EventUtil::ST_SLEEPING || preparedEvents->count( currentEvent->getId() ) == 0) {

		return false;
	}

	evs = getEvents();
	if (evs != NULL) {
		i = evs->begin();
		while (i != evs->end()) {
			if ((*i)->getCurrentState() != EventUtil::ST_SLEEPING) {
				(*i)->abort();
			}
			++i;
		}
		delete evs;
		evs = NULL;
	}

	transMan->resetTimeIndex();
	pauseCount = 0;
	return true;
}

bool ApplicationExecutionObject::pause() {
	FormatterEvent* event;
	vector<FormatterEvent*>* evs;
	vector<FormatterEvent*>::iterator i;

	if (currentEvent == NULL || currentEvent->getCurrentState() != EventUtil::ST_OCCURRING || preparedEvents->count( currentEvent->getId() ) == 0) {

		return false;
	}

	evs = getEvents();
	if (evs != NULL) {
		if (pauseCount == 0) {
			i = evs->begin();
			while (i != evs->end()) {
				event = *i;
				if (event->getCurrentState() == EventUtil::ST_OCCURRING) {
					event->pause();
				}
				++i;
			}
		}
		delete evs;
		evs = NULL;
	}

	pauseCount++;
	return true;
}

bool ApplicationExecutionObject::resume() {
	FormatterEvent* event;
	vector<FormatterEvent*>* evs;
	vector<FormatterEvent*>::iterator i;

	if (pauseCount == 0) {
		return false;

	} else {
		pauseCount--;
		if (pauseCount > 0) {
			return false;
		}
	}

	evs = getEvents();
	if (evs != NULL) {
		if (pauseCount == 0) {
			i = evs->begin();
			while (i != evs->end()) {
				event = *i;
				if (event->getCurrentState() == EventUtil::ST_PAUSED) {
					event->resume();
				}
				++i;
			}
		}
		delete evs;
		evs = NULL;
	}

	return true;
}

bool ApplicationExecutionObject::unprepare() {
	LDEBUG("ApplicationExecutionObject", "unprepare(%s)", id.c_str());

	if (currentEvent == NULL || currentEvent->getCurrentState() != EventUtil::ST_SLEEPING || preparedEvents->count( currentEvent->getId() ) == 0) {

		return false;
	}

	if (currentEvent->instanceOf( "AnchorEvent" ) && ((AnchorEvent*) currentEvent)->getAnchor() != NULL && ((AnchorEvent*) currentEvent)->getAnchor()->instanceOf( "LambdaAnchor" )) {

		FormatterEvent* event;
		vector<FormatterEvent*>* evs;
		vector<FormatterEvent*>::iterator i;

		evs = getEvents();
		if (evs != NULL) {
			i = evs->begin();
			while (i != evs->end()) {
				event = *i;
				if (event->getCurrentState() != EventUtil::ST_SLEEPING) {
					event->stop();
				}
				++i;
			}
			delete evs;
			evs = NULL;
		}
	}

	map<Node*, void*>::iterator i;
	CompositeExecutionObject* parentObject;

	i = parentTable->begin();
	while (i != parentTable->end()) {
		parentObject = (CompositeExecutionObject*) (i->second);
		// unregister parent as a currentEvent listener
		currentEvent->delEventListener( parentObject );
		++i;
	}

	if (preparedEvents->count( currentEvent->getId() ) != 0) {
		preparedEvents->erase( preparedEvents->find( currentEvent->getId() ) );
	}
	currentEvent = NULL;
	return true;
}
}
}
}
}
}
}
}
