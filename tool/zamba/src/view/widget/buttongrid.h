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
 
#include "window.h"
#include "buttonset.h"
#include "button.h"
#include "scrollbar.h"
 
namespace wgt {
  
class ButtonSet;
 
class ButtonGrid : public Window {
public:
	ButtonGrid(int x, int y, int w, int h, unsigned int rowsVisible, unsigned int rowH, unsigned int rowSeparation, unsigned int rows, const colorMap_t &colorMap);
	virtual ~ButtonGrid();
	
	void draw(View* view);

	void appendRow( unsigned int offset=0 );
	void selectRow( unsigned int row );
	void addButton(ButtonPtr b, unsigned int row);
	bool selectButton(int pos);
	
	virtual void fixOffsets(Widget* w=0);

	void scrollbarArrows( const std::string &up, const std::string &down );
	
	int activeRow();
	int activeColumn();

protected:
	virtual bool onLeftArrow();
	virtual bool onRightArrow();
	virtual bool onUpArrow();
	virtual bool onDownArrow();
	virtual bool onOk();
	void addRow( int x, int y, int w, int h );

	virtual const std::string& currentHelp();
	void updateScrollPosition();
	void updateFromTo();

private:
	std::vector< ButtonSetPtr > _buttons;
	unsigned int _row;
	unsigned int _rows;
	unsigned int _hElements;
	unsigned int _rowH;
	unsigned int _rowSeparation;
	colorMap_t _colorMap;
	unsigned int _visible;
	Scrollbar *_scrollbar;
};

DEF_SHARED(ButtonGrid);
 
} 
