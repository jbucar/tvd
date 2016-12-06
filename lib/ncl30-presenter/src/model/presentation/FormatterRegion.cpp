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

#include "../../../include/model/FormatterRegion.h"
#include "../../../include/model/CascadingDescriptor.h"
#include "../../../include/adapters/FormatterPlayerAdapter.h"
#include "../../animation/animation.h"
#include <ncl30/util/functions.h>
#include <ncl30/transition/Transition.h>

#include <gingaplayer/player.h>
#include <canvas/point.h>
#include <canvas/size.h>
#include <canvas/rect.h>
#include <util/log.h>
#include <util/mcr.h>
#include "generated/config.h"

#include <boost/lexical_cast.hpp>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace presentation {
FormatterRegion::FormatterRegion( string objectId, bptgnmp::CascadingDescriptor* descriptor ) {

	_objectId = objectId;
	_player = NULL;
	_selected = false;
	_enabled = true;
	if (descriptor != NULL) {
		_region = descriptor->getRegion()->cloneRegion();
		_transitionIn = getSupportedTransition( descriptor->getInputTransitions() );
		_transitionOut = getSupportedTransition( descriptor->getOutputTransitions() );
	}
	_sys = NULL;
}

FormatterRegion::~FormatterRegion() {
	CLEAN_ALL( bptgnan::Animation *, _animations );
	if (_region != NULL) {
		DEL( _region );
	}
}

bool FormatterRegion::updateRegionBounds() {
	bool result = true; //	TODO false, Es porque hay un llamado donde no tiene player aun
	if (_player != NULL && _player->getPlayer()->isGraphic() && _enabled) {
		canvas::Rect region( _region->getLeft(), _region->getTop(), _region->getWidth(), _region->getHeight() );

		LDEBUG("FormatterRegion", "bounds: x=%d, y=%d, w=%d, h=%d, z=%d", region.x, region.y, region.w, region.h, _region->getZIndex());
		result &= _player->getPlayer()->setProperty( "bounds", region );
		result &= _player->getPlayer()->setProperty( "zIndex", _region->getZIndex() );
	}

	return result;
}

LayoutRegion* FormatterRegion::getLayoutRegion() {
	return _region;
}

void FormatterRegion::prepareOutputDisplay( FormatterPlayerAdapter* player ) {
	_player = player;
	updateRegionBounds();
	_sys = _player->getSystem();
}

void FormatterRegion::unprepareOutputDisplay( void ) {
	_player = NULL;
	CLEAN_ALL( bptgnan::Animation *, _animations );
}

bool FormatterRegion::isVisible() {
	return _player->getPlayer()->isVisible();
}

bool FormatterRegion::isSelected() {
	return _selected;
}

bool FormatterRegion::setSelection( bool selected ) {
	if (_selected != selected) {
		_selected = selected;
		_player->getPlayer()->setProperty( "focusSelected", _selected );
		return _selected;
	}
	return false;
}

void FormatterRegion::setFocus( bool focus ) {
	_player->getPlayer()->setProperty( "focus", focus );
	_player->setKeyHandler( focus );
}

bool FormatterRegion::setPropertyToPlayer( const std::string &name, const std::string &value ) {
	return _player->setPropertyToPlayer( name, value );
}

//	Animations
bool FormatterRegion::startAnimation( bptgnan::Animation* animation ) {
	if (animation->start( this )) {
		_animations.push_back( animation );
		return true;
	} else {
		DEL(animation)
		return false;
	}
}

void FormatterRegion::stopAnimation( bptgnan::Animation* animation ) {
	std::vector<bptgnan::Animation*>::iterator it = std::find( _animations.begin(), _animations.end(), animation );
	if (it != _animations.end()) {
		bptgnan::Animation *a = (*it);
		_animations.erase( it );
		a->stop();
		delete a;
		LINFO("FormatterRegion", "animation removed");
	}
}

void FormatterRegion::stepAnimation( bptgnan::Animation* animation ) {
	if (!animation->step()) {
		stopAnimation( animation );
	}
}

