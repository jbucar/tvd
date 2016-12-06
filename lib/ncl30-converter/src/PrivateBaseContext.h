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

#ifndef PrivateBaseContext_H_
#define PrivateBaseContext_H_

#include "ncl30/components/PrivateBase.h"
#include "ncl30/components/ContextNode.h"
#include "ncl30/components/Node.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl30/switches/Rule.h"
#include "ncl30/switches/SwitchNode.h"
#include "ncl30/switches/RuleBase.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "ncl30/interfaces/SwitchPort.h"
#include "ncl30/interfaces/Anchor.h"
#include "ncl30/interfaces/InterfacePoint.h"
#include "ncl30/interfaces/Port.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl30/connectors/Connector.h"
#include "ncl30/connectors/ConnectorBase.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl30/layout/LayoutRegion.h"
#include "ncl30/layout/RegionBase.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl30/descriptor/GenericDescriptor.h"
#include "ncl30/descriptor/DescriptorBase.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl30/link/Link.h"
using namespace ::br::pucrio::telemidia::ncl::link;

#include "ncl30/transition/Transition.h"
#include "ncl30/transition/TransitionBase.h"
using namespace ::br::pucrio::telemidia::ncl::transition;

#include "ncl30/Base.h"
#include "ncl30/NclDocument.h"
#include "ncl30/IPrivateBaseContext.h"
using namespace ::br::pucrio::telemidia::ncl;

#include <map>
#include <vector>
#include <string>
using namespace std;

typedef struct
{
	string embeddedNclNodeId;
	string embeddedNclNodeLocation;
	NclDocument* embeddedDocument;
} EmbeddedNclData;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
class PrivateBaseContext: public IPrivateBaseContext
{
private:
	map<string, NclDocument*>* baseDocuments;
	map<string, NclDocument*>* visibleDocuments;
	map<string, EmbeddedNclData*>* embeddedDocuments;
	map<string, string>* idToLocation;
	PrivateBase* privateBase;

public:
	PrivateBaseContext();
	virtual ~PrivateBaseContext();

	void createPrivateBase( const std::string &id );

private:
	NclDocument* compileDocument( string location );

public:
	NclDocument* addDocument( const string &location );

	NclDocument* embedDocument( string docId, string nodeId, string location );

	void* addVisibleDocument( const std::string &tempLocation );
	string getDocumentLocation( string docId );
	string getEmbeddedDocumentLocation( string parentDocId, string nodeId );
	NclDocument* getDocument( string id );
	NclDocument* getEmbeddedDocument( string parendDocId, string nodeId );
	vector<NclDocument*>* getDocuments();
	NclDocument* removeDocument( string id );
	NclDocument* removeEmbeddedDocument( string parentDocId, string nodeId );
	void clear();

private:
	NclDocument* getBaseDocument( string documentId );
	void* compileEntity( string location, NclDocument* document, void* parentObject );

public:
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

private:
	Base* getBase( NclDocument* document, string baseId );

public:
	Base* addImportBase( string documentId, string docBaseId, string xmlImportBase );

	Base* removeImportBase( string documentId, string docBaseId, string documentURI );

	NclDocument* addImportedDocumentBase( string documentId, string xmlImportedDocumentBase );

	NclDocument* removeImportedDocumentBase( string documentId, string importedDocumentBaseId );

	NclDocument* addImportNCL( string documentId, string xmlImportNCL );
	NclDocument* removeImportNCL( string documentId, string documentURI );
	Node* addNode( string documentId, string compositeId, string xmlNode );
	InterfacePoint* addInterface( string documentId, string nodeId, string xmlInterface );

	Link* addLink( string documentId, string compositeId, string xmlLink );
	PrivateBase* getPrivateBase();
};
}
}
}
}
}

#endif /*PrivateBaseContext_H_*/
