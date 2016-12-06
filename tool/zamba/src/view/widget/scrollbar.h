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
#include <string>

namespace wgt {

class Scrollbar {
public:
	Scrollbar();
	virtual ~Scrollbar();

	void init( int x, int y, int visible, int count, int rowHeight, int rowSep );
	void update( int current, int visible, int count, int margins );

	void updateFromTo( int current, int visible, int count );
	void initFromTo( int current, int visible, int count );

	struct {
		int x;
		int y;
		int w;
		int h;
		int margin;
	} scroll;

	struct {
		int x;
		int y;
		int w;
		int h;
	} handle;

	unsigned int from();
	unsigned int to();

	void arrowUp( const std::string &filename );
	const std::string &arrowUp() const;

	void arrowDown( const std::string &filename );
	const std::string &arrowDown() const;

	const Color &scrollColor() const;
	void scrollColor( const Color &color );

	const Color &bgColor() const;
	void bgColor( const Color &color );

private:
	int _from;
	int _to;
	Color _scrollColor;
	Color _bgColor;
	std::string _arrowUp;
	std::string _arrowDown;
};

}
