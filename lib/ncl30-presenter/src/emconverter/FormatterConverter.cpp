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

#include "../../include/emconverter/FormatterConverter.h"
#include "../../include/emconverter/FormatterLinkConverter.h"
#include "../model/event/eventmanager.h"
#include <gingaplayer/types.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/lexical_cast.hpp>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace emconverter {
FormatterConverter::FormatterConverter( RuleAdapter* ruleAdapter, player::System *system ) {
	executionObjects = new map<string, ExecutionObject*>;
	settingObjects = new set<ExecutionObject*>;
	linkCompiler = (void*) (new FormatterLinkConverter( this, system ));
	_eventManager = new ncl30presenter::event::EventManager();
	this->scheduler = NULL;
	this->ruleAdapter = ruleAdapter;
	this->depthLevel = 1; //Formatter::DEEPEST_LEVEL;
	this->handling = false;
}

FormatterConverter::~FormatterConverter() {
	ruleAdapter = NULL;
	scheduler = NULL;

	delete _eventManager;
	_eventManager = NULL;

	if (executionObjects != NULL) {
		delete executionObjects;
		executionObjects = NULL;
	}

	if (settingObjects != NULL) {
		delete settingObjects;
		settingObjects = NULL;
	}

	if (linkCompiler != NULL) {
		delete (FormatterLinkConverter*) linkCompiler;
		linkCompiler = NULL;
	}

	CLEAN_ALL( FormatterEvent *, _events );
}

void FormatterConverter::stopListening( void* /*event*/ ) {

}

set<ExecutionObject*>* FormatterConverter::getRunningObjects() {
	map<string, ExecutionObject*>::iterator i;
	set<ExecutionObject*>* objects;
	ExecutionObject* object;
	FormatterEvent* ev;

	objects = new set<ExecutionObject*>;
	i = executionObjects->begin();
	while (i != executionObjects->end()) {
		object = i->second;
		ev = object->getMainEvent();
		if (ev != NULL && ev->getCurrentState() != EventUtil::ST_SLEEPING) {
			objects->insert( object );
		}
		++i;
	}
	return objects;
}

void FormatterConverter::setHandlingStatus( bool handling ) {
	ExecutionObject* object;
	map<string, ExecutionObject*>::iterator i;

	this->handling = handling;

	i = executionObjects->begin();
	while (i != executionObjects->end()) {
		object = i->second;
		object->setHandling( handling );
		++i;
	}
}

ExecutionObject* FormatterConverter::getObjectFromNodeId( string id ) {
	NodeEntity* dataObject;
	ExecutionObject* expectedObject;
	map<string, ExecutionObject*>::iterator i;

	i = executionObjects->begin();
	while (i != executionObjects->end()) {
		expectedObject = i->second;
		dataObject = (NodeEntity*) (expectedObject->getDataObject()->getDataEntity());

		if (dataObject->getId() == id) {
			return expectedObject;
		}
		++i;
	}

	return NULL;
}

void FormatterConverter::setScheduler( FormatterScheduler* scheduler ) {
	this->scheduler = scheduler;
}

void FormatterConverter::setDepthLevel( int level ) {
	depthLevel = level;
}

int FormatterConverter::getDepthLevel() {
	return depthLevel;
}

CompositeExecutionObject* FormatterConverter::addSameInstance( ExecutionObject* executionObject, ReferNode* referNode ) {

	NodeNesting* referPerspective;
	CompositeExecutionObject* referParentObject;
	CascadingDescriptor* desc;
	string objectId;

	referPerspective = new NodeNesting( referNode->getPerspective() );

	referParentObject = getParentExecutionObject( referPerspective, depthLevel );

	if (referParentObject != NULL) {
		executionObject->addParentObject( referNode, referParentObject, referPerspective->getNode( referPerspective->getNumNodes() - 2 ) );

		// A new entry for the execution object is inserted using
		// the refer node id. As a consequence, links referring to the
		// refer node will generate events in the execution object.
		desc = executionObject->getDescriptor();
		if (desc != NULL) {
			objectId = (referPerspective->getId() + "/" + executionObject->getDescriptor()->getId());

		} else {
			objectId = referPerspective->getId();
		}

		(*executionObjects)[objectId] = executionObject;
	}

	return referParentObject;
}

void FormatterConverter::checkGradSameInstance( set<ReferNode*>* /*gradSame*/, ExecutionObject* /*object*/ ) {
/*
	NodeNesting* entityPerspective;
	NodeNesting* perspective;
	NodeNesting* referPerspective;
	set<ReferNode*>::iterator i;
	NodeEntity* dataObject;

	dataObject = (NodeEntity*) (object->getDataObject());
	perspective = object->getNodePerspective();
	entityPerspective = new NodeNesting( dataObject->getPerspective() );
	i = gradSame->begin();
	while (i != gradSame->end()) {
		referPerspective = new NodeNesting( (*i)->getPerspective() );

		++i;
	}
*/
}

