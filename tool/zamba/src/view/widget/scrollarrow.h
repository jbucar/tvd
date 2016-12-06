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

#include "widget.h"
#include "buttonset.h"
#include "widgetset.h"
#include <string>

namespace wgt {

class View;

class ScrollArrow {
public:
	ScrollArrow(const std::string &arrowLeft, const std::string &arrowRight, int x, int y, int w, int h );
	virtual ~ScrollArrow();

	int x();
	void x( int x );
	int y();
	void y( int y );
	int w();
	int h();

	void canScrollRight( bool value );
	void canScrollLeft( bool value );

	void arrowRight( const std::string &filename );
	const std::string &arrowRight() const;

	void arrowLeft( const std::string &filename );
	const std::string &arrowLeft() const;

	void deactiveImgs( const std::string &right_filename, const std::string &left_filename );

private:
	std::string _arrowLeft;
	std::string _arrowRight;
	std::string _arrowRight_deactived;
	std::string _arrowLeft_deactived;

	int _x;
	int _y;
	int _w;
	int _h;
	bool _canScrollRight;
	bool _canScrollLeft;
};

class Scroll : public WidgetSet {
public:
	Scroll( int x, int y, int w, int h, int sep, const colorMap_t &colorMap, ButtonSetPtr wgt );
	virtual ~Scroll();

	virtual const std::vector<WidgetPtr>& getChildren();

	unsigned int limit();

	const Color bgColor();
	virtual void draw( View* view );

	ScrollArrow *view();

	virtual void selectChild( size_t ix );

	virtual void fixOffsets( Widget * /*w*/);

	void adjust();
	void init( int limit=-1 );

protected:

	virtual bool onLeftArrow();
	virtual bool onRightArrow();

private:
	unsigned int _separation;
	unsigned int _limit;
	ScrollArrow *_view;
	ButtonSetPtr _scrollee;
	std::vector<WidgetPtr> _btns;
};

DEF_SHARED(Scroll);

}
