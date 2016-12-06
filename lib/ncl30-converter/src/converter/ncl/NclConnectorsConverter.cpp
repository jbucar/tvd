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

#include "../../../include/ncl/NclConnectorsConverter.h"
#include "../../../include/ncl/NclDocumentConverter.h"
#include "ncl30/NclDocument.h"
#include "ncl30/Parameter.h"
#include "ncl30/animation/Animation.h"
#include "ncl30/connectors/EventUtil.h"
#include "ncl30/connectors/ValueAssessment.h"
#include "ncl30/connectors/ConnectorBase.h"
#include "ncl30/connectors/CausalConnector.h"
#include "ncl30/connectors/SimpleAction.h"
#include "ncl30/connectors/SimpleCondition.h"
#include "ncl30/connectors/CompoundAction.h"
#include "ncl30/connectors/CompoundCondition.h"
#include "ncl30/connectors/CompoundStatement.h"
#include "ncl30/connectors/AttributeAssessment.h"
#include "ncl30/connectors/AssessmentStatement.h"
#include "ncl30/connectors/ConditionExpression.h"
#include "ncl30/connectors/Statement.h"
#include "ncl30/connectors/Action.h"
#include "ncl30/util/Comparator.h"
#include <util/log.h>
#include <util/xml/nodehandler.h>
#include <boost/lexical_cast.hpp>