void FormatterRegion::enabled( bool enable ) {
	_enabled = enable;
}

util::DWORD FormatterRegion::uptime() const {
	return _player->uptime();
}

void FormatterRegion::transitionIn( void ) {
	if (haveTransitionIn()) {
		processTransition( _transitionIn, true );
	}
}

void FormatterRegion::transitionOut( void ) {
	if (haveTransitionOut()) {
		processTransition( _transitionOut, false );
	}
}

bool FormatterRegion::haveTransitionIn( void ) {
	return _transitionIn != NULL;
}

bool FormatterRegion::haveTransitionOut( void ) {
	return _transitionOut != NULL;
}

bptnt::Transition* FormatterRegion::getSupportedTransition( vector<bptnt::Transition*>* transitions ) {
	bptnt::Transition* transition = NULL;
	if (!transitions->empty()) {
		unsigned int i;
		for (i = 0; i < transitions->size(); i++) {
			transition = (*transitions)[i];
			switch ( transition->getType() ) {
				case Transition::TYPE_FADE: {
					return transition;
					break;
				}
				case Transition::TYPE_BARWIPE: {
					return transition;
					break;
				}
				default:
					break;
			}
		}
	}
	return NULL;
}

void FormatterRegion::processTransition( bptnt::Transition* transition, bool onPlay ) {
	switch ( transition->getType() ) {
		case Transition::TYPE_FADE: {
			fade( transition, onPlay );
			break;
		}
		case Transition::TYPE_BARWIPE: {
			barWipe( transition, onPlay );
			break;
		}
		default:
			break;
	};
}

void FormatterRegion::barWipe( bptnt::Transition* transition, bool onPlay ) {
	int duration = (int) transition->getDur() / 1000;
	double startProgress = transition->getStartProgress();
	double endProgress = transition->getEndProgress();

	bptgnan::Animation *anim = new bptgnan::Animation( duration, 0, _sys );

	switch ( transition->getSubtype() ) {
		case bptnt::Transition::SUBTYPE_BARWIPE_LEFTTORIGHT: { //TODO Transition::DIRECTION_REVERSE
			int w = _region->getWidth();
			int l = _region->getLeft();
			if (onPlay) {
				anim->addProperty( "width", startProgress * w, endProgress * w );
			} else {
				anim->addProperty( "width", endProgress * w, startProgress * w );
				anim->addProperty( "left", l, endProgress * w );
			}
			break;
		}
		case bptnt::Transition::SUBTYPE_BARWIPE_TOPTOBOTTOM: { //TODO Transition::DIRECTION_REVERSE
			int h = _region->getHeight();
			int t = _region->getTop();
			if (onPlay) {
				anim->addProperty( "height", startProgress * h, endProgress * h );
			} else {
				anim->addProperty( "height", endProgress * h, startProgress * h );
				anim->addProperty( "top", t, endProgress * t + h );
			}
			break;
		}

		default:
			break;
	};

	if (anim != NULL) {
		if (!onPlay) {
			anim->onEnd( boost::bind( &PlayerAdapterManager::stop, _player->getManager(), _player->getObject(), false ) );
		}
		startAnimation( anim );
	}

}

void FormatterRegion::fade( bptnt::Transition* transition, bool onPlay ) {

	int duration = (int) transition->getDur() / 1000;
	//TODO startProgress endProgress

	bptgnan::Animation *anim = new bptgnan::Animation( duration, 0, _sys );

	switch ( transition->getSubtype() ) {
		case bptnt::Transition::SUBTYPE_FADE_CROSSFADE: { //TODO Transition::DIRECTION_REVERSE
			if (onPlay) {
				anim->addProperty( "transparency", 100, 0 );
			} else {
				anim->addProperty( "transparency", 0, 100 );
			}
		}
	};

	if (anim != NULL) {
		if (!onPlay) {
			anim->onEnd( boost::bind( &PlayerAdapterManager::stop, _player->getManager(), _player->getObject(), false ) );
		}
		startAnimation( anim );
	}

}

}
}
}
}
}
}
}

