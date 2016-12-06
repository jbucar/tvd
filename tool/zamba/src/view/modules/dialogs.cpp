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
 * dialogs.cpp
 *
 *  Created on: Jun 17, 2011
 *      Author: gonzalo
 */

#include "luawgt.h"
#include "wgt.h"
#include "../widget/pindialog.h"
#include "../luacallback.h"
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

namespace wgt {

namespace yesnodialog {

std::map<WidgetId, YesNoDialogPtr> _widgets;

int create(lua_State* L)
{
	const int x = luaL_checkint(L, 2);
	const int y = luaL_checkint(L, 3);
	const int w = luaL_checkint(L, 4);
	const int h = luaL_checkint(L, 5);
	const colorMap_t colorMap = parseColorTable(L, 6);
	unsigned short zIndex = (unsigned short) luaL_checkint(L, 7);
	const int borderWidth   = luaL_checkint(L, 8);
	const int bw = luaL_checkint(L, 9);
	const int bh = luaL_checkint(L, 10);

	YesNoDialogPtr ww;

	if (lua_gettop(L) == 11) {
		ww = boost::make_shared<YesNoDialog>(x,y, w,h, colorMap, borderWidth, bw, bh, luaL_checkint(L, 11));
	} else {
		ww = boost::make_shared<YesNoDialog>(x,y, w,h, colorMap, borderWidth, bw, bh);
	}

	ww->zIndex(zIndex);
	WidgetId id = Wgt::view()->addWidget(ww);
	_widgets[id] = ww;

	lua_pushnumber(L, id);
	return 1;
}

int yesAction(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	YesNoDialogPtr dialog = _widgets[id];
 	dialog->yesAction(boost::bind(&luaCallback, L, ref));

	return 0;
}

int noAction(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	YesNoDialogPtr dialog = _widgets[id];
	dialog->noAction(boost::bind(&luaCallback, L, ref));

	return 0;
}

int yesLabel(lua_State* L)
{
	const WidgetId id = luaL_checkint(L, 2);
	const char *label = luaL_checkstring(L, 3);

	YesNoDialogPtr dialog = _widgets[id];

	if (lua_gettop(L) == 4) {
 		dialog->yesLabel(label, luaL_checkint(L, 4));
	} else {
		dialog->yesLabel(label);
	}

	return 0;
}

int noLabel(lua_State* L)
{
	const WidgetId id = luaL_checkint(L, 2);
	const char *label = luaL_checkstring(L, 3);

	YesNoDialogPtr dialog = _widgets[id];

	if (lua_gettop(L) == 4) {
 		dialog->noLabel(label, luaL_checkint(L, 4));
	} else {
		dialog->noLabel(label);
	}

	return 0;
}

int title(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* title = luaL_checkstring(L, narg++);

	YesNoDialogPtr dialog = _widgets[id];
	dialog->title(title);

	return 0;
}

int icon(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* path = luaL_checkstring(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);

	YesNoDialogPtr dialog = _widgets[id];

	if (lua_gettop(L) == 4) {
		const char* pathDisabled = luaL_checkstring(L, narg++);
		dialog->iconPath(path, w,h, pathDisabled);
	} else {
		dialog->iconPath(path, w,h);
	}

	return 0;
}

int text( lua_State* L ) {
	const WidgetId id = luaL_checkint(L, 2);
	const char* text = luaL_checkstring(L, 3);
	const int maxLines = luaL_checkint(L, 4);

	YesNoDialogPtr dialog = _widgets[id];

	dialog->text()->text(text);
	dialog->text()->lines(maxLines);

	if (lua_gettop(L) > 4) {
		dialog->text()->h(luaL_checkint(L, 5));
		if (lua_gettop(L) > 5) {
			dialog->text()->fontSize(luaL_checkint(L, 6));
			if (lua_gettop(L) > 6) {
				dialog->text()->margin(luaL_checkint(L, 7));
			}
		}
	}

	dialog->needsRepaint(true);

	lua_pushnumber(L, dialog->text()->x());
	lua_pushnumber(L, dialog->text()->y());
	lua_pushnumber(L, dialog->text()->w());
	lua_pushnumber(L, dialog->text()->h());

	return 4;
}

int titleColor(lua_State* L)
{
	int narg = 2;
	const WidgetId id   = luaL_checkint(L, narg++);
	const ColorId color = luaL_checkint(L, narg++);

	YesNoDialogPtr dialog = _widgets[id];
	dialog->titleColor(Wgt::view()->getColorById(color));

	return 0;
}

int addChild(lua_State* L)
{
	int narg = 2;
	const WidgetId fid = luaL_checkint(L, narg++);
	const WidgetId sid = luaL_checkint(L, narg++);

	YesNoDialogPtr father = _widgets[fid];
	WidgetPtr son  = Wgt::view()->getWidgetById(sid);

	father->addChild(son);

	return 0;
}

int textColor(lua_State* L)
{
	int narg = 2;
	const WidgetId id   = luaL_checkint(L, narg++);
	const ColorId color = luaL_checkint(L, narg++);

	YesNoDialogPtr dialog = _widgets[id];
	dialog->textColor(Wgt::view()->getColorById(color));

	return 0;
}

int buttonColor(lua_State* L)
{
	int narg = 2;
	const WidgetId id   = luaL_checkint(L, narg++);
	const ColorId color       = luaL_checkint(L, narg++);
	const ColorId activeColor = luaL_checkint(L, narg++);

	YesNoDialogPtr dialog = _widgets[id];
	dialog->buttonColor(Wgt::view()->getColorById(color), Wgt::view()->getColorById(activeColor));

	return 0;
}

int buttonTextColor(lua_State* L)
{
	int narg = 2;
	const WidgetId id   = luaL_checkint(L, narg++);
	const ColorId color       = luaL_checkint(L, narg++);
	const ColorId activeColor = luaL_checkint(L, narg++);

	YesNoDialogPtr dialog = _widgets[id];
	dialog->buttonTextColor(Wgt::view()->getColorById(color), Wgt::view()->getColorById(activeColor));

	return 0;
}

const struct luaL_Reg methods[] = {
	{"new",                  create           },
	{"yesAction",            yesAction        },
	{"noAction",             noAction         },
	{"yesLabel",             yesLabel         },
	{"noLabel",              noLabel          },
	{"title",                title            },
	{"icon",                 icon             },
	{"text",                 text             },
	{"titleColor",           titleColor       },
	{"textColor",            textColor        },
	{"buttonColor",          buttonColor      },
	{"buttonTextColor",      buttonTextColor  },
	{"addChild",             addChild         },
	{NULL,                   NULL             }
};

void init(lua_State *L) {

	luaL_register(L, "yesnodialog", methods);
}

} // yesnodialog

namespace basicdialog {

std::map<WidgetId, DialogPtr> _widgets;

int create(lua_State* L)
{
	int narg = 2;
	const int x = luaL_checkint(L, narg++);
	const int y = luaL_checkint(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);
	const colorMap_t colorMap = parseColorTable( L, narg++ );
	unsigned short zIndex = (unsigned short)luaL_checkint(L, narg++);
	const int borderWidth   = luaL_checkint(L, narg++);

	DialogPtr ww = boost::make_shared<Dialog>(x,y, w,h, colorMap, borderWidth);
	ww->fixOffsets();
	ww->zIndex(zIndex);
	WidgetId id = Wgt::view()->addWidget(ww);
	_widgets[id] = ww;

	lua_pushnumber(L, id);
	return 1;
}

int title(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* title = luaL_checkstring(L, narg++);

	DialogPtr dialog = _widgets[id];
	dialog->title(title);

	return 0;
}

int icon(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* path = luaL_checkstring(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);

	DialogPtr dialog = _widgets[id];
	dialog->iconPath(path, w, h);

	return 0;
}

int text(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* text = luaL_checkstring(L, narg++);
	const int maxLines = luaL_checkint(L, narg++);

	DialogPtr dialog = _widgets[id];
	dialog->text()->text(text);
	dialog->text()->lines(maxLines);

	return 0;
}

int addChild(lua_State* L)
{
	int narg = 2;
	const WidgetId fid = luaL_checkint(L, narg++);
	const WidgetId sid = luaL_checkint(L, narg++);

	DialogPtr father = _widgets[fid];
	WidgetPtr son  = Wgt::view()->getWidgetById(sid);

	father->addChild(son);

	return 0;
}

int titleColor(lua_State* L)
{
	int narg = 2;
	const WidgetId id   = luaL_checkint(L, narg++);
	const ColorId color = luaL_checkint(L, narg++);

	DialogPtr dialog = _widgets[id];
	dialog->titleColor(Wgt::view()->getColorById(color));

	return 0;
}

int textColor(lua_State* L)
{
	int narg = 2;
	const WidgetId id   = luaL_checkint(L, narg++);
	const ColorId color = luaL_checkint(L, narg++);

	DialogPtr dialog = _widgets[id];
	dialog->textColor(Wgt::view()->getColorById(color));

	return 0;
}

void init(lua_State *L)
{
	const struct luaL_Reg methods[] = {
			{"new",         create},
			{"addChild",    addChild},

			{"title",    title},
			{"icon",     icon},
			{"text",     text},

			{"titleColor",      titleColor},
			{"textColor",       textColor},

			{NULL,  NULL}
	};

	luaL_register(L, "basicdialog", methods);
}
} // basicdialog

//--------------------------------------------------------------------------//

namespace pindialog {

std::map<WidgetId, PINDialogPtr> _widgets;

int create(lua_State* L)
{
	int narg = 2;
	const int x = luaL_checkint(L, narg++);
	const int y = luaL_checkint(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);
	const colorMap_t colorMap = parseColorTable( L, narg++ );
	unsigned short zIndex = (unsigned short)luaL_checkint(L, narg++);
	const int borderWidth = luaL_checkint(L, narg++);
	const unsigned int pinSize = luaL_checkint(L, narg++);

	PINDialogPtr ww = boost::make_shared<PINDialog>(x,y, w,h, colorMap, borderWidth, pinSize);
	ww->zIndex(zIndex);
	WidgetId id = Wgt::view()->addWidget(ww);
	_widgets[id] = ww;

	lua_pushnumber(L, id);
	return 1;
}

int prompt(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* prompt = luaL_checkstring(L, narg++);

	PINDialogPtr dialog = _widgets[id];
	dialog->prompt(prompt);

	return 0;
}

int pinCorrect(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	PINDialogPtr dialog = _widgets[id];
	dialog->pinCorrect();

	return 0;
}

int pinIncorrect(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	PINDialogPtr dialog = _widgets[id];
	dialog->pinIncorrect();

	return 0;
}

int pinClose(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	PINDialogPtr dialog = _widgets[id];
	dialog->pinClose();

	return 0;
}


int callbacks(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* fOk        = luaL_checkstring(L, narg++);
	const char* fCancelled = luaL_checkstring(L, narg++);

	PINDialogPtr dialog = _widgets[id];
	dialog->callbacks(std::string(fOk), std::string(fCancelled));

	return 0;
}

int onWrongPassword(lua_State* L) {
	const WidgetId id = luaL_checkint(L, 2);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	PINDialogPtr dialog = _widgets[id];
 	dialog->onWrongPassword(boost::bind(&luaCallback, L, ref));
	return 0;
}

int title(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* title = luaL_checkstring(L, narg++);

	PINDialogPtr dialog = _widgets[id];
	dialog->title(title);

	return 0;
}

int icon(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* path = luaL_checkstring(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);

	PINDialogPtr dialog = _widgets[id];
	dialog->iconPath(path, w,h);

	return 0;
}

int text(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* text = luaL_checkstring(L, narg++);
	const int maxLines = luaL_checkint(L, narg++);

	PINDialogPtr dialog = _widgets[id];
	dialog->text()->text(text);
	dialog->text()->lines(maxLines);

	return 0;
}

int help(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* help = luaL_checkstring(L, narg++);

	PINDialogPtr widget = _widgets[id];
	widget->help(help);

	return 0;
}

int lines(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const int lns = luaL_checkint(L, narg++);

	PINDialogPtr widget = _widgets[id];
	widget->text()->lines(lns);

	return 0;
}

int titleColor(lua_State* L)
{
	int narg = 2;
	const WidgetId id   = luaL_checkint(L, narg++);
	const ColorId color = luaL_checkint(L, narg++);

	PINDialogPtr dialog = _widgets[id];
	dialog->titleColor(Wgt::view()->getColorById(color));

	return 0;
}

int textColor(lua_State* L)
{
	int narg = 2;
	const WidgetId id   = luaL_checkint(L, narg++);
	const ColorId color = luaL_checkint(L, narg++);

	PINDialogPtr dialog = _widgets[id];
	dialog->textColor(Wgt::view()->getColorById(color));

	return 0;
}

const struct luaL_Reg pindialog_methods[] = {
	{"new",                    create           },
	{"prompt",                 prompt           },
	{"pinCorrect",             pinCorrect       },
	{"pinIncorrect",           pinIncorrect     },
	{"pinClose",               pinClose         },
	{"onWrongPassword",        onWrongPassword  },
	{"callbacks",              callbacks        },
	{"title",                  title            },
	{"icon",                   icon             },
	{"text",                   text             },
	{"help",                   help             },
	{"lines",                  lines            },
	{"titleColor",             titleColor       },
	{"textColor",              textColor        },
	{NULL,                     NULL             }
};

void init(lua_State *L) {
	luaL_register(L, "pindialog", pindialog_methods);
}

} // pindialog

} // wgt

