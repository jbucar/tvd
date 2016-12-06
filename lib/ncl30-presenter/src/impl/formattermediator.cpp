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

#include "formattermediator.h"
#include "editingcommandevents.h"
#include <util/log.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {

FormatterMediatorImpl::FormatterMediatorImpl( const std::string &baseId, const std::string &docPath, player::System *system ) :
		FormatterMediator( baseId, docPath, system ) {
}

FormatterMediatorImpl::~FormatterMediatorImpl() {
}

bool FormatterMediatorImpl::editingCommand( StartDocumentEC* ec ) {
	LDEBUG( "FormatterMediatorImpl", "StartDocument\n\tDocument id=%s\n\tInterface id=%s", ec->getDocumentId().c_str(), ec->getInterfaceId().c_str() );

	bool res = startDocument( ec->getDocumentId(), ec->getInterfaceId() );
	if (res) {
		setKeyHandler( true );
	} else {
		LWARN( "FormatterMediatorImpl", "editingCommand: can't startDocument" );
	}
	return res;
}

bool FormatterMediatorImpl::editingCommand( StopDocumentEC* ec ) {
	return stopDocument( ec->getDocumentId() );
}

bool FormatterMediatorImpl::editingCommand( AddLinkEC* ec ) {
	Link* l = addLink( ec->getDocumentId(), ec->getCompositeId(), ec->getXmlLink() );
	return l != NULL;
}

bool FormatterMediatorImpl::editingCommand( SetPropertyValueEC* ec ) {
	return setPropertyValue( ec->getDocumentId(), ec->getNodeId(), ec->getPropertyId(), ec->getPropertyValue() );
}

bool FormatterMediatorImpl::editingCommand( AddNodeEC* ec ) {
	Node* n = addNode( ec->getDocumentId(), ec->getCompositeId(), ec->getXmlNode() );
	if (n != NULL) {
		LDEBUG( "FormatterMediatorImpl", "editingCommand: node added '%s'", n->getId().c_str() );
	} else {
		LWARN( "FormatterMediatorImpl", "editingCommand: cant addnode doc='%s' compositeId='%s'", ec->getDocumentId().c_str(), ec->getCompositeId().c_str() );
	}
	return n != NULL;
}

bool FormatterMediatorImpl::editingCommand( AddInterfaceEC* ec ) {
	InterfacePoint* intPoint;
	intPoint = addInterface( ec->getDocumentId(), ec->getNodeId(), ec->getXmlInterface() );
	LDEBUG( "FormatterMediatorImpl", "editingCommand: addInterf doc='%s' nodeId='%s' xml='%s'", ec->getDocumentId().c_str(), ec->getNodeId().c_str(), ec->getXmlInterface().c_str() );

	if (intPoint != NULL) {
		LDEBUG( "FormatterMediatorImpl", "editingCommand: interface added '%s'", intPoint->getId().c_str() );
	} else {
		LWARN( "FormatterMediatorImpl", "editingCommand: cant addInterf doc='%s' nodeId='%s' xml='%s'", ec->getDocumentId().c_str(), ec->getNodeId().c_str(), ec->getXmlInterface().c_str() );
	}
	return intPoint != NULL;
}

