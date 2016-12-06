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

#include "widgetset.h"
#include <boost/bind.hpp>

namespace wgt {

WidgetSet::WidgetSet(int x, int y, int w, int h, const colorMap_t &colorMap)
	: Window(x,y,w,h, colorMap)
	, _circular(false)
{}

WidgetSet::~WidgetSet(){
}

void WidgetSet::circular(bool b)
{
	_circular = b;
}

const std::string& WidgetSet::currentHelp()
{
	return activeChild()->help();
}

bool WidgetSet::moveToChild( Callback func )
{
	updatePreviousActiveChild();

	WidgetPtr btn;
	WidgetPtr first = boost::static_pointer_cast<Widget>(activeChild());
	do {
		func();
		btn = boost::static_pointer_cast<Widget>(activeChild());
	} while (!btn->isEnabled() && first != btn );

	if (first != btn) {
		first->activate(false);
		btn->activate(true, false);
		return true;
	} else {
		return false;
	}

}

void WidgetSet::enableChild(size_t ix, bool enabled)
{
	WidgetPtr wgt = boost::static_pointer_cast<Widget>(getChildren()[ix]);
	if (!enabled && (ix == getActiveIndex())) {
		wgt->activate(false);
		wgt->enable(false);
		moveToChild( boost::bind(&Window::selectNextChild, this, _circular) );
	} else {
		wgt->enable(enabled);
	}
}

}
