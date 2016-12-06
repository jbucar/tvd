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

#ifndef _SIMPLEACTION_H_
#define _SIMPLEACTION_H_

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace animation {
class Animation;
}
}
}
}
}
namespace bptna = ::br::pucrio::telemidia::ncl::animation;

#include "Action.h"
#include "Role.h"
#include <string>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
  
/**
 * La clase @c SimpleAction define una acción a realizar en un connector.
 */
class SimpleAction: public Action, public Role
{
private:
	short actionType;
	short qualifier;
	std::string repeat;
	std::string repeatDelay;
	std::string value;
	bptna::Animation* animation;

public:
	static const short ACT_START = 1;
	static const short ACT_PAUSE = 2;
	static const short ACT_RESUME = 3;
	static const short ACT_STOP = 4;
	static const short ACT_ABORT = 5;
	static const short ACT_SET = 6;

	explicit SimpleAction( const std::string &role );
	virtual ~SimpleAction();

	void setLabel( const std::string &id );

	short getQualifier();
	void setQualifier( short qualifier );
	const std::string &getRepeat();
	const std::string &getRepeatDelay();
	void setRepeatDelay( const std::string &time );
	void setRepeat( const std::string &newRepetitions );
	short getActionType();
	void setActionType( short action );
	const std::string &getValue();
	void setValue( const std::string &value );
	bptna::Animation* getAnimation();
	void setAnimation( bptna::Animation* animation );

	bool instanceOf( const std::string &type ) {
		return Action::instanceOf( type );
	}
};
}
}
}
}
}

#endif //_SIMPLEACTION_H_
