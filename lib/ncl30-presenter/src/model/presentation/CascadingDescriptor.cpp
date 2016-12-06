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

#include "../../../include/model/CascadingDescriptor.h"
#include <ncl30/util/functions.h>
#include <boost/lexical_cast.hpp>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace presentation {
CascadingDescriptor::CascadingDescriptor( GenericDescriptor* firstDescriptor ) {

	initializeCascadingDescriptor();

	if (firstDescriptor != NULL) {
		cascade( firstDescriptor );
	}
}

CascadingDescriptor::CascadingDescriptor( CascadingDescriptor* descriptor ) {

	initializeCascadingDescriptor();

	int i, size;

	if (descriptor != NULL) {
		size = ((CascadingDescriptor*) descriptor)->descriptors->size();
		for (i = 0; i < size; i++) {
			cascade( (GenericDescriptor*) ((*((CascadingDescriptor*) descriptor)->descriptors)[i]) );
		}
		size = ((CascadingDescriptor*) descriptor)->unsolvedDescriptors->size();
		for (i = 0; i < size; i++) {
			cascade( (GenericDescriptor*) ((*((CascadingDescriptor*) descriptor)->unsolvedDescriptors)[i]) );
		}
	}
}

CascadingDescriptor::~CascadingDescriptor() {

	if (formatterRegion != NULL) {
		delete formatterRegion;
		formatterRegion = NULL;
	}

	if (inputTransitions != NULL) {
		delete inputTransitions;
		inputTransitions = NULL;
	}

	if (outputTransitions != NULL) {
		delete outputTransitions;
		outputTransitions = NULL;
	}

	if (parameters != NULL) {
		delete parameters;
		parameters = NULL;
	}

	if (descriptors != NULL) {
		delete descriptors;
		descriptors = NULL;
	}

	if (unsolvedDescriptors != NULL) {
		delete unsolvedDescriptors;
		unsolvedDescriptors = NULL;
	}
}

void CascadingDescriptor::initializeCascadingDescriptor() {
	typeSet.insert( "CascadingDescriptor" );

	id = "";
	explicitDuration = ncl_util::NaN();
	playerName = "";
	repetitions = 0;
	freeze = false;
	region = NULL;
	formatterRegion = NULL;

	focusIndex = "";
	moveUp = "";
	moveDown = "";
	moveLeft = "";
	moveRight = "";
	_focusDecoration = NULL;

	inputTransitions = new vector<Transition*> ;
	outputTransitions = new vector<Transition*> ;
	parameters = new map<string, string> ;
	descriptors = new vector<GenericDescriptor*> ;
	unsolvedDescriptors = new vector<GenericDescriptor*> ;
}

bool CascadingDescriptor::instanceOf( string s ) {
	if (typeSet.empty())
		return false;
	else
		return (typeSet.find( s ) != typeSet.end());
}

string CascadingDescriptor::getId() {
	return id;
}

void CascadingDescriptor::cascadeDescriptor( Descriptor* descriptor ) {
	if (descriptor->getPlayerName() != "")
		playerName = descriptor->getPlayerName();

	region = descriptor->getRegion();
	if (!ncl_util::isNaN( descriptor->getExplicitDuration() )) {
		explicitDuration = descriptor->getExplicitDuration();
	}

	this->freeze = descriptor->isFreeze();

	if (descriptor->getKeyNavigation() != NULL) {
		KeyNavigation* keyNavigation;
		keyNavigation = (KeyNavigation*) (descriptor->getKeyNavigation());

		if (keyNavigation->getFocusIndex() != "") {
			focusIndex = keyNavigation->getFocusIndex();
		}

		if (keyNavigation->getMoveUp() != "") {
			moveUp = keyNavigation->getMoveUp();
		}

		if (keyNavigation->getMoveDown() != "") {
			moveDown = keyNavigation->getMoveDown();
		}

		if (keyNavigation->getMoveLeft() != "") {
			moveLeft = keyNavigation->getMoveLeft();
		}

		if (keyNavigation->getMoveRight() != "") {
			moveRight = keyNavigation->getMoveRight();
		}
	}

	_focusDecoration = descriptor->getFocusDecoration();

	vector<Transition*>* transitions = NULL;
	transitions = descriptor->getInputTransitions();
	if (transitions != NULL) {
		if (this->inputTransitions == NULL) {
			this->inputTransitions = new vector<Transition*> ;
		}

		inputTransitions->insert( inputTransitions->end(), transitions->begin(), transitions->end() );
	}

	transitions = descriptor->getOutputTransitions();
	if (transitions != NULL) {
		if (this->outputTransitions == NULL) {
			this->outputTransitions = new vector<Transition*> ;
		}

		outputTransitions->insert( outputTransitions->end(), transitions->begin(), transitions->end() );
	}

	Parameter* param;
	vector<Parameter*>* paramsMap;
	vector<Parameter*>::iterator it;

	paramsMap = descriptor->getParameters();
	if (paramsMap != NULL) {
		for (it = paramsMap->begin(); it != paramsMap->end(); ++it) {
			param = *it;
			(*parameters)[param->getName()] = param->getValue();
		}
		delete paramsMap;
		paramsMap = NULL;
	}
}

