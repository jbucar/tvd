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

#ifndef _FORMATTEREVENT_H_
#define _FORMATTEREVENT_H_

#include "ncl30/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include <string>
#include <iostream>
#include <set>
using namespace std;

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
namespace model {
namespace components {
class ExecutionObject;
}
namespace event {
class IEventListener;

/**
 * Clase formateadora de eventos.
 */
class FormatterEvent
{

public:
	FormatterEvent( const std::string &id, components::ExecutionObject* eObject, ncl30presenter::event::EventManager *manager );
	virtual ~FormatterEvent();

	bool instanceOf( const std::string &s );
	const std::string &getId();
	components::ExecutionObject* getExecutionObject();

	void addEventListener( IEventListener* listener );
	bool delEventListener( IEventListener* listener );

	virtual bool start();
	virtual bool stop();
	bool abort();
	bool pause();
	bool resume();
	void setCurrentState( short newState );
	short getCurrentState();
	long getOccurrences();
	static string getStateName( short state );

protected:
	short getNewState( short transition );
	short getTransition( short newState );
	bool changeState( short newState, short transition );
	set<string> typeSet;
	short currentState;
	long occurrences;

private:
	static const short ST_ABORTED = 50;
	std::string _id;
	components::ExecutionObject* _eObject;
	ncl30presenter::event::EventManager* _eventManager;
	bool deleting;

};
}
}
}
}
}
}
}

#endif //_FORMATTEREVENT_H_
