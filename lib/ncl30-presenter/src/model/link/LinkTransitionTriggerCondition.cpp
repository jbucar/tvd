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
#include <util/log.h>
#include "../../../include/model/LinkTransitionTriggerCondition.h"
/*#include "../../../include/model/ExecutionObject.h"
 using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;*/

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
LinkTransitionTriggerCondition::LinkTransitionTriggerCondition( FormatterEvent* event, short transition, player::System *system ) :
		LinkTriggerCondition( system ) {

	typeSet.insert( "LinkTransitionTriggerCondition" );
	this->event = event;
	this->transition = transition;
	if (event != NULL) {
		this->event->addEventListener( this );

	} else {
		LWARN("LinkTransitionTriggerCondition", "LinkTransitionTriggerCondition: creating a link with NULL event");
	}
}

LinkTransitionTriggerCondition::~LinkTransitionTriggerCondition() {
	listener = NULL;
	if (event != NULL) {
		event->delEventListener( this );
		event = NULL;
	}
}

void LinkTransitionTriggerCondition::stopListening( void* event ) {
	if (this->event == event) {
		this->event = NULL;
	}
}

void LinkTransitionTriggerCondition::eventStateChanged( void* /*someEvent*/, short transition, short /*previousState*/ ) {

	/*FormatterEvent* event = (FormatterEvent*)someEvent;
	ExecutionObject* obj = (ExecutionObject*)(event->getExecutionObject());
	LDEBUG("LinkTransitionTriggerCondition", "eventStateChanged: from object '%s' transition='%h'", obj->getId().c_str(), transition);
	*/

	if (this->transition == transition) {
		notifyConditionObservers( LinkTriggerListener::EVALUATION_STARTED );

		LinkTriggerCondition::conditionSatisfied( this );
	}
}

short LinkTransitionTriggerCondition::getPriorityType() {
	return IEventListener::PT_LINK;
}

FormatterEvent* LinkTransitionTriggerCondition::getEvent() {
	return event;
}

short LinkTransitionTriggerCondition::getTransition() {
	return transition;
}

vector<FormatterEvent*>* LinkTransitionTriggerCondition::getEvents() {
	if (event == NULL
	)
		return NULL;

	vector<FormatterEvent*>* events = new vector<FormatterEvent*>;

	events->push_back( event );
	return events;
}
}
}
}
}
}
}
}
