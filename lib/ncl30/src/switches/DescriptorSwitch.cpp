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

#include "../../include/switches/DescriptorSwitch.h"
#include "../../include/switches/Rule.h"
#include <util/log.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace switches {
DescriptorSwitch::DescriptorSwitch( const std::string &id ) :
		GenericDescriptor( id ) {

	defaultDescriptor = NULL;
	selectedDescriptor = NULL;
	typeSet.insert( "DescriptorSwitch" );
}

bool DescriptorSwitch::addDescriptor( unsigned int index, GenericDescriptor* descriptor, Rule* rule ) {

	if (index > descriptorList.size() || getDescriptor( descriptor->getId() ) != NULL) {

		return false;
	}

	std::vector<Rule*>::iterator it;
	for (it = ruleList.begin(); it != ruleList.end(); it++) {
		if (*it == rule)
			return false;
	}

	if (index == descriptorList.size()) {
		descriptorList.push_back( descriptor );
		ruleList.push_back( rule );
	} else {
		descriptorList.insert( descriptorList.begin() + index, descriptor );
		ruleList.insert( ruleList.begin() + index, rule );
	}
	return true;
}

bool DescriptorSwitch::addDescriptor( GenericDescriptor* descriptor, Rule* rule ) {

	return addDescriptor( descriptorList.size(), descriptor, rule );
}

void DescriptorSwitch::clear() {
	descriptorList.clear();
	ruleList.clear();
	defaultDescriptor = NULL;
}

bool DescriptorSwitch::containsRule( Rule* rule ) {
	std::vector<Rule*>::iterator iterRule;
	for (iterRule = ruleList.begin(); iterRule != ruleList.end(); ++iterRule) {

		if ((*iterRule)->getId() == rule->getId()) {
			return true;
		}
	}
	return false;
}

void DescriptorSwitch::exchangeDescriptorsAndRules( unsigned int index1, unsigned int index2 ) {

	if (index1 >= descriptorList.size() || index2 >= descriptorList.size()) {
		return;
	}

	GenericDescriptor* auxDesc;
	auxDesc = static_cast<GenericDescriptor*>( descriptorList[index1] );
	Rule* auxRule = static_cast<Rule*>( ruleList[index1] );
	descriptorList[index1] = descriptorList[index2];
	descriptorList[index2] = auxDesc;
	ruleList[index1] = ruleList[index2];
	ruleList[index2] = auxRule;
}

int DescriptorSwitch::indexOfRule( Rule *rule ) {
	int i = 0;
	std::vector<Rule*>::iterator it;
	for (it = ruleList.begin(); it != ruleList.end(); it++) {
		if (*it == rule)
			return i;
		i++;
	}
	return ruleList.size() + 1;
}

bptnd::GenericDescriptor* DescriptorSwitch::getDefaultDescriptor() {
	return defaultDescriptor;
}

unsigned int DescriptorSwitch::indexOfDescriptor( GenericDescriptor* descriptor ) {

	unsigned int i = 0;
	std::vector<GenericDescriptor*>::iterator iterDescr;

	for (iterDescr = descriptorList.begin(); iterDescr != descriptorList.end(); ++iterDescr) {

		if ((*(*iterDescr)).getId() == descriptor->getId())
			return i;
		i++;
	}
	return (descriptorList.size() + 1);
}

bptnd::GenericDescriptor* DescriptorSwitch::getDescriptor( unsigned int index ) {
	if (index >= descriptorList.size())
		return NULL;

	return static_cast<GenericDescriptor*>( descriptorList[index] );
}

bptnd::GenericDescriptor* DescriptorSwitch::getDescriptor( const std::string &descriptorId ) {
	int i, size;
	GenericDescriptor *descriptor;

	if (defaultDescriptor != NULL) {
		if (defaultDescriptor->getId() == descriptorId) {
			return defaultDescriptor;
		}
	}

	size = descriptorList.size();
	for (i = 0; i < size; i++) {
		descriptor = descriptorList[i];
		if (descriptor->getId() == descriptorId) {
			return descriptor;
		}
	}

	return NULL;
}

bptnd::GenericDescriptor* DescriptorSwitch::getDescriptor( Rule* rule ) {
	unsigned int index;

	index = indexOfRule( rule );
	if (index > ruleList.size())
		return NULL;

	return static_cast<GenericDescriptor*>( descriptorList[index] );
}

Rule* DescriptorSwitch::getRule( unsigned int index ) {
	if (index >= ruleList.size())
		return NULL;

	return static_cast<Rule*>( ruleList[index] );
}

unsigned int DescriptorSwitch::getNumDescriptors() {
	return descriptorList.size();
}

unsigned int DescriptorSwitch::getNumRules() {
	return ruleList.size();
}

bool DescriptorSwitch::removeDescriptor( unsigned int index ) {
	if (index >= descriptorList.size())
		return false;

	std::vector<GenericDescriptor*>::iterator iterDescr;
	iterDescr = descriptorList.begin();
	iterDescr = iterDescr + index;
	descriptorList.erase( iterDescr );
	return true;
}

bool DescriptorSwitch::removeDescriptor( GenericDescriptor* descriptor ) {
	return removeDescriptor( indexOfDescriptor( descriptor ) );
}

bool DescriptorSwitch::removeRule( Rule *rule ) {
	int index;

	index = indexOfRule( rule );
	if (index < 0 || index > (int) ruleList.size())
		return false;

	descriptorList.erase( descriptorList.begin() + index );
	ruleList.erase( ruleList.begin() + index );
	return true;
}

void DescriptorSwitch::setDefaultDescriptor( GenericDescriptor* descriptor ) {
	defaultDescriptor = descriptor;
}

void DescriptorSwitch::select( GenericDescriptor* descriptor ) {
	std::vector<GenericDescriptor*>::iterator i;

	i = descriptorList.begin();
	while (i != descriptorList.end()) {
		if (*i == descriptor) {
			selectedDescriptor = descriptor;
			LDEBUG("DescriptorSwitch", "Selected descriptor: selectedDescriptor %s", getId().c_str())
			break;
		}
		++i;
	}
}

void DescriptorSwitch::selectDefault() {
	if (defaultDescriptor != NULL) {
		selectedDescriptor = defaultDescriptor;
		LDEBUG("DescriptorSwitch", "Selected descriptor: selectedDescriptor %s", getId().c_str())
	}
}

bptnd::GenericDescriptor* DescriptorSwitch::getSelectedDescriptor() {
	return selectedDescriptor;
}
}
}
}
}
}
