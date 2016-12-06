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

#ifndef _CASCADINGDESCRIPTOR_H_
#define _CASCADINGDESCRIPTOR_H_

#include "ncl30/Parameter.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "ncl30/descriptor/Descriptor.h"
#include "ncl30/descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl30/layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl30/switches/DescriptorSwitch.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "ncl30/navigation/FocusDecoration.h"
#include "ncl30/navigation/KeyNavigation.h"
using namespace ::br::pucrio::telemidia::ncl::navigation;

#include "ncl30/transition/Transition.h"
using namespace ::br::pucrio::telemidia::ncl::transition;

#include "FormatterRegion.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include <string>
#include <map>
#include <vector>
#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace presentation {
class CascadingDescriptor
{
protected:
	set<string> typeSet;

private:
	string id;
	vector<GenericDescriptor*>* descriptors;
	vector<GenericDescriptor*>* unsolvedDescriptors;
	map<string, string>* parameters;
	double explicitDuration;
	string playerName;
	long repetitions;
	bool freeze;
	LayoutRegion* region;
	FormatterRegion* formatterRegion;

	string focusIndex, moveUp, moveDown, moveLeft, moveRight;
	vector<Transition*>* inputTransitions;
	vector<Transition*>* outputTransitions;
	FocusDecoration* _focusDecoration;

	void cascadeDescriptor( Descriptor* descriptor );

public:
	CascadingDescriptor( GenericDescriptor* firstDescriptor );
	CascadingDescriptor( CascadingDescriptor* descriptor );
	~CascadingDescriptor();

	bool instanceOf( string s );
	string getId();
	bool isLastDescriptor( GenericDescriptor* descriptor );
	void cascade( GenericDescriptor* preferredDescriptor );
	GenericDescriptor* getFirstUnsolvedDescriptor();
	vector<GenericDescriptor*>* getUnsolvedDescriptors();
	void cascadeUnsolvedDescriptor();
	double getExplicitDuration();
	bool getFreeze();
	string getPlayerName();
	LayoutRegion* getRegion();
	FormatterRegion* getFormatterRegion();
	void setFormatterLayout();
	long getRepetitions();
	vector<Parameter*>* getParameters();
	string getParameterValue( string paramName );
	vector<GenericDescriptor*>* getNcmDescriptors();
	const std::string &getFocusBorderColor();
	const std::string &getFocusBorderTransparency();
	const std::string &getFocusBorderWidth();
	void setFocusIndex(const std::string &value);
	string getFocusIndex();
	string getFocusSrc();
	string getSelectionSrc();
	string getMoveDown();
	string getMoveLeft();
	string getMoveRight();
	string getMoveUp();
	const std::string &getSelBorderColor();
	vector<Transition*>* getInputTransitions();
	vector<Transition*>* getOutputTransitions();
	FocusDecoration* focusDecoration();

protected:
	void initializeCascadingDescriptor();

};
}
}
}
}
}
}
}

#endif //_CASCADINGDESCRIPTOR_H_
