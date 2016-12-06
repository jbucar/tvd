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

#include "../include/FormatterMediator.h"

#include "generated/config.h"
#include <ncl30/layout/types.h>
#include <gingaplayer/system.h>
#include <gingaplayer/device.h>
#include <canvas/canvas.h>
#include <canvas/system.h>
#include <util/mcr.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <util/log.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {

FormatterMediator::FormatterMediator( const std::string &baseId, const std::string &docPath, player::System *system ) {
	
	_docData.baseId = baseId;
	_docData.docPath = docPath;

	this->currentDocument = NULL;
	_sys = system;

	playerManager = new PlayerAdapterManager( _docData.docPath, system );

	isEmbedded = _docData.parentDocId != "";

	ruleAdapter = new RuleAdapter();

	compiler = new FormatterConverter( ruleAdapter, system );

	scheduler = new FormatterScheduler( playerManager, ruleAdapter, compiler, this, system );

	compiler->setScheduler( scheduler );

	scheduler->setKeyHandler( true );

	documentEvents = new map<string, FormatterEvent*>;
	documentEntryEvents = new map<string, vector<FormatterEvent*>*>;

	privateBaseManager = new PrivateBaseManager();
	privateBaseManager->createPrivateBase( _docData.baseId );

}

FormatterMediator::~FormatterMediator() {

	if (scheduler != NULL) {
		// scheduler->removeSchedulerListener( this );
	}

	if (currentDocument != NULL) {
		std::string docId = currentDocument->getId();
		if (compiler != NULL) {
			ExecutionObject* bodyObject = compiler->getObjectFromNodeId( docId );
			if (bodyObject != NULL) {
				compiler->removeExecutionObject( bodyObject );
			}
		}
		removeDocument( docId );
		currentDocument = NULL;
	}

	DEL( ruleAdapter );
	DEL( playerManager );
	//TODO: check if we can delete events too
	DEL( documentEvents );
	DEL( documentEntryEvents );
	DEL( scheduler );
	DEL( compiler );
	DEL( privateBaseManager );

}

void FormatterMediator::release() {
}

/**
 * Parsea el documento NCL actual.
 */
bool FormatterMediator::parseDocument( void ) {

	if (currentDocument != NULL) {
		LWARN("FormatterMediator", "parseDocument: currentDocument != NULL");
		return false;
	}

	currentDocument = (NclDocument*) addDocument( _docData.docPath );
	if ( currentDocument != NULL ) {
		_docData.docId = currentDocument->getId();
	}

	return currentDocument != NULL;
}

void* FormatterMediator::addDocument( string docLocation ) {
	NclDocument* addedDoc = NULL;

	if (isEmbedded) {
		addedDoc = privateBaseManager->embedDocument( _docData.baseId, _docData.parentDocId, _docData.nodeId, docLocation );

	} else {
		addedDoc = privateBaseManager->addDocument( _docData.baseId, docLocation );
	}

	if (addedDoc == NULL) {
		LWARN("FormatterMediator", "addDocument: Can't add document for '%s'", docLocation.c_str());
		return NULL;
	}

	_docData.docId = addedDoc->getId();

	if (prepareDocument( addedDoc->getId() )) {
		return addedDoc;
	}
	LWARN("FormatterMediator", "addDocument: Can't prepare document for '%s'", docLocation.c_str());
	return NULL;
}

bool FormatterMediator::removeDocument( string documentId ) {
	NclDocument* document;

	if (documentEvents->count( documentId ) != 0) {
		stopDocument( documentId );
	}

	if (isEmbedded && documentId == _docData.docId) {
		document = privateBaseManager->removeEmbeddedDocument( _docData.baseId, _docData.parentDocId, _docData.nodeId );

	} else {
		document = privateBaseManager->removeDocument( _docData.baseId, documentId );
	}

	if (document != NULL) {
		delete document;
		return true;

	} else {
		return false;
	}
}

