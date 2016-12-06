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

#include "../../include/connectors/SimpleAction.h"
#include "../../include/connectors/EventUtil.h"
#include "../../include/connectors/CompoundAction.h"
#include "../../include/animation/Animation.h"

#include <boost/algorithm/string.hpp>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
SimpleAction::SimpleAction( const std::string &role ) :
		Action(),
		Role() {

	SimpleAction::setLabel( role );
	qualifier = CompoundAction::OP_PAR;
	repeat = "0";
	repeatDelay = "0";
	value = "";
	animation = NULL;
	typeSet.insert( "SimpleAction" );
	typeSet.insert( "Role" );
}

SimpleAction::~SimpleAction() {
	delete animation;
	animation = NULL;
}

void SimpleAction::setLabel( const std::string &id ) {
	label = id;
	std::string lowerLabel = boost::to_lower_copy( label );

	if (lowerLabel == "start") {
		actionType = ACT_START;
		eventType = EventUtil::EVT_PRESENTATION;

	} else if (lowerLabel == "stop") {
		actionType = ACT_STOP;
		eventType = EventUtil::EVT_PRESENTATION;

	} else if (lowerLabel == "set") {
		actionType = ACT_SET;
		eventType = EventUtil::EVT_ATTRIBUTION;

	} else if (lowerLabel == "abort") {
		actionType = ACT_ABORT;
		eventType = EventUtil::EVT_PRESENTATION;

	} else if (lowerLabel == "pause") {
		actionType = ACT_PAUSE;
		eventType = EventUtil::EVT_PRESENTATION;

	} else if (lowerLabel == "resume") {
		actionType = ACT_RESUME;
		eventType = EventUtil::EVT_PRESENTATION;
	}
}

short SimpleAction::getQualifier() {
	return qualifier;
}

void SimpleAction::setQualifier( short qualifier ) {
	this->qualifier = qualifier;
}

const std::string &SimpleAction::getRepeat() {
	return repeat;
}

const std::string &SimpleAction::getRepeatDelay() {
	return repeatDelay;
}

void SimpleAction::setRepeatDelay( const std::string &time ) {
	repeatDelay = time;
}

void SimpleAction::setRepeat( const std::string &newRepetitions ) {
	repeat = newRepetitions;
}

short SimpleAction::getActionType() {
	return actionType;
}

void SimpleAction::setActionType( short action ) {
	actionType = action;
}

const std::string &SimpleAction::getValue() {
	return value;
}

void SimpleAction::setValue( const std::string &value ) {
	this->value = value;
}

animation::Animation* SimpleAction::getAnimation() {
	return animation;
}

void SimpleAction::setAnimation( animation::Animation* animation ) {
	this->animation = animation;
}
}
}
}
}
}
