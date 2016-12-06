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

#ifndef NCLCONNECTORSPARSER_H_
#define NCLCONNECTORSPARSER_H_

#include "../ModuleParser.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
	class Parameter;
	
namespace connectors {
	class Connector;
	class SimpleCondition;
	class CompoundCondition;
	class AssessmentStatement;
	class CompoundStatement;
	class Action;
	class SimpleAction;
	class CompoundAction;
	class CausalConnector;
	class ConditionExpression;
	class ConnectorBase;
	class Statement;
	class ValueAssessment;
	class AttributeAssessment;
}}}}}
namespace bptn = ::br::pucrio::telemidia::ncl;
namespace bptncon = ::br::pucrio::telemidia::ncl::connectors;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
	class DocumentParser;
}}}}}
namespace bptcf = ::br::pucrio::telemidia::converter::framework;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {

class NclImportParser;

/**
 * Clase encargada de parsear los conectores.
 */
class NclConnectorsParser: public ModuleParser {
	//Ref Classes
private:
	NclImportParser *importParser;

public:
	explicit NclConnectorsParser( bptcf::DocumentParser *documentParser );
	bptncon::CompoundCondition *parseCompoundCondition( util::xml::dom::Node parentElement );
	bptncon::AssessmentStatement *parseAssessmentStatement( util::xml::dom::Node parentElement );
	bptncon::CompoundStatement *parseCompoundStatement( util::xml::dom::Node parentElement );
	bptncon::CompoundAction *parseCompoundAction( util::xml::dom::Node parentElement );
	bptncon::CausalConnector *parseCausalConnector( util::xml::dom::Node parentElement );
	void *parseConnectorBase( util::xml::dom::Node parentElement );
	
	virtual bptncon::SimpleCondition *createSimpleCondition( util::xml::dom::Node parentElement )=0;
	virtual bptncon::CompoundCondition *createCompoundCondition( util::xml::dom::Node parentElement )=0;
	
	virtual void addConditionExpressionToCompoundCondition(bptncon::CompoundCondition *parentObject, bptncon::ConditionExpression *childObject)=0;
	virtual bptncon::AssessmentStatement *createAssessmentStatement( util::xml::dom::Node parentElement )=0;
	virtual void addAttributeAssessmentToAssessmentStatement(bptncon::AssessmentStatement *parentObject, bptncon::AttributeAssessment *childObject )=0;
	virtual void addValueAssessmentToAssessmentStatement(bptncon::AssessmentStatement *parentObject, bptncon::ValueAssessment *childObject)=0;
	
	virtual bptncon::AttributeAssessment *createAttributeAssessment( util::xml::dom::Node parentElement )=0;
	virtual bptncon::ValueAssessment *createValueAssessment( util::xml::dom::Node parentElement )=0;
	
	virtual bptncon::CompoundStatement *createCompoundStatement( util::xml::dom::Node parentElement )=0;
	virtual void addStatementToCompoundStatement(bptncon::CompoundStatement *parentObject, bptncon::Statement *childObject)=0;
	
	virtual bptncon::SimpleAction *createSimpleAction( util::xml::dom::Node parentElement )=0;
	virtual bptncon::CompoundAction *createCompoundAction( util::xml::dom::Node parentElement )=0;
	
	virtual void addActionToCompoundAction(bptncon::CompoundAction *parentObject, bptncon::Action *childObject)=0;
	virtual bptn::Parameter *createConnectorParam( util::xml::dom::Node parentElement )=0;
	virtual bptncon::CausalConnector *createCausalConnector( util::xml::dom::Node parentElement )=0;
	virtual void addConditionToCausalConnector(bptncon::CausalConnector *parentObject, bptncon::ConditionExpression *childObject)=0;
	virtual void addActionToCausalConnector(bptncon::CausalConnector *parentObject, bptncon::Action *childObject)=0;
	virtual void addConnectorParamToCausalConnector(bptncon::Connector *parentObject, bptn::Parameter *childObject)=0;
	virtual bptncon::ConnectorBase *createConnectorBase( util::xml::dom::Node parentElement )=0;
	virtual void addImportBaseToConnectorBase(bptncon::ConnectorBase *parentObject, util::xml::dom::Node childObject )=0;
	virtual void addCausalConnectorToConnectorBase(bptncon::ConnectorBase *parentObject, bptncon::Connector *childObject)=0;

	NclImportParser *getImportParser(){
		return importParser;
	}
	
	void setImportParser( NclImportParser *importParser ){
		this->importParser = importParser;
	}
};
}
}
}
}
}
}

#endif /*NCLCONNECTORSPARSER_H_*/