/*

bool FormatterMediatorImpl::nclEdit( string nclEditApi ) {
	string commandTag = trim( nclEditApi.substr( 0, nclEditApi.find_first_of( "," ) ) );

	return editingCommand( commandTag, nclEditApi.substr( nclEditApi.find_first_of( "," ) + 1, nclEditApi.length() - (nclEditApi.find_first_of( "," ) + 1) ) );
}

bool FormatterMediatorImpl::editingCommand( string commandTag, string privateDataPayload ) {

	vector<string>* args;
	vector<string>::iterator i;
	string arg, uri, ior, docUri, docIor, uName, docId;
	IGingaLocatorFactory* glf = NULL;

#if HAVE_COMPSUPPORT
	glf = ((GingaLocatorFactoryCreator*)(cm->getObject(
							"GingaLocatorFactory")))();
#else
	glf = GingaLocatorFactory::getInstance();
#endif

	args = split( privateDataPayload, "," );
	i = args->begin();
	arg = *i;
	++i;

	LDEBUG( "FormatterMediatorImpl", "editingCommand('%s'): '%d'", commandTag.c_str(), args->size() );

	//parse command
	if (commandTag == EC_START_DOCUMENT) {
		LDEBUG( "FormatterMediatorImpl", "editingCommand: startDocument" );
		string interfaceId;
		docId = *i;
		++i;

		if (args->size() == 2) {
			if (startDocument( docId, "" )) {
				setKeyHandler( true );
			}

		} else if (args->size() == 3) {
			interfaceId = *i;
			if (startDocument( docId, interfaceId )) {
				setKeyHandler( true );
			}

		} else {
			LWARN( "FormatterMediatorImpl", "editingCommand: can't startDocument, wrong number of params:%d", args->size() );
		}

	} else if (commandTag == EC_STOP_DOCUMENT) {
		stopDocument( *i );

	} else if (commandTag == EC_PAUSE_DOCUMENT) {
		pauseDocument( *i );

	} else if (commandTag == EC_RESUME_DOCUMENT) {
		resumeDocument( *i );

	} else if (commandTag == EC_ADD_REGION) {
		LDEBUG( "FormatterMediatorImpl", "editingCommand: addRegion" );
		string regionId, regionBaseId, xmlRegion;
		LayoutRegion* region;

		if (args->size() == 5) {
			docId = *i;
			++i;
			regionBaseId = *i;
			++i;
			regionId = *i;
			++i;
			xmlRegion = *i;
			region = addRegion( docId, regionBaseId, regionId, xmlRegion );
			if (region != NULL) {
				LDEBUG( "FormatterMediatorImpl", "editingCommand: region added '%s'", region->getId().c_str() );

			} else {
				LWARN( "FormatterMediatorImpl", "editingCommand: cant addregion doc='%s' regionId='%s' xml='%s'", docId.c_str(), regionId.c_str(), xmlRegion.c_str() );
			}

		} else {
			LWARN( "FormatterMediatorImpl", "editingCommand: can't addRegion, wrong number of params:%d", args->size() );
		}

	} else if (commandTag == EC_REMOVE_REGION) {
		string regionId, regionBaseId;

		if (args->size() == 4) {
			regionBaseId = *i;
			++i;
			docId = *i;
			++i;
			regionId = *i;
			removeRegion( docId, regionBaseId, regionId );
		}

	} else if (commandTag == EC_ADD_REGION_BASE) {
		string xmlRegionBase;
		RegionBase* regionBase;

		if (args->size() == 3) {
			docId = *i;
			++i;
			xmlRegionBase = *i;
			regionBase = addRegionBase( docId, xmlRegionBase );
			if (regionBase != NULL) {
				LDEBUG( "FormatterMediatorImpl", "editingCommand] regionBase added '%s'", regionBase->getId().c_str() );

			} else {
				LWARN( "FormatterMediatorImpl", "editingCommand: cant addrgbase doc='%s' xml='%s'", docId.c_str(), xmlRegionBase.c_str() );
			}

		} else {
			LWARN( "FormatterMediatorImpl", "editingCommand: can't addRegionBase, wrong number of params:%d", args->size() );
		}

	} else if (commandTag == EC_REMOVE_REGION_BASE) {
		string baseId;

		if (args->size() == 3) {
			docId = *i;
			++i;
			baseId = *i;
			removeRegionBase( docId, baseId );
		}

	} else if (commandTag == EC_ADD_RULE) {
		string xmlRule;

		if (args->size() == 3) {
			docId = *i;
			++i;
			xmlRule = *i;
			addRule( docId, xmlRule );
		}

	} else if (commandTag == EC_REMOVE_RULE) {
		string ruleId;

		if (args->size() == 3) {
			docId = *i;
			++i;
			ruleId = *i;
			removeRule( docId, ruleId );
		}

	} else if (commandTag == EC_ADD_RULE_BASE) {
		string xmlRuleBase;

		if (args->size() == 3) {
			docId = *i;
			++i;
			xmlRuleBase = *i;
			addRuleBase( docId, xmlRuleBase );
		}

	} else if (commandTag == EC_REMOVE_RULE_BASE) {
		string baseId;

		if (args->size() == 3) {
			docId = *i;
			++i;
			baseId = *i;
			removeRuleBase( docId, baseId );
		}

	} else if (commandTag == EC_ADD_CONNECTOR) {
		string xmlConnector;

		if (args->size() == 3) {
			docId = *i;
			++i;
			xmlConnector = *i;
			addConnector( docId, xmlConnector );
		}

	} else if (commandTag == EC_REMOVE_CONNECTOR) {
		string connectorId;

		if (args->size() == 3) {
			docId = *i;
			++i;
			connectorId = *i;
			removeConnector( docId, connectorId );
		}

	} else if (commandTag == EC_ADD_CONNECTOR_BASE) {
		LDEBUG( "FormatterMediatorImpl", "editingCommand: addConnectorBase" );
		string xmlConnectorBase;
		ConnectorBase* connBase;

		if (args->size() == 3) {
			docId = *i;
			++i;
			xmlConnectorBase = *i;
			connBase = addConnectorBase( docId, xmlConnectorBase );
			if (connBase != NULL) {
				LDEBUG( "FormatterMediatorImpl", "editingCommand: connBase added '%s'", connBase->getId().c_str() );
			} else {
				LWARN( "FormatterMediatorImpl", "editingCommand: cant addconnBs doc='%s' xmlConnBs='%s'", docId.c_str(), xmlConnectorBase.c_str() );
			}

		} else {
			LWARN( "FormatterMediatorImpl", "editingCommand: can't addConnBase, wrong number of params:%d", args->size() );
		}

	} else if (commandTag == EC_REMOVE_CONNECTOR_BASE) {
		string baseId;

		if (args->size() == 3) {
			docId = *i;
			++i;
			baseId = *i;
			removeConnectorBase( docId, baseId );
		}

	} else if (commandTag == EC_ADD_DESCRIPTOR) {
		LDEBUG( "FormatterMediatorImpl", "editingCommand: addDescriptor" );
		string xmlDescriptor;
		GenericDescriptor* desc;

		if (args->size() == 3) {
			docId = *i;
			++i;
			xmlDescriptor = *i;
			desc = addDescriptor( docId, xmlDescriptor );
			if (desc != NULL) {
				LDEBUG( "FormatterMediatorImpl", "editingCommand: descriptor added '%s'", desc->getId().c_str() );

			} else {
				LWARN( "FormatterMediatorImpl", "editingCommand: cant addDesc doc='%s' xmlDesc='%s'", docId.c_str(), xmlDescriptor.c_str() );

			}

		} else {
			LWARN( "FormatterMediatorImpl", "editingCommand: can't addDesc, wrong number of params: %d", args->size() );
		}

	} else if (commandTag == EC_REMOVE_DESCRIPTOR) {
		string descId;

		if (args->size() == 3) {
			docId = *i;
			++i;
			descId = *i;
			removeDescriptor( docId, descId );
		}

	} else if (commandTag == EC_ADD_DESCRIPTOR_SWITCH) {
		string xmlDesc;

		if (args->size() == 3) {
			docId = *i;
			++i;
			xmlDesc = *i;
			addDescriptor( docId, xmlDesc );
		}

	} else if (commandTag == EC_REMOVE_DESCRIPTOR_SWITCH) {
		string descId;

		if (args->size() == 3) {
			docId = *i;
			++i;
			descId = *i;
			removeDescriptor( docId, descId );
		}

	} else if (commandTag == EC_ADD_DESCRIPTOR_BASE) {
		LDEBUG( "FormatterMediatorImpl", "editingCommand: addDescriptorBase" );
		string xmlDescBase;
		DescriptorBase* dBase;

		if (args->size() == 3) {
			docId = *i;
			++i;
			xmlDescBase = *i;
			dBase = addDescriptorBase( docId, xmlDescBase );
			if (dBase != NULL) {
				LDEBUG( "FormatterMediatorImpl", "editingCommand] dBase added '%s'", dBase->getId().c_str() );
			} else {
				LWARN( "FormatterMediatorImpl", "editingCommand: cant adddBase doc='%s' xmlDescBase='%s'", docId.c_str(), xmlDescBase.c_str() );
			}

		} else {
			LWARN( "FormatterMediatorImpl", "editingCommand: can't addDescBase, wrong number of params: %d", args->size() );
		}

	} else if (commandTag == EC_REMOVE_DESCRIPTOR_BASE) {
		string baseId;

		if (args->size() == 3) {
			docId = *i;
			++i;
			baseId = *i;
			removeDescriptorBase( docId, baseId );
		}

	} else if (commandTag == EC_ADD_TRANSITION) {
		string xmlTrans;

		if (args->size() == 3) {
			docId = *i;
			++i;
			xmlTrans = *i;
			addTransition( docId, xmlTrans );
		}

	} else if (commandTag == EC_REMOVE_TRANSITION) {
		string transId;

		if (args->size() == 3) {
			docId = *i;
			++i;
			transId = *i;
			removeTransition( docId, transId );
		}

	} else if (commandTag == EC_ADD_TRANSITION_BASE) {
		string xmlTransBase;

		if (args->size() == 3) {
			docId = *i;
			++i;
			xmlTransBase = *i;
			addTransitionBase( docId, xmlTransBase );
		}

	} else if (commandTag == EC_REMOVE_TRANSITION_BASE) {
		string baseId;

		if (args->size() == 3) {
			docId = *i;
			++i;
			baseId = *i;
			removeTransitionBase( docId, baseId );
		}

	} else if (commandTag == EC_ADD_IMPORT_BASE) {
		string docBaseId, xmlImportBase;

		if (args->size() == 4) {
			docId = *i;
			++i;
			docBaseId = *i;
			++i;
			xmlImportBase = *i;
			addImportBase( docId, docBaseId, xmlImportBase );
		}

	} else if (commandTag == EC_REMOVE_IMPORT_BASE) {
		string docBaseId;

		if (args->size() == 4) {
			docId = *i;
			++i;
			docBaseId = *i;
			++i;
			docUri = *i;
			removeImportBase( docId, docBaseId, docUri );
		}

	} else if (commandTag == EC_ADD_IMPORTED_DOCUMENT_BASE) {
		string xmlImportedBase;

		if (args->size() == 3) {
			docId = *i;
			++i;
			xmlImportedBase = *i;
			addImportedDocumentBase( docId, xmlImportedBase );
		}

	} else if (commandTag == EC_REMOVE_IMPORTED_DOCUMENT_BASE) {
		string baseId;

		if (args->size() == 3) {
			docId = *i;
			++i;
			baseId = *i;
			removeImportedDocumentBase( docId, baseId );
		}

	} else if (commandTag == EC_ADD_IMPORT_NCL) {
		string xmlImportNcl;

		if (args->size() == 3) {
			docId = *i;
			++i;
			xmlImportNcl = *i;
			addImportNCL( docId, xmlImportNcl );
		}

	} else if (commandTag == EC_REMOVE_IMPORT_NCL) {
		if (args->size() == 3) {
			docId = *i;
			++i;
			docUri = *i;
			removeImportNCL( docId, docUri );
		}

	} else if (commandTag == EC_ADD_NODE) {
		LDEBUG( "FormatterMediatorImpl", "editingCommand: addNode" );
		string compositeId, xmlNode;
		Node* n;

		if (args->size() == 4) {
			docId = *i;
			++i;
			compositeId = *i;
			++i;
			xmlNode = *i;
			n = addNode( docId, compositeId, xmlNode );
			if (n != NULL) {
				LDEBUG( "FormatterMediatorImpl", "editingCommand: node added '%s'", n->getId().c_str() );
			} else {
				LWARN( "FormatterMediatorImpl", "editingCommand: cant addnode doc='%s' compositeId='%s' xmlNode='%s'", docId.c_str(), compositeId.c_str(), xmlNode.c_str() );
			}

		} else {
			LWARN( "FormatterMediatorImpl", "editingCommand: can't addNode, wrong number of params: %d", args->size() );
		}

	} else if (commandTag == EC_REMOVE_NODE) {
		string compositeId, nodeId;

		if (args->size() == 4) {
			docId = *i;
			++i;
			compositeId = *i;
			++i;
			nodeId = *i;
			removeNode( docId, compositeId, nodeId );
		}

	} else if (commandTag == EC_ADD_INTERFACE) {
		LDEBUG( "FormatterMediatorImpl", "editingCommand: addInterface" );
		string nodeId, xmlInterface;
		InterfacePoint* intPoint;

		if (args->size() == 4) {
			docId = *i;
			++i;
			nodeId = *i;
			++i;
			xmlInterface = *i;
			intPoint = addInterface( docId, nodeId, xmlInterface );
			LDEBUG( "FormatterMediatorImpl", "editingCommand: addInterf doc='%s' nodeId='%s' xml='%s'", docId.c_str(), nodeId.c_str(), xmlInterface.c_str() );

			if (intPoint != NULL) {
				LDEBUG( "FormatterMediatorImpl", "editingCommand] interface added '%s'", intPoint->getId().c_str() );

			} else {
				LWARN( "FormatterMediatorImpl", "editingCommand: cant addInterf doc='%s' nodeId='%s' xml='%s'", docId.c_str(), nodeId.c_str(), xmlInterface.c_str() );

			}

		} else {
			LWARN( "FormatterMediatorImpl", "editingCommand: can't addInterface, wrong number of params:%d", args->size() );
		}

	} else if (commandTag == EC_REMOVE_INTERFACE) {
		string nodeId, interfaceId;

		if (args->size() == 4) {
			docId = *i;
			++i;
			nodeId = *i;
			++i;
			interfaceId = *i;
			removeInterface( docId, nodeId, interfaceId );
		}

	} else if (commandTag == EC_ADD_LINK) {
		LDEBUG( "FormatterMediatorImpl", "%s %s : commandTag == EC_ADD_LINK!", __FILE__, __LINE__ );
		string compositeId, xmlLink;

		if (args->size() == 4) {
			docId = *i;
			++i;
			compositeId = *i;
			++i;
			xmlLink = *i;

			LDEBUG( "FormatterMediatorImpl", "%s %s: docId = %s ,compositeId = %s xmlLink = %s", __FILE__, __LINE__, docId.c_str(), compositeId.c_str(), xmlLink.c_str() );
			addLink( docId, compositeId, xmlLink );
		}

	} else if (commandTag == EC_REMOVE_LINK) {
		string compositeId, linkId;

		if (args->size() == 4) {
			docId = *i;
			++i;
			compositeId = *i;
			++i;
			linkId = *i;
			removeLink( docId, compositeId, linkId );
		}

	} else if (commandTag == EC_SET_PROPERTY_VALUE) {
		string nodeId, propId, value;

		if (args->size() == 5) {
			docId = *i;
			++i;
			nodeId = *i;
			++i;
			propId = *i;
			++i;
			value = *i;

			setPropertyValue( docId, nodeId, propId, value );
		}
	}

	delete args;
	args = NULL;
	return true;
}
*/
}
}
}
}
}
