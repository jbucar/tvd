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

#include "../../../../include/framework/ncl/NclConnectorsParser.h"
#include "../../../../include/framework/ncl/NclImportParser.h"
#include "ncl30/Parameter.h"
#include "ncl30/connectors/SimpleCondition.h"
#include "ncl30/connectors/CompoundCondition.h"
#include "ncl30/connectors/AssessmentStatement.h"
#include "ncl30/connectors/CompoundStatement.h"
#include "ncl30/connectors/CompoundAction.h"
#include "ncl30/connectors/CausalConnector.h"
#include "ncl30/connectors/ConditionExpression.h"
#include "ncl30/connectors/Action.h"
#include "ncl30/connectors/SimpleAction.h"
#include "ncl30/connectors/ConnectorBase.h"
#include "ncl30/connectors/Statement.h"
#include "ncl30/connectors/ValueAssessment.h"
#include "ncl30/connectors/AttributeAssessment.h"
#include <util/xml/nodehandler.h>
#include <boost/foreach.hpp>

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {

NclConnectorsParser::NclConnectorsParser( DocumentParser *documentParser )
	: ModuleParser( documentParser )
{
}

bptncon::CompoundCondition *NclConnectorsParser::parseCompoundCondition( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptncon::CompoundCondition *parentObject = createCompoundCondition( parentElement );
	if (parentObject == NULL) {
		return NULL;
	}

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node element, elementNodeList ) {
		const std::string &elementTagName = nodeHnd->tagName(element);
		bptncon::ConditionExpression *expression = NULL;
		if (elementTagName.compare( "simpleCondition" ) == 0) {
			expression = static_cast<bptncon::ConditionExpression*>(createSimpleCondition(element));
		} else if (elementTagName.compare( "assessmentStatement" ) == 0) {
			expression = static_cast<bptncon::ConditionExpression*>(parseAssessmentStatement(element));
		} else if (elementTagName.compare( "compoundCondition" ) == 0) {
			expression = static_cast<bptncon::ConditionExpression*>(parseCompoundCondition(element));
		} else if (elementTagName.compare( "compoundStatement" ) == 0) {
			expression = static_cast<bptncon::ConditionExpression*>(parseCompoundStatement(element));
		}
		if (expression != NULL){
			addConditionExpressionToCompoundCondition(parentObject, expression);
			expression = NULL;
		}
	}
	return parentObject;
}

bptncon::AssessmentStatement *NclConnectorsParser::parseAssessmentStatement( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptncon::AssessmentStatement *parentObject = createAssessmentStatement( parentElement );
	if (parentObject == NULL) {
		return NULL;
	}

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node element, elementNodeList ) {
		const std::string &elementTagName = nodeHnd->tagName(element);
		if (elementTagName.compare( "attributeAssessment" ) == 0) {
			bptncon::AttributeAssessment *aAssesstment = createAttributeAssessment(element);
			if (aAssesstment != NULL) {
				addAttributeAssessmentToAssessmentStatement( parentObject, aAssesstment);
			}
		} else if (elementTagName.compare( "valueAssessment" ) == 0) {
			bptncon::ValueAssessment *vAssesstment = createValueAssessment(element);
			if (vAssesstment != NULL) {
				addValueAssessmentToAssessmentStatement( parentObject, vAssesstment);
			}
		}
	}
	return parentObject;
}

bptncon::CompoundStatement *NclConnectorsParser::parseCompoundStatement( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptncon::CompoundStatement *parentObject = createCompoundStatement( parentElement );
	if (parentObject == NULL) {
		return NULL;
	}

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node element, elementNodeList ) {
		const std::string &elementTagName = nodeHnd->tagName(element);
		bptncon::Statement *statement = NULL;
		if (elementTagName.compare( "assessmentStatement" ) == 0) {
			statement = parseAssessmentStatement( element );
		} else if (elementTagName.compare( "compoundStatement" ) == 0) {
			statement = parseCompoundStatement( element );
		}
		if(statement != NULL){
			addStatementToCompoundStatement(parentObject, statement);
			statement = NULL;
		}
	}
	return parentObject;
}

bptncon::CompoundAction *NclConnectorsParser::parseCompoundAction( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptncon::CompoundAction *parentObject = createCompoundAction( parentElement );
	if (parentObject == NULL) {
		return NULL;
	}

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node element, elementNodeList ) {
		const std::string &elementTagName = nodeHnd->tagName(element);
		bptncon::Action *action = NULL;

		if (elementTagName.compare( "simpleAction" ) == 0) {

			action = static_cast<bptncon::Action*>(createSimpleAction(element));
		} else if (elementTagName.compare( "compoundAction" ) == 0) {

			action = static_cast<bptncon::Action*>(parseCompoundAction(element));
		}
		if(action != NULL){
			addActionToCompoundAction( parentObject, action);
			action = NULL;
		}
	}
	return parentObject;
}

bptncon::CausalConnector *NclConnectorsParser::parseCausalConnector( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	//pre-compile attributes
	bptncon::CausalConnector *parentObject = createCausalConnector( parentElement );
	if (parentObject == NULL) {
		return NULL;
	}

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node element, elementNodeList ) {
		const std::string &elementTagName = nodeHnd->tagName(element);
		if (elementTagName.compare( "simpleCondition" ) == 0) {
			bptncon::SimpleCondition *condition = createSimpleCondition(element);
			if (condition != NULL) {
				addConditionToCausalConnector( parentObject, static_cast<bptncon::ConditionExpression*>(condition));
			}
		} else if (elementTagName.compare( "simpleAction" ) == 0) {
			bptncon::SimpleAction *action = createSimpleAction(element );
			if (action != NULL) {
				addActionToCausalConnector( parentObject, static_cast<bptncon::Action*>(action));
			}
		} else if (elementTagName.compare( "compoundAction" ) == 0) {
			bptncon::CompoundAction *action = parseCompoundAction( element );
			if (action != NULL) {
				addActionToCausalConnector( parentObject, static_cast<bptncon::Action*>(action));
			}
		} else if (elementTagName.compare( "connectorParam" ) == 0) {
			bptn::Parameter *param = createConnectorParam(element);
			if (param != NULL) {
				addConnectorParamToCausalConnector(static_cast<bptncon::Connector*>(parentObject), param);
			}
		} else if (elementTagName.compare( "compoundCondition" ) == 0) {
			bptncon::CompoundCondition *condition = parseCompoundCondition( element );
			if (condition != NULL) {
				addConditionToCausalConnector( parentObject, static_cast<bptncon::ConditionExpression*>(condition));
			}
		}
	}
	return parentObject;
}

void *NclConnectorsParser::parseConnectorBase( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptncon::ConnectorBase *parentObject = createConnectorBase( parentElement );
	if (parentObject == NULL) {
		return NULL;
	}

	std::vector<util::xml::dom::Node> elementNodeList;
	nodeHnd->children(parentElement, elementNodeList);

	BOOST_FOREACH( util::xml::dom::Node element, elementNodeList ) {
		const std::string &elementTagName = nodeHnd->tagName(element);
		if (elementTagName.compare( "importBase" ) == 0) {
				addImportBaseToConnectorBase( parentObject, element );
		} else if (elementTagName.compare( "causalConnector" ) == 0) {
			bptncon::CausalConnector *connector = parseCausalConnector( element );
			if (connector != NULL) {
				addCausalConnectorToConnectorBase( parentObject, static_cast<bptncon::Connector*>(connector));
			}
		}
	}
	return (void*)parentObject;
}

}
}
}
}
}
}
