/*******************************************************************************

 Copyright 2010 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

 ********************************************************************************

 This file is part of Ginga implementation.

 This program is free software: you can redistribute it and/or modify it
 under the terms of the GNU General Public License as published by the Free
 Software Foundation, either version 2 of the License.

 Ginga is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see <http://www.gnu.org/licenses/>.

 ********************************************************************************

 Este archivo es parte de la implementación de Ginga.

 Este programa es Software Libre: Ud. puede redistribuirlo y/o modificarlo
 bajo los términos de la Licencia Pública General GNU como es publicada por la
 Free Software Foundation, según la versión 2 de la licencia.

 Ginga se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
 GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
 PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
 General GNU.

 Ud. debería haber recibido una copia de la Licencia Pública General GNU
 junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

 *******************************************************************************/
#include "editingcommandevents.h"
#include <connector/handler/editingcommandhandler.h>
#include <util/buffer.h>
#include <util/types.h>

#include <boost/algorithm/string.hpp>

#include <iostream>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {

EditingCommand::EditingCommand() {
}

EditingCommand::~EditingCommand() {
}

std::string EditingCommand::getBaseId() {
	return baseId;
}

void EditingCommand::setBaseId( std::string baseId ) {
	this->baseId = baseId;
}

DocumentEC::DocumentEC() :
		EditingCommand() {
}

DocumentEC::~DocumentEC() {
}

void DocumentEC::setDocumentId( std::string documentId ) {
	this->documentId = documentId;
}

std::string DocumentEC::getDocumentId() const {
	return documentId;
}

AddDocumentEC::AddDocumentEC() :
		EditingCommand() {
}

AddDocumentEC::~AddDocumentEC() {
}

bool AddDocumentEC::parsePayload( connector::EditingCommandData* ecd ) {
	// addDocument (baseID, {uri,ior}+)
	std::string payload( (const char *) ecd->getPayload(), (size_t) ecd->getPayloadSize() );
	std::vector<std::string> tokens;
	boost::split( tokens, payload, boost::is_any_of( "," ) );
	if (tokens.size() > 1) {
		baseId = tokens[0];
		size_t index = 1;
		for (; index + 1 < tokens.size(); index += 2) {
			documentUris.push_back( tokens.at( index ) );
			documentIors.push_back( tokens.at( index + 1 ) );
		}
		if (index < tokens.size()) {
			// HACK: Should fix util::split here...
			for (; index < tokens.size(); index++) {
				documentUris.push_back( tokens.at( index ) );
				documentIors.push_back( "" );
			}
		}
		return true;
	} else {
		return false;
	}
}

int AddDocumentEC::getDocumentCount() {
	return (int) documentUris.size();
}

std::string AddDocumentEC::getDocumentUri( int index ) {
	return documentUris[index];
}
std::string AddDocumentEC::getDocumentIor( int index ) {
	return documentIors[index];
}

void AddDocumentEC::addDocument( std::string uri, std::string ior ) {
	this->documentUris.push_back( uri );
	this->documentIors.push_back( ior );
}

StartDocumentEC::StartDocumentEC() :
		DocumentEC() {
}

StartDocumentEC::~StartDocumentEC() {
}

std::string StartDocumentEC::getInterfaceId() const {
	return interfaceId;
}

void StartDocumentEC::setData( std::string documentId, std::string interfaceId ) {
	this->documentId = documentId;
	this->interfaceId = interfaceId;
}

bool StartDocumentEC::parsePayload( connector::EditingCommandData* ecd ) {
	// startDocument (baseID, documentID, interfaceID)
	std::string payload( (const char *) ecd->getPayload(), (size_t) ecd->getPayloadSize() );
	std::vector<std::string> tokens;
	boost::split( tokens, payload, boost::is_any_of( "," ) );
	int token_cnt = tokens.size();
	if (token_cnt > 1) {
		baseId = tokens[0];
		documentId = tokens[1];
		if (token_cnt > 2)
			interfaceId = tokens[2];

		return true;
	} else {
		return false;
	}

}

StopDocumentEC::StopDocumentEC() :
		DocumentEC() {
}

StopDocumentEC::~StopDocumentEC() {
}

void StopDocumentEC::setData( std::string documentId ) {
	this->documentId = documentId;
}

bool StopDocumentEC::parsePayload( connector::EditingCommandData* ecd ) {
	// stopDocument (baseID, documentID)
	std::string payload( (const char *) ecd->getPayload(), (size_t) ecd->getPayloadSize() );
	std::vector<std::string> tokens;
	boost::split( tokens, payload, boost::is_any_of( "," ) );
	int token_cnt = tokens.size();
	if (token_cnt > 1) {
		baseId = tokens[0];
		documentId = tokens[1];
		return true;
	} else {
		return false;
	}
}

AddLinkEC::AddLinkEC() :
		DocumentEC() {
}

AddLinkEC::~AddLinkEC() {
}

std::string AddLinkEC::getCompositeId() const {
	return compositeId;
}

std::string AddLinkEC::getXmlLink() const {
	return xmlLink;
}