ContextNode* FormatterMediator::getDocumentContext( string documentId ) {
	NclDocument* nclDocument;

	if (documentEvents->count( documentId ) != 0) {
		return NULL;
	}

	if (isEmbedded && documentId == _docData.docId) {
		nclDocument = privateBaseManager->getEmbeddedDocument( _docData.baseId, _docData.parentDocId, _docData.nodeId );

	} else {
		nclDocument = privateBaseManager->getDocument( _docData.baseId, documentId );
	}

	if (nclDocument == NULL) {
		return NULL;
	}

	return nclDocument->getBody();
}

void FormatterMediator::setDepthLevel( int level ) {
	compiler->setDepthLevel( level );
}

int FormatterMediator::getDepthLevel() {
	return compiler->getDepthLevel();
}

vector<FormatterEvent*>* FormatterMediator::processDocument( string documentId, string interfaceId ) {

	vector<FormatterEvent*>* entryEvents;
	vector<Port*>* ports;
	int i, size;
	ContextNode* context;
	Port* port;
	NodeNesting* contextPerspective;
	FormatterEvent* event;

	port = NULL;

	// look for the entry point perspective
	context = getDocumentContext( documentId );
	if (context == NULL) {
		// document has no body
		LWARN("FormatterMediator", "processDocument: Doc '%s': without body!", documentId.c_str());
		return NULL;
	}

	ports = new vector<Port*>;
	if (interfaceId == "") {
		size = context->getNumPorts();
		for (i = 0; i < size; i++) {
			port = context->getPort( i );
			if (port != NULL && port->getEndInterfacePoint() != NULL && port->getEndInterfacePoint()->instanceOf( "ContentAnchor" )) {

				ports->push_back( port );
			}
		}

	} else {
		port = context->getPort( interfaceId );
		if (port != NULL) {
			ports->push_back( port );
		}
	}

	if (ports->empty()) {
		// interfaceId not found
		LWARN("FormatterMediator", "processDocument: Doc '%s': without interfaces", documentId.c_str());
		delete ports;
		return NULL;
	}

	contextPerspective = new NodeNesting( privateBaseManager->getPrivateBase( _docData.baseId ) );

	contextPerspective->insertAnchorNode( context );

	entryEvents = new vector<FormatterEvent*>;
	size = ports->size();
	for (i = 0; i < size; i++) {
		port = (*ports)[i];
		event = compiler->insertContext( contextPerspective, port );
		if (event != NULL) {
			entryEvents->push_back( event );
		}
	}

	delete ports;
	delete contextPerspective;

	if (entryEvents->empty()) {
		LWARN("FormatterMediator", "processDocument: Doc '%s': without entry events", documentId.c_str());
		return NULL;
	}

	return entryEvents;
}

vector<FormatterEvent*>* FormatterMediator::getDocumentEntryEvent( string documentId ) {

	if (documentEntryEvents->count( documentId ) != 0) {
		return (*documentEntryEvents)[documentId];

	} else {
		return NULL;
	}
}

bool FormatterMediator::compileDocument( string documentId, string interfaceId ) {

	vector<FormatterEvent*>* entryEvents;
	FormatterEvent* event;
	ExecutionObject* executionObject;
	CompositeExecutionObject* parentObject;
	FormatterEvent* documentEvent;

	if (documentEvents->count( documentId ) != 0) {
		return true;
	}

	entryEvents = processDocument( documentId, interfaceId );
	if (entryEvents == NULL) {
		return false;
	}

	event = (*entryEvents)[0];
	executionObject = (ExecutionObject*) (event->getExecutionObject());
	parentObject = (CompositeExecutionObject*) (executionObject->getParentObject());

	if (parentObject != NULL) {
		while (parentObject->getParentObject() != NULL) {
			executionObject = parentObject;
			parentObject = (CompositeExecutionObject*) (parentObject->getParentObject());
		}
		documentEvent = executionObject->getWholeContentPresentationEvent();

	} else {
		documentEvent = event;
	}

	(*documentEvents)[documentId] = documentEvent;
	(*documentEntryEvents)[documentId] = entryEvents;

	return true;
}

