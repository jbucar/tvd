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

#ifndef _PLAYERADAPTERMANAGER_H_
#define _PLAYERADAPTERMANAGER_H_

#include <util/types.h>
#include <string>
#include <map>

namespace player {
class System;
}

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
class FormatterPlayerAdapter;
class INclEditListener;
}
namespace model {
namespace presentation {
class CascadingDescriptor;
}
namespace event {
class AttributionEvent;
class FormatterEvent;
}
}
}
}
}
}
}

namespace bptgna = ::br::pucrio::telemidia::ginga::ncl::adapters;
namespace bptgnmp = ::br::pucrio::telemidia::ginga::ncl::model::presentation;
namespace bptgnme = ::br::pucrio::telemidia::ginga::ncl::model::event;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace components {
class ExecutionObject;
}
}
}
}
namespace ncl {
namespace animation {
class Animation;
}
namespace components {
class NodeEntity;
}
}
}
}
}
namespace bptna = ::br::pucrio::telemidia::ncl::animation;
namespace bptnc = ::br::pucrio::telemidia::ncl::components;
namespace bptgnmc = ::br::pucrio::telemidia::ginga::ncl::model::components;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {

/**
 * Esta clase hace manejo del estado de los players, interactuando con @c FormatterPlayerAdapter.
 */
class PlayerAdapterManager
{
public:
	PlayerAdapterManager( const std::string &docPath, player::System *system );
	virtual ~PlayerAdapterManager();

	bool start( bptgnmc::ExecutionObject* object );
	bool stop( bptgnmc::ExecutionObject* object, bool proccessTransitions=true );
	bool pause( bptgnmc::ExecutionObject* object );
	bool resume( bptgnmc::ExecutionObject* object );
	bool abort( bptgnmc::ExecutionObject* object );

	bool hasPrepared( bptgnmc::ExecutionObject* object );
	bool prepare( bptgnmc::ExecutionObject* object, bptgnme::FormatterEvent* event );
	bool setCurrentEvent( bptgnmc::ExecutionObject* object, bptgnme::FormatterEvent* event );
	void setProperty( bptgnmc::ExecutionObject* object, bptgnme::FormatterEvent* event, const std::string &value );
	bool startAnimation( bptgnmc::ExecutionObject* object, bptgnme::AttributionEvent* event, const std::string &value, bptna::Animation* animation );
	util::DWORD uptime( bptgnmc::ExecutionObject* object );
	bool setKeyHandler( bptgnmc::ExecutionObject* object, bool handler );
	void onTransitionTimer( bptgnmc::ExecutionObject* object, double ms );
	void proccessTransition( bptgnmc::ExecutionObject* object, double ms );
	bool getUrl( const std::string &fileName, std::string &url );

protected:
	bptgna::FormatterPlayerAdapter* getPlayer( bptgnmc::ExecutionObject* execObj, bool init=false );
	bptgna::FormatterPlayerAdapter* initializePlayer( bptgnmc::ExecutionObject* object );
	bool deletePlayer( const std::string &objectId );

private:
	typedef std::map<std::string, bptgna::FormatterPlayerAdapter*> Players;

	Players _players;
	std::string _docPath;
	player::System *_sys;
};

}
}
}
}
}
}

#endif //_PLAYERADAPTERMANAGER_H_
