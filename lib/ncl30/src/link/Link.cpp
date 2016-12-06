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

#include "../../include/link/Link.h"
#include "../../include/link/Bind.h"
#include "../../include/Parameter.h"
#include "../../include/components/ContextNode.h"
#include "../../include/components/Node.h"
#include "../../include/descriptor/GenericDescriptor.h"
#include "../../include/connectors/Connector.h"
#include "../../include/connectors/Role.h"
#include "../../include/interfaces/Port.h"
#include "../../include/interfaces/InterfacePoint.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace link {
Link::Link( const std::string &id, bptncon::Connector* newConnector ) :
		Entity( id ),
		connector( newConnector ),
		roleBinds( new std::map<std::string, std::vector<Bind*>*> ),
		binds( new std::vector<Bind*> ),
		parameters( new std::map<std::string, bptn::Parameter*> ),
		composition( NULL ) {

	typeSet.insert( "Link" );
}

Link::~Link() {
	std::map<std::string, std::vector<Bind*>*>::iterator i;
	std::vector<Bind*>::iterator j;
	std::map<std::string, bptn::Parameter*>::iterator k;
	std::vector<Bind*>* rmBinds;

	if (roleBinds != NULL) {
		i = roleBinds->begin();
		while (i != roleBinds->end()) {
			rmBinds = i->second;
			rmBinds->clear();
			delete rmBinds;
			++i;
		}
		delete roleBinds;
		roleBinds = NULL;
	}

	if (binds != NULL) {
		j = binds->begin();
		while (j != binds->end()) {
			delete *j;
			++j;
		}
		delete binds;
		binds = NULL;
	}

	if (parameters != NULL) {
		delete parameters;
		parameters = NULL;
	}
}

Bind* Link::bind( bptnc::Node* node, bptni::InterfacePoint* interfPt, bptnd::GenericDescriptor* desc, const std::string &roleId ) {

	bptncon::Role* role;

	role = connector->getRole( roleId );
	if ( role == NULL )
		return NULL;

	return bind( node, interfPt, desc, role );
}

/**
 * Crea un bind donde se asocia un nodo a un role.
 * @param node Parámetro usado para identificar el nodo.
 * @param interfPt Se usa para hacer referencia a la interfaz del nodo.
 * @param desc Se usa para hacer referencia a un descriptor a ser asociado a un nodo.
 * @param role Usado para hacer referencia a un papel del conector.
 * @return Una instancia de @c Bind.
 */
Bind* Link::bind( bptnc::Node* node, bptni::InterfacePoint* interfPt, bptnd::GenericDescriptor* desc, bptncon::Role* role ) {

	Bind *bind;
	std::vector<Bind*> *roleBindList;
	std::string label;

	label = role->getLabel();
	if (roleBinds->count( label ) == 0) {
		roleBindList = new std::vector<Bind*>;
		(*roleBinds)[label] = roleBindList;

	} else {
		roleBindList = (*roleBinds)[label];
	}

	bind = new Bind( node, interfPt, desc, role );
	roleBindList->push_back( bind );
	binds->push_back( bind );
	return bind;

}

/**
 * @return True en caso de que el link sea consistente, false de lo contrario.
 * @note Un link es consistente cuando el número de conectores no supera el máximo.
 */
bool Link::isConsistent() {
	std::vector<bptncon::Role*>::iterator iterator;
	bptncon::Role *role;
	int minConn, maxConn;
	std::vector<bptncon::Role*> *roles;

	roles = connector->getRoles();
	iterator = roles->begin();
	while (iterator != roles->end()) {
		role = *iterator;
		//if (role instanceof ICardinalityRole) {
		minConn = role->getMinCon();
		maxConn = role->getMaxCon();
		//}
		//else {
		//	minConn = 1;
		//	maxConn = 1;
		//}
		if ((int) getNumRoleBinds( role ) < minConn || (maxConn != bptncon::Role::UNBOUNDED && (int) getNumRoleBinds( role ) > maxConn)) {

			delete roles;
			return false;
		}
	}
	delete roles;
	return true;
}

Bind* Link::getBind( bptnc::Node* node, bptni::InterfacePoint* interfPt, bptnd::GenericDescriptor* desc, bptncon::Role* role ) {

	std::map<std::string, std::vector<Bind*>*>::iterator i;

	bool containsKey = false;
	for (i = roleBinds->begin(); i != roleBinds->end(); ++i)
		if (i->first == role->getLabel())
			containsKey = true;

	if (!containsKey)
		return NULL;

	Bind* bind;
	std::vector<Bind*>::iterator bindIterator;

	std::vector<Bind*>* roleBindList;
	roleBindList = (*roleBinds)[role->getLabel()];

	for (bindIterator = roleBindList->begin(); bindIterator != roleBindList->end(); ++roleBindList) {

		bind = (*bindIterator);
		if ((bind->getNode() == node) && (bind->getInterfacePoint() == interfPt) && (bind->getDescriptor() == desc))
			return bind;
	}

	return NULL;
}

std::vector<Bind*>* Link::getBinds() {
	if (binds->empty())
		return NULL;

	return binds;
}

bptncon::Connector* Link::getConnector() {
	return connector;
}

void Link::setParentComposition( LinkComposition* composition ) {
	if (composition == NULL || ((bptnc::ContextNode*) composition)->containsLink( this )) {

		this->composition = composition;
	}
}

LinkComposition* Link::getParentComposition() {
	return composition;
}

unsigned int Link::getNumBinds() {
	return binds->size();
}

