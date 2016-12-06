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

#ifndef WGT_H
#define WGT_H

typedef struct lua_State lua_State;

#include "../view.h"
#include <vector>
#include <string>

namespace wgt{

class Window;

class Wgt
{
	public:
		virtual ~Wgt();
		
		static void init(View* view, lua_State *luaState);
		static void fin();
		static Wgt* getInstance();

		ColorId newColor(int r, int g, int b);

		void ignoreKey (WidgetId w, util::key::type key, bool flag);
		void setVisible(WidgetId w, bool visible, bool redrawOnlyThis=false);
		void setValue  (WidgetId w, int  value);
		void setEnable( WidgetId w, bool enable );
		void setText   (WidgetId w, std::string text);
		void setHelp   (WidgetId w, const std::string& help);
		void setPosition(WidgetId w, int x, int y);
		void setBounds(WidgetId widget, int x, int y, int w, int h);
		void size(WidgetId id, int &w, int &h );
		int processKey(WidgetId w, int  key);
		void reset(WidgetId w);
		void open(WidgetId w);
		void close(WidgetId w);
		void repaint(WidgetId w);
		void invalidate(WidgetId w);

		void fixOffsets( WidgetId w );
		void fixOffsets( WidgetId self_id, WidgetId wid );
		
		const std::string& getHelp(WidgetId w);

		static View* view();
		bool isVisible(WidgetId w);

		static void call(const std::string& f);
		static void call(const std::string& f, int n);

		static lua_State* luaState();

	private:
		View* _view;
		static Wgt* _instance;
		static lua_State* _lua;
		Wgt(View* view);
};

}

#endif // WGT_H
