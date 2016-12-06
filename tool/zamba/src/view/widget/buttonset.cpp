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
 * buttonset.cpp
 *
 *  Created on: May 20, 2011
 *      Author: gonzalo
 */

#include "buttonset.h"
#include <util/log.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

namespace wgt
{

ButtonSet::ButtonSet(int x, int y, int w, int h, const colorMap_t &colorMap)
	: WidgetSet(x,y,w,h,colorMap)
{
	_numElements = 0;
}

ButtonSet::~ButtonSet()
{}

bool ButtonSet::onLeftArrow()
{
	moveToChild( boost::bind(&Window::selectPreviousChild, this, _circular) );
	return true;
}

bool ButtonSet::onRightArrow()
{
	moveToChild( boost::bind(&Window::selectNextChild, this, _circular) );
	return true;
}

void ButtonSet::addButton(ButtonPtr b)
{
	addChild(b);
	_numElements++;
}

unsigned int ButtonSet::currentPos()
{
	return _activeChild;
}

bool ButtonSet::setCurrent( int pos )
{
	int tmp = (pos > (int)_numElements - 1) ? ((int)_numElements - 1) : pos;
	LTRACE("ButtonSet", "On set current. pos to set = %d calc position = %d", pos, tmp);
	selectChild(tmp);
	return true;
}

void ButtonSet::selectChild(size_t ix)
{
	Window::selectChild(ix);
	ButtonPtr btn = boost::static_pointer_cast<Button>(activeChild());
	if (!btn->isEnabled()) {
		moveToChild( boost::bind(&Window::selectNextChild, this, _circular) );
	}
}

void ButtonSet::restorePositions()
{
	BOOST_FOREACH(WidgetPtr widget, getChildren()) {
		((Button*)widget.get())->restorePositions();
		widget->fixOffsets( this );
	}
	needsRepaint( true );
}


}