bool FormatterMediator::prepareDocument( string documentId ) {
	string src, docLocation;
	ContextNode* body;

	body = getDocumentContext( documentId );
	if (body == NULL) {
		// document has no body
		LWARN("FormatterMediator", "prepareDocument: Doc '%s': without body!", documentId.c_str());
		return false;
	}

	if (isEmbedded && documentId == _docData.docId) {
		docLocation = privateBaseManager->getEmbeddedDocumentLocation( _docData.baseId, _docData.parentDocId, _docData.nodeId );

	} else {
		docLocation = privateBaseManager->getDocumentLocation( _docData.baseId, documentId );
	}

	if (docLocation == "") {
		return false;
	}

	//solving remote descriptors URIs
	if (isEmbedded && documentId == _docData.docId) {
		privateBaseManager->getEmbeddedDocument( _docData.baseId, _docData.parentDocId, _docData.nodeId );

	} else {
		privateBaseManager->getDocument( _docData.baseId, documentId );
	}

	return true;
}

bool FormatterMediator::startDocument( string documentId, string interfaceId ) {

	vector<FormatterEvent*>* entryEvents;
	FormatterEvent* documentEvent;

	LDEBUG("FormatterMediator", "start doc, id=%s, interface=%s", documentId.c_str(), interfaceId.c_str());
	if (compileDocument( documentId, interfaceId )) {
		if (documentEvents->count( documentId ) != 0) {
			documentEvent = (*documentEvents)[documentId];
			entryEvents = (*documentEntryEvents)[documentId];
			scheduler->startDocument( documentEvent, entryEvents );
			return true;
		}
	}

	return false;
}

bool FormatterMediator::stopDocument( string documentId ) {
	FormatterEvent* documentEvent;

	if (documentEvents->count( documentId ) == 0) {
		return false;
	}

	documentEvent = (*documentEvents)[documentId];
	scheduler->stopDocument( documentEvent );
	if (documentEvents != NULL && documentEvents->count( documentId ) != 0) {
		documentEvents->erase( documentEvents->find( documentId ) );
	}

	if (documentEntryEvents != NULL) {
		EntryEvents::iterator it = documentEntryEvents->find( documentId );
		vector<FormatterEvent*>* entryEvents;
		if ( it != documentEntryEvents->end() ) {
			entryEvents = documentEntryEvents->at( documentId );
			BOOST_FOREACH( FormatterEvent *event, *entryEvents ) {
					delete event;
			}
			entryEvents->clear();
			documentEntryEvents->erase( it );
			delete entryEvents;
		}
	}

	return true;
}

bool FormatterMediator::pauseDocument( string documentId ) {
	FormatterEvent* documentEvent;

	if (documentEvents->count( documentId ) == 0) {
		return false;
	}

	documentEvent = (*documentEvents)[documentId];
	scheduler->pauseDocument( documentEvent );
	return true;
}

bool FormatterMediator::resumeDocument( string documentId ) {
	FormatterEvent* documentEvent;

	if (documentEvents->count( documentId ) == 0) {
		return false;
	}

	documentEvent = (*documentEvents)[documentId];
	scheduler->resumeDocument( documentEvent );
	return true;
}

void FormatterMediator::presentationCompleted( FormatterEvent* documentEvent ){
	documentEvent->delEventListener( (IEventListener*) this );
}


LayoutRegion* FormatterMediator::addRegion( string documentId, string regionBaseId, string regionId, string xmlRegion ) {

	return privateBaseManager->addRegion( _docData.baseId, documentId, regionBaseId, regionId, xmlRegion );
}

LayoutRegion* FormatterMediator::removeRegion( string documentId, string regionBaseId, string regionId ) {

	return privateBaseManager->removeRegion( _docData.baseId, documentId, regionBaseId, regionId );
}

RegionBase* FormatterMediator::addRegionBase( string documentId, string xmlRegionBase ) {

	return privateBaseManager->addRegionBase( _docData.baseId, documentId, xmlRegionBase );
}

