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

#include "../../../include/model/LinkAction.h"
#include <gingaplayer/system.h>
#include <util/log.h>
#include <boost/bind.hpp>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
LinkAction::LinkAction( player::System *system ) {
	this->delay = 0;
	progressionListeners = new vector<LinkActionProgressionListener*> ;
	_sys = system;
	typeSet.insert( "LinkAction" );
}

LinkAction::~LinkAction() {
	if (progressionListeners != NULL) {
		delete progressionListeners;
		progressionListeners = NULL;
	}
}

bool LinkAction::instanceOf( string s ) {
	if (typeSet.empty())
		return false;
	else
		return (typeSet.find( s ) != typeSet.end());
}

double LinkAction::getWaitDelay() {
	return this->delay;
}

void LinkAction::setWaitDelay( double delay ) {
	this->delay = delay;
}

bool LinkAction::hasDelay() {
	return getWaitDelay() > 0;
}

void LinkAction::run( double delay/*=0*/) {
	if (delay || hasDelay()) {
		LINFO("LinkAction", "Timer started: %g o %g ms", delay, getWaitDelay());
		_sys->registerTimer( (util::DWORD) (delay ? delay : getWaitDelay()), boost::bind( &LinkAction::onRun, this ) );
	} else {
		onRun();
	}
}

void LinkAction::onRun() {
}

void LinkAction::addActionProgressionListener( LinkActionProgressionListener* listener ) {

	vector<LinkActionProgressionListener*>::iterator i;
	i = progressionListeners->begin();
	while (i != progressionListeners->end()) {
		if (listener == *i) {
			LWARN("LinkAction", "Trying to add the same listener twice");
			return;
		}
		++i;
	}
	progressionListeners->push_back( listener );
}

void LinkAction::removeActionProgressionListener( LinkActionProgressionListener* listener ) {

	vector<LinkActionProgressionListener*>::iterator i;
	for (i = progressionListeners->begin(); i != progressionListeners->end(); ++i) {

		if (*i == listener) {
			progressionListeners->erase( i );
			break;
		}
	}
}

void LinkAction::notifyProgressionListeners( bool start ) {
	int i, size;
	LinkActionProgressionListener* listener;

	size = progressionListeners->size();
	for (i = 0; i < size; i++) {
		listener = (*progressionListeners)[i];
		listener->actionProcessed( start );
	}
}
}
}
}
}
}
}
}
