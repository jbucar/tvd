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

#include "../../include/link/Bind.h"
#include "../../include/Parameter.h"
#include "../../include/interfaces/Port.h"
#include "../../include/interfaces/InterfacePoint.h"
#include "../../include/components/CompositeNode.h"
#include "../../include/components/Node.h"
#include "../../include/descriptor/GenericDescriptor.h"
#include "../../include/connectors/Role.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace link {
Bind::Bind( components::Node* node, bptni::InterfacePoint* interfPt, bptnd::GenericDescriptor* desc, bptncon::Role* role ) {

	this->node = node;
	this->interfacePoint = interfPt;
	this->descriptor = desc;
	this->role = role;
	this->parameters = new std::map<std::string, Parameter*>;
	typeSet.insert( "Bind" );
}

bool Bind::instanceOf( const std::string &s ) {
	if (typeSet.empty())
		return false;
	else
		return (typeSet.find( s ) != typeSet.end());
}

bptnd::GenericDescriptor* Bind::getDescriptor() {
	return descriptor;
}

bptni::InterfacePoint* Bind::getInterfacePoint() {
	return this->interfacePoint;
}

bptnc::Node* Bind::getNode() {
	return this->node;
}

bptncon::Role* Bind::getRole() {
	return this->role;
}

void Bind::setInterfacePoint( bptni::InterfacePoint* interfPt ) {
	this->interfacePoint = interfPt;
}

void Bind::setNode( components::Node* node ) {
	this->node = node;
}

void Bind::setRole( bptncon::Role* role ) {
	this->role = role;
}

void Bind::setDescriptor( bptnd::GenericDescriptor* desc ) {
	this->descriptor = desc;
}

bptn::Parameter *Bind::setParameterValue( const std::string &propertyLabel, bptn::Parameter *propertyValue ) {

	return (*parameters)[propertyLabel] = propertyValue;
}

std::string Bind::toString() {
	return (getRole()->getLabel() + '/' + getNode()->getId() + '/' + getInterfacePoint()->getId());
}

void Bind::addParameter( bptn::Parameter* parameter ) {
	if (parameter == NULL || parameters == NULL) {
		return;
	}

	(*parameters)[parameter->getName()] = parameter;
}

std::vector<bptn::Parameter*>* Bind::getParameters() {
	if (parameters->empty())
		return NULL;

	std::map<std::string, Parameter*>::iterator i;
	std::vector<bptn::Parameter*>* params;

	params = new std::vector<bptn::Parameter*>;
	for (i = parameters->begin(); i != parameters->end(); ++i)
		params->push_back( i->second );

	return params;
}

bptn::Parameter* Bind::getParameter( const std::string &paramName ) {
	if (parameters->empty())
		return NULL;

	std::map<std::string, bptn::Parameter*>::iterator i;
	for (i = parameters->begin(); i != parameters->end(); ++i)
		if (i->first == paramName)
			return (Parameter*) (i->second);

	return NULL;
}

bool Bind::removeParameter( bptn::Parameter *parameter ) {
	if (parameters->empty())
		return false;

	std::map<std::string, bptn::Parameter*>::iterator i;

	for (i = parameters->begin(); i != parameters->end(); ++i) {
		if (i->first == parameter->getName()) {
			parameters->erase( i );
			return true;
		}
	}

	return false;
}

std::vector<bptnc::Node*>* Bind::getNodeNesting() {

	std::vector<bptnc::Node*>* nodeNesting;
	std::vector<bptnc::Node*>* nodeSequence;
	std::vector<bptnc::Node*>::iterator i;

	nodeNesting = new std::vector<bptnc::Node*>;
	nodeNesting->push_back( node );
	if (interfacePoint != NULL) {
		if (interfacePoint->instanceOf( "Port" ) && !(interfacePoint->instanceOf( "SwitchPort" ))) {

			nodeSequence = ((bptni::Port*) interfacePoint)->getMapNodeNesting();

			if (nodeSequence != NULL) {
				for (i = nodeSequence->begin(); i != nodeSequence->end(); ++i) {

					nodeNesting->push_back( *i );
				}
			}

			delete nodeSequence;
		}
	}

	return nodeNesting;
}

bptni::InterfacePoint* Bind::getEndPointInterface() {
	bptnc::NodeEntity* nodeEntity;
	bptnc::CompositeNode* compositeNode;
	bptni::Port* port;

	nodeEntity = (components::NodeEntity*) (node->getDataEntity());
	if (nodeEntity->instanceOf( "CompositeNode" ) && interfacePoint->instanceOf( "Port" )) {

		compositeNode = (components::CompositeNode*) nodeEntity;
		port = (bptni::Port*) interfacePoint;
		return compositeNode->getMapInterface( port );

	} else {
		return interfacePoint;
	}
}
}
}
}
}
}