RegionBase* FormatterMediator::removeRegionBase( string documentId, string regionBaseId ) {

	return privateBaseManager->removeRegionBase( _docData.baseId, documentId, regionBaseId );
}

Rule* FormatterMediator::addRule( string documentId, string xmlRule ) {
	return privateBaseManager->addRule( _docData.baseId, documentId, xmlRule );
}

Rule* FormatterMediator::removeRule( string documentId, string ruleId ) {
	return privateBaseManager->removeRule( _docData.baseId, documentId, ruleId );
}

RuleBase* FormatterMediator::addRuleBase( string documentId, string xmlRuleBase ) {

	return privateBaseManager->addRuleBase( _docData.baseId, documentId, xmlRuleBase );
}

RuleBase* FormatterMediator::removeRuleBase( string documentId, string ruleBaseId ) {

	return privateBaseManager->removeRuleBase( _docData.baseId, documentId, ruleBaseId );
}

Transition* FormatterMediator::addTransition( string documentId, string xmlTransition ) {

	return privateBaseManager->addTransition( _docData.baseId, documentId, xmlTransition );
}

Transition* FormatterMediator::removeTransition( string documentId, string transitionId ) {

	return privateBaseManager->removeTransition( _docData.baseId, documentId, transitionId );
}

TransitionBase* FormatterMediator::addTransitionBase( string documentId, string xmlTransitionBase ) {

	return privateBaseManager->addTransitionBase( _docData.baseId, documentId, xmlTransitionBase );
}

TransitionBase* FormatterMediator::removeTransitionBase( string documentId, string transitionBaseId ) {

	return privateBaseManager->removeTransitionBase( _docData.baseId, documentId, transitionBaseId );
}

Connector* FormatterMediator::addConnector( string documentId, string xmlConnector ) {

	return privateBaseManager->addConnector( _docData.baseId, documentId, xmlConnector );
}

Connector* FormatterMediator::removeConnector( string documentId, string connectorId ) {

	return privateBaseManager->removeConnector( _docData.baseId, documentId, connectorId );
}

ConnectorBase* FormatterMediator::addConnectorBase( string documentId, string xmlConnectorBase ) {

	return privateBaseManager->addConnectorBase( _docData.baseId, documentId, xmlConnectorBase );
}

ConnectorBase* FormatterMediator::removeConnectorBase( string documentId, string connectorBaseId ) {

	return privateBaseManager->removeConnectorBase( _docData.baseId, documentId, connectorBaseId );
}

GenericDescriptor* FormatterMediator::addDescriptor( string documentId, string xmlDescriptor ) {

	return privateBaseManager->addDescriptor( _docData.baseId, documentId, xmlDescriptor );
}

GenericDescriptor* FormatterMediator::removeDescriptor( string documentId, string descriptorId ) {

	return privateBaseManager->removeDescriptor( _docData.baseId, documentId, descriptorId );
}

DescriptorBase* FormatterMediator::addDescriptorBase( string documentId, string xmlDescriptorBase ) {

	return privateBaseManager->addDescriptorBase( _docData.baseId, documentId, xmlDescriptorBase );
}

DescriptorBase* FormatterMediator::removeDescriptorBase( string documentId, string descriptorBaseId ) {

	return privateBaseManager->removeDescriptorBase( _docData.baseId, documentId, descriptorBaseId );
}

Base* FormatterMediator::addImportBase( string documentId, string docBaseId, string xmlImportBase ) {

	return privateBaseManager->addImportBase( _docData.baseId, documentId, docBaseId, xmlImportBase );
}

Base* FormatterMediator::removeImportBase( string documentId, string docBaseId, string documentURI ) {

	return privateBaseManager->removeImportBase( _docData.baseId, documentId, docBaseId, documentURI );
}

NclDocument* FormatterMediator::addImportedDocumentBase( string documentId, string xmlImportedDocumentBase ) {

	return privateBaseManager->addImportedDocumentBase( _docData.baseId, documentId, xmlImportedDocumentBase );
}

