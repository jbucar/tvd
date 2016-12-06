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

#ifndef TEXTAREA_H
#define TEXTAREA_H

#pragma once

#include "borderedwindow.h"
#include "label.h"

namespace wgt{

class View;

class TextArea : public BorderedWindow
{
	public:
		TextArea(int x, int y, int w, int h, const colorMap_t &colorMap, const std::string& text, int fontSize, int lines, int margin, int align = alignment::vTop|alignment::hLeft, int borderWidth = 0);
		virtual ~TextArea();

		void draw(View* view);
		
		void text  (const std::string& text);
		void append(const std::string& text);
		void clearText();
		
		void lines(int lines);
		void fontSize(int fontSize);
		void margin(int margin);
		void alignment(int alignment);
		void fitToText(bool flag);
		void leading(int leading);
		
		const std::string& text();
		int lines();
		int fontSize();
		int margin();
		int alignment();
		bool fitToText();
		int leading();

		const Color bgColor();
		const Color textColor();

	private:
		int _lines;
		int _fontSize;
		std::string _text;
		int _margin;
		int _alignment;
		int _leading;
		bool _fitToText;
};

DEF_SHARED(TextArea);

}

#endif // TEXTAREA_H
