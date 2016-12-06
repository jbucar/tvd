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

#ifndef _DESCRIPTOR_H_
#define _DESCRIPTOR_H_

#include "GenericDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
class Parameter;

namespace layout {
class LayoutRegion;
}
namespace navigation {
class FocusDecoration;
class KeyNavigation;
}
namespace transition {
class Transition;
}
}
}
}
}
namespace bptn = ::br::pucrio::telemidia::ncl;
namespace bptnl = ::br::pucrio::telemidia::ncl::layout;
namespace bptnn = ::br::pucrio::telemidia::ncl::navigation;
namespace bptnt = ::br::pucrio::telemidia::ncl::transition;

#include <string>
#include <map>
#include <vector>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace descriptor {
class Descriptor: public GenericDescriptor
{
public:
	static const short FIT_FILL = 0;
	static const short FIT_HIDDEN = 1;
	static const short FIT_MEET = 2;
	static const short FIT_MEETBEST = 3;
	static const short FIT_SLICE = 4;

	static const short SCROLL_NONE = 0;
	static const short SCROLL_HORIZONTAL = 1;
	static const short SCROLL_VERTICAL = 2;
	static const short SCROLL_BOTH = 3;
	static const short SCROLL_AUTOMATIC = 4;

protected:
	double explicitDuration;
	std::string presentationTool;
	bool freeze;
	long repetitions;
	bptnl::LayoutRegion* region;
	std::map<std::string, bptn::Parameter*>* parameters;

	bptnn::KeyNavigation* keyNavigation;
	bptnn::FocusDecoration* focusDecoration;
	std::vector<bptnt::Transition*>* inputTransitions;
	std::vector<bptnt::Transition*>* outputTransitions;

public:
	explicit Descriptor( const std::string &id );
	virtual ~Descriptor();
	double getExplicitDuration();
	std::string getPlayerName();
	bptnl::LayoutRegion* getRegion();
	long getRepetitions();
	bool isFreeze();
	void setFreeze( bool freeze );
	void setExplicitDuration( double d );
	void setPlayerName( const std::string &name );
	void setRegion( bptnl::LayoutRegion* region );
	void setRepetitions( long r );
	void addParameter( bptn::Parameter *parameter );
	std::vector<bptn::Parameter*> *getParameters();
	Parameter *getParameter( const std::string &paramName );
	void removeParameter( bptn::Parameter *parameter );
	bptnn::KeyNavigation* getKeyNavigation();
	void setKeyNavigation( bptnn::KeyNavigation* keyNav );
	bptnn::FocusDecoration* getFocusDecoration();
	void setFocusDecoration( bptnn::FocusDecoration* focusDec );
	std::vector<bptnt::Transition*>* getInputTransitions();
	bool addInputTransition( bptnt::Transition* transition, int pos );
	void removeInputTransition( bptnt::Transition* transition );
	void removeAllInputTransitions();
	std::vector<bptnt::Transition*>* getOutputTransitions();
	bool addOutputTransition( bptnt::Transition* transition, int pos );
	void removeOutputTransition( bptnt::Transition* transition );
	void removeAllOutputTransitions();
};
}
}
}
}
}

#endif //_DESCRIPTOR_H_
