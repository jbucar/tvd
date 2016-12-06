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

#ifndef EDITINGCOMMANDEVENTS_H_
#define EDITINGCOMMANDEVENTS_H_

#include <string>
#include <vector>

namespace connector {
class EditingCommandData;
}

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {

//nclEdit base
static const unsigned char EC_OPEN_BASE = 0x0;
static const unsigned char EC_ACTIVATE_BASE = 0x1;
static const unsigned char EC_DEACTIVATE_BASE = 0x2;
static const unsigned char EC_SAVE_BASE = 0x3;
static const unsigned char EC_CLOSE_BASE = 0x4;

//nclEdit document
static const unsigned char EC_ADD_DOCUMENT = 0x5;
static const unsigned char EC_REMOVE_DOCUMENT = 0x6;
static const unsigned char EC_START_DOCUMENT = 0x7;
static const unsigned char EC_STOP_DOCUMENT = 0x8;
static const unsigned char EC_PAUSE_DOCUMENT = 0x9;
static const unsigned char EC_RESUME_DOCUMENT = 0xA;

//nclEdit region
static const unsigned char EC_ADD_REGION = 0xB;
static const unsigned char EC_REMOVE_REGION = 0xC;
static const unsigned char EC_ADD_REGION_BASE = 0xD;
static const unsigned char EC_REMOVE_REGION_BASE = 0xE;

//nclEdit rule
static const unsigned char EC_ADD_RULE = 0xF;
static const unsigned char EC_REMOVE_RULE = 0x10;
static const unsigned char EC_ADD_RULE_BASE = 0x11;
static const unsigned char EC_REMOVE_RULE_BASE = 0x12;

//nclEdit connector
static const unsigned char EC_ADD_CONNECTOR = 0x13;
static const unsigned char EC_REMOVE_CONNECTOR = 0x14;
static const unsigned char EC_ADD_CONNECTOR_BASE = 0x15;
static const unsigned char EC_REMOVE_CONNECTOR_BASE = 0x16;

//nclEdit descriptor
static const unsigned char EC_ADD_DESCRIPTOR = 0x17;
static const unsigned char EC_REMOVE_DESCRIPTOR = 0x18;
static const unsigned char EC_ADD_DESCRIPTOR_SWITCH = 0x19;
static const unsigned char EC_REMOVE_DESCRIPTOR_SWITCH = 0x1A;
static const unsigned char EC_ADD_DESCRIPTOR_BASE = 0x1B;
static const unsigned char EC_REMOVE_DESCRIPTOR_BASE = 0x1C;

//nclEdit transition
static const unsigned char EC_ADD_TRANSITION = 0x1D;
static const unsigned char EC_REMOVE_TRANSITION = 0x1E;
static const unsigned char EC_ADD_TRANSITION_BASE = 0x1F;
static const unsigned char EC_REMOVE_TRANSITION_BASE = 0x20;

//nclEdit reuse
static const unsigned char EC_ADD_IMPORT_BASE = 0x21;
static const unsigned char EC_REMOVE_IMPORT_BASE = 0x22;
static const unsigned char EC_ADD_IMPORTED_DOCUMENT_BASE = 0x23;
static const unsigned char EC_REMOVE_IMPORTED_DOCUMENT_BASE = 0x24;
static const unsigned char EC_ADD_IMPORT_NCL = 0x25;
static const unsigned char EC_REMOVE_IMPORT_NCL = 0x26;

//nclEdit components
static const unsigned char EC_ADD_NODE = 0x27;
static const unsigned char EC_REMOVE_NODE = 0x28;

//nclEdit interface
static const unsigned char EC_ADD_INTERFACE = 0x29;
static const unsigned char EC_REMOVE_INTERFACE = 0x2A;

//nclEdit link
static const unsigned char EC_ADD_LINK = 0x2B;
static const unsigned char EC_REMOVE_LINK = 0x2C;

static const unsigned char EC_SET_PROPERTY_VALUE = 0x2D;

class EditingCommand
{
public:
	EditingCommand();
	virtual ~EditingCommand();

	std::string getBaseId();
	void setBaseId( std::string baseId );

	virtual bool parsePayload( connector::EditingCommandData* ecd ) = 0;

protected:
	std::string baseId;
};

class DocumentEC: public EditingCommand
{
public:
	DocumentEC();
	virtual ~DocumentEC();
	std::string getDocumentId() const;
	void setDocumentId( std::string );

protected:
	std::string documentId;
};

class AddDocumentEC: public EditingCommand
{
public:
	AddDocumentEC();
	virtual ~AddDocumentEC();

	int getDocumentCount();
	std::string getDocumentUri( int index );
	std::string getDocumentIor( int index );

	void addDocument( std::string uri, std::string ior );

	bool parsePayload( connector::EditingCommandData* ecd );
protected:

	std::vector<std::string> documentUris;
	std::vector<std::string> documentIors;
};

class StartDocumentEC: public DocumentEC
{
public:
	StartDocumentEC();
	virtual ~StartDocumentEC();

	std::string getInterfaceId() const;

	void setData( std::string documentId/*, std::string locationId*/, std::string interfaceId = "" );

	bool parsePayload( connector::EditingCommandData* ecd );

protected:
	std::string interfaceId;
};

class StopDocumentEC: public DocumentEC
{
public:
	StopDocumentEC();
	virtual ~StopDocumentEC();

	void setData( std::string documentId );

	bool parsePayload( connector::EditingCommandData* ecd );
};

class AddLinkEC: public DocumentEC
{
public:
	AddLinkEC();
	virtual ~AddLinkEC();

	std::string getCompositeId() const;
	std::string getXmlLink() const;

	void setData( std::string _documentId, std::string _compositeId, std::string _xmlLink );

	bool parsePayload( connector::EditingCommandData* ecd );
protected:
	std::string compositeId;
	std::string xmlLink;
};

class SetPropertyValueEC: public DocumentEC
{
public:
	SetPropertyValueEC();
	virtual ~SetPropertyValueEC();

	std::string getNodeId() const;
	std::string getPropertyId() const;
	std::string getPropertyValue() const;

	void setData( std::string _documentId, std::string _nodeId, std::string _propertyId, std::string _propertyValue );

	bool parsePayload( connector::EditingCommandData* ecd );
protected:

	std::string nodeId;
	std::string propertyId;
	std::string propertyValue;
};

class AddNodeEC: public DocumentEC
{
public:
	AddNodeEC();
	virtual ~AddNodeEC();

	std::string getCompositeId() const;
	std::string getXmlNode() const;

	void setData( std::string _documentId, std::string _compositeId, std::string _xmlNode );

	bool parsePayload( connector::EditingCommandData* ecd );
protected:

	std::string compositeId;
	std::string xmlNode;
};

class AddInterfaceEC: public DocumentEC
{
public:
	AddInterfaceEC();
	virtual ~AddInterfaceEC();

	std::string getNodeId() const;
	std::string getXmlInterface() const;

	void setData( std::string _documentId, std::string _compositeId, std::string _xmlInterface );

	bool parsePayload( connector::EditingCommandData* ecd );
protected:

	std::string nodeId;
	std::string xmlInterface;
};

}
}
}
}
}
#endif /*EDITINGCOMMANDEVENTS_H_*/
