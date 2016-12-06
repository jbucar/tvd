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

#ifndef LABEL_H
#define LABEL_H

#include "../color.h"
#include "widget.h"

#include "../alignment.h"

#include <string>

namespace wgt {
	
class View;

class Label : public Widget 
{
	public:
		static int None;
		static int vCenter_hLeft;
		static int vCenter_hRight;
		static int vCenter_hCenter;
		static int vTop_hLeft;
		static int vTop_hCenter;
		
		Label(int x, int y, int w, int h, const colorMap_t &colorMap, const std::string& text, int fontSize, int alignment, int margin = 0);
		virtual ~Label();

		void draw(View* view);

		const std::string& text();
		virtual std::string rawText();
		void text(const std::string& text);
		int fontSize();
		Alignment alignment();
		int margin();

		void alignment(int a);
		void fontSize(int s);

		void mask(const std::string& m);
		void emptyMask(const std::string& m);
		void textLength(size_t length);

		virtual void reset();
	protected:
		size_t _textLenght;
		std::string _text;
		std::string _mask;
		std::string _emptyMask;
		std::string _masked;
		virtual const std::string& maskedText();
	private:
		int _fontSize;
		Alignment _alignment;
		int _margin;
};

DEF_SHARED(Label);

}

#endif // LABEL_H