void AddLinkEC::setData( std::string _documentId, std::string _compositeId, std::string _xmlLink ) {
	documentId = _documentId;
	compositeId = _compositeId;
	xmlLink = _xmlLink;
}

bool AddLinkEC::parsePayload( connector::EditingCommandData* ecd ) {
	// addLink(baseID, documentID, compositeId, xmlLink)
	unsigned int argsLength = 0;
	std::string payload( (const char *) ecd->getPayload(), (size_t) ecd->getPayloadSize() );
	std::vector<std::string> tokens;
	boost::split( tokens, payload, boost::is_any_of( "," ) );
	int token_cnt = tokens.size();
	if (token_cnt > 3) {
		baseId = tokens[0];
		argsLength += baseId.length();

		documentId = tokens[1];
		argsLength += documentId.length();

		compositeId = tokens[2];
		argsLength += compositeId.length();

		argsLength += 3; //por las comas
		xmlLink = payload.substr( argsLength, payload.length() - argsLength );

		return true;
	} else {
		return false;
	}
}

SetPropertyValueEC::SetPropertyValueEC() {
}

SetPropertyValueEC::~SetPropertyValueEC() {
}

std::string SetPropertyValueEC::getNodeId() const {
	return nodeId;
}

std::string SetPropertyValueEC::getPropertyId() const {
	return propertyId;
}

std::string SetPropertyValueEC::getPropertyValue() const {
	return propertyValue;
}

void SetPropertyValueEC::setData( std::string _documentId, std::string _nodeId, std::string _propertyId, std::string _propertyValue ) {
	documentId = _documentId;
	nodeId = _nodeId;
	propertyId = _propertyId;
	propertyValue = _propertyValue;
}

bool SetPropertyValueEC::parsePayload( connector::EditingCommandData* ecd ) {
	//setPropertyValue(baseID, documentID, nodeId, propertyId, value)
	unsigned int argsLength = 0;
	std::string payload( (const char *) ecd->getPayload(), (size_t) ecd->getPayloadSize() );
	std::vector<std::string> tokens;
	boost::split( tokens, payload, boost::is_any_of( "," ) );
	int token_cnt = tokens.size();
	if (token_cnt > 4) { // Hack - should fix split
		baseId = tokens[0];
		argsLength += baseId.length();

		documentId = tokens[1];
		argsLength += documentId.length();

		nodeId = tokens[2];
		argsLength += nodeId.length();

		propertyId = tokens[3];
		argsLength += propertyId.length();

		argsLength += 4; //por las comas        
		propertyValue = payload.substr( argsLength, payload.length() - argsLength );

		return true;
	} else {
		return false;
	}
}

AddNodeEC::AddNodeEC() :
		DocumentEC() {
}

AddNodeEC::~AddNodeEC() {
}

std::string AddNodeEC::getCompositeId() const {
	return compositeId;
}

std::string AddNodeEC::getXmlNode() const {
	return xmlNode;
}

void AddNodeEC::setData( std::string _documentId, std::string _compositeId, std::string _xmlNode ) {
	documentId = _documentId;
	compositeId = _compositeId;
	xmlNode = _xmlNode;
}

bool AddNodeEC::parsePayload( connector::EditingCommandData* ecd ) {
	//addNode (baseId, documentId,compositeId, {uri, id}+)
	std::string payload( (const char *) ecd->getPayload(), (size_t) ecd->getPayloadSize() );
	std::vector<std::string> tokens;
	boost::split( tokens, payload, boost::is_any_of( "," ) );
	int token_cnt = tokens.size();
	if (token_cnt > 3) {
		baseId = tokens[0];
		documentId = tokens[1];
		compositeId = tokens[2];
		xmlNode = tokens[3];
		return true;
	} else {
		return false;
	}
}

AddInterfaceEC::AddInterfaceEC() :
		DocumentEC() {
}

AddInterfaceEC::~AddInterfaceEC() {
}

std::string AddInterfaceEC::getNodeId() const {
	return nodeId;
}

std::string AddInterfaceEC::getXmlInterface() const {
	return xmlInterface;
}

void AddInterfaceEC::setData( std::string _documentId, std::string _nodeId, std::string _xmlInterface ) {
	documentId = _documentId;
	nodeId = _nodeId;
	xmlInterface = _xmlInterface;
}

bool AddInterfaceEC::parsePayload( connector::EditingCommandData* ecd ) {
	// addLink(baseID, documentID, compositeId, xmlLink)
	unsigned int argsLength = 0;
	std::string payload( (const char *) ecd->getPayload(), (size_t) ecd->getPayloadSize() );
	std::vector<std::string> tokens;
	boost::split( tokens, payload, boost::is_any_of( "," ) );
	int token_cnt = tokens.size();
	if (token_cnt > 3) {
		baseId = tokens[0];
		argsLength += baseId.length();

		documentId = tokens[1];
		argsLength += documentId.length();

		nodeId = tokens[2];
		argsLength += nodeId.length();

		argsLength += 3; //por las comas
		xmlInterface = payload.substr( argsLength, payload.length() - argsLength );
		return true;
	} else {
		return false;
	}
}

}
}
}
}
}