bool CascadingDescriptor::isLastDescriptor( GenericDescriptor* descriptor ) {

	if ((descriptors->size() > 0) && (descriptor->getId() == ((GenericDescriptor*) (*descriptors)[descriptors->size() - 1])->getId()))

		return true;
	else
		return false;
}

void CascadingDescriptor::cascade( GenericDescriptor *descriptor ) {
	GenericDescriptor* preferredDescriptor;

	preferredDescriptor = (GenericDescriptor*) (descriptor->getDataEntity());

	if ((preferredDescriptor == NULL) || preferredDescriptor->instanceOf( "CascadingDescriptor" ))

		return;

	if (isLastDescriptor( preferredDescriptor ))
		return;

	descriptors->push_back( preferredDescriptor );
	if (id == "")
		id = preferredDescriptor->getId();
	else
		id = id + "+" + preferredDescriptor->getId();

	if (preferredDescriptor->instanceOf( "Descriptor" ) && unsolvedDescriptors->empty()) {

		cascadeDescriptor( (Descriptor*) preferredDescriptor );

	} else {
		unsolvedDescriptors->push_back( preferredDescriptor );
	}
}

GenericDescriptor* CascadingDescriptor::getFirstUnsolvedDescriptor() {

	if (unsolvedDescriptors->size() > 0)
		return (GenericDescriptor*) (*(unsolvedDescriptors->begin()));
	else
		return NULL;
}

vector<GenericDescriptor*>* CascadingDescriptor::getUnsolvedDescriptors() {
	return unsolvedDescriptors;
}

void CascadingDescriptor::cascadeUnsolvedDescriptor() {
	if (unsolvedDescriptors->empty())
		return;

	GenericDescriptor* genericDescriptor, *descriptor;
	DescriptorSwitch* descAlternatives;
	GenericDescriptor* auxDescriptor;

	genericDescriptor = (GenericDescriptor*) ((*unsolvedDescriptors)[0]);

	if (genericDescriptor->instanceOf( "DescriptorSwitch" )) {
		descAlternatives = (DescriptorSwitch*) genericDescriptor;
		auxDescriptor = descAlternatives->getSelectedDescriptor();
		descriptor = (GenericDescriptor*) auxDescriptor->getDataEntity();

	} else {
		descriptor = (Descriptor*) genericDescriptor;
	}

	unsolvedDescriptors->erase( unsolvedDescriptors->begin() );

	if (isLastDescriptor( descriptor )) {
		return;
	}

	cascadeDescriptor( (Descriptor*) descriptor );
}

double CascadingDescriptor::getExplicitDuration() {
	return (this->explicitDuration);
}

bool CascadingDescriptor::getFreeze() {
	return freeze;
}

string CascadingDescriptor::getPlayerName() {
	return playerName;
}

LayoutRegion* CascadingDescriptor::getRegion() {
	return region;
}

FormatterRegion* CascadingDescriptor::getFormatterRegion() {
	return formatterRegion;
}

void CascadingDescriptor::setFormatterLayout() {
	if (region != NULL) {
		if (this->formatterRegion != NULL) {
			delete formatterRegion;
			formatterRegion = NULL;
		}
		formatterRegion = new FormatterRegion( id, this );
	}
}

long CascadingDescriptor::getRepetitions() {
	return repetitions;
}

vector<Parameter*>* CascadingDescriptor::getParameters() {
	vector<Parameter*>* params;
	params = new vector<Parameter*> ;
	Parameter* parameter;

	map<string, string>::iterator it;

	for (it = parameters->begin(); it != parameters->end(); ++it) {
		parameter = new Parameter( it->first, it->second );
		params->push_back( parameter );
	}
	return params;
}

string CascadingDescriptor::getParameterValue( string paramName ) {
	string::size_type pos;
	string paramValue;

	if (parameters == NULL || parameters->count( paramName ) == 0) {
		return "";
	}

	paramValue = (*parameters)[paramName];

	if (paramValue == "") {
		return paramValue;
	}

	pos = paramValue.find_last_of( "%" );
	if (pos != std::string::npos && pos == paramValue.length() - 1) {
		paramValue = paramValue.substr( 0, paramValue.length() - 1 );
		try {
			double value = boost::lexical_cast<double>( paramValue ) / 100;
			paramValue = boost::lexical_cast<string>( value );
		} catch (boost::bad_lexical_cast &) {
		}

	}

	return paramValue;
}

vector<GenericDescriptor*>* CascadingDescriptor::getNcmDescriptors() {
	return this->descriptors;
}

void CascadingDescriptor::setFocusIndex(const std::string &value){
	focusIndex = value;
}

string CascadingDescriptor::getFocusIndex() {
	return focusIndex;
}

string CascadingDescriptor::getMoveDown() {
	return moveDown;
}

string CascadingDescriptor::getMoveLeft() {
	return moveLeft;
}

string CascadingDescriptor::getMoveRight() {
	return moveRight;
}

string CascadingDescriptor::getMoveUp() {
	return moveUp;
}

vector<Transition*>* CascadingDescriptor::getInputTransitions() {
	return this->inputTransitions;
}

vector<Transition*>* CascadingDescriptor::getOutputTransitions() {
	return this->outputTransitions;
}

FocusDecoration* CascadingDescriptor::focusDecoration() {
	return _focusDecoration;
}

}
}
}
}
}
}
}