void FormatterConverter::addExecutionObject( ExecutionObject* executionObject, CompositeExecutionObject* parentObject, int depthLevel ) {

	NodeEntity* dataObject;
	set<ReferNode*>* sameInstances;
	set<ReferNode*>::iterator i;
	ReferNode* referNode;
	Node* headNode;
	CascadingDescriptor* descriptor;
	Entity* entity;

	vector<Node*>::iterator j;

	(*executionObjects)[executionObject->getId()] = executionObject;

	dataObject = (NodeEntity*) (executionObject->getDataObject());


	if (dataObject->instanceOf( "ContentNode" ) && ((ContentNode*) dataObject)->isSettingNode()) {

		settingObjects->insert( executionObject );
	}

	if (dataObject->instanceOf( "ReferNode" )) {
		if (((ReferNode*) dataObject)->getInstanceType() == "instSame") {
			entity = ((ReferNode*) dataObject)->getDataEntity();
			if (entity->instanceOf( "ContentNode" ) && ((ContentNode*) entity)->isSettingNode()) {

				settingObjects->insert( executionObject );
			}
		}
	}

	if (parentObject != NULL) {
		parentObject->addExecutionObject( executionObject );
	}

	if (dataObject->instanceOf( "NodeEntity" )) {
		sameInstances = dataObject->getInstSameInstances();
		if (sameInstances != NULL) {
			i = sameInstances->begin();
			while (i != sameInstances->end()) {
				referNode = (ReferNode*) (*i);
				headNode = executionObject->getNodePerspective()->getHeadNode();

				if (headNode->instanceOf( "CompositeNode" )) {
					if (((CompositeNode*) headNode)->recursivelyContainsNode( referNode )) {

						addSameInstance( executionObject, referNode );
					}
				}
				++i;
			}
		}
	}

	descriptor = executionObject->getDescriptor();
	if (descriptor != NULL) {
		descriptor->setFormatterLayout();
	}

	if (descriptor != NULL && descriptor->getPlayerName() != ""
	        && ((descriptor->getPlayerName() == "VideoChannelPlayerAdapter") || (descriptor->getPlayerName() == "AudioChannelPlayerAdapter")
	                || (descriptor->getPlayerName() == "JmfVideoChannelPlayerAdapter") || (descriptor->getPlayerName() == "JmfAudioChannelPlayerAdapter")
	                || (descriptor->getPlayerName() == "QtVideoChannelPlayerAdapter") || (descriptor->getPlayerName() == "QtAudioChannelPlayerAdapter"))) {

		createMultichannelObject( (CompositeExecutionObject*) executionObject, depthLevel );
	}

	if (depthLevel != 0) {
		if (depthLevel > 0) {
			depthLevel--;
		}
		compileExecutionObjectLinks( executionObject, depthLevel );

	} else if (executionObject->instanceOf( "CompositeExecutionObject" )) {
		compileExecutionObjectLinks( executionObject, depthLevel );
	}
}

void FormatterConverter::compileExecutionObjectLinks( ExecutionObject* executionObject, int depthLevel ) {

	vector<Node*>* nodes;
	vector<Node*>::iterator i;
	Node* node;

	nodes = executionObject->getNodes();
	if (nodes != NULL) {
		i = nodes->begin();
		while (i != nodes->end()) {
			node = (Node*) (*i);
			compileExecutionObjectLinks( executionObject, node, (CompositeExecutionObject*) (executionObject->getParentObject( node )), 
			depthLevel);

			++i;
		}
		delete nodes;
		nodes = NULL;
	}
}

ExecutionObject* FormatterConverter::getExecutionObject( NodeNesting* perspective, GenericDescriptor* descriptor, int depthLevel ) {

	map<string, ExecutionObject*>::iterator i;
	CascadingDescriptor* cascadingDescriptor = NULL;
	string id;
	CompositeExecutionObject* parentObject;
	ExecutionObject* executionObject;

	id = perspective->getId() + "/";
	cascadingDescriptor = getCascadingDescriptor( perspective, descriptor );
	if (cascadingDescriptor != NULL) {
		id = id + cascadingDescriptor->getId();
	}

	i = executionObjects->find( id );
	if (i != executionObjects->end()) {
		if (cascadingDescriptor != NULL) {
			delete cascadingDescriptor;
			cascadingDescriptor = NULL;
		}
		executionObject = i->second;
		return executionObject;
	}

	parentObject = getParentExecutionObject( perspective, depthLevel );

	executionObject = createExecutionObject( id, perspective, cascadingDescriptor, depthLevel );

	if (executionObject == NULL) {
		if (cascadingDescriptor != NULL) {
			delete cascadingDescriptor;
			cascadingDescriptor = NULL;
		}
		return NULL;
	}

	addExecutionObject( executionObject, parentObject, depthLevel );

	return executionObject;
}

