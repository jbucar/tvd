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

#ifndef COMBOBOX_H
#define COMBOBOX_H

#pragma once

#include "borderedwindow.h"
#include "list.h"
#include "image.h"
#include "label.h"
#include "scrollbar.h"

#include <boost/optional.hpp>
#include <boost/function.hpp>

namespace wgt {

class View;

class ComboBox : public Window {
public:
	typedef boost::function<void ()> Action;
	ComboBox(int x, int y, int w, int h, const colorMap_t &colorMap, const std::string& title, int titleFontSize, const std::vector<std::string>& items, int itemsFontSize, int items_w);
	virtual ~ComboBox();

	virtual void draw(View* view);
	
	const std::string& title();
	const std::vector<std::string>& items();
	
	void title(const std::string& title);
	void items(const std::vector<std::string>& items);
	void setTempItem(const std::string& item);
	void hideTempItem();

	int getIndex();
	void setIndex( unsigned int selected );
	
	void activate(bool b, bool fromTop=true);

	void action(Action f);
	void action(const std::string& f);
	void needsRepaint(bool flag);


	void maxLines(int n);
	int maxLines();

	bool isOpen();
	const BorderedWindowPtr& combo();
	void close();

	void scrollbarArrows( const std::string &up, const std::string &down );

protected:
	int selected();
	void selected(unsigned int selected, bool repaint=false);

private:
	bool _open;
	unsigned int _selected;
	unsigned int _lastSelected;
	int _initialH;
	int _itemsW;
	int _itemsFontSize;
	LabelPtr _title;
	BorderedWindowPtr _combo;
	std::vector<std::string> _items;
	std::vector<LabelPtr> _labels;
	boost::optional<Action> _action;
	std::string _laction;
	int _maxLines;
	bool _parentNeedsRepaint;
	LabelPtr _tempItem;
	ImagePtr _arrowDown;
	ImagePtr _arrowDownSelected;
	Scrollbar *_scrollbar;
	bool _tempItemVisible;

	struct {
		int x;
		int y;
		int w;
		int h;
	} _labelOpen, _labelClosed;

	virtual bool onOk();
	virtual bool onExit();
	virtual bool onUpArrow();
	virtual bool onDownArrow();
	void open();
	void createItems();

	int visibleItemCount();
};

DEF_SHARED(ComboBox);

}

#endif // COMBOBOX_H
