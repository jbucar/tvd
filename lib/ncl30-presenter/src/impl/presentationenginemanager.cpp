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

#include "presentationenginemanager.h"
#include "editingcommandevents.h"
#include "formattermediator.h"
#include <connector/handler/editingcommandhandler.h>
#include <gingaplayer/system.h>
#include <gingaplayer/device.h>
#include <canvas/system.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/function.hpp>

#define PROCESS_EDIT( name, event )	{ \
		name *ev = new name(); \
		if (ev->parsePayload(event)) { \
			LINFO("PresentationEngineManagerImpl", "%s event received", #name ); \
			if ((ev->getBaseId() != baseId()) && (ev->getBaseId() != "null")) { \
				LWARN("PresentationEngineManagerImpl", "BaseId %s not found", ev->getBaseId().c_str()); \
			} \
			else { \
				_formatter->editingCommand(ev); \
			} \
		} \
		delete ev; }

using std::string;

using ::br::pucrio::telemidia::ginga::ncl::EC_ACTIVATE_BASE;
using ::br::pucrio::telemidia::ginga::ncl::EC_OPEN_BASE;
using ::br::pucrio::telemidia::ginga::ncl::EC_DEACTIVATE_BASE;
using ::br::pucrio::telemidia::ginga::ncl::EC_SAVE_BASE;
using ::br::pucrio::telemidia::ginga::ncl::EC_CLOSE_BASE;
using ::br::pucrio::telemidia::ginga::ncl::EC_ADD_DOCUMENT;
using ::br::pucrio::telemidia::ginga::ncl::EC_START_DOCUMENT;
using ::br::pucrio::telemidia::ginga::ncl::EC_STOP_DOCUMENT;
using ::br::pucrio::telemidia::ginga::ncl::EC_ADD_LINK;
using ::br::pucrio::telemidia::ginga::ncl::EC_SET_PROPERTY_VALUE;
using ::br::pucrio::telemidia::ginga::ncl::EC_ADD_NODE;
using ::br::pucrio::telemidia::ginga::ncl::EC_ADD_INTERFACE;

using ::br::pucrio::telemidia::ginga::ncl::AddDocumentEC;
using ::br::pucrio::telemidia::ginga::ncl::StartDocumentEC;
using ::br::pucrio::telemidia::ginga::ncl::StopDocumentEC;
using ::br::pucrio::telemidia::ginga::ncl::AddLinkEC;
using ::br::pucrio::telemidia::ginga::ncl::SetPropertyValueEC;
using ::br::pucrio::telemidia::ginga::ncl::AddNodeEC;
using ::br::pucrio::telemidia::ginga::ncl::AddInterfaceEC;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {

PresentationEngineManagerImpl::PresentationEngineManagerImpl( player::System *sys ) :
	PresentationEngineManager( sys ) {
	_formatter = NULL;
}

PresentationEngineManagerImpl::~PresentationEngineManagerImpl() {
}

FormatterMediator *PresentationEngineManagerImpl::formatter() {
	return _formatter;
}

void PresentationEngineManagerImpl::initFormatter( const std::string &file ) {
	_formatter = new FormatterMediatorImpl( baseId(), file, sys() );
}

void PresentationEngineManagerImpl::delFormatter() {
	DEL( _formatter );
}

void PresentationEngineManagerImpl::onStart() {
	LDEBUG( "PresentationEngineManagerImpl", "Setup editing command" );
	sys()->onEditingCommand( boost::bind( &PresentationEngineManagerImpl::connectorEditingCommandProcess, this, _1 ) );
}

void PresentationEngineManagerImpl::onStop() {
	LDEBUG( "PresentationEngineManagerImpl", "Cleanup editing command" );
	boost::function<void( connector::EditingCommandData *event )> empty;
	sys()->onEditingCommand( empty );
}

void PresentationEngineManagerImpl::addDocumentEvent( AddDocumentEC *event ) {
	LINFO( "PresentationEngineManagerImpl", "AddDocumentEvent received" );

	if ((event->getBaseId() != baseId()) && (event->getBaseId() != "null")) {
		LWARN( "PresentationEngineManagerImpl", "BaseId %s not found", event->getBaseId().c_str() );
		return;
	}

	for (int i = 0; i < event->getDocumentCount(); ++i) {
		if (event->getDocumentIor( i ) == "") {
			string docUri = event->getDocumentUri( i );

			LINFO( "PresentationEngineManagerImpl", "connectorAddDocumentEvent createNclPlayer" );
			startDocument( docUri );

			LDEBUG( "PresentationEngineManagerImpl", "connectorAddDocumentEvent createNclPlayer done" );
		} else {
			// TODO This must be an StreamEvent received addDocument.
			LWARN( "PresentationEngineManagerImpl", "Unsupported AddDocument format: IOR" );
		}
	}
	LINFO( "PresentationEngineManagerImpl", "AddDocumentEvent processed" );
}

void PresentationEngineManagerImpl::connectorEditingCommandProcess( connector::EditingCommandData *event ) {
	unsigned char commandTag = event->getCommandTag();
	switch ( commandTag ) {
		case EC_ADD_DOCUMENT: {
			AddDocumentEC* adec = new AddDocumentEC();
			if (adec->parsePayload( event )) {
				addDocumentEvent( adec );
			}
			delete adec;
			break;
		}
		case EC_START_DOCUMENT:
			PROCESS_EDIT( StartDocumentEC, event );
			break;
		case EC_STOP_DOCUMENT:
			PROCESS_EDIT( StopDocumentEC, event );
			break;
		case EC_ADD_LINK:
			PROCESS_EDIT( AddLinkEC, event );
			break;
		case EC_SET_PROPERTY_VALUE:
			PROCESS_EDIT( SetPropertyValueEC, event );
			break;
		case EC_ADD_NODE:
			PROCESS_EDIT( AddNodeEC, event );
			break;
		case EC_ADD_INTERFACE:
			PROCESS_EDIT( AddInterfaceEC, event );
			break;
		default:
			break;
	}
}

}
}
}
}
}
