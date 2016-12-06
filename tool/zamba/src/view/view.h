
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

#include "widget/widget.h"
#include "color.h"
#include <canvas/color.h>
#include <map>
#include <string>
#include <deque>
#include <boost/function.hpp>

namespace wgt{

typedef int ColorId;

class Window;
class BorderedWindow;
class Label;
class MaskedLabel;
class Image;
class ImageViewer;
class VideoViewer;
class List;
class TextArea;
class Bar;
class MainMenuList;
class Page;
class TabbedWindow;
class PageTab;
class Button;
class ComboBox;
class Frame;
class ScheduleRow;
class Schedule;
class ButtonGrid;
class Scroll;
class Scrollbar;
class ScrollArrow;

class View {

	public:
		virtual ~View();

		ColorId  addColor(const Color& color);
		WidgetId addWidget(WidgetPtr widget);

		const std::string &fontFace();
		Color getColorById(ColorId id);
		WidgetPtr getWidgetById(WidgetId id);
		WidgetPtr getTopWidget();

		void draw(Window* window);
		void draw(Button* but);
		void draw(BorderedWindow* borderedWindow);
		void draw(Label* label);
		void draw(MaskedLabel* label);
		void draw(Image* image);
		void draw(List* image);
		void draw(TextArea* textArea);
		void draw(Bar* bar);
		void draw(Page* page);
		void draw(TabbedWindow* menu);
		void draw(ComboBox* combo);
		void draw(ImageViewer* iv);
		void draw(VideoViewer* vv);
		void draw(Frame* frame);
		void draw(ScheduleRow* row);
		void draw(Schedule* schedule);
		void draw( Scroll* scroll );
		void draw( Scrollbar *scrollbar );
		void draw( ScrollArrow *scroll );
		
		void clear(Widget* widget);

		void push(WidgetPtr widget);
		void pop(WidgetPtr widget);
		void sortWidgets();

		void refresh();

		static void init(lua_State *L);
		static void fin();
		static View* instance();
		
		void fitToSize(int text_w, int text_h, int margin, int &x, int &w, int &h);
		void invalidate(WidgetPtr widget);

	private:
		View();

		std::map<ColorId, Color>   _colors;
		std::map<WidgetId, WidgetPtr> _keyHandlingWidgets;
		std::vector<WidgetPtr> _widgets;
		std::vector<WidgetPtr> _widgetsToClear;

		ColorId  _cId;
		WidgetId _wId;

		std::string _fontFace;
		bool _needsFullRedraw;
		bool _desaturateColors;

		ColorId  getNextColorId();
		WidgetId getNextWidgetId();
		void drawChildren(Window* window);
		void drawActiveTab(PageTab* tab);
		void drawWidget(WidgetPtr w);

		canvas::Color toLuaColor(const Color& color);
		Color adjust(const Color& c);
		const std::string& adjustedPath(Image* img);
};

}
