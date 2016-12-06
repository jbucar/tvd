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

#ifndef _RULEADAPTER_H_
#define _RULEADAPTER_H_

#include "../model/CascadingDescriptor.h"
#include "../model/CompositeExecutionObject.h"
#include "../model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "../model/ExecutionObjectSwitch.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::switches;

#include "ncl30/descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl30/switches/DescriptorSwitch.h"
#include "ncl30/switches/CompositeRule.h"
#include "ncl30/switches/Rule.h"
#include "ncl30/switches/SimpleRule.h"
#include "ncl30/switches/SwitchNode.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "ncl30/util/Comparator.h"

#include "ncl30/components/Node.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "math.h"

#include <map>
#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adaptation {
namespace context {
class RuleAdapter
{
private:
	map<string, vector<Rule*>*>* ruleListenMap;
	map<Rule*, vector<ExecutionObjectSwitch*>*>* entityListenMap;
	map<Rule*, vector<DescriptorSwitch*>*>* descListenMap;

public:
	RuleAdapter();
	virtual ~RuleAdapter();

	void reset();
	void adapt( CompositeExecutionObject* compositeObject, bool force );
	void initializeAttributeRuleRelation( Rule* topRule, Rule* rule );

	void initializeRuleObjectRelation( ExecutionObjectSwitch* object );

	void adapt( ExecutionObjectSwitch* objectAlternatives, bool force );

	bool adaptDescriptor( ExecutionObject* executionObject );
	Node* adaptSwitch( SwitchNode* switchNode );
	bool evaluateRule( Rule* rule );

private:
	bool evaluateCompositeRule( CompositeRule* rule );
	bool evaluateSimpleRule( SimpleRule* rule );

public:
	virtual void update( void* arg0, void* arg1 );
};
}
}
}
}
}
}
}

#endif //_RULEADAPTER_H_
