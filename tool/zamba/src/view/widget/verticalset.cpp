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

#include "verticalset.h"
#include <boost/bind.hpp>

namespace wgt {

VerticalSet::VerticalSet(int x, int y, int w, int h, const colorMap_t &colorMap)
	: WidgetSet(x,y,w,h, colorMap)
{}

VerticalSet::~VerticalSet(){
}

void VerticalSet::addWidget(WidgetPtr w){
	addChild(w);
}

void VerticalSet::activated(){
	selectChild(0);
	updatePreviousActiveChild();
}

bool VerticalSet::onUpArrow(){
	moveToChild( boost::bind(&Window::selectPreviousChild, this, _circular) );
	return true;
}

bool VerticalSet::onDownArrow(){
	moveToChild( boost::bind(&Window::selectNextChild, this, _circular) );
	return true;
}

bool VerticalSet::onExit(){
	deactivateChild();
	return false; //so the page ca handle this exit
}

void VerticalSet::needsRepaint(bool flag) {
	if (flag) {
		if (_previousActiveChild) {
			_previousActiveChild->needsRepaint(flag);
			activeChild()->needsRepaint(flag);
		}
	}  else {
		_needsRepaint = flag;
	}
}

void VerticalSet::rawNeedsRepaint(bool flag) {
	_needsRepaint = flag;
}

void VerticalSet::updatePreviousActiveChild()
{
	_previousActiveChild = activeChild();
}

}