NclDocument* FormatterMediator::removeImportedDocumentBase( string documentId, string importedDocumentBaseId ) {

	return privateBaseManager->removeImportedDocumentBase( _docData.baseId, documentId, importedDocumentBaseId );
}

NclDocument* FormatterMediator::addImportNCL( string documentId, string xmlImportNCL ) {

	return privateBaseManager->addImportNCL( _docData.baseId, documentId, xmlImportNCL );
}

NclDocument* FormatterMediator::removeImportNCL( string documentId, string documentURI ) {

	return privateBaseManager->removeImportNCL( _docData.baseId, documentId, documentURI );
}

void FormatterMediator::processInsertedReferNode( ReferNode* referNode ) {
	NodeEntity* nodeEntity;
	ExecutionObject* executionObject;
	CompositeExecutionObject* parentObject;
	int depthLevel;
	string instanceType;

	nodeEntity = (NodeEntity*) (referNode->getDataEntity());
	instanceType = referNode->getInstanceType();
	if (nodeEntity->instanceOf( "ContentNode" ) && instanceType == "instSame") {

		executionObject = compiler->hasExecutionObject( nodeEntity, NULL );
		if (executionObject != NULL) {
			parentObject = compiler->addSameInstance( executionObject, referNode );

			if (parentObject != NULL) {
				depthLevel = compiler->getDepthLevel();
				if (depthLevel > 0) {
					depthLevel = depthLevel - 1;
				}

				compiler->compileExecutionObjectLinks( executionObject, referNode, parentObject, depthLevel );
			}
		} else {
			LWARN("FormatterMediator", "processInsertedReferNode: referred object is NULL for '%s' instType = '%s'", nodeEntity->getId().c_str(), instanceType.c_str());
		}
	} else {
		LDEBUG("FormatterMediator", "processInsertedReferNode: referred object for '%s' instType = '%s'", nodeEntity->getId().c_str(), instanceType.c_str());
	}
}

void FormatterMediator::processInsertedComposition( CompositeNode* composition ) {
	vector<Node*>* nodes;

	nodes = composition->getNodes();
	if (nodes != NULL) {
		vector<Node*>::iterator i;
		i = nodes->begin();
		while (i != nodes->begin()) {
			if ((*i)->instanceOf( "ReferNode" )) {
				processInsertedReferNode( (ReferNode*) (*i) );

			} else if ((*i)->instanceOf( "CompositeNode" )) {
				processInsertedComposition( (CompositeNode*) (*i) );
			}
		}
	}
}

Node* FormatterMediator::addNode( string documentId, string compositeId, string xmlNode ) {

	Node* node;

	node = privateBaseManager->addNode( _docData.baseId, documentId, compositeId, xmlNode );

	if (node == NULL) {
		return NULL;
	}

	if (node->instanceOf( "ReferNode" )) {
		processInsertedReferNode( (ReferNode*) node );

	} else if (node->instanceOf( "CompositeNode" )) {
		// look for child nodes with refer and newInstance=false
		processInsertedComposition( (CompositeNode*) node );
	}

	return node;
}

