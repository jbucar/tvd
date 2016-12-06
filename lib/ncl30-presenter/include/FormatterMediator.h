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

#ifndef _FormatterMediator_H_
#define _FormatterMediator_H_

#include "ncl30/components/ContextNode.h"
#include "ncl30/components/CompositeNode.h"
#include "ncl30/components/ContentNode.h"
#include "ncl30/components/Node.h"
#include "ncl30/components/NodeEntity.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl30/interfaces/Anchor.h"
#include "ncl30/interfaces/PropertyAnchor.h"
#include "ncl30/interfaces/Port.h"
#include "ncl30/interfaces/SwitchPort.h"
#include "ncl30/interfaces/InterfacePoint.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl30/switches/Rule.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "ncl30/descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl30/link/Bind.h"
#include "ncl30/link/CausalLink.h"
#include "ncl30/link/Link.h"
#include "ncl30/link/LinkComposition.h"
using namespace ::br::pucrio::telemidia::ncl::link;

#include "ncl30/connectors/EventUtil.h"
#include "ncl30/connectors/SimpleAction.h"
#include "ncl30/connectors/Connector.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl30/layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl30/reuse/ReferNode.h"
using namespace ::br::pucrio::telemidia::ncl::reuse;

#include "ncl30/Base.h"
#include "ncl30/NclDocument.h"
#include "ncl30/connectors/ConnectorBase.h"
#include "ncl30/descriptor/DescriptorBase.h"
#include "ncl30/layout/RegionBase.h"
#include "ncl30/switches/RuleBase.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "ncl30/transition/Transition.h"
#include "ncl30/transition/TransitionBase.h"
using namespace ::br::pucrio::telemidia::ncl::transition;

#include "model/CompositeExecutionObject.h"
#include "model/ExecutionObject.h"
#include "model/NodeNesting.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "model/FormatterEvent.h"
#include "model/AttributionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "model/LinkActionListener.h"
#include "model/LinkAssignmentAction.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::link;

#include "adapters/RuleAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adaptation::context;

#include "emconverter/FormatterConverter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::emconverter;

#include "FormatterScheduler.h"
#include "privatebase/PrivateBaseManager.h"

#include <map>
#include <set>
#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {

typedef struct
{
	std::string baseId;
	std::string docPath;
	std::string parentDocId;
	std::string nodeId;
	std::string docId;
} DocData;

/**
 * Clase que act+ua como formateador.
 */
class FormatterMediator
{

	typedef map<string, FormatterEvent*> Events;
	typedef map<string, vector<FormatterEvent*>*> EntryEvents;
private:

	DocData _docData;
	NclDocument* currentDocument;

	PrivateBaseManager* privateBaseManager;
	Events* documentEvents;
	EntryEvents* documentEntryEvents;
	FormatterScheduler* scheduler;
	RuleAdapter* ruleAdapter;
	FormatterConverter* compiler;
	PlayerAdapterManager* playerManager;
	IDeviceLayout* deviceLayout;
	bool isEmbedded;
	player::System *_sys;

public:
	FormatterMediator( const std::string &baseId, const std::string &docPath, player::System *system );
	virtual ~FormatterMediator();

	static void release();
	void refreshContent() {
	}
	;
	bool parseDocument( void );

private:
	virtual void* addDocument( string fName );
	bool removeDocument( string documentId );
	ContextNode* getDocumentContext( string documentId );

public:
	void setDepthLevel( int level );
	int getDepthLevel();

protected:
	vector<FormatterEvent*>* processDocument( string documentId, string interfaceId );

	vector<FormatterEvent*>* getDocumentEntryEvent( string documentId );
	bool compileDocument( string documentId, string interfaceId );
	bool prepareDocument( string documentId );

	bool startDocument( string documentId, string interfaceId );
	bool stopDocument( string documentId );
	bool pauseDocument( string documentId );
	bool resumeDocument( string documentId );

	LayoutRegion* addRegion( string documentId, string regionBaseId, string regionId, string xmlRegion );

	LayoutRegion* removeRegion( string documentId, string regionBaseId, string regionId );

	RegionBase* addRegionBase( string documentId, string xmlRegionBase );
	RegionBase* removeRegionBase( string documentId, string regionBaseId );

	Rule* addRule( string documentId, string xmlRule );
	Rule* removeRule( string documentId, string ruleId );
	RuleBase* addRuleBase( string documentId, string xmlRuleBase );
	RuleBase* removeRuleBase( string documentId, string ruleBaseId );
	Transition* addTransition( string documentId, string xmlTransition );
	Transition* removeTransition( string documentId, string transitionId );

	TransitionBase* addTransitionBase( string documentId, string xmlTransitionBase );

	TransitionBase* removeTransitionBase( string documentId, string transitionBaseId );

	Connector* addConnector( string documentId, string xmlConnector );
	Connector* removeConnector( string documentId, string connectorId );
	ConnectorBase* addConnectorBase( string documentId, string xmlConnectorBase );

	ConnectorBase* removeConnectorBase( string documentId, string connectorBaseId );

	GenericDescriptor* addDescriptor( string documentId, string xmlDescriptor );

	GenericDescriptor* removeDescriptor( string documentId, string descriptorId );

	DescriptorBase* addDescriptorBase( string documentId, string xmlDescriptorBase );

	DescriptorBase* removeDescriptorBase( string documentId, string descriptorBaseId );

	Base* addImportBase( string documentId, string docBaseId, string xmlImportBase );

	Base* removeImportBase( string documentId, string docBaseId, string documentURI );

	NclDocument* addImportedDocumentBase( string documentId, string xmlImportedDocumentBase );

	NclDocument* removeImportedDocumentBase( string documentId, string importedDocumentBaseId );

	NclDocument* addImportNCL( string documentId, string xmlImportNCL );
	NclDocument* removeImportNCL( string documentId, string documentURI );

	void processInsertedReferNode( ReferNode* referNode );
	void processInsertedComposition( CompositeNode* composition );

	Node* addNode( string documentId, string compositeId, string xmlNode );

	Node* removeNode( string documentId, string compositeId, string nodeId );

	InterfacePoint* addInterface( string documentId, string nodeId, string xmlInterface );

	void removeInterfaceMappings( Node* node, InterfacePoint* interfacePoint, CompositeNode* composition );

	void removeInterfaceLinks( Node* node, InterfacePoint* interfacePoint, LinkComposition* composition );

	void removeInterface( Node* node, InterfacePoint* interfacePoint );

	InterfacePoint* removeInterface( string documentId, string nodeId, string interfaceId );

	Link* addLink( string documentId, string compositeId, string xmlLink );

	void removeLink( LinkComposition* composition, Link* link );
	Link* removeLink( string documentId, string compositeId, string linkId );

	bool setPropertyValue( string documentId, string nodeId, string propertyId, string value );
	void setDevicesSize( NclDocument* aDocument );
	void iterateBases( RegionBase *aBase );
	void setRegionSize( RegionBase* regBase );

public:
	bool setKeyHandler( bool isHandler );
	void presentationCompleted( FormatterEvent* documentEvent );

	void play();
	void stop();
	void pause();
	void resume();

};
}
}
}
}
}

#endif //_FormatterMediator_H_
