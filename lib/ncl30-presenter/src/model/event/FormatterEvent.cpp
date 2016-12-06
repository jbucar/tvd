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

#include "../../../include/model/FormatterEvent.h"
#include "../../../include/model/IEventListener.h"
#include "eventmanager.h"
#include <boost/foreach.hpp>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace event {
FormatterEvent::FormatterEvent( const std::string &id, components::ExecutionObject* eObject, ncl30presenter::event::EventManager *manager ) :
		_id( id ),
		_eObject( eObject ),
		_eventManager( manager ) {

	typeSet.insert( "FormatterEvent" );
	currentState = EventUtil::ST_SLEEPING;
	occurrences = 0;
	deleting = false;
}

FormatterEvent::~FormatterEvent() {
	deleting = true;
	_eObject = NULL;
	_eventManager = NULL;
}

bool FormatterEvent::instanceOf( const std::string &s ) {
	if (typeSet.empty()) {
		return false;
	} else {
		return (typeSet.find( s ) != typeSet.end());
	}
}

/**
 * Agrega un listener de eventos
 * @param listener El objeto listener a agregar
 */
void FormatterEvent::addEventListener( IEventListener* listener ) {
	_eventManager->addListener( listener, this );
}

/**
 * Remueve un listener de eventos
 * @param listener El objeto listener a remover
 */
bool FormatterEvent::delEventListener( IEventListener* listener ) {
	return _eventManager->delListener( listener, this );
}

short FormatterEvent::getNewState( short transition ) {
	switch ( transition ) {
		case EventUtil::TR_STOPS:
			return EventUtil::ST_SLEEPING;

		case EventUtil::TR_STARTS:
		case EventUtil::TR_RESUMES:
			return EventUtil::ST_OCCURRING;

		case EventUtil::TR_PAUSES:
			return EventUtil::ST_PAUSED;

		case EventUtil::TR_ABORTS:
			return ST_ABORTED;

		default:
			return -1;
	}
}

bool FormatterEvent::abort() {
	switch ( currentState ) {
		case EventUtil::ST_OCCURRING:
		case EventUtil::ST_PAUSED:
			return changeState( ST_ABORTED, EventUtil::TR_ABORTS );

		default:
			return false;
	}
}

bool FormatterEvent::start() {
	switch ( currentState ) {
		case EventUtil::ST_SLEEPING:
			return changeState( EventUtil::ST_OCCURRING, EventUtil::TR_STARTS );
		default:

			return false;
	}
}

bool FormatterEvent::stop() {
	switch ( currentState ) {
		case EventUtil::ST_OCCURRING:
		case EventUtil::ST_PAUSED:
			return changeState( EventUtil::ST_SLEEPING, EventUtil::TR_STOPS );
		default:
			return false;
	}
}

bool FormatterEvent::pause() {
	switch ( currentState ) {
		case EventUtil::ST_OCCURRING:
			return changeState( EventUtil::ST_PAUSED, EventUtil::TR_PAUSES );

		default:
			return false;
	}
}

bool FormatterEvent::resume() {
	switch ( currentState ) {
		case EventUtil::ST_PAUSED:
			return changeState( EventUtil::ST_OCCURRING, EventUtil::TR_RESUMES );

		default:
			return false;
	}
}

void FormatterEvent::setCurrentState( short newState ) {
	currentState = newState;
}

bool FormatterEvent::changeState( short newState, short transition ) {

	if (transition == EventUtil::TR_STOPS) {
		occurrences++;
	}

	short previousState = currentState;
	currentState = newState;

	if (deleting) {
		return false;
	}

	_eventManager->onEvent( this, transition, previousState );

	if (currentState == ST_ABORTED) {
		currentState = EventUtil::ST_SLEEPING;
	}

	return true;
}

short FormatterEvent::getCurrentState() {
	return currentState;
}

components::ExecutionObject* FormatterEvent::getExecutionObject() {
	return _eObject;
}

const std::string &FormatterEvent::getId() {
	return _id;
}

long FormatterEvent::getOccurrences() {
	return occurrences;
}

string FormatterEvent::getStateName( short state ) {
	switch ( state ) {
		case EventUtil::ST_OCCURRING:
			return "occurring";

		case EventUtil::ST_PAUSED:
			return "paused";

		case EventUtil::ST_SLEEPING:
			return "sleeping";

		default:
			return "";
	}
}
}
}
}
}
}
}
}