Node* FormatterMediator::removeNode( string documentId, string compositeId, string nodeId ) {

	NclDocument* document;

	document = privateBaseManager->getDocument( _docData.baseId, documentId );

	if (document == NULL) {
		// document does not exist
		return NULL;
	}

	Node* parentNode;

	parentNode = document->getNode( compositeId );
	if (parentNode == NULL || !(parentNode->instanceOf( "ContextNode" ))) {
		// composite node (compositeId) does exist or is not a context node
		return NULL;
	}

	Node* node;
	CompositeNode* compositeNode;

	compositeNode = (ContextNode*) parentNode;
	node = compositeNode->getNode( nodeId );
	if (node == NULL) {
		// node (nodeId) is not a compositeId child node
		return NULL;
	}

	ExecutionObject* executionObject;
	vector<Anchor*>* nodeInterfaces;
	vector<Anchor*>* anchors;
	vector<Anchor*>::iterator it;
	vector<Port*>* ports;
	vector<Port*>::iterator j;
	int i, size;
	InterfacePoint* nodeInterface;

	// remove all node interfaces
	nodeInterfaces = new vector<Anchor*>;

	anchors = node->getAnchors();
	if (anchors != NULL) {
		it = anchors->begin();
		while (it != anchors->end()) {
			nodeInterfaces->push_back( *it );
			++it;
		}
	}

	if (node->instanceOf( "CompositeNode" )) {
		ports = ((CompositeNode*) node)->getPorts();
		if (ports != NULL) {
			j = ports->begin();
			while (j != ports->end()) {
				nodeInterfaces->push_back( (Anchor*) (*j) );
			}
		}
	}

	size = nodeInterfaces->size();
	for (i = 0; i < size; i++) {
		nodeInterface = (*nodeInterfaces)[i];
		removeInterface( node, nodeInterface );
	}

	// remove the execution object
	executionObject = compiler->hasExecutionObject( node, NULL );

	if (executionObject != NULL) {
		if (node->instanceOf( "ReferNode" ) && ((ReferNode*) node)->getInstanceType() != "new") {

			// remove the object entry
			compiler->removeExecutionObject( executionObject, (ReferNode*) node );

		} else {
			// remove the whole execution object
			if (executionObject->getMainEvent() != NULL) {
				scheduler->stopEvent( executionObject->getMainEvent() );
			}
			compiler->removeExecutionObject( executionObject );
		}
	}

	compositeNode->removeNode( node );
	return node;
}

InterfacePoint* FormatterMediator::addInterface( string documentId, string nodeId, string xmlInterface ) {

	return privateBaseManager->addInterface( _docData.baseId, documentId, nodeId, xmlInterface );
}

void FormatterMediator::removeInterfaceMappings( Node* node, InterfacePoint* interfacePoint, CompositeNode* composition ) {

	vector<Port*>* portsToBeRemoved;
	vector<Port*>* ports;
	vector<Port*>* mappings;
	vector<Port*>::iterator j, k;

	int i, size;
	Port* port;
	Port* mapping;

	portsToBeRemoved = new vector<Port*>;

	ports = composition->getPorts();
	if (ports != NULL) {
		j = ports->begin();
		while (j != ports->end()) {
			port = *j;

			if (port->instanceOf( "SwitchPort" )) {
				mappings = ((SwitchPort*) port)->getPorts();
				if (mappings != NULL) {
					k = mappings->begin();
					while (k != mappings->end()) {
						mapping = (Port*) (*k);
						if (mapping->getNode() == node && mapping->getInterfacePoint() == interfacePoint) {

							portsToBeRemoved->push_back( port );
							break;
						}
						++k;
					}
				}

			} else if (port->getNode() == node && port->getInterfacePoint() == interfacePoint) {

				portsToBeRemoved->push_back( port );
			}
			++j;
		}
	}

	size = portsToBeRemoved->size();
	for (i = 0; i < size; i++) {
		port = (Port*) ((*portsToBeRemoved)[i]);
		removeInterface( composition, port );
	}
}

void FormatterMediator::removeInterfaceLinks( Node* node, InterfacePoint* interfacePoint, LinkComposition* composition ) {

	vector<Link*>* linksToBeRemoved;
	vector<Link*>* links;
	vector<Link*>::iterator j;
	vector<Bind*>* binds;
	vector<Bind*>::iterator k;

	Link* link;
	Bind* bind;
	int i, size;

	linksToBeRemoved = new vector<Link*>;
	links = composition->getLinks();
	if (links != NULL) {
		j = links->begin();
		while (j != links->end()) {
			link = (Link*) (*j);
			// verify if node and interface point participate in link
			binds = link->getBinds();
			if (binds != NULL) {
				k = binds->begin();
				while (k != binds->end()) {
					bind = (Bind*) (*k);
					if (bind->getNode() == node && bind->getInterfacePoint() == interfacePoint) {

						linksToBeRemoved->push_back( link );
						break;
					}
					++k;
				}
			}
			++j;
		}
	}

	size = linksToBeRemoved->size();
	for (i = 0; i < size; i++) {
		link = (Link*) ((*linksToBeRemoved)[i]);
		removeLink( composition, link );
	}
}

