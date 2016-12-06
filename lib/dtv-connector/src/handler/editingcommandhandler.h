/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#pragma once

#include "messagehandler.h"
#include <util/buffer.h>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>

// 241 bytes each EC + 128 posible sequence numbers
#define MAX_PAYLOAD_SIZE  (241*128)

#define MAX_EDITINGCOMMAND_SIZE 241

namespace connector {

class Connector;
class EditingCommandData;
typedef boost::shared_ptr<EditingCommandData> EditingCommandDataPtr;

class EditingCommandHandler : public MessageHandler {
public:
	EditingCommandHandler( void );
	virtual ~EditingCommandHandler( void );

	void send( Connector *conn, EditingCommandData* data );
	void send( Connector *conn, const ::util::Buffer &descriptorSection);

	virtual void process( ::util::Buffer *msg );
	virtual messages::type type() const;

	typedef boost::function<void ( const EditingCommandDataPtr &event )> EditingCommandCallbackT;
	void onEditingCommandHandler(const EditingCommandCallbackT  &callback);

	/* Used for testing... */
	int getIncompleteEditingCommandsCount();
private:

	EditingCommandCallbackT      _onEditingCommandHndl;
	/**
	* @brief Keeps Editing Commands data until its final flag is 0.
	* An editing command may arrive in several pieces, identified
	* by its increasing sequenceNumber until the last comes with
	* finalFlag == 0.
	*/
	std::map< ::util::BYTE , EditingCommandData*> _incompleteEditingCommands;
};

/**
*  @brief Editing command de Ginga.
*  The NPT field states when the command is to be executed.
*  The EventID field is used as defined in DSM-CC standard.
*  The commandTag states what type of command it is.
*  The sequence number and final flag are used when an EC
*  needs more than the available private data payload,
*  the command is sent as multiple EC, each numbering
*  the sequence number from 0 to last-1, and the final flag
*  is set only in the last EC of the sequence.
*/
class EditingCommandData {
public:
	EditingCommandData();
	EditingCommandData (unsigned char commandTag);
	void copyFrom (EditingCommandData* ecd);
	virtual ~EditingCommandData();

	virtual void setFinal(bool);
	virtual void setNPT(unsigned long long npt);
	virtual void setEventID(unsigned long);
	virtual void setSequenceNumber(unsigned char);
	void setPrivateDataPayload(const unsigned char* payload, int payloadSize);

	virtual void addPayload(const unsigned char*, const int length);
	virtual void payloadCompleted();

	virtual bool isFinal() const;
	virtual unsigned long long getNPT() const;
	virtual unsigned long getEventID() const;
	virtual unsigned char getCommandTag() const;
	virtual unsigned char getSequenceNumber() const;

	virtual int getPayloadSize() const;
	virtual const unsigned char* getPayload() const;

	// These two should be made in subclasses, but in all of them.
	// So I moved them here.
	/*   std::string getBaseID();
	void setBaseID   (std::string baseID);*/

protected:
	/**
	*  @brief Parses the private payload to setup
	*  subclasses specific fields.
	*  This method is called from setPayload method.
	*/
	virtual void parsePayloadImpl() {}

	/**
	*  @brief The private data field should be updated
	*  to reflect current EC status.
	*/
	virtual void updateBufferImpl() {}

	unsigned long      eventID;
	unsigned long long eventNPT;
	unsigned char      commandTag;

	unsigned char sequenceNumber;

	/**
	* @brief WARNING! finalFlag is 0 when this is the final
	* part of an editing command.
	*/
	bool finalFlag;
	
	::util::Buffer* privateDataPayload;
};

} // namespace Connector
