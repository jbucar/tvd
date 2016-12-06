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

#include "../../include/adapters/RuleAdapter.h"
#include <gingaplayer/player/settings.h>
#include <util/log.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adaptation {
namespace context {
RuleAdapter::RuleAdapter() {
	ruleListenMap = new map<string, vector<Rule*>*>;
	entityListenMap = new map<Rule*, vector<ExecutionObjectSwitch*>*>;
	descListenMap = new map<Rule*, vector<DescriptorSwitch*>*>;
	// TODO register callback to update method
}

RuleAdapter::~RuleAdapter() {

	if (ruleListenMap != NULL) {
		vector<Rule*>* rules;
		map<string, vector<Rule*>*>::iterator i;

		i = ruleListenMap->begin();
		while (i != ruleListenMap->end()) {
			rules = i->second;
			if (rules != NULL) {
				delete rules;
				rules = NULL;
			}
			++i;
		}

		ruleListenMap->clear();
		delete ruleListenMap;
		ruleListenMap = NULL;
	}

	if (entityListenMap != NULL) {
		map<Rule*, vector<ExecutionObjectSwitch*>*>::iterator j;
		vector<ExecutionObjectSwitch*>* objects;

		j = entityListenMap->begin();
		while (j != entityListenMap->end()) {
			objects = j->second;
			if (objects != NULL) {
				delete objects;
				objects = NULL;
			}
			++j;
		}

		entityListenMap->clear();
		delete entityListenMap;
		entityListenMap = NULL;
	}

	if (descListenMap != NULL) {
		map<Rule*, vector<DescriptorSwitch*>*>::iterator k;
		vector<DescriptorSwitch*>* descs;

		k = descListenMap->begin();
		while (k != descListenMap->end()) {
			descs = k->second;
			if (descs != NULL) {
				delete descs;
				descs = NULL;
			}
			++k;
		}

		descListenMap->clear();
		delete descListenMap;
		descListenMap = NULL;
	}
}

void RuleAdapter::reset() {
	ruleListenMap->clear();
	entityListenMap->clear();
	descListenMap->clear();
}

void RuleAdapter::adapt( CompositeExecutionObject* compositeObject, bool force ) {

	ExecutionObject* object;
	map<string, ExecutionObject*>* objs;
	map<string, ExecutionObject*>::iterator i;

	objs = compositeObject->getExecutionObjects();
	if (objs != NULL) {
		i = objs->begin();
		while (i != objs->end()) {
			object = i->second;
			if (object->instanceOf( "ExecutionObjectSwitch" )) {
				initializeRuleObjectRelation( (ExecutionObjectSwitch*) object );

				adapt( (ExecutionObjectSwitch*) object, force );
				object = ((ExecutionObjectSwitch*) object)->getSelectedObject();
			}

			adaptDescriptor( object );
			if (object->instanceOf( "CompositeExecutionObject" )) {
				adapt( (CompositeExecutionObject*) object, force );
			}
			++i;
		}
		delete objs;
		objs = NULL;
	}
}

void RuleAdapter::initializeAttributeRuleRelation( Rule* topRule, Rule* rule ) {

	vector<Rule*>* ruleVector = NULL;
	vector<Rule*>::iterator rules;

	if (rule->instanceOf( "SimpleRule" )) {
		map<string, vector<Rule*>*>::iterator i;
		for (i = ruleListenMap->begin(); i != ruleListenMap->end(); ++i) {
			if (((SimpleRule*) rule)->getAttribute() == i->first) {
				ruleVector = i->second;
				break;
			}
		}

		if (ruleVector == NULL) {
			ruleVector = new vector<Rule*>;
			(*ruleListenMap)[(((SimpleRule*) rule)->getAttribute())] = ruleVector;
		}
		ruleVector->push_back( topRule );

	} else {
		ruleVector = ((CompositeRule*) rule)->getRules();
		if (ruleVector != NULL) {
			rules = ruleVector->begin();
			while (rules != ruleVector->end()) {
				initializeAttributeRuleRelation( topRule, (Rule*) (*rules) );
				++rules;
			}
		}
	}
}

void RuleAdapter::initializeRuleObjectRelation( ExecutionObjectSwitch* /*objectAlternatives*/) {

	/*
	 vector<ExecutionObjectSwitch*>* objectVector;
	 ExecutionObject* object;
	 Rule* rule;
	 int i, size;
	 size = objectAlternatives->getNumRules();
	 map<Rule*, vector<ExecutionObjectSwitch*>*>::iterator j;
	 for (i = 0; i < size; i++) {
	 rule = objectAlternatives->getRule(i);
	 initializeAttributeRuleRelation(rule, rule);

	 // the switch will pertain to a set of objects that depend on this rule
	 bool containsKey = false;
	 for (j = entityListenMap->begin(); j != entityListenMap->end(); ++j) {
	 if (j->first == rule) {
	 containsKey = true;
	 objectVector = j->second;
	 break;
	 }
	 }

	 if (!containsKey) {
	 objectVector = new vector<ExecutionObjectSwitch*>;
	 (*entityListenMap)[rule] = objectVector;
	 }

	 vector<ExecutionObjectSwitch*>::iterator j;
	 bool containsObject = false;
	 for (j = objectVector->begin(); j != objectVector->end(); ++j) {
	 if ((*j) == objectAlternatives) {
	 containsObject = true;
	 }
	 }

	 if (!containsObject) {
	 objectVector->push_back(objectAlternatives);
	 }

	 object = objectAlternatives->getExecutionObject(i);
	 if (object->instanceOf("ExecutionObjectSwitch")) {
	 initializeRuleObjectRelation((ExecutionObjectSwitch*)object);
	 }
	 }
	 */
}

void RuleAdapter::adapt( ExecutionObjectSwitch* /*objectAlternatives*/, bool /*force*/) {

	/*
	 int i, size;
	 Rule* rule;
	 ExecutionObject* object;
	 vector<FormatterEvent*>* events;
	 bool selected, result;

	 if (!force && objectAlternatives->getSelectedObject() != NULL) {
	 return;
	 }

	 object = objectAlternatives->getSelectedObject();
	 if (object != NULL) {
	 events = object->getEvents();
	 if (events != NULL) {
	 vector<FormatterEvent*>::iterator i;
	 i = events->begin();
	 while (i != events->end()) {
	 if ((*i)->getCurrentState() == Event::ST_OCCURRING) {
	 return;
	 }
	 ++i;
	 }
	 }
	 }

	 selected = false;
	 size = objectAlternatives->getNumRules();
	 for (i = 0; i < size && !selected; i++) {
	 rule = objectAlternatives->getRule(i);
	 result = evaluateRule(rule);
	 if (result && !selected) {
	 selected = true;
	 objectAlternatives->select(objectAlternatives->getExecutionObject(i));
	 }
	 }

	 if (!selected)
	 objectAlternatives->selectDefault();

	 object = objectAlternatives->getSelectedObject();
	 if (object != NULL) {
	 if (object->instanceOf("ExecutionObjectSwitch")) {
	 adapt((ExecutionObjectSwitch*)object, force);
	 }
	 }
	 */
}

bool RuleAdapter::adaptDescriptor( ExecutionObject* executionObject ) {
	CascadingDescriptor* cascadingDescriptor;
	GenericDescriptor* selectedDescriptor;
	GenericDescriptor* unsolvedDescriptor;
	DescriptorSwitch* descAlternatives;
	int i, size;
	Rule* rule;
	bool selected, result;
	vector<DescriptorSwitch*>* objectVector;
	bool adapted = false;

	cascadingDescriptor = executionObject->getDescriptor();
	if (cascadingDescriptor == NULL) {
		return adapted;
	}

	unsolvedDescriptor = cascadingDescriptor->getFirstUnsolvedDescriptor();
	map<Rule*, vector<DescriptorSwitch*>*>::iterator k;
	while (unsolvedDescriptor != NULL) {
		if (unsolvedDescriptor->instanceOf( "DescriptorSwitch" )) {
			descAlternatives = (DescriptorSwitch*) unsolvedDescriptor;
			selectedDescriptor = descAlternatives->getSelectedDescriptor();
			selected = false;
			size = descAlternatives->getNumRules();
			for (i = 0; i < size; i++) {
				rule = descAlternatives->getRule( i );
				result = evaluateRule( rule );
				if (result && !selected) {
					selected = true;
					descAlternatives->select( descAlternatives->getDescriptor( i ) );
				}

				if (descListenMap->count( rule ) == 0) {
					objectVector = new vector<DescriptorSwitch*>;
					(*descListenMap)[rule] = objectVector;
				} else {
					objectVector = ((*descListenMap)[rule]);
				}

				objectVector->push_back( descAlternatives );
			}
			if (!selected) {
				descAlternatives->selectDefault();
			}

			if (selectedDescriptor != descAlternatives->getSelectedDescriptor()) {

				adapted = true;
			}
		}
		cascadingDescriptor->cascadeUnsolvedDescriptor();
		unsolvedDescriptor = cascadingDescriptor->getFirstUnsolvedDescriptor();
	}
	return adapted;
}

Node* RuleAdapter::adaptSwitch( SwitchNode* switchNode ) {
	int i, size;
	Rule* rule;
	Node* selectedNode;

	selectedNode = NULL;
	size = switchNode->getNumRules();
	for (i = 0; i < size; i++) {
		rule = switchNode->getRule( i );
		if (evaluateRule( rule )) {
			selectedNode = switchNode->getNode( i );
		}
	}

	if (selectedNode == NULL) {
		selectedNode = switchNode->getDefaultNode();
	}

	return selectedNode;
}

bool RuleAdapter::evaluateRule( Rule* rule ) {
	if (rule->instanceOf( "SimpleRule" )) {
		return evaluateSimpleRule( (SimpleRule*) rule );

	} else if (rule->instanceOf( "CompositeRule" )) {
		return evaluateCompositeRule( (CompositeRule*) rule );

	} else {
		return false;
	}
}

bool RuleAdapter::evaluateCompositeRule( CompositeRule* rule ) {

	Rule* childRule;

	vector<Rule*>* rules;
	vector<Rule*>::iterator iterator;

	rules = (rule->getRules()); //sf
	if (rules != NULL) {
		iterator = rules->begin();
		switch ( rule->getOperator() ) {
			case CompositeRule::OP_OR:
				while (iterator != rules->end()) {
					childRule = (*iterator);
					if (evaluateRule( childRule ))
						return true;
					++iterator;
				}
				return false;

			case CompositeRule::OP_AND:

			default:
				while (iterator != rules->end()) {
					childRule = (*iterator);
					if (!evaluateRule( childRule ))
						return false;
					++iterator;
				}
				return true;
		}
	}
	return false;
}

bool RuleAdapter::evaluateSimpleRule( SimpleRule* rule ) {
	string attribute;
	short op;
	string ruleValue;
	string attributeValue;

	attribute = rule->getAttribute();
	attributeValue = player::settings::getProperty( attribute );

	ruleValue = rule->getValue();

	LDEBUG("RuleAdapter", "evaluate rule: property=%s, value='%s', ruleValue='%s'", attribute.c_str(), attributeValue.c_str(), ruleValue.c_str() );

	op = rule->getOperator();
	switch ( op ) {
		case ncl_util::Comparator::CMP_EQ:
			if (attributeValue == "" && ruleValue == "") {
				return true;
			} else if (attributeValue == "") {
				return false;
			} else {
				return ncl_util::Comparator::evaluate( attributeValue, ruleValue, op );
			}

		case ncl_util::Comparator::CMP_NE:
			if (attributeValue == "" && ruleValue == "") {
				return false;
			} else if (attributeValue == "") {
				return true;
			} else {
				return ncl_util::Comparator::evaluate( attributeValue, ruleValue, op );
			}

		default:
			return ncl_util::Comparator::evaluate( attributeValue, ruleValue, op );
	}
}

void RuleAdapter::update( void* /*arg0*/, void* arg1 ) {
	string arg;
	arg = *((string*) (arg1));

	vector<Rule*>* ruleVector = NULL;
	map<string, vector<Rule*>*>::iterator i;
	for (i = ruleListenMap->begin(); i != ruleListenMap->end(); ++i) {
		if (i->first == arg) {
			ruleVector = i->second;
		}
	}

	if (ruleVector == NULL) {
		return;
	}

	vector<Rule*>::iterator ruleIter;
	vector<ExecutionObjectSwitch*>::iterator objIter;

	Rule* rule;
	ExecutionObjectSwitch* object;

	for (ruleIter = ruleVector->begin(); ruleIter != ruleVector->end(); ++ruleIter) {

		rule = (Rule*) (*ruleIter);

		if (entityListenMap->count( rule ) != 0) {
			vector<ExecutionObjectSwitch*>* objectVector;
			objectVector = ((*entityListenMap)[rule]);

			for (objIter = objectVector->begin(); objIter != objectVector->end(); ++objIter) {

				object = (*objIter);
				if (object->instanceOf( "ExecutionObjectSwitch" )) {
					adapt( object, true );

				} else {
					// TODO: precisa pensar melhor como adaptar
					// descritores dinamicamente.
				}
			}
		}
	}
}

}
}
}
}
}
}
}
