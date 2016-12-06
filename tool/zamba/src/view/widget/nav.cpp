/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-luaz implementation.

    DTV-luaz is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-luaz is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-luaz.

    DTV-luaz es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-luaz se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

/*
 * Nav.cpp
 *
 *  Created on: May 4, 2011
 *      Author: gonzalo
 */

#include "nav.h"

#include <boost/foreach.hpp>
#include <iostream>

namespace wgt
{

NavController::NavController()
{}

NavController::~NavController()
{}

void NavController::ignoreKey(util::key::type key, bool flag)
{
	if (flag) {
		// add key to ignored ones
		_ignoredKeys.insert(key);
	}else{
		// remove key from ignored ones
		for (std::set<util::key::type>::iterator it = _ignoredKeys.begin(); it!=_ignoredKeys.end(); it++) {
			if((*it)==key){
				_ignoredKeys.erase(it);
			}
		}
	}
}

void NavController::ignoreKeys(const std::vector<util::key::type>& keys)
{
	BOOST_FOREACH(util::key::type key, keys) {
		ignoreKey(key);
	}
}

bool NavController::processKey(int key)
{
	return processKey(util::key::type(key));
}

bool NavController::processKey(util::key::type key)
{
	// si no lo procesó ninguno de los que están arriba de mi...
	if (_ignoredKeys.find(key) == _ignoredKeys.end()) { // y no está dentro de las ignoradas
		bool processed = false;
		switch (key) {
			case util::key::menu         : processed = onMenu(); break;
			case util::key::enter        :
			case util::key::ok           : processed = onOk(); break;
			case util::key::cursor_up    : processed = onUpArrow(); break;
			case util::key::cursor_down  : processed = onDownArrow(); break;
			case util::key::cursor_left  : processed = onLeftArrow(); break;
			case util::key::cursor_right : processed = onRightArrow(); break;
			case util::key::number_0     :
			case util::key::number_1     :
			case util::key::number_2     :
			case util::key::number_3     :
			case util::key::number_4     :
			case util::key::number_5     :
			case util::key::number_6     :
			case util::key::number_7     :
			case util::key::number_8     :
			case util::key::number_9     : processed = onDigitPressed(int(key)-1); break;
			case util::key::info         : processed = onInfo(); break;
			case util::key::epg          : processed = onEPG(); break;
			case util::key::channel_down : processed = onChannelDown(); break;
			case util::key::channel_up   : processed = onChannelUp(); break;
			case util::key::volume_down  : processed = onVolumeDown(); break;
			case util::key::volume_up    : processed = onVolumeUp(); break;
			case util::key::red          : processed = onRed(); break;
			case util::key::green        : processed = onGreen(); break;
			case util::key::yellow       : processed = onYellow(); break;
			case util::key::blue         : processed = onBlue(); break;
			case util::key::escape       :
			case util::key::exit         : processed = onExit(); break;
			case util::key::mute         : processed = onMute(); break;
			case util::key::audio        : processed = onAudio(); break;
			case util::key::previous     : processed = onPrevious(); break;
			case util::key::favorites    : processed = onFavorite(); break;
			case util::key::subtitle     : processed = onSubtitle(); break;
			case util::key::aspect       : processed = onAspect(); break;
			case util::key::ch34         : processed = onCh3_4(); break;
			case util::key::mode         : processed = onVideoMode(); break;
			case util::key::page_up      : processed = onPageUp(); break;
			case util::key::page_down    : processed = onPageDown(); break;
			default: ;
				//std::cout << "[NavController] key " << key << " not processed" << std::endl;
		}
		//std::cout << "[NavController] key " << key << " processed" << std::endl;
		return processed;
	} else {
		//std::cout << "[NavController] key " << key << " ignored" << std::endl;
	}

	return true;
}

bool NavController::onMenu() {
	return false;
}
bool NavController::onOk() {
	return false;
}

bool NavController::onInfo()
{
	return false;
}
bool NavController::onEPG()
{
	return false;
}
bool NavController::onChannelDown()
{
	return false;
}
bool NavController::onChannelUp()
{
	return false;
}

bool NavController::onUpArrow() {
	return false;
}
bool NavController::onDownArrow() {
	return false;
}
bool NavController::onLeftArrow() {
	return false;
}
bool NavController::onRightArrow() {
	return false;
}
bool NavController::onVolumeUp() {
	return false;
}
bool NavController::onVolumeDown() {
	return false;
}
bool NavController::onDigitPressed(int){
	return false;
}
bool NavController::onGreen()
{
	return false;
}
bool NavController::onExit()
{
	return false;
}
bool NavController::onYellow()
{
	return false;
}
bool NavController::onAudio()
{
	return false;
}
bool NavController::onMute()
{
	return false;
}
bool NavController::onBlue()
{
	return false;
}
bool NavController::onPrevious()
{
	return false;
}
bool NavController::onRed()
{
	return false;
}
bool NavController::onFavorite()
{
	return false;
}
bool NavController::onSubtitle()
{
	return false;
}
bool NavController::onAspect()
{
	return false;
}
bool NavController::onCh3_4()
{
	return false;
}
bool NavController::onVideoMode()
{
	return false;
}

bool NavController::onPageUp()
{
	return false;
}

bool NavController::onPageDown()
{
	return false;
}

void NavController::close()
{
	// std::cout << "[NavController] close" << std::endl;
}

void NavController::open()
{
	// std::cout << "[NavController] open" << std::endl;
}

} // end namespace wgt