void FormatterMediator::removeInterface( Node* node, InterfacePoint* interfacePoint ) {

	CompositeNode* parentNode;

	parentNode = (CompositeNode*) (node->getParentComposition());
	removeInterfaceMappings( node, interfacePoint, parentNode );

	if (parentNode != NULL && parentNode->instanceOf( "LinkComposition" )) {
		removeInterfaceLinks( node, interfacePoint, (LinkComposition*) parentNode );
	}

	if (interfacePoint->instanceOf( "Anchor" )) {
		node->removeAnchor( (Anchor*) interfacePoint );

	} else {
		((CompositeNode*) node)->removePort( (Port*) interfacePoint );
		//TODO verify if a special treatment is necessary for switch ports
	}
}

InterfacePoint* FormatterMediator::removeInterface( string documentId, string nodeId, string interfaceId ) {

	NclDocument* document;
	Node* node;
	InterfacePoint* interfacePoint;

	document = privateBaseManager->getDocument( _docData.baseId, documentId );

	if (document == NULL) {
		// document does not exist
		return NULL;
	}

	node = document->getNode( nodeId );
	if (node == NULL) {
		// node (nodeId) does not exist
		return NULL;
	}

	interfacePoint = node->getAnchor( interfaceId );
	if (interfacePoint == NULL && node->instanceOf( "CompositeNode" )) {
		interfacePoint = ((CompositeNode*) node)->getPort( interfaceId );
	}

	if (interfacePoint == NULL) {
		// interface (interfaceId) does not exist or
		// does not pertain to node
		return NULL;
	}

	removeInterface( node, interfacePoint );
	return interfacePoint;
}

Link* FormatterMediator::addLink( string documentId, string compositeId, string xmlLink ) {

	Link* link;
	NclDocument* document;
	ContextNode* contextNode;

	link = privateBaseManager->addLink( _docData.baseId, documentId, compositeId, xmlLink );

	if (link != NULL) {
		document = privateBaseManager->getDocument( _docData.baseId, documentId );

		contextNode = (ContextNode*) (document->getNode( compositeId ));
		if (link->instanceOf( "CausalLink" )) {
			LDEBUG("FormatterMediator", "%s %d: link->instanceOf(CausalLink) es true...", __FILE__, __LINE__);
			compiler->addCausalLink( contextNode, (CausalLink*) link );
		} else {
			LDEBUG("FormatterMediator", "%s %d: link->instanceOf(CausalLink) es false!", __FILE__ , __LINE__);
		}
	}
	return link;
}

void FormatterMediator::removeLink( LinkComposition* composition, Link* link ) {
	CompositeExecutionObject* compositeObject;

	if (composition->instanceOf( "CompositeNode" )) {
		compositeObject = (CompositeExecutionObject*) (compiler->hasExecutionObject( (CompositeNode*) composition, NULL ));

		if (compositeObject != NULL) {
			compositeObject->removeNcmLink( link );
		}
	}
	composition->removeLink( link );
}

Link* FormatterMediator::removeLink( string documentId, string compositeId, string linkId ) {

	NclDocument* document;
	Node* node;
	ContextNode* contextNode;
	Link* link;

	document = privateBaseManager->getDocument( _docData.baseId, documentId );

	if (document == NULL) {
		// document does not exist
		return NULL;
	}

	node = document->getNode( compositeId );
	if (node == NULL || !(node->instanceOf( "ContextNode" ))) {
		// composite node (compositeId) does exist or is not a context node
		return NULL;
	}

	contextNode = (ContextNode*) node;
	link = contextNode->getLink( linkId );
	if (link == NULL) {
		// link (linkId) is not a nodeId child link
		return NULL;
	}

	removeLink( contextNode, link );
	return link;
}

