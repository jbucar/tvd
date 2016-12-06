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

#ifndef SLIDEBAR_H
#define SLIDEBAR_H

#pragma once

#include "window.h"
#include "label.h"

#include <boost/optional.hpp>
#include <boost/function.hpp>

namespace wgt {

class SlideBar : public Window
{
	typedef boost::function<void ()> Action;

	public:
		SlideBar(int x, int y, int w, int h
			, LabelPtr label
			, int minValue
			, int maxValue
			, const colorMap_t &colorMap
			, int bar_w
			, int step = 1);
		virtual ~SlideBar();
		void setValue(int value);
		virtual void activate(bool b, bool fromTop=true);
		void action(Action f);
		void action(const std::string& f);

		int value();
	
	protected:
		int _value;
		int _step;
		int _minValue;
		int _maxValue;
		int _maxW;
		LabelPtr _title;
		LabelPtr _valueL;
		WindowPtr _valueW;
		boost::optional<Action> _action;
		std::string _laction;
		
		virtual bool onRightArrow();
		virtual bool onLeftArrow();
		virtual bool onOk();
};

DEF_SHARED(SlideBar);

}

#endif // SLIDEBAR_H