set<ExecutionObject*>* FormatterConverter::getSettingNodeObjects() {
	return new set<ExecutionObject*>( *settingObjects );
}

CompositeExecutionObject* FormatterConverter::getParentExecutionObject( NodeNesting* perspective, int depthLevel ) {

	NodeNesting* parentPerspective;
	CompositeExecutionObject* cObj;

	if (perspective->getNumNodes() > 1) {
		parentPerspective = perspective->copy();
		parentPerspective->removeAnchorNode();
		cObj = (CompositeExecutionObject*) (this->getExecutionObject( parentPerspective, NULL, depthLevel ));

		delete parentPerspective;
		return cObj;

	} else {
		return NULL;
	}
}

FormatterEvent* FormatterConverter::getEvent( ExecutionObject* executionObject, InterfacePoint* interfacePoint, int ncmEventType, string key ) {
	string id;

	if (key == "") {
		id = interfacePoint->getId() + "_" + boost::lexical_cast<string>( ncmEventType );

	} else {
		id = interfacePoint->getId() + "_" + boost::lexical_cast<string>( ncmEventType ) + "_" + key;
	}

	FormatterEvent *event = executionObject->getEvent( id );
	if (event != NULL) {
		return event;
	}

	if (executionObject->instanceOf( "ExecutionObjectSwitch" )) {
		event = new SwitchEvent( id, (ExecutionObjectSwitch*) executionObject, interfacePoint, ncmEventType, key, _eventManager );

	} else {
		if (ncmEventType == EventUtil::EVT_PRESENTATION) {
			event = new PresentationEvent( id, executionObject, (ContentAnchor*) interfacePoint, _eventManager );

		} else {
			if (executionObject->instanceOf( "CompositeExecutionObject" )) {
				// TODO: eventos internos da composicao.
				// Estao sendo tratados nos elos.
				if (ncmEventType == EventUtil::EVT_ATTRIBUTION) {
					if (interfacePoint->instanceOf( "PropertyAnchor" )) {
						event = new AttributionEvent( id, executionObject, (PropertyAnchor*) interfacePoint, _eventManager );

					} else {
						LWARN("FormatterConverter","getEvent, NCM event type is attribution, but interface point isn't");
						event = new AttributionEvent( id, executionObject, NULL, _eventManager );
					}
				}

			} else {
				switch ( ncmEventType ) {
					case EventUtil::EVT_ATTRIBUTION:
						if (interfacePoint->instanceOf( "PropertyAnchor" )) {
							event = new AttributionEvent( id, executionObject, (PropertyAnchor*) interfacePoint, _eventManager );

						} else {
							LWARN("FormatterConverter","getEvent, NCM event type is attribution, but interface point isn't");


							if (interfacePoint->instanceOf( "IntervalAnchor" )) {
								LWARN("FormatterConverter","getEvent, it was supposed to be a PRESENTATION EVENT");
								//TODO: find the correct way to solve this
								// event = new PresentationEvent( id, executionObject, (ContentAnchor*) interfacePoint, _eventManager );
							}

							return NULL;
						}
						break;

					case EventUtil::EVT_SELECTION:
						event = new SelectionEvent( id, executionObject, (ContentAnchor*) interfacePoint, _eventManager );
						if (key != "") {
							((SelectionEvent*) event)->setSelectionCode( key );
						}
						break;
				}
			}
		}
	}

	if (event != NULL) {
		executionObject->addEvent( event );
	}

	return event;
}

void FormatterConverter::createMultichannelObject( CompositeExecutionObject* compositeObject, int depthLevel ) {

	CompositeNode* compositeNode;
	vector<Node*>* nodes;
	Node* node;
	NodeNesting* perspective;
	string id;
	CascadingDescriptor* cascadingDescriptor;
	ExecutionObject* childObject;

	compositeNode = (CompositeNode*) compositeObject->getDataObject();
	nodes = compositeNode->getNodes();
	if (nodes != NULL) {
		vector<Node*>::iterator i;
		i = nodes->begin();
		while (i != nodes->end()) {
			node = *i;
			perspective = new NodeNesting( compositeObject->getNodePerspective() );

			perspective->insertAnchorNode( node );

			id = perspective->getId() + "/";
			cascadingDescriptor = getCascadingDescriptor( perspective, NULL );

			if (cascadingDescriptor != NULL) {
				id += cascadingDescriptor->getId();
			}

			childObject = createExecutionObject( id, perspective, cascadingDescriptor, depthLevel );

			if (childObject != NULL) {
				getEvent( childObject, ((NodeEntity*) node->getDataEntity())->getLambdaAnchor(), EventUtil::EVT_PRESENTATION, "" );

				addExecutionObject( childObject, compositeObject, depthLevel );
			}

			++i;
		}
	}
}

