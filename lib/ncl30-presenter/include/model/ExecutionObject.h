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

#ifndef _EXECUTIONOBJECT_H_
#define _EXECUTIONOBJECT_H_

#include <util/keydefs.h>

#include "ncl30/descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl30/interfaces/LabeledAnchor.h"
#include "ncl30/interfaces/LambdaAnchor.h"
#include "ncl30/interfaces/IntervalAnchor.h"
#include "ncl30/interfaces/PropertyAnchor.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl30/layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl30/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "CascadingDescriptor.h"
#include "FormatterEvent.h"
#include "PresentationEvent.h"
#include "SelectionEvent.h"
#include "AttributionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "BeginEventTransition.h"
#include "EndEventTransition.h"
#include "EventTransition.h"
#include "EventTransitionManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event::transition;

#include "FormatterRegion.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include "NodeNesting.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace components {

/**
 * Esta clase es responsable de manejar la ejecución de los eventos en la presentación.
 */
class ExecutionObject
{
public:
	ExecutionObject( string id, Node* node, bool handling );
	ExecutionObject( string id, Node* node, GenericDescriptor* descriptor, bool handling );
	ExecutionObject( string id, Node* node, CascadingDescriptor* descriptor, bool handling );
	virtual ~ExecutionObject();

	bool instanceOf( string s );
	int compareToUsingId( ExecutionObject* object );
	Node* getDataObject();
	CascadingDescriptor* getDescriptor();
	string getId();
	void* getParentObject(); //CompositeExecutionObject
	void* getParentObject( Node* node ); //CompositeExecutionObject
	void addParentObject( void* parentObject, Node* parentNode );
	void addParentObject( Node* node, void* parentObject, Node* parentNode );
	void setDescriptor( CascadingDescriptor* cascadingDescriptor );
	void setDescriptor( GenericDescriptor* descriptor );
	string toString();
	virtual bool addEvent( FormatterEvent* event );
	void addPresentationEvent( PresentationEvent* event );
	int compareTo( ExecutionObject* object );
	int compareToUsingStartTime( ExecutionObject* object );
	bool containsEvent( FormatterEvent* event );
	FormatterEvent* getEventFromAnchorId( string anchorId );
	FormatterEvent* getEvent( string id );
	vector<FormatterEvent*>* getEvents();
	set<AnchorEvent*>* getSampleEvents();
	double getExpectedStartTime();
	PresentationEvent* getWholeContentPresentationEvent();
	void setStartTime( double t );
	void updateEventDurations();
	void updateEventDuration( PresentationEvent* event );
	bool removeEvent( FormatterEvent* event );
	bool isCompiled();
	void setCompiled( bool status );
	void removeNode( Node* node );
	vector<Node*>* getNodes();
	NodeNesting* getNodePerspective();
	NodeNesting* getNodePerspective( Node* node );
	vector<ExecutionObject*>* getObjectPerspective();
	vector<ExecutionObject*>* getObjectPerspective( Node* node );
	vector<Node*>* getParentNodes();
	FormatterEvent* getMainEvent();
	virtual bool prepare( FormatterEvent* event, double offsetTime );
	bool canStart();
	virtual void start();
	bool updateTransitionTable( double value, short int transType );

	void resetTransitionEvents( short int transType );
	void prepareTransitionEvents( short int transType, double startTime );
	set<double>* getTransitionsValues( short int transType );
	EventTransition* getNextTransition();
	virtual bool stop();
	virtual bool abort();
	virtual bool pause();
	virtual bool resume();
	virtual bool setPropertyValue( AttributionEvent* event, string value );
	virtual bool setProperty( const std::string &name, const std::string &value );

	virtual bool unprepare();

	void setHandling( bool isHandling );
	void setHandler( bool isHandler );
	void selectionEvent(  util::key::type key, double currentTime );

protected:
	map<string, FormatterEvent*>* events;
	vector<PresentationEvent*>* presEvents;
	set<SelectionEvent*>* selectionEvents;
	vector<FormatterEvent*>* otherEvents;
	int pauseCount;
	FormatterEvent* mainEvent;
	EventTransitionManager* transMan;
	map<Node*, void*>* parentTable; //CompositionExecutionObject
	bool visible;
	string id;
	Node* dataObject;
	CascadingDescriptor* descriptor;
	double offsetTime;
	double startTime;
	PresentationEvent* wholeContent;

	set<string> typeSet;
	bool deleting;
	bool isHandler;
	bool isHandling;

private:
	void initializeExecutionObject( string id, Node* node, CascadingDescriptor* descriptor, bool handling );

	map<Node*, Node*>* nodeParentTable;
	bool isItCompiled;
};

}
}
}
}
}
}
}

#endif //_EXECUTIONOBJECT_H_
