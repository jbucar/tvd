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

#ifndef _FORMATTERPLAYER_H_
#define _FORMATTERPLAYER_H_

#include "ncl30/components/Content.h"
#include "ncl30/components/ContentNode.h"
#include "ncl30/components/NodeEntity.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl30/interfaces/LambdaAnchor.h"
#include "ncl30/interfaces/IntervalAnchor.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl30/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "../model/CompositeExecutionObject.h"
#include "../model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "../model/AttributionEvent.h"
#include "../model/FormatterEvent.h"
#include "../model/PresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "INclEditListener.h"

#include "../model/AttributionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include <ncl30/animation/Animation.h>
namespace bptna = ::br::pucrio::telemidia::ncl::animation;

#include "PlayerAdapterManager.h"

#include <gingaplayer/player/lua/event/types.h>
#include <canvas/types.h>
#include <util/types.h>

#include <string>

namespace player {
class Player;
}

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {

namespace adapters {

typedef std::map<std::string, std::string> Properties;

/**
 * Esta clase realiza el manejo de los players. Desde crearlos, hasta setearles las propiedas leidas de los elementos @c media del 
 * documento NCL.
 */
class FormatterPlayerAdapter: public IAttributeValueMaintainer
{

public:
	FormatterPlayerAdapter( PlayerAdapterManager* manager, player::System *system );
	virtual ~FormatterPlayerAdapter();

	bool instanceOf( string s );

	bool setKeyHandler( bool isHandler );
	string getPropertyValue( AttributionEvent* event );

	void setPlayer( player::Player* player );
	void stopPlayer();

	virtual bool start();
	virtual bool stop( bool proccessTransitions=true );
	virtual bool pause();
	virtual bool resume();
	virtual bool abort();

	virtual bool prepare( ExecutionObject* object, FormatterEvent* mainEvent );
	virtual bool hasPrepared();

	virtual bool setPropertyValue( AttributionEvent* event, const std::string &value );
	virtual bool setPropertyToPlayer( const std::string &name, const std::string &value );

	virtual bool setCurrentEvent( FormatterEvent* event );

	// animations
	bool startAnimation( AttributionEvent* event, const std::string &value, bptna::Animation* animation );
	void stopAnimation( bptgnan::Animation* animation );

	ExecutionObject* getObject( void ) const;
	player::Player* getPlayer( void ) const;
	player::System* getSystem();
	PlayerAdapterManager* getManager( void ) const;

	virtual void timeShift( const std::string &direction );
	::util::DWORD uptime() const;
	void processTransitions( void );

	//	Key callback
	typedef boost::function<void( ::util::key::type key )> KeyPressCallback;
	static void onKeyPress( const KeyPressCallback &callback );
	void onReserveKeys( util::key::Keys keys );

protected:
	bool setDescriptorParams();
	bool checkRepeat( PresentationEvent* mainEvent );

	virtual bool createPlayer();

	const char *toPlayerProperty( const std::string &propertyName );
	virtual void setProperty( const std::string &propertyName, const std::string &propertyValue );

	set<string> getTypeSet( void ) const;

	void setObject( ExecutionObject* object );

	virtual void userEventReceived( ::util::key::type key, bool isUp );
	virtual void onButtonEvent( canvas::input::ButtonEvent *evt );
	bool isKeyHandler( void ) const;

private:
	player::Player* _player;
	PlayerAdapterManager* manager;
	set<string> typeSet;
	ExecutionObject* object;
	Properties _properties;
	bool _isKeyhandler;
	player::System *_sys;
	boost::signals2::connection _onButton;
};
}
}
}
}
}
}

#endif //_FORMATTERPLAYER_H_