bool FormatterConverter::isEmbeddedApp( NodeEntity* dataObject ) {
	string mediaType = "";
	Descriptor* descriptor;

	//first, descriptor
	descriptor = dynamic_cast<Descriptor*>( dataObject->getDescriptor());

if(	descriptor != NULL) {
		mediaType = descriptor->getPlayerName();
		if (!mediaType.empty()) {
			if (
			mediaType == "LuaPlayerAdapter" ||
			mediaType == "LinksPlayerAdapter" ||
			mediaType == "NCLPlayerAdapter")
			{
				return true;
			}
		}
	}

	//second, media type
	if (dataObject->instanceOf( "ContentNode" )) {
		std::string mime = ((ContentNode*) dataObject)->getNodeType();
		std::string url = ((ContentNode*) dataObject)->getUrl();
		player::schema::type schema;
		std::string body;
		player::url::parse( url, schema, body );		
		player::type::type type = player::type::get( schema, body, mime );
		return (type == player::type::ncl || type == player::type::lua);
	}

	return false;
}

ExecutionObject* FormatterConverter::createExecutionObject( string id, NodeNesting* perspective, CascadingDescriptor* descriptor, int depthLevel ) {
	ExecutionObject* executionObject = NULL;
	NodeEntity *nodeEntity = (NodeEntity*) (perspective->getAnchorNode()->getDataEntity());

	// solve execution object cross reference coming from refer nodes with
	// new instance = false
	if (nodeEntity->instanceOf( "ContentNode" ) && ((ContentNode*) nodeEntity)->getNodeType() != "" && !((ContentNode*) nodeEntity)->isSettingNode()) {
		Node* node = perspective->getAnchorNode();
		if (node->instanceOf( "ReferNode" ) && ((ReferNode*) node)->getInstanceType() != "new") {
			NodeNesting *nodePerspective = new NodeNesting( nodeEntity->getPerspective() );
			// verify if both nodes are in the same base.
			if (nodePerspective->getHeadNode() == perspective->getHeadNode()) {
				executionObject = getExecutionObject( nodePerspective, NULL, depthLevel );
			} else {
				// not in the same base => create a new version
				if (isEmbeddedApp( nodeEntity )) {
					executionObject = new ApplicationExecutionObject( id, nodeEntity, descriptor, handling );
				} else {
					executionObject = new ExecutionObject( id, nodeEntity, descriptor, handling );
				}
				//TODO informa a substituicao
			}
			if (executionObject != NULL) {
				return executionObject;
			}
		}
	}

	PresentationEvent* compositeEvent = NULL;
	if (nodeEntity->instanceOf( "SwitchNode" )) {
		executionObject = new ExecutionObjectSwitch( id, perspective->getAnchorNode(), handling );
		int i = EventUtil::EVT_PRESENTATION;
		compositeEvent = new PresentationEvent( nodeEntity->getLambdaAnchor()->getId() + "_" + boost::lexical_cast<string>( i ), executionObject, (ContentAnchor*) (nodeEntity->getLambdaAnchor()), _eventManager );
		executionObject->addEvent( compositeEvent );
		// to monitor the switch presentation and clear the selection after
		// each execution
		compositeEvent->addEventListener( this );
	} else if (nodeEntity->instanceOf( "CompositeNode" )) {
		executionObject = new CompositeExecutionObject( id, perspective->getAnchorNode(), descriptor, handling );
		int i = EventUtil::EVT_PRESENTATION;
		compositeEvent = new PresentationEvent( nodeEntity->getLambdaAnchor()->getId() + "_" + boost::lexical_cast<string>( i ), executionObject, (ContentAnchor*) (nodeEntity->getLambdaAnchor()), _eventManager );
		executionObject->addEvent( compositeEvent );
		// to monitor the presentation and remove object at stops
		//compositeEvent->addEventListener(this);
	} else if (isEmbeddedApp( nodeEntity )) {
		executionObject = new ApplicationExecutionObject( id, perspective->getAnchorNode(), descriptor, handling );
	} else {
		executionObject = new ExecutionObject( id, perspective->getAnchorNode(), descriptor, handling );
	}

	if(compositeEvent != NULL) {
		_events.insert( compositeEvent );
	}
	return executionObject;
}