bool FormatterMediator::setPropertyValue( string documentId, string nodeId, string propertyId, string value ) {

	NclDocument* document;
	Node* node;
	Anchor* anchor;
	NodeNesting* perspective;
	ExecutionObject* executionObject;
	FormatterEvent* event;
	LinkAssignmentAction* setAction;

	document = privateBaseManager->getDocument( _docData.baseId, documentId );

	if (document == NULL) {
		// document does not exist
		return false;
	}

	node = document->getNode( nodeId );
	if (node == NULL) {
		// node (nodeId) does exist
		return false;
	}

	anchor = node->getAnchor( propertyId );
	if (anchor == NULL || !(anchor->instanceOf( "PropertyAnchor" ))) {
		//property inexistente or interface (interfaceId) is not a property
		LWARN("FormatterMediator", "setPropertyValue: Trying to set a NULL property for '%s'", node->getId().c_str());
		return false;
	}

	perspective = new NodeNesting( node->getPerspective() );
	executionObject = compiler->getExecutionObject( perspective, NULL, compiler->getDepthLevel() );

	event = compiler->getEvent( executionObject, anchor, EventUtil::EVT_ATTRIBUTION, "" );

	if (event == NULL || !(event->instanceOf( "AttributionEvent" ))) {
		return false;
	}

	setAction = new LinkAssignmentAction( (AttributionEvent*) event, SimpleAction::ACT_SET, value, _sys );

	((LinkActionListener*) scheduler)->runAction( setAction );
	return true;
}

bool FormatterMediator::setKeyHandler( bool isHandler ) {
	scheduler->setKeyHandler( isHandler );
	return isHandler;
}

void FormatterMediator::iterateBases( RegionBase *aBase ) {
	setRegionSize( aBase );
	BOOST_FOREACH( Base* regBase, *aBase->getBases() ) {
		iterateBases( static_cast<RegionBase*>( regBase ) );
	}
}

void FormatterMediator::setDevicesSize( NclDocument* aDocument ) {
	typedef std::map<int, bptnl::RegionBase*> RegionsBase;

	BOOST_FOREACH( const RegionsBase::value_type& regionBase, *aDocument->getRegionBases() ) {
		iterateBases( regionBase.second );
	}

	vector<NclDocument*> *docs = aDocument->getDocuments();
	if ( docs != NULL) {
		BOOST_FOREACH( NclDocument *aDoc, *docs ) {
			setDevicesSize( aDoc );
		}
	}
}

void FormatterMediator::setRegionSize( RegionBase* regBase ) {
	LayoutRegion *region = regBase->getLayout();
	bptnl::Device &device = region->getDevice();
	player::Device *dev = _sys->getDevice( device.name, device.number );
	if ( dev != NULL ) {
		canvas::Size size = dev->system()->canvas()->size();
		region->setProperty( "width", boost::lexical_cast<std::string> (size.w));
		region->setProperty( "height", boost::lexical_cast<std::string> (size.h));
	}
}

/**
 * Comienza la ejecución del documento actual
 */
void FormatterMediator::play() {
	if (currentDocument != NULL) {
		setDevicesSize( currentDocument );
		startDocument( currentDocument->getId(), "" );
	} else {
		LWARN("FormatterMediator", "play: Trying to start a NULL doc for '%s'", _docData.docPath.c_str());
	}
}

/**
 * Detiene la ejecución del documento actual
 */
void FormatterMediator::stop() {
	if (currentDocument != NULL) {
		stopDocument( currentDocument->getId() );
	}
}

/**
 * Pone en pausa a la ejecución del documento actual
 */
void FormatterMediator::pause() {
	if (currentDocument != NULL) {
		pauseDocument( currentDocument->getId() );
	}
}

/**
 * Reanuda la ejecución del documento actual.
 */
void FormatterMediator::resume() {
	if (currentDocument != NULL) {
		resumeDocument( currentDocument->getId() );
	}
}

}
}
}
}
}
