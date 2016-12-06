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

#ifndef _APPLICATIONPLAYERADAPTER_H_
#define _APPLICATIONPLAYERADAPTER_H_

#include "../model/ApplicationExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "FormatterPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include "INclEditListener.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace application {
  
/**
 * Esta clase hereda de @c FormatterPlayerAdapter. También realiza el manejo de los players, como iniciar la ejecución o detenerlos.
 */
class ApplicationPlayerAdapter: public FormatterPlayerAdapter
{
private:
	INclEditListener* editingCommandListener;

protected:
	map<string, FormatterEvent*>* preparedEvents;
	FormatterEvent* currentEvent;

public:
	ApplicationPlayerAdapter( PlayerAdapterManager* manager, player::System *system );
	virtual ~ApplicationPlayerAdapter();

	void setNclEditListener( INclEditListener* listener );
	virtual bool hasPrepared();
	virtual bool prepare( ExecutionObject* object, FormatterEvent* mainEvent );

protected:
	void prepare( FormatterEvent* event );
	virtual void setProperty( const std::string &propertyName, const std::string &propertyValue );
	virtual void userEventReceived( ::util::key::type key, bool isUp );

public:
	virtual bool start();
	virtual bool stop( bool proccessTransitions=true );
	virtual void onEvent( player::event::nclEvtType::type type, player::event::evtAction::type action, const std::string &parameter = "", const std::string &value = "" );

	virtual bool setCurrentEvent( FormatterEvent* event );
	void onReserveKeys( util::key::Keys keys );

};
}
}
}
}
}
}
}

#endif //_ApplicationPlayerAdapter_H_