CascadingDescriptor* FormatterConverter::getCascadingDescriptor( NodeNesting* nodePerspective, GenericDescriptor* descriptor ) {

	CascadingDescriptor* cascadingDescriptor = NULL;
	NodeEntity* node;
	ContextNode* context;
	int size;

	node = (NodeEntity*) (nodePerspective->getAnchorNode()->getDataEntity());

	//	there is a node descriptor?
	if (node->getDescriptor() != NULL) {
		cascadingDescriptor = new CascadingDescriptor( node->getDescriptor() );
	}

	// there is a node descriptor defined in the node context?
	size = nodePerspective->getNumNodes();
	if (size > 1 && nodePerspective->getNode( size - 2 ) != NULL && nodePerspective->getNode( size - 2 )->instanceOf( "ContextNode" )) {

		context = (ContextNode*) (nodePerspective->getNode( size - 2 )->getDataEntity());

		if (context->getNodeDescriptor( node ) != NULL) {
			if (cascadingDescriptor == NULL) {
				cascadingDescriptor = new CascadingDescriptor( context->getNodeDescriptor( node ) );

			} else {
				cascadingDescriptor->cascade( context->getNodeDescriptor( node ) );
			}
		}
	}

	// there is an explicit descriptor (user descriptor)?
	if (descriptor != NULL) {
		if (cascadingDescriptor == NULL) {
			cascadingDescriptor = new CascadingDescriptor( descriptor );
		} else {
			cascadingDescriptor->cascade( descriptor );
		}
	}

	return cascadingDescriptor;
}

void FormatterConverter::compileExecutionObjectLinks( ExecutionObject* executionObject, Node* dataObject, CompositeExecutionObject* parentObject, int depthLevel ) {

	set<Link*>* dataLinks;
	set<Link*>* uncompiledLinks;
	set<Link*>::iterator i;
	set<ReferNode*>* sameInstances;
	set<ReferNode*>::iterator j;
	Link* ncmLink;
	vector<GenericDescriptor*>* descriptors;
	GenericDescriptor* descriptor;
	FormatterLink* formatterLink;
	bool contains;
	NodeEntity* nodeEntity = NULL;

	executionObject->setCompiled( true );
	if (parentObject == NULL) {
		return;
	}

	if (executionObject->getDataObject() != NULL && executionObject->getDataObject()->instanceOf( "NodeEntity" )) {

		nodeEntity = (NodeEntity*) (executionObject->getDataObject());
	}

	uncompiledLinks = parentObject->getUncompiledLinks();
	if (uncompiledLinks != NULL) {
		dataLinks = new set<Link*>( *uncompiledLinks );

		i = dataLinks->begin();
		while (i != dataLinks->end()) {
			contains = false;
			ncmLink = *i;

			LDEBUG("FormatterConverter","compileExecutionObjectLinks, processing ncmLink '%s' inside '%s'",ncmLink->getId().c_str(),parentObject->getId().c_str());

			// since the link may be removed in a deepest compilation its
			// necessary to certify that the link was not compiled
			if (parentObject->containsUncompiledLink( ncmLink )) {
				descriptor = NULL;
				if (executionObject->getDescriptor() != NULL) {
					descriptors = executionObject->getDescriptor()->getNcmDescriptors();

					if (descriptors != NULL && !(descriptors->empty())) {
						descriptor = (*descriptors)[descriptors->size() - 1];
					}
				}

				if (ncmLink->instanceOf( "CausalLink" )) {

					if (nodeEntity != NULL) {
						sameInstances = nodeEntity->getInstSameInstances();
						if (sameInstances != NULL) {
							j = sameInstances->begin();
							while (j != sameInstances->end()) {
								contains = ((CausalLink*) ncmLink)->containsSourceNode( *j, descriptor );

								if (contains) {
									break;
								}
								++j;
							}
						}

						if (!contains) {
							sameInstances = nodeEntity->getGradSameInstances();

							if (sameInstances != NULL) {
								j = sameInstances->begin();
								while (j != sameInstances->end()) {
									contains = ((CausalLink*) ncmLink)->containsSourceNode( *j, descriptor );

									if (contains) {
										break;
									}
									++j;
								}
							}
						}
					}

					// verify if execution object is part of
					// link conditions
					if (((CausalLink*) ncmLink)->containsSourceNode( dataObject, descriptor ) || contains) {

						// compile causal link
						parentObject->removeLinkUncompiled( ncmLink );
						formatterLink = ((FormatterLinkConverter*) linkCompiler)->createCausalLink( (CausalLink*) ncmLink, parentObject, depthLevel );

						if (formatterLink != NULL) {
							setActionListener( ((FormatterCausalLink*) formatterLink)->getAction() );

							parentObject->setLinkCompiled( formatterLink );
						}
					} else {
						LDEBUG("FormatterConverter","compileExecutionObjectLinks, can't process ncmLink '%s' inside '%s'  because ncmLink does not contains '%s' src",ncmLink->getId().c_str(), parentObject->getId().c_str(),dataObject->getId().c_str());
					}
				} else {
					LDEBUG("FormatterConverter","compileExecutionObjectLinks, can't process ncmLink '%s' inside '%s'    because isn't a causal link",ncmLink->getId().c_str(), parentObject->getId().c_str());
				}
			} else {
				 LDEBUG("FormatterConverter","compileExecutionObjectLinks, can't process ncmLink '%s' inside '%s' link may be removed in a deepest compilation",ncmLink->getId().c_str(), parentObject->getId().c_str());
			}
			++i;
		}

		delete dataLinks;
		dataLinks = NULL;

		compileExecutionObjectLinks( executionObject, dataObject, (CompositeExecutionObject*) (parentObject->getParentObject()), 
		depthLevel);
	}
}

