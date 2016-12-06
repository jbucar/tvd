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

#ifndef FORMATTERREGION_H_
#define FORMATTERREGION_H_

#include "ncl30/layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl30/descriptor/Descriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl30/transition/Transition.h"
#include "ncl30/transition/TransitionUtil.h"
using namespace ::br::pucrio::telemidia::ncl::transition;

#include <util/types.h>
#include <vector>
#include <string>
using namespace std;

namespace player {
class Player;
class System;
}

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace transition {
class Transition;
}
}

namespace ginga {
namespace ncl {

namespace model {
namespace presentation {
class CascadingDescriptor;
}
}
namespace adapters {
class FormatterPlayerAdapter;
}

namespace animation {
class Animation;
}

namespace bptgnmp = ::br::pucrio::telemidia::ginga::ncl::model::presentation;
namespace bptgnan = ::br::pucrio::telemidia::ginga::ncl::animation;
namespace bptnt = ::br::pucrio::telemidia::ncl::transition;
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

namespace model {
namespace presentation {
class FormatterRegion
{

public:
	FormatterRegion( string objectId, bptgnmp::CascadingDescriptor* descriptor );
	~FormatterRegion();

	bool updateRegionBounds();

	LayoutRegion* getLayoutRegion();
	void prepareOutputDisplay( FormatterPlayerAdapter* player );
	void unprepareOutputDisplay( void );

	bool isVisible();
	short getFocusState();
	bool isSelected();
	bool setSelection( bool selected );
	void setFocus( bool focus );

	bool setPropertyToPlayer( const std::string &name, const std::string &value );

	//	Animations
	bool startAnimation( bptgnan::Animation* animation );
	void stopAnimation( bptgnan::Animation* animation );
	void stepAnimation( bptgnan::Animation* animation );

	void enabled( bool enable );
	::util::DWORD uptime() const;

	//	Transitions
	void transitionIn( void );
	void transitionOut( void );
	bool haveTransitionIn( void );
	bool haveTransitionOut( void );
	bptnt::Transition *getSupportedTransition( vector<bptnt::Transition*>* transitions );

	void processTransition( bptnt::Transition* transition, bool onPlay );
	void barWipe( bptnt::Transition* transition, bool onPlay );
	void fade( bptnt::Transition* transition, bool onPlay );

private:
	LayoutRegion* _region;
	FormatterPlayerAdapter* _player;
	string _objectId;
	bool _selected;
	bool _enabled;
	vector<bptgnan::Animation*> _animations;
	bptnt::Transition* _transitionIn;
	bptnt::Transition* _transitionOut;
	player::System *_sys;
};
}
}
}
}
}
}
}

struct transT
{
	::br::pucrio::telemidia::ginga::ncl::model::presentation::FormatterRegion* fr;
	Transition* t;
};

#endif /*FORMATTERREGION_H_*/
