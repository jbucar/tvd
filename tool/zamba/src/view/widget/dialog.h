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
 * dialog.h
 *
 *  Created on: May 24, 2011
 *      Author: gonzalo
 */

#ifndef DIALOG_H_
#define DIALOG_H_

#include "borderedwindow.h"
#include "button.h"
#include "buttonset.h"
#include "textarea.h"
#include "label.h"
#include "image.h"

#include <boost/function.hpp>

namespace wgt
{

class Dialog: public wgt::BorderedWindow
{
public:
	Dialog( int x, int y, int w, int h, const colorMap_t &colorMap, int borderWidth, int margin=20 );
	virtual ~Dialog();

	void title(const std::string& t);
	void iconPath(const std::string& path, int imgw, int imgh, const std::string& pathDisabled="");

	TextAreaPtr text();

	void titleColor(const Color& color);
	void textColor(const Color& color);

protected:
	int _margin;
	int _titleHeight;

protected:
	virtual bool onExit();

//private:
	LabelPtr _title;
	TextAreaPtr _text;
	ImagePtr _icon;
};

DEF_SHARED(Dialog);

//----------------------------------------------------------------------------------//

class YesNoDialog : public Dialog {
public:
	YesNoDialog( int x, int y, int w, int h, const colorMap_t &colorMap, int borderWidth, int bw, int bh, int margin=20 );

	virtual void open();

	void yesAction( Button::Action fYes );
	void noAction( Button::Action fNo );

	void yesLabel( const std::string &label, int fontSize=-1 );
	void noLabel( const std::string &label, int fontSize=-1 );

	void buttonColor( const Color& color, const Color& activeColor );
	void buttonTextColor( const Color& color, const Color& activeColor );

protected:
	virtual bool onExit();

private:
	ButtonSetPtr _buttonSet;
	ButtonPtr _yes;
	ButtonPtr _no;

	void closeFirst(Button::Action f);
};

DEF_SHARED(YesNoDialog);


} // end namespace

#endif /* DIALOG_H_ */