void FormatterConverter::setActionListener( LinkAction* action ) {
	if (action->instanceOf( "LinkSimpleAction" )) {
		((LinkSimpleAction*) action)->setActionListener( (FormatterScheduler*) scheduler );

	} else {
		vector<LinkAction*>* actions;
		vector<LinkAction*>::iterator i;
		actions = ((LinkCompoundAction*) action)->getActions();
		if (actions != NULL) {
			i = actions->begin();
			while (i != actions->end()) {
				setActionListener( (LinkAction*) (*i) );
				++i;
			}
			delete actions;
		}
	}
}

ExecutionObject* FormatterConverter::processExecutionObjectSwitch( ExecutionObjectSwitch* switchObject ) {

	SwitchNode* switchNode;
	Node* selectedNode;
	NodeNesting* selectedPerspective;
	string id;
	CascadingDescriptor* descriptor;
	map<string, ExecutionObject*>::iterator i;
	ExecutionObject* selectedObject;

	switchNode = (SwitchNode*) (switchObject->getDataObject()->getDataEntity());

	LDEBUG("FormatterConverter","processExecutionObjectSwitch, '%s' call adaptSwitch",switchObject->getId().c_str());
	selectedNode = ruleAdapter->adaptSwitch( switchNode );
	if (selectedNode == NULL) {
		return NULL;
	}

	selectedPerspective = switchObject->getNodePerspective();
	selectedPerspective->insertAnchorNode( selectedNode );

	id = selectedPerspective->getId() + "/";

	descriptor = FormatterConverter::getCascadingDescriptor( selectedPerspective, NULL );

	if (descriptor != NULL) {
		id += descriptor->getId();
	}

	i = executionObjects->find( id );
	if (i != executionObjects->end()) {
		selectedObject = i->second;
		switchObject->select( selectedObject );
		resolveSwitchEvents( switchObject, depthLevel );
		if (descriptor != NULL) {
			delete descriptor;
			descriptor = NULL;
		}
		return selectedObject;
	}

	selectedObject = createExecutionObject( id, selectedPerspective, descriptor, depthLevel );

	if (selectedObject == NULL) {
		if (descriptor != NULL) {
			delete descriptor;
			descriptor = NULL;
		}
		return NULL;
	}

	addExecutionObject( selectedObject, switchObject, depthLevel );
	switchObject->select( selectedObject );
	resolveSwitchEvents( switchObject, depthLevel );
	return selectedObject;
}

