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
 * button.h
 *
 *  Created on: May 19, 2011
 *      Author: gonzalo
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "window.h"

#include "label.h"
#include "image.h"
#include "../color.h"

#include <boost/optional.hpp>
#include <boost/function.hpp>

namespace wgt
{

class Button: public wgt::Window
{
public:
	typedef boost::function<void ()> Action;
	typedef boost::function<void ( bool & )> SelectedAction;

	Button(int x, int y, int w, int h, const colorMap_t &colorMap);
	virtual ~Button();

	virtual void x( int x );
	virtual int x() { return Widget::x(); };

	void draw(View* view);

	void activate(bool b, bool fromTop=true);

	void label(const std::string& text);
	void icon(const std::string& path, const std::string& activePath, const std::string disablePath, int imgw, int imgh, int iconH, int separation);
	void icon(const std::string& path, const std::string& activePath, int imgw, int imgh, int iconH, int separation);

	void action( Action callback );
	void selectedCallback( SelectedAction callback );

	LabelPtr label();
	ImagePtr icon();
	int iconH();
	int separation();
	void execute();
	
	void restorePositions();
	virtual const Color bgColor();

protected:
	const Color textColor();
	const std::string& iconPath();

	virtual bool onOk();

private:
	LabelPtr _label;
	ImagePtr _icon;

	std::string _normalIconPath;
	std::string _activeIconPath;
	std::string _disableIconPath;

	int _iconH;
	int _separation;
	int _originalX;
	int _originalY;

	Action _callback;
	SelectedAction _selectedCbk;
};

DEF_SHARED(Button);

}

#endif /* BUTTON_H_ */
