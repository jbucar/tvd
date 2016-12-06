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

#ifndef FORMATTERCONVERTER_H_
#define FORMATTERCONVERTER_H_

#include "ncl30/switches/SwitchNode.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "ncl30/interfaces/PropertyAnchor.h"
#include "ncl30/interfaces/ContentAnchor.h"
#include "ncl30/interfaces/LambdaAnchor.h"
#include "ncl30/interfaces/SwitchPort.h"
#include "ncl30/interfaces/Port.h"
#include "ncl30/interfaces/InterfacePoint.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl30/components/CompositeNode.h"
#include "ncl30/components/ContextNode.h"
#include "ncl30/components/ContentNode.h"
#include "ncl30/components/Node.h"
#include "ncl30/components/NodeEntity.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl30/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl30/descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl30/link/Bind.h"
#include "ncl30/link/CausalLink.h"
#include "ncl30/link/Link.h"
using namespace ::br::pucrio::telemidia::ncl::link;

#include "ncl30/reuse/ReferNode.h"
using namespace ::br::pucrio::telemidia::ncl::reuse;

#include "../model/ExecutionObjectSwitch.h"
#include "../model/SwitchEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::switches;

#include "../model/AttributionEvent.h"
#include "../model/PresentationEvent.h"
#include "../model/SelectionEvent.h"
#include "../model/IEventListener.h"
#include "../model/FormatterEvent.h"
#include "../model/PresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "../model/FormatterCausalLink.h"
#include "../model/FormatterLink.h"
#include "../model/LinkAction.h"
#include "../model/LinkCompoundAction.h"
#include "../model/LinkSimpleAction.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::link;

#include "../model/CascadingDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include "../model/CompositeExecutionObject.h"
#include "../model/ApplicationExecutionObject.h"
#include "../model/ExecutionObject.h"
#include "../model/NodeNesting.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "../adapters/RuleAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adaptation::context;

#include "../FormatterScheduler.h"

#include <map>
#include <vector>
#include <set>
#include <string>
using namespace std;

namespace player {
class System;
}

namespace ncl30presenter {
namespace event {
class EventManager;
}
}

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace emconverter {
  
/**
 * Esta clase interactua con @c ExecutionObject asi como también realiza manejo de eventos.
 */
class FormatterConverter: public IEventListener
{
private:
	int depthLevel;
	map<string, ExecutionObject*>* executionObjects;
	set<ExecutionObject*>* settingObjects;
	void* linkCompiler; //FormatterLinkConverter*
	FormatterScheduler* scheduler;
	ncl30presenter::event::EventManager *_eventManager;
	RuleAdapter* ruleAdapter;
	bool handling;
	std::set<FormatterEvent*> _events;

public:
	FormatterConverter( RuleAdapter* ruleAdapter, player::System *system );
	virtual ~FormatterConverter();

	listenerType( FormatterConverter );

	void stopListening( void* event );
	set<ExecutionObject*>* getRunningObjects();
	void setHandlingStatus( bool hanling );
	ExecutionObject* getObjectFromNodeId( string id );

	void setScheduler( FormatterScheduler* scheduler );
	void setDepthLevel( int level );
	int getDepthLevel();
	void checkGradSameInstance( set<ReferNode*>* gradSame, ExecutionObject* object );

	CompositeExecutionObject* addSameInstance( ExecutionObject* executionObject, ReferNode* referNode );

private:
	void addExecutionObject( ExecutionObject* executionObject, CompositeExecutionObject* parentObject, int depthLevel );

public:
	void compileExecutionObjectLinks( ExecutionObject* executionObject, int depthLevel );

	ExecutionObject* getExecutionObject( NodeNesting* perspective, GenericDescriptor* descriptor, int depthLevel );

	set<ExecutionObject*>* getSettingNodeObjects();

private:
	CompositeExecutionObject* getParentExecutionObject( NodeNesting* perspective, int depthLevel );

public:
	FormatterEvent* getEvent( ExecutionObject* executionObject, InterfacePoint* interfacePoint, int ncmEventType, string key );

private:
	void createMultichannelObject( CompositeExecutionObject* compositeObject, int depthLevel );

	ExecutionObject* createExecutionObject( string id, NodeNesting* perspective, CascadingDescriptor* descriptor, int depthLevel );

	bool isEmbeddedApp( NodeEntity* dataObject );

public:
	static CascadingDescriptor* getCascadingDescriptor( NodeNesting* nodePerspective, GenericDescriptor* descriptor );

	void compileExecutionObjectLinks( ExecutionObject* executionObject, Node* dataObject, CompositeExecutionObject* parentObject, int depthLevel );

private:
	void setActionListener( LinkAction* action );

public:
	ExecutionObject* processExecutionObjectSwitch( ExecutionObjectSwitch* switchObject );

private:
	void resolveSwitchEvents( ExecutionObjectSwitch* switchObject, int depthLevel );

	FormatterEvent* insertNode( NodeNesting* perspective, InterfacePoint* interfacePoint, GenericDescriptor* descriptor );

public:
	FormatterEvent* insertContext( NodeNesting* contextPerspective, Port* port );

	bool removeExecutionObject( ExecutionObject* executionObject, ReferNode* referNode );

	bool removeExecutionObject( ExecutionObject* executionObject );
	ExecutionObject* hasExecutionObject( Node* node, GenericDescriptor* descriptor );

	FormatterCausalLink* addCausalLink( ContextNode* context, CausalLink* link );

	void eventStateChanged( void* someEvent, short transition, short previousState );

	short getPriorityType();
	void reset();

};
}
}
}
}
}
}

#endif /*FORMATTERCONVERTER_H_*/