void FormatterConverter::resolveSwitchEvents( ExecutionObjectSwitch* switchObject, int depthLevel ) {

	ExecutionObject* selectedObject;
	ExecutionObject* endPointObject;
	Node* selectedNode;
	NodeEntity* selectedNodeEntity;
	vector<FormatterEvent*>* events;
	vector<FormatterEvent*>::iterator i;
	SwitchEvent* switchEvent;
	InterfacePoint* interfacePoint;
	SwitchPort* switchPort;
	vector<Node*>* nestedSeq;
	vector<Port*>* mappings;
	vector<Port*>::iterator j;
	Port* mapping;
	NodeNesting* nodePerspective;
	FormatterEvent* mappedEvent;

	LDEBUG("FormatterConverter","resolveSwitchEvents, for '%s'",switchObject->getId().c_str());

	selectedObject = switchObject->getSelectedObject();
	if (selectedObject == NULL) {
		LWARN("FormatterConverter","resolveSwitchEvents, selected object is NULL");
		return;
	}

	selectedNode = selectedObject->getDataObject();
	selectedNodeEntity = (NodeEntity*) (selectedNode->getDataEntity());
	events = switchObject->getEvents();
	if (events != NULL && !events->empty()) {
		i = events->begin();
		while (i != events->end()) {
			mappedEvent = NULL;
			switchEvent = (SwitchEvent*) (*i);
			interfacePoint = switchEvent->getInterfacePoint();
			if (interfacePoint->instanceOf( "LambdaAnchor" )) {
				mappedEvent = getEvent( selectedObject, selectedNodeEntity->getLambdaAnchor(), switchEvent->getEventType(), switchEvent->getKey() );

			} else {
				switchPort = (SwitchPort*) interfacePoint;
				mappings = ((SwitchPort*) switchPort)->getPorts();
				if (mappings != NULL && !mappings->empty()) {
					j = mappings->begin();
					while (j != mappings->end()) {
						mapping = (*j);
						if (mapping->getNode() == selectedNode) {
							nodePerspective = switchObject->getNodePerspective();

							nestedSeq = mapping->getMapNodeNesting();
							nodePerspective->append( nestedSeq );
							delete nestedSeq;

							endPointObject = getExecutionObject( nodePerspective, NULL, depthLevel );

							if (endPointObject != NULL) {
								mappedEvent = getEvent( endPointObject, mapping->getEndInterfacePoint(), switchEvent->getEventType(), switchEvent->getKey() );
							}

							break;
						}
						++j;
					}

				} else {
					LDEBUG("FormatterConverter","resolveSwitchEvents, there is no mapped events ");
				}
			}

			if (mappedEvent != NULL) {
				switchEvent->setMappedEvent( mappedEvent );
				LDEBUG("FormatterConverter","resolveSwitchEvents, setting '%s' as mapped event of '%s'",mappedEvent->getId().c_str(),switchEvent->getId().c_str());

			} else {
				LWARN("FormatterConverter","resolveSwitchEvents, can't set a mapped event for '%s'",switchEvent->getId().c_str());
			}

			++i;
		}
		delete events;
		events = NULL;

	} else {
		LWARN("FormatterConverter","resolveSwitchEvents, can't find events");
	}
}

FormatterEvent* FormatterConverter::insertNode( NodeNesting* perspective, InterfacePoint* interfacePoint, GenericDescriptor* descriptor ) {

	ExecutionObject* executionObject;
	FormatterEvent* event;

	event = NULL;
	executionObject = getExecutionObject( perspective, descriptor, depthLevel );

	if (executionObject != NULL) {
		// get the event corresponding to the node anchor
		event = getEvent( executionObject, interfacePoint, EventUtil::EVT_PRESENTATION, "" );
	}

	return event;
}

FormatterEvent* FormatterConverter::insertContext( NodeNesting* contextPerspective, Port* port ) {

	vector<Node*>* nestedSeq;
	NodeNesting* perspective;
	FormatterEvent* newEvent;

	if (contextPerspective == NULL || port == NULL || !(port->getEndInterfacePoint()->instanceOf( "ContentAnchor" ) || port->getEndInterfacePoint()->instanceOf( "SwitchPort" ))
	        || !(contextPerspective->getAnchorNode()->getDataEntity()->instanceOf( "ContextNode" ))) {

		return NULL;
	}

	nestedSeq = port->getMapNodeNesting();
	perspective = new NodeNesting( contextPerspective );
	perspective->append( nestedSeq );
	delete nestedSeq;

	newEvent = insertNode( perspective, port->getEndInterfacePoint(), NULL );
	delete perspective;

	return newEvent;
}

bool FormatterConverter::removeExecutionObject( ExecutionObject* executionObject, ReferNode* referNode ) {

	NodeNesting* referPerspective;
	map<string, ExecutionObject*>::iterator i;
	set<ExecutionObject*>::iterator j;

	if (executionObject == NULL || referNode == NULL) {
		return false;
	}

	executionObject->removeNode( referNode );
	referPerspective = new NodeNesting( referNode->getPerspective() );
	string objectId;
	objectId = referPerspective->getId() + "/" + executionObject->getDescriptor()->getId();

	i = executionObjects->find( objectId );
	if (i != executionObjects->end()) {
		executionObjects->erase( i );
	}

	j = settingObjects->find( executionObject );
	if (j != settingObjects->end()) {
		settingObjects->erase( j );
	}
	//TODO: problema se esse era a base para outros objetos
	return true;
}