namespace bptna = ::br::pucrio::telemidia::ncl::animation;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {

NclConnectorsConverter::NclConnectorsConverter( bptcf::DocumentParser* documentParser )
	: NclConnectorsParser( documentParser )
{
}

void NclConnectorsConverter::addCausalConnectorToConnectorBase(bptncon::ConnectorBase *parentObject, bptncon::Connector *childObject){
	parentObject->addConnector(childObject);
}

void NclConnectorsConverter::addConnectorParamToCausalConnector(bptncon::Connector *parentObject, bptn::Parameter *childObject) {
	parentObject->addParameter(childObject);
}

void NclConnectorsConverter::addImportBaseToConnectorBase(bptncon::ConnectorBase *parentObject, util::xml::dom::Node childObject ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	// get the external location
	std::string baseLocation = nodeHnd->attribute(childObject, "documentURI");

	NclDocumentConverter *compiler = (NclDocumentConverter*) getDocumentParser();
	bptn::NclDocument *importedDocument = compiler->importDocument( baseLocation );
	if (importedDocument == NULL) {
		return;
	}

	bptncon::ConnectorBase *connectorBase = importedDocument->getConnectorBase();
	if (connectorBase == NULL) {
		return;
	}

	// get the external base alias and insert it into the document connector base
	std::string baseAlias = nodeHnd->attribute(childObject, "alias");
	parentObject->addBase( connectorBase, baseAlias, baseLocation );

	/*
	 * Hashtable connBaseimports = new Hashtable();
	 * connBaseimports->put(baseAlias,baseLocation);
	 * getDocumentParser()->addObject("return","ConnectorImports",
	 * connBaseimports);
	 */
}

/**
 * Crea un causalConnector.
 * @param parentElement El elemento DOM que tiene la información del conector.
 * @return Una nueva de instancia de @c CausalConnector.
 */
bptncon::CausalConnector *NclConnectorsConverter::createCausalConnector( util::xml::dom::Node parentElement ) {
	std::string connectorId = "";

	/*
	 * if (connectorUri->equalsIgnoreCase("")) { //se nao tiver uma uri do
	 * arquivo do conector, atribuir somente o id
	 * do elemento conector como id do conector
	 * connectorId = parentElement->getAttribute("id"); connectorId =
	 * "#" + parentElement->getAttribute("id"); } else { //atribuir a id do
	 * conector como sendo a uri do seu arquivo
	 * connectorId = connectorUri;
	 */
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();
	connectorId = nodeHnd->attribute(parentElement, "id");

	bptncon::CausalConnector *connector = new bptncon::CausalConnector( connectorId );
	return connector;
}

/**
 * Crea un causalConnector.
 * @param parentElement El elemento DOM que tiene la información del conectorBase.
 * @return Una nueva de instancia de @c ConnectorBase.
 */
bptncon::ConnectorBase *NclConnectorsConverter::createConnectorBase( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();
	// criar nova base de conectores com id gerado a partir do nome de seu elemento
	bptncon::ConnectorBase *connBase = new bptncon::ConnectorBase( nodeHnd->attribute(parentElement, "id") );
	return connBase;
}

bptn::Parameter *NclConnectorsConverter::createConnectorParam( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();
	bptn::Parameter *parameter = new bptn::Parameter(
		nodeHnd->attribute(parentElement, "name"),
		nodeHnd->attribute(parentElement, "type")
	);
	return parameter;
}

void NclConnectorsConverter::compileRoleInformation( bptncon::Role *role, util::xml::dom::Node parentElement ) {
	std::string attValue;
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();
	// event type
	if (nodeHnd->hasAttribute(parentElement, "eventType")) {
		attValue = nodeHnd->attribute(parentElement, "eventType");
		role->setEventType( bptncon::EventUtil::getTypeCode( attValue ) );
	}

	//  cardinality
	if (nodeHnd->hasAttribute(parentElement, "min")) {
		attValue = nodeHnd->attribute(parentElement, "min");
		role->setMinCon( (atoi( attValue.c_str() )) );
	}

	if (nodeHnd->hasAttribute(parentElement, "max")) {
		attValue = nodeHnd->attribute(parentElement, "max");

		if (attValue.compare( "unbounded" ) == 0) {
			role->setMaxCon( bptncon::Role::UNBOUNDED );
		} else {
			role->setMaxCon( atoi( attValue.c_str() ) );
		}
	}
}

bptncon::SimpleCondition *NclConnectorsConverter::createSimpleCondition( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();
	std::string roleLabel = nodeHnd->attribute(parentElement, "role");

	bptncon::SimpleCondition* conditionExpression = new bptncon::SimpleCondition(roleLabel);
	compileRoleInformation(conditionExpression, parentElement);

	std::string attValue;
	// transition
	if (nodeHnd->hasAttribute(parentElement, "transition")) {
		attValue = nodeHnd->attribute(parentElement, "transition");
		conditionExpression->setTransition( bptncon::EventUtil::getTransitionCode( attValue ) );
	}

	// parametro
	if (conditionExpression->getEventType() == bptncon::EventUtil::EVT_SELECTION) {
		if (nodeHnd->hasAttribute(parentElement, "key")) {
			attValue = nodeHnd->attribute(parentElement, "key");
			conditionExpression->setKey( attValue );
		}
	}

	// qualifier
	if (nodeHnd->hasAttribute(parentElement, "qualifier" )) {
		attValue = nodeHnd->attribute(parentElement, "qualifier");
		if (attValue.compare( "or" ) == 0) {
			conditionExpression->setQualifier( bptncon::CompoundCondition::OP_OR );
		} else {
			conditionExpression->setQualifier( bptncon::CompoundCondition::OP_AND );
		}
	}

	//testar delay
	if (nodeHnd->hasAttribute(parentElement, "delay")) {
		attValue = nodeHnd->attribute(parentElement, "delay");
		if (attValue[0] == '$') {
			conditionExpression->setDelay( attValue );
		} else {
			double delayValue;
			delayValue = boost::lexical_cast<float>( attValue.substr( 0, (attValue.length() - 1) ) ) * 1000;
			conditionExpression->setDelay( boost::lexical_cast<std::string>( delayValue ) );
		}
	}

	// retornar expressao de condicao
	return conditionExpression;
}

bptncon::CompoundCondition *NclConnectorsConverter::createCompoundCondition( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptncon::CompoundCondition *conditionExpression = new bptncon::CompoundCondition();
	if (nodeHnd->attribute(parentElement, "operator").compare("and") == 0) {
		conditionExpression->setOperator( bptncon::CompoundCondition::OP_AND );
	} else {
		conditionExpression->setOperator( bptncon::CompoundCondition::OP_OR );
	}

	//  testar delay
	if (nodeHnd->hasAttribute(parentElement, "delay")) {
		std::string attValue = nodeHnd->attribute(parentElement, "delay");
		if (attValue[0] == '$') {
			conditionExpression->setDelay( attValue );
		} else {
			double delayValue = boost::lexical_cast<float>( attValue.substr( 0, (attValue.length() - 1) ) ) * 1000;
			conditionExpression->setDelay( boost::lexical_cast<std::string>( delayValue ) );
		}
	}

	// retornar expressao de condicao
	return conditionExpression;
}

bptncon::AttributeAssessment* NclConnectorsConverter::createAttributeAssessment( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	std::string attValue;
	std::string roleLabel = nodeHnd->attribute(parentElement, "role");
	bptncon::AttributeAssessment *attributeAssessment = new bptncon::AttributeAssessment( roleLabel );

	// event type
	if (nodeHnd->hasAttribute(parentElement, "eventType")) {
		attValue = nodeHnd->attribute(parentElement, "eventType");
		attributeAssessment->setEventType( bptncon::EventUtil::getTypeCode( attValue ) );
	}

	// event type
	if (nodeHnd->hasAttribute(parentElement, "attributeType")) {
		attValue = nodeHnd->attribute(parentElement, "attributeType");
		attributeAssessment->setAttributeType( bptncon::EventUtil::getAttributeTypeCode( attValue ) );
	}

	// parameter
	if (attributeAssessment->getEventType() == bptncon::EventUtil::EVT_SELECTION) {
		if (nodeHnd->hasAttribute(parentElement, "key")) {
			attValue = nodeHnd->attribute(parentElement, "key");
			attributeAssessment->setKey( attValue );
		}
	}

	//testing offset
	if (nodeHnd->hasAttribute(parentElement, "offset")) {
		attValue = nodeHnd->attribute(parentElement, "offset");
		attributeAssessment->setOffset( attValue );
	}

	return attributeAssessment;
}

bptncon::ValueAssessment *NclConnectorsConverter::createValueAssessment( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	std::string attValue = nodeHnd->attribute(parentElement, "value");
	return new bptncon::ValueAssessment(attValue);
}

bptncon::AssessmentStatement *NclConnectorsConverter::createAssessmentStatement( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptncon::AssessmentStatement *assessmentStatement;
	if (nodeHnd->hasAttribute(parentElement, "comparator")) {
		std::string attValue = nodeHnd->attribute(parentElement, "comparator");
		assessmentStatement = new bptncon::AssessmentStatement( ncl_util::Comparator::fromString( attValue ) );
	} else {
		assessmentStatement = new bptncon::AssessmentStatement( ncl_util::Comparator::CMP_EQ );
	}

	return assessmentStatement;
}

bptncon::CompoundStatement *NclConnectorsConverter::createCompoundStatement( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptncon::CompoundStatement *compoundStatement = new bptncon::CompoundStatement();
	if (nodeHnd->attribute(parentElement, "operator").compare( "and" ) == 0) {
		compoundStatement->setOperator( bptncon::CompoundStatement::OP_AND );
	} else {
		compoundStatement->setOperator( bptncon::CompoundStatement::OP_OR );
	}

	// testing isNegated
	if (nodeHnd->hasAttribute(parentElement, "isNegated")) {
		std::string attValue = nodeHnd->attribute(parentElement, "isNegated");
		compoundStatement->setNegated( attValue.compare( "true" ) == 0 );
	}

	return compoundStatement;
}

/**
 * Crea una acción simple.
 * @param parentElement El elemento DOM que tiene la información de la acción.
 * @return Una nueva de instancia de @c SimpleAction.
 */
bptncon::SimpleAction *NclConnectorsConverter::createSimpleAction( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	std::string attValue = nodeHnd->attribute(parentElement, "role");
	bptncon::SimpleAction *actionExpression = new bptncon::SimpleAction( attValue );

	//animation
	if (attValue == "set") {
		bptna::Animation* animation = NULL;
		if (nodeHnd->hasAttribute(parentElement, "duration")) {
			attValue = nodeHnd->attribute(parentElement, "duration");

			animation = new bptna::Animation();
			if (attValue[0] != '$') {
				if (attValue != "" && attValue.find_last_of( 's' ) == attValue.length() - 1)
					attValue = attValue.substr( 0, attValue.length() - 1 );
				try {
					boost::lexical_cast<float>( attValue );
				} catch (boost::bad_lexical_cast &) {
					LERROR("NclConnectorsConverter", "Invalid parameter (duration) for animation");
					attValue = "0";
				}
			}
			animation->setDuration( attValue );
		}

		if (nodeHnd->hasAttribute(parentElement, "by")) {
			attValue = nodeHnd->attribute(parentElement, "by");
			if (animation == NULL) {
				animation = new bptna::Animation();
			}

			if (attValue[0] == '$') {
				animation->setBy( attValue );
			} else {
				try {
					boost::lexical_cast<float>( attValue );
					animation->setBy( attValue );
				} catch (boost::bad_lexical_cast &) {
					LERROR("NclConnectorsConverter", "Invalid parameter (by) for animation");
					animation->setBy( "0" );
				}
			}

		}
		actionExpression->setAnimation( animation );
	}

	compileRoleInformation( actionExpression, parentElement );

	// transition
	if (nodeHnd->hasAttribute(parentElement, "actionType")) {
		attValue = nodeHnd->attribute(parentElement, "actionType");
		actionExpression->setActionType( convertActionType( attValue ) );
	}

	if (nodeHnd->hasAttribute(parentElement, "qualifier")) {
		if (nodeHnd->attribute(parentElement, "qualifier").compare( "seq" ) == 0) {
			actionExpression->setQualifier( bptncon::CompoundAction::OP_SEQ );
		} else { // any
			actionExpression->setQualifier( bptncon::CompoundAction::OP_PAR );
		}
	}

	//testing delay
	if (nodeHnd->hasAttribute(parentElement, "delay")) {
		attValue = nodeHnd->attribute(parentElement, "delay");
		if (attValue[0] == '$') {
			actionExpression->setDelay( attValue );
		} else {
			actionExpression->setDelay( boost::lexical_cast<std::string>( boost::lexical_cast<float>( attValue.substr( 0, attValue.length() - 1 ) ) * 1000 ) );
		}
	}

	//  testing repeatDelay
	if (nodeHnd->hasAttribute(parentElement, "repeatDelay")) {
		attValue = nodeHnd->attribute(parentElement, "repeatDelay");
		actionExpression->setDelay( attValue );
		if (attValue[0] == '$') {
			actionExpression->setDelay( attValue );
		} else {
			actionExpression->setDelay( boost::lexical_cast<std::string>( boost::lexical_cast<float>( attValue.substr( 0, attValue.length() - 1 ) ) * 1000 ) );
		}
	}

	// repeat
	if (nodeHnd->hasAttribute(parentElement, "repeat")) {
		attValue = nodeHnd->attribute(parentElement, "repeat");
		if (attValue.compare( "infinite" ) == 0) {
			actionExpression->setRepeat( boost::lexical_cast<std::string>( 2 ^ 30 ) );
		} else {
			actionExpression->setRepeat( attValue );
		}
	}

	// testing value
	if (nodeHnd->hasAttribute(parentElement, "value")) {
		attValue = nodeHnd->attribute(parentElement, "value");
		actionExpression->setValue( attValue );
	}

	// returning action expression
	return actionExpression;
}

bptncon::CompoundAction *NclConnectorsConverter::createCompoundAction( util::xml::dom::Node parentElement ) {
	util::xml::dom::NodeHandler *nodeHnd = getNodeHandler();

	bptncon::CompoundAction *actionExpression = new bptncon::CompoundAction();
	if (nodeHnd->attribute(parentElement, "operator").compare( "seq" ) == 0) {
		actionExpression->setOperator( bptncon::CompoundAction::OP_SEQ );
	} else {
		actionExpression->setOperator( bptncon::CompoundAction::OP_PAR );
	}

	//  testar delay
	if (nodeHnd->hasAttribute(parentElement, "delay")) {
		std::string attValue = nodeHnd->attribute(parentElement, "delay");
		if (attValue[0] == '$') {
			actionExpression->setDelay( attValue );
		} else {
			actionExpression->setDelay( boost::lexical_cast<std::string>( boost::lexical_cast<float>( attValue.substr( 0, attValue.length() - 1 ) ) * 1000 ) );
		}
	}

	// retornar expressao de condicao
	return actionExpression;
}

short NclConnectorsConverter::convertActionType( const std::string &actionType ) {
	if (actionType == "start")
		return bptncon::SimpleAction::ACT_START;

	else if (actionType == "stop")
		return bptncon::SimpleAction::ACT_STOP;

	else if (actionType == "set")
		return bptncon::SimpleAction::ACT_SET;

	else if (actionType == "pause")
		return bptncon::SimpleAction::ACT_PAUSE;

	else if (actionType == "resume")
		return bptncon::SimpleAction::ACT_RESUME;

	else if (actionType == "abort")
		return bptncon::SimpleAction::ACT_ABORT;

	return -1;
}

short NclConnectorsConverter::convertEventState( const std::string &eventState ) {
	if (eventState == "occurring") {
		return bptncon::EventUtil::ST_OCCURRING;

	} else if (eventState == "paused") {
		return bptncon::EventUtil::ST_PAUSED;

	} else if (eventState == "sleeping") {
		return bptncon::EventUtil::ST_SLEEPING;

	}

	return -1;
}

void NclConnectorsConverter::addConditionExpressionToCompoundCondition(bptncon::CompoundCondition *parentObject, bptncon::ConditionExpression *childObject){

	parentObject->addConditionExpression(childObject);
}

void NclConnectorsConverter::addAttributeAssessmentToAssessmentStatement(bptncon::AssessmentStatement *parentObject, bptncon::AttributeAssessment *childObject ) {

	if (parentObject->getMainAssessment() == NULL) {
		parentObject->setMainAssessment(childObject);

	} else {
		parentObject->setOtherAssessment(childObject);
	}
}

void NclConnectorsConverter::addValueAssessmentToAssessmentStatement(bptncon::AssessmentStatement *parentObject, bptncon::ValueAssessment *childObject) {

	parentObject->setOtherAssessment(childObject);
}

void NclConnectorsConverter::addStatementToCompoundStatement(bptncon::CompoundStatement *parentObject, bptncon::Statement *childObject) {

	parentObject->addStatement(childObject );
}

void NclConnectorsConverter::addActionToCompoundAction(bptncon::CompoundAction *parentObject, bptncon::Action *childObject) {

	parentObject->addAction(childObject);
}

void NclConnectorsConverter::addConditionToCausalConnector(bptncon::CausalConnector *parentObject, bptncon::ConditionExpression *childObject) {

	parentObject->setConditionExpression(childObject);
}

/**
 * Agrega una acción a un causal conector.
 * @param parentObject El conector al que va a ser agregada la acción.
 * @param childObject La acción a agregar.
 */
void NclConnectorsConverter::addActionToCausalConnector(bptncon::CausalConnector *parentObject, bptncon::Action *childObject){

	parentObject->setAction(childObject);
}

}
}
}
}
}
