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

#pragma once

#include "../color.h"
#include "widget.h"

#include <vector>

namespace wgt {
	
class View;

class Window
	: public Widget
{
	public:
		Window(int x, int y, int w, int h, const colorMap_t &colorMap);
		virtual ~Window();
		void draw(View* view);

		void removeChildren();
		virtual const std::vector<WidgetPtr>& getChildren();
		void addChild(WidgetPtr widget, bool canHandleKey=true);

		bool processKey(util::key::type key);

		virtual const Color bgColor();

		virtual void fixOffsets(Widget* w=0);

		virtual void selectPreviousChild(bool circular=false);
		virtual void selectNextChild(bool circular=false);
		virtual void selectChild(size_t ix);
		void deactivateChild();

		WidgetPtr activeChild();
		size_t getActiveIndex();

	protected:

		std::vector<WidgetPtr> _children; // all
		std::vector<WidgetPtr> _keyHandlingChildren; // only those that can handle keys
		size_t _activeChild;

};

DEF_SHARED(Window);

}
