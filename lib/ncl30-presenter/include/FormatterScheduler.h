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

#ifndef _FORMATTERSCHEDULER_H_
#define _FORMATTERSCHEDULER_H_

#include "model/ExecutionObject.h"
#include "model/NodeNesting.h"
#include "model/CompositeExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "model/ExecutionObjectSwitch.h"
#include "model/SwitchEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::switches;

#include "model/AttributionEvent.h"
#include "model/IEventListener.h"
#include "model/FormatterEvent.h"
#include "model/PresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "model/LinkAssignmentAction.h"
#include "model/LinkActionListener.h"
#include "model/LinkSimpleAction.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::link;

#include "adapters/RuleAdapter.h"

#include "adapters/PlayerAdapterManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include "ncl30/connectors/SimpleAction.h"
#include "ncl30/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl30/components/CompositeNode.h"
#include "ncl30/components/ContentNode.h"
#include "ncl30/components/Node.h"
#include "ncl30/components/NodeEntity.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl30/interfaces/Port.h"
#include "ncl30/interfaces/Anchor.h"
#include "ncl30/interfaces/ContentAnchor.h"
#include "ncl30/interfaces/PropertyAnchor.h"
#include "ncl30/interfaces/SwitchPort.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl30/reuse/ReferNode.h"
using namespace ::br::pucrio::telemidia::ncl::reuse;

#include "focus/FormatterFocusManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::focus;

#include <iostream>
#include <string>
#include <map>
#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
class FormatterMediator;

/**
 * Realiza el schedule del documento, esto es realizar el play, stop o pause recibiendo llamadas desde @c FormatterMediator.
 */
class FormatterScheduler: public LinkActionListener, public IEventListener
{

private:
	RuleAdapter* ruleAdapter;
	PlayerAdapterManager* playerManager;
	FormatterFocusManager* focusManager;

	void* compiler; //FormatterConverter*
	FormatterMediator* _formatterMediator;
	vector<FormatterEvent*>* documentEvents;
	map<FormatterEvent*, bool>* documentStatus;
	set<string> typeSet;
	player::System *_sys;

public:
	FormatterScheduler( PlayerAdapterManager* playerManager, RuleAdapter* ruleAdapter, void* compiler, FormatterMediator* formatterMediator, player::System *system ); //FormatterConverter
	virtual ~FormatterScheduler();

	listenerType( FormatterScheduler );

	void stopListening( void* event );
	bool setKeyHandler( bool isHandler );
	FormatterFocusManager* getFocusManager();

private:
	bool isDocumentRunning( FormatterEvent* event );

	void setTimeBaseObject( ExecutionObject* object, string nodeId );

public:
	void runAction( void* action );

private:
	void runAction( FormatterEvent* event, void* action );
	void runActionOverProperty( FormatterEvent* event, LinkSimpleAction* action );

	void runActionOverApplicationObject( ApplicationExecutionObject* executionObject, FormatterEvent* event, LinkSimpleAction* action );

	void runActionOverComposition( CompositeExecutionObject* compositeObject, LinkSimpleAction* action );

	void runActionOverSwitch( ExecutionObjectSwitch* switchObject, SwitchEvent* event, LinkSimpleAction* action );

	void runSwitchEvent( ExecutionObjectSwitch* switchObject, SwitchEvent* switchEvent, ExecutionObject* selectedObject, LinkSimpleAction* action );

	string solveImplicitRefAssessment( string propValue, LinkAssignmentAction* action, AttributionEvent* event );

public:
	void startEvent( FormatterEvent* event );
	void stopEvent( FormatterEvent* event );
	void pauseEvent( FormatterEvent* event );
	void resumeEvent( FormatterEvent* event );

private:
	void initializeDocumentSettings( Node* node );

public:
	void startDocument( FormatterEvent* documentEvent, vector<FormatterEvent*>* entryEvents );

private:
	void removeDocument( FormatterEvent* documentEvent );

public:
	void stopDocument( FormatterEvent* documentEvent );
	void pauseDocument( FormatterEvent* documentEvent );
	void resumeDocument( FormatterEvent* documentEvent );
	void stopAllDocuments();
	void pauseAllDocuments();
	void resumeAllDocuments();
	void eventStateChanged( void* someEvent, short transition, short previousState );

	short getPriorityType();

};
}
}
}
}
}

#endif //_FORMATTERSCHEDULER_H_
