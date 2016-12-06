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
 * List.h
 *
 *  Created on: May 10, 2011
 *      Author: gonzalo
 */

#ifndef LIST_H_
#define LIST_H_

#include "widget.h"

#include "scrollbar.h"

#include "../alignment.h"
#include "../color.h"

#include <boost/shared_ptr.hpp>

#include <map>
#include <vector>
#include <string>

namespace wgt {

class View;

typedef std::map<size_t, std::map<std::string,std::string> > ColumnMapping;
typedef std::vector<std::string> Row;

class List : public Widget {
public:
	List(int x, int y, int w, int h, int rowH, int rowSep, const colorMap_t &colorMap, int fontSize);
	virtual ~List();

	void draw(View* view);

	void contents(const std::vector<Row>& rows, const std::vector<std::string>& values);
	void updateRow(size_t index, const Row& row, const std::string& value);
	void addRow(const Row& row, const std::string& value);
	void selected(int n);
	void setVisibleRows(int n);
	void addImage(const std::string& key, const std::string& value);

	int selected();

	const std::vector<Row>& contents();
	const std::vector<int>& cellWidths();
	const std::vector<std::string>& cellHeaders();
	const std::vector<Alignment>& cellAlignments();
	ColumnMapping& images_normal();
	ColumnMapping& images_selected();

	void cellAlignments( const std::vector<Alignment> &alignments );
	void cellWidths(const std::vector<int>& widths);
	void cellHeaders(const std::vector<std::string>& headers);

	int rowHeight();
	int rowSep();

	int current();
	int visible();
	Scrollbar* scrollBar();

	const std::string& value();

	int fontSize();
	void moveOneUp();
	void moveOneDown();

	void addColumnMapping(size_t column, const std::string& valueFrom, const std::string& valueToN, const std::string& valueToS);

	virtual void fixOffsets(Widget* w=0);
	void setUpdateScrollbar(bool update);
	void autoscroll( bool autoscroll );

	void scrollbarArrows( const std::string &up, const std::string &down );

	int from() const;
	int to() const;

protected:
	virtual bool onUpArrow();
	virtual bool onDownArrow();
	virtual bool onPageUp();
	virtual bool onPageDown();

	void updateScrollPosition();
	void updateFromTo();

private:
	std::vector<Row> _contents;
	std::vector<int> _cellWidths;
	std::vector<std::string> _headers;
	std::vector<Alignment> _alignments;
	std::map<std::string, std::string> _images;
	ColumnMapping _mappings_normal;
	ColumnMapping _mappings_selected;
	std::vector<std::string> _values;
	Scrollbar *_scrollbar;

	int _current;
	int _visible;
	bool _updateScrollbar;
	bool _autoscroll;

	int _rowHeight;
	int _rowSep;
	int _fontSize;
};

DEF_SHARED(List);

}

#endif /* LIST_H_ */