bool FormatterConverter::removeExecutionObject( ExecutionObject* executionObject ) {

	CompositeExecutionObject* compositeObject;
	ExecutionObject* childObject;

	map<string, ExecutionObject*>* objects;
	map<string, ExecutionObject*>::iterator i;

	if (executionObject == NULL) {
		return false;
	}

	if (executionObject->instanceOf( "CompositeExecutionObject" )) {
		compositeObject = (CompositeExecutionObject*) executionObject;
		objects = compositeObject->getExecutionObjects();
		if (objects != NULL) {
			i = objects->begin();
			while (i != objects->end()) {
				childObject = i->second;
				compositeObject->removeExecutionObject( childObject );

				delete objects;
				objects = NULL;

				//TODO make remove / destroy better
				removeExecutionObject( childObject );

				objects = compositeObject->getExecutionObjects();
				if (objects == NULL) {
					break;
				} else {
					i = objects->begin();
				}
			}

			if (objects != NULL) {
				delete objects;
				objects = NULL;
			}
		}
	}

	if (executionObjects->count( executionObject->getId() ) != 0) {
		executionObjects->erase( executionObjects->find( executionObject->getId() ) );
	}

	if (settingObjects->count( executionObject ) != 0) {
		settingObjects->erase( settingObjects->find( executionObject ) );
	}

	delete executionObject;
	executionObject = NULL;
	return true;
}

ExecutionObject* FormatterConverter::hasExecutionObject( Node* node, GenericDescriptor* descriptor ) {

	ExecutionObject* expectedObject;
	map<string, ExecutionObject*>::iterator i;
	NodeNesting* perspective;
	string id;
	CascadingDescriptor* cascadingDescriptor;

	//TODO procurar por potenciais substitutos no caso de REFER

	perspective = new NodeNesting( node->getPerspective() );
	id = perspective->getId() + "/";
	cascadingDescriptor = getCascadingDescriptor( perspective, descriptor );
	if (cascadingDescriptor != NULL) {
		id += cascadingDescriptor->getId();
		delete cascadingDescriptor;
		cascadingDescriptor = NULL;
	}

	i = executionObjects->find( id );
	if (i != executionObjects->end()) {
		expectedObject = i->second;
		return expectedObject;
	}

	return NULL;
}

FormatterCausalLink* FormatterConverter::addCausalLink( ContextNode* context, CausalLink* link ) {

	ExecutionObject* object;

	object = hasExecutionObject( context, NULL );
	if (object == NULL) {
		return NULL;
	}

	CompositeExecutionObject* contextObject;
	vector<Bind*>* binds;

	contextObject = (CompositeExecutionObject*) object;
	contextObject->addNcmLink( link );

	binds = link->getConditionBinds();
	if (binds != NULL) {
		ExecutionObject* childObject;
		vector<Bind*>::iterator i;
		Bind* bind;
		FormatterCausalLink* formatterLink;

		i = binds->begin();
		while (i != binds->end()) {
			bind = (*i);
			childObject = hasExecutionObject( bind->getNode(), bind->getDescriptor() );

			if (childObject != NULL) {
				// compile causal link
				contextObject->removeLinkUncompiled( link );
				formatterLink = ((FormatterLinkConverter*) linkCompiler)->createCausalLink( link, contextObject, depthLevel );

				if (formatterLink != NULL) {
					setActionListener( formatterLink->getAction() );
					contextObject->setLinkCompiled( formatterLink );
				}

				delete binds;
				return formatterLink;
			}
			++i;
		}

		delete binds;
	}
	return NULL;
}

void FormatterConverter::eventStateChanged( void* someEvent, short transition, short /*previousState*/ ) {

	ExecutionObject* executionObject;
	FormatterEvent* event;


	event = (FormatterEvent*) someEvent;
	executionObject = (ExecutionObject*) (event->getExecutionObject());

	if (executionObject->instanceOf( "ExecutionObjectSwitch" )) {
		if (transition == EventUtil::TR_STOPS || transition == EventUtil::TR_ABORTS) {
			/*
			 removeExecutionObject(
			 ((ExecutionObjectSwitch*)executionObject)->
			 getSelectedObject());
			 */
			((ExecutionObjectSwitch*) executionObject)->select( NULL );
			// removeExecutionObject(executionObject);
		}

	} else if (executionObject->instanceOf( "CompositeExecutionObject" )) {
		if (transition == EventUtil::TR_STOPS || transition == EventUtil::TR_ABORTS) {
			removeExecutionObject( executionObject );
		}
	}
}

short FormatterConverter::getPriorityType() {
	return IEventListener::PT_CORE;
}

void FormatterConverter::reset() {
	executionObjects->clear();
}
}
}
}
}
}
}
