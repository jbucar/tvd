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
#pragma once

#include "../model/CascadingDescriptor.h"
#include "../model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "../model/SelectionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "../model/FormatterRegion.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include "../adapters/PlayerAdapterManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include <boost/function.hpp>
#include <string>
#include <vector>
#include <set>
#include <map>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace emconverter {
class FormatterConverter;
}
namespace focus {
  
/**
 * Realiza el control del foco.
 */
class FormatterFocusManager

{

public:
	FormatterFocusManager( PlayerAdapterManager* playerManager, emconverter::FormatterConverter* converter, player::System *system );
	virtual ~FormatterFocusManager();

	bool setKeyHandler( bool isHandler );
	void setKeyMaster( const std::string &focusIndex );
	void setFocus( const std::string &focusIndex );
	void showObject( ExecutionObject* object );
	void hideObject( ExecutionObject* object );
	void registerNavigationKeys();

	//	Key callback
	typedef boost::function<void( ::util::key::type key )> KeyPressCallback;
	static void onKeyPress( const KeyPressCallback &callback );
	void onReserveKeys( util::key::Keys keys );

	bool userEventReceived( ::util::key::type key, bool isUp );

protected:
	void setHandlingObjects( bool isHandling );
	void recoveryDefaultState( ExecutionObject* object );

	ExecutionObject* getObjectFromFocusIndex( const std::string &focusIndex );
	void insertObject( ExecutionObject* object, const std::string &focusIndex );
	void removeObject( ExecutionObject* object, const std::string &focusIndex );

	bool keyCodeOk( ExecutionObject* currentObject );
	bool keyCodeBack();
	bool enterSelection( ExecutionObject* object );
	void exitSelection( ExecutionObject* object );
	void unregister();

	void changeSettingState( const std::string &name, const std::string &act );

private:
	bool isHandler;

	map<string, set<ExecutionObject*>*> focusTable;
	std::vector<std::string> focusSequence;
	std::string currentFocus;
	std::string objectToSelect;

	ExecutionObject* selectedObject;
	PlayerAdapterManager* playerManager;
	emconverter::FormatterConverter* converter;
	player::System *_sys;

};
}
}
}
}
}
}