unsigned int Link::getNumRoleBinds( bptncon::Role* role ) {
	std::map<std::string, std::vector<Bind*>*>::iterator i;

	bool containsKey = false;
	for (i = roleBinds->begin(); i != roleBinds->end(); ++i)
		if (i->first == role->getLabel())
			containsKey = true;

	if (!containsKey)
		return 0;

	std::vector<Bind*>* roleBindList;
	roleBindList = (*roleBinds)[role->getLabel()];
	return roleBindList->size();
}

std::vector<Bind*>* Link::getRoleBinds( bptncon::Role* role ) {
	if (roleBinds->empty()) {
		return NULL;
	}

	std::vector<Bind*>* roleBindList;
	std::map<std::string, std::vector<Bind*>*>::iterator i;
	for (i = roleBinds->begin(); i != roleBinds->end(); ++i) {
		if (i->first == role->getLabel()) {
			roleBindList = i->second;
			return roleBindList;
		}
	}

	return NULL;
}

bool Link::isMultiPoint() {
	if (binds->size() > 2)
		return true;
	else
		return false;
}

std::string Link::toString() {
	std::string linkStr;
	std::vector<Bind*>::iterator iterator;
	Bind* bind;

	linkStr = (getId()) + "\n" + ((this->getConnector())->getId()) + "\n";

	iterator = binds->begin();
	while (iterator++ != binds->end()) {
		bind = (*iterator);
		linkStr += ((bind->getRole())->getLabel()) + " " + ((bind->getNode())->getId()) + " " + ((bind->getInterfacePoint())->getId()) + "\n" + ((bind->getRole())->getLabel());
		++iterator;
	}
	return linkStr;
}

void Link::setConnector( bptncon::Connector* connector ) {
	this->connector = connector;

	roleBinds->clear();
	binds->clear();
}

bool Link::unBind( Bind *bind ) {
	std::vector<Bind*> *roleBindList;
	bool containsBind = false;

	std::vector<Bind*>::iterator it;
	for (it = binds->begin(); it != binds->end(); ++it) {
		if (bind == *it) {
			containsBind = true;
			binds->erase( it );
			break;
		}
	}

	if (!containsBind) {
		return false;
	}

	if (roleBinds->count( bind->getRole()->getLabel() ) == 1) {
		roleBindList = (*roleBinds)[bind->getRole()->getLabel()];
		std::vector<Bind*>::iterator i;
		for (i = roleBindList->begin(); i != roleBindList->end(); ++i) {
			if (*i == bind) {
				roleBindList->erase( i );
				break;
			}
		}
	}
	return true;
}

void Link::addParameter( Parameter* parameter ) {
	if (parameter == NULL
	)
		return;

	(*parameters)[parameter->getName()] = parameter;
}

std::vector<Parameter*>* Link::getParameters() {
	if (parameters->empty())
		return NULL;

	std::vector<Parameter*>* params;
	params = new std::vector<Parameter*>;

	std::map<std::string, Parameter*>::iterator i;
	for (i = parameters->begin(); i != parameters->end(); ++i)
		params->push_back( i->second );

	return params;
}

Parameter* Link::getParameter( const std::string &name ) {
	if (parameters->empty())
		return NULL;

	std::map<std::string, Parameter*>::iterator i;
	for (i = parameters->begin(); i != parameters->end(); ++i)
		if (i->first == name)
			return (Parameter*) (i->second);

	return NULL;
}

void Link::removeParameter( Parameter *parameter ) {
	if (parameters->empty())
		return;

	std::map<std::string, Parameter*>::iterator i;
	for (i = parameters->begin(); i != parameters->end(); ++i) {
		if (i->first == parameter->getName()) {
			parameters->erase( i );
			return;
		}
	}
}

void Link::updateConnector( bptncon::Connector *newConnector ) {
	int i, size;
	Bind *bind;
	bptncon::Role *newRole;

	if (this->connector == NULL) {
		//TODO test if the set of roles is identical
		return;
	}

	size = binds->size();
	for (i = 0; i < size; i++) {
		bind = (Bind*) (*binds)[i];
		newRole = newConnector->getRole( bind->getRole()->getLabel() );
		bind->setRole( newRole );
	}
	this->connector = newConnector;
}

bool Link::containsNode( bptnc::Node *node, bptnd::GenericDescriptor *descriptor ) {
	return containsNode( node, descriptor, getBinds() );
}

bool Link::containsNode( bptnc::Node *node, bptnd::GenericDescriptor *descriptor, std::vector<Bind*> *binds ) {

	Bind *bind;
	bptnc::Node *bindNode;
	bptni::InterfacePoint *bindInterface;
	bptnd::GenericDescriptor *bindDescriptor;

	std::vector<Bind*>::iterator i;
	i = binds->begin();
	while (i != binds->end()) {
		bind = (*i);
		bindInterface = bind->getInterfacePoint();
		if (bindInterface != NULL && bindInterface->instanceOf( "Port" )) {
			bindNode = ((bptni::Port*) bindInterface)->getEndNode();

		} else {
			bindNode = bind->getNode();
		}



		if (node == bindNode) {
			bindDescriptor = bind->getDescriptor();
			if (bindDescriptor != NULL) {
				if (bindDescriptor == descriptor) {
					return true;
				}

			} else {
				if (((bptnc::NodeEntity*) node->getDataEntity())->getDescriptor() == descriptor) {

					return true;
				}
			}
		}
		++i;
	}
	return false;
}
}
}
}
}
}
