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

#include "../../include/descriptor/Descriptor.h"
#include "../../include/Parameter.h"
#include "../../include/layout/LayoutRegion.h"
#include "../../include/navigation/FocusDecoration.h"
#include "../../include/navigation/KeyNavigation.h"
#include "../../include/transition/Transition.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace descriptor {
Descriptor::Descriptor( const std::string &id ) :
		GenericDescriptor( id ) {

	explicitDuration = -1.0;
	presentationTool = "";
	repetitions = 0;
	freeze = false;
	region = NULL;
	parameters = new std::map<std::string, Parameter*>;

	keyNavigation = NULL;
	focusDecoration = NULL;
	inputTransitions = new std::vector<bptnt::Transition*>;
	outputTransitions = new std::vector<bptnt::Transition*>;

	typeSet.insert( "Descriptor" );
}

Descriptor::~Descriptor() {
// 		if (region != NULL) {
// 			delete region;
// 			region = NULL;
// 		}

	if (parameters != NULL) {
		for (std::map<std::string, bptn::Parameter*>::const_iterator it = parameters->begin(); it != parameters->end(); it++) {
			Parameter *pm = it->second;
			if (pm != NULL) {
				delete pm;
				pm = NULL;
			}
		}
		delete parameters;
		parameters = NULL;
	}

	if (keyNavigation != NULL) {
		delete keyNavigation;
		keyNavigation = NULL;
	}

	if (focusDecoration != NULL) {
		delete focusDecoration;
		focusDecoration = NULL;
	}

	if (inputTransitions != NULL) {
		delete inputTransitions;
		inputTransitions = NULL;
	}

	if (outputTransitions != NULL) {
		delete outputTransitions;
		outputTransitions = NULL;
	}
}

double Descriptor::getExplicitDuration() {
	return explicitDuration;
}

std::string Descriptor::getPlayerName() {
	return presentationTool;
}

bptnl::LayoutRegion* Descriptor::getRegion() {
	return region;
}

long Descriptor::getRepetitions() {
	return repetitions;
}

bool Descriptor::isFreeze() {
	return freeze;
}

void Descriptor::setFreeze( bool freeze ) {
	this->freeze = freeze;
}

void Descriptor::setExplicitDuration( double dur ) {
	explicitDuration = dur;
}

void Descriptor::setPlayerName( const std::string &name ) {
	presentationTool = name;
}

void Descriptor::setRegion( bptnl::LayoutRegion* someRegion ) {
	region = someRegion;
}

void Descriptor::setRepetitions( long r ) {
	repetitions = r;
}

void Descriptor::addParameter( bptn::Parameter *parameter ) {
	(*parameters)[parameter->getName()] = parameter;
}

std::vector<bptn::Parameter*> *Descriptor::getParameters() {
	std::vector<bptn::Parameter*> *ret = new std::vector<bptn::Parameter*>;
	std::map<std::string, bptn::Parameter*>::iterator it;
	for (it = parameters->begin(); it != parameters->end(); ++it) {
		ret->push_back( it->second );
	}
	return ret;
}

bptn::Parameter *Descriptor::getParameter( const std::string &paramName ) {
	if (parameters->count( paramName ) == 0)
		return NULL;
	else
		return (*parameters)[paramName];
}

void Descriptor::removeParameter( bptn::Parameter *parameter ) {
	std::map<std::string, bptn::Parameter*>::iterator it;
	it = parameters->find( parameter->getName() );
	if (it != parameters->end()) {
		parameters->erase( it );
	}
}

bptnn::KeyNavigation* Descriptor::getKeyNavigation() {
	return keyNavigation;
}

void Descriptor::setKeyNavigation( bptnn::KeyNavigation* keyNav ) {
	keyNavigation = keyNav;
}

bptnn::FocusDecoration* Descriptor::getFocusDecoration() {
	return focusDecoration;
}

void Descriptor::setFocusDecoration( bptnn::FocusDecoration* focusDec ) {
	focusDecoration = focusDec;
}

std::vector<bptnt::Transition*>* Descriptor::getInputTransitions() {
	return inputTransitions;
}

bool Descriptor::addInputTransition( bptnt::Transition* transition, int somePos ) {
	unsigned int pos;
	pos = (unsigned int) somePos;
	if ( pos > inputTransitions->size() || transition == NULL) {
		return false;
	}

	if (pos == inputTransitions->size()) {
		inputTransitions->push_back( transition );
		return true;
	}

	std::vector<bptnt::Transition*>::iterator i;
	i = inputTransitions->begin() + pos;

	inputTransitions->insert( i, transition );
	return true;
}

void Descriptor::removeInputTransition( bptnt::Transition* transition ) {
	std::vector<bptnt::Transition*>::iterator i;
	i = inputTransitions->begin();
	while (i != inputTransitions->end()) {
		if (*i == transition) {
			inputTransitions->erase( i );
			break;
		}
		++i;
	}
}

void Descriptor::removeAllInputTransitions() {
	inputTransitions->clear();
}

std::vector<bptnt::Transition*>* Descriptor::getOutputTransitions() {
	return outputTransitions;
}

bool Descriptor::addOutputTransition( bptnt::Transition* transition, int somePos ) {
	unsigned int pos;
	pos = (unsigned int) somePos;
	if ( pos > outputTransitions->size() || transition == NULL) {
		return false;
	}

	if (pos == outputTransitions->size()) {
		outputTransitions->push_back( transition );
		return true;
	}

	std::vector<bptnt::Transition*>::iterator i;
	i = outputTransitions->begin() + pos;

	outputTransitions->insert( i, transition );
	return true;
}

void Descriptor::removeOutputTransition( bptnt::Transition* transition ) {
	std::vector<bptnt::Transition*>::iterator i;
	i = outputTransitions->begin();
	while (i != outputTransitions->end()) {
		if (*i == transition) {
			outputTransitions->erase( i );
			break;
		}
		++i;
	}
}

void Descriptor::removeAllOutputTransitions() {
	outputTransitions->clear();
}
}
}
}
}
}
