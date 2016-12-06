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

#include "wgt.h"
#include "luawgt.h"
#include "../alignment.h"
#include <luaz/lua.h>
#include <util/mcr.h>
#include <util/log.h>

namespace wgt {

Wgt* Wgt::_instance = NULL;
lua_State* Wgt::_lua = NULL;

static int l_processKey(lua_State* L){
	int narg = 2;
	const WidgetId w = luaL_checkint(L, narg++);
	const int key  = luaL_checkint(L, narg++);

	//retornar error si no encuentra el widget
	lua_pushboolean(L, Wgt::getInstance()->processKey(w, key));
	return 1;
}

static int l_setValue(lua_State* L){
	int narg = 2;
	const WidgetId w = luaL_checkint(L, narg++);
	const int value  = luaL_checkint(L, narg++);

	//retornar error si no encuentra el widget
	Wgt::getInstance()->setValue(w, value);
	return 0;
}

static int l_position(lua_State* L){
	int narg = 2;
	const WidgetId w = luaL_checkint(L, narg++);
	const int x  = luaL_checkint(L, narg++);
	const int y  = luaL_checkint(L, narg++);

	//retornar error si no encuentra el widget
	Wgt::getInstance()->setPosition(w, x, y);
	return 0;
}

static int l_size( lua_State *L ) {
	int narg = 2;
	const WidgetId widget = luaL_checkint(L, narg++);
	int w, h;
	Wgt::getInstance()->size(widget, w, h);
	lua_pushnumber( L, w );
	lua_pushnumber( L, h );
	return 2;
}

static int l_bounds(lua_State* L){
	int narg = 2;
	const WidgetId widget = luaL_checkint(L, narg++);
	const int x  = luaL_checkint(L, narg++);
	const int y  = luaL_checkint(L, narg++);
	const int w  = luaL_checkint(L, narg++);
	const int h  = luaL_checkint(L, narg++);

	Wgt::getInstance()->setBounds(widget, x, y, w, h);
	return 0;
}

static int l_setVisible(lua_State* L)
{
	int narg = 2;
	const WidgetId w = luaL_checkint(L, narg++);
	const bool b = luaL_checkint(L, narg++) != 0;
	if (lua_gettop(L) == 4) {
		const bool onlythis = luaL_checkint(L, narg++) != 0;
		Wgt::getInstance()->setVisible(w, b, onlythis);
	} else {
		Wgt::getInstance()->setVisible(w, b);
	}
	return 0;
}

static int l_isVisible(lua_State* L)
{
	int narg = 2;
	const WidgetId w = luaL_checkint(L, narg++);

	lua_pushnumber(L, Wgt::getInstance()->isVisible(w)? 1 : 0);

	return 1;
}

static int l_newColor(lua_State* L)
{
	int narg = 2;
	const int r = luaL_checkint(L, narg++);
	const int g = luaL_checkint(L, narg++);
	const int b = luaL_checkint(L, narg++);
	
	ColorId id = Wgt::getInstance()->newColor(r, g, b);
	lua_pushnumber(L, id);

	return 1;
}

static int l_open(lua_State* L)
{
	int narg = 2;
	const int id = luaL_checkint(L, narg++);

	Wgt::getInstance()->open(id);

	return 0;
}

static int l_close(lua_State* L)
{
	int narg = 2;
	const int id = luaL_checkint(L, narg++);

	Wgt::getInstance()->close(id);

	return 0;
}

static int l_setEnable( lua_State* L ) {
	const int id = luaL_checkint(L, 2);
	bool enable = luaz::lua::checkBool(L, 3);
	Wgt::getInstance()->setEnable(id, enable);

	return 0;
}

static int l_getName( lua_State* L ) {
	WidgetPtr widget = WidgetPtr();
	if (lua_gettop(L) == 1) {
		widget = Wgt::getInstance()->view()->getTopWidget();
	} else {
		int id = luaL_checkint(L, 2);
		widget = Wgt::getInstance()->view()->getWidgetById(id);
	}

	lua_pushstring(L, widget->widgetname().c_str());
	return 1;
}

static int l_setName( lua_State* L ) {
	const int id = luaL_checkint(L, 2);
	const char* name = luaL_checkstring(L, 3);

	WidgetPtr widget = Wgt::getInstance()->view()->getWidgetById(id);
	widget->widgetname(name);
	return 0;
}

static int l_setZIndex( lua_State* L ) {
	const int id = luaL_checkint(L, 2);
	const unsigned short zindex = (unsigned short) luaL_checkint(L, 3);

	WidgetPtr widget = Wgt::getInstance()->view()->getWidgetById(id);
	widget->zIndex(zindex);
	widget->needsRepaint(true);

	return 0;
}

static int l_getZIndex( lua_State* L ) {
	const int id = luaL_checkint(L, 2);
	WidgetPtr widget = Wgt::getInstance()->view()->getWidgetById(id);

	lua_pushnumber(L, widget->zIndex());
	return 1;
}

static int l_repaint(lua_State* L)
{
	int narg = 2;
	const int id = luaL_checkint(L, narg++);

	Wgt::getInstance()->repaint(id);

	return 0;
}

static int l_fixOffsets(lua_State* L)
{
	const int id = luaL_checkint(L, 2);

	if (lua_gettop(L) == 3) {
		Wgt::getInstance()->fixOffsets(id, luaL_checkint(L, 3));
	} else {
		Wgt::getInstance()->fixOffsets(id);
	}

	return 0;
}

static int l_getTopWidget(lua_State* L)
{
	WidgetPtr widget = Wgt::getInstance()->view()->getTopWidget();
	if (widget) {
		lua_pushnumber(L, widget->id());
	} else {
		lua_pushnumber(L, -1);
	}

	return 1;
}

static int l_setHelp(lua_State* L){
	int narg = 2;
	const WidgetId w = luaL_checkint(L, narg++);
	const char* help = luaL_checkstring(L, narg++);

	Wgt::getInstance()->setHelp(w, help);

	return 0;
}

static int l_getHelp(lua_State* L){
	int narg = 2;
	const WidgetId w = luaL_checkint(L, narg++);

	lua_pushstring(L, Wgt::getInstance()->getHelp(w).c_str());

	return 1;
}

static int l_refresh(lua_State* /* L */)
{
	Wgt::getInstance()->view()->refresh();

	return 0;
}

static int l_restack( lua_State* /* L */ ) {
	Wgt::getInstance()->view()->sortWidgets();
	return 0;
}

static int l_ignoreKey(lua_State* L)
{
	int narg = 2;
	const WidgetId w = luaL_checkint(L, narg++);
	const int key = luaL_checkint(L, narg++);
	const bool b  = luaL_checkint(L, narg++) != 0;

	Wgt::getInstance()->ignoreKey(w, (util::key::type) key, b);
	return 0;
}

static int l_invalidate(lua_State* L) {
	int narg = 2;
	const int id = luaL_checkint(L, narg++);
	Wgt::getInstance()->invalidate(id);

	return 0;
}

static int l_redraw( lua_State* L ) {
	WidgetPtr widget = WidgetPtr();
	const int id = luaL_checkint(L, 2);

	widget = Wgt::getInstance()->view()->getWidgetById(id);

	if (lua_gettop(L) == 3 && luaz::lua::checkBool(L, 3)) {
		widget->clear(Wgt::getInstance()->view());
	}

	widget->needsRepaint(true);
	widget->draw(Wgt::getInstance()->view());

	return 0;
}

static int l_needsClear( lua_State* L ) {
	WidgetPtr widget = WidgetPtr();
	const int id = luaL_checkint(L, 2);
	widget = Wgt::getInstance()->view()->getWidgetById(id);
	widget->needsClear(luaz::lua::checkBool(L, 3));

	return 0;
}

static int l_setColorMap( lua_State* L ) {
	WidgetPtr widget = WidgetPtr();
	const int id = luaL_checkint(L, 2);
	const colorMap_t colorMap = parseColorTable(L, 3);

	widget = Wgt::getInstance()->view()->getWidgetById(id);

	widget->colorMap(colorMap);

	return 0;
}

static int l_allowDesaturation( lua_State* L ) {
	const WidgetId id = luaL_checkint(L, 2);
	WidgetPtr widget = Wgt::getInstance()->view()->getWidgetById(id);

	if (lua_gettop(L) == 3) {
		widget->allowDesaturation(luaz::lua::checkBool(L, 3));
		return 0;
	} else {
		lua_pushnumber(L, widget->allowDesaturation());
		return 1;
	}
}

const struct luaL_Reg wgt_methods[] = {
	{"getName",                 l_getName                 },
	{"setName",                 l_setName                 },
	{"setZIndex",               l_setZIndex               },
	{"setEnable",               l_setEnable               },
	{"setColorMap",             l_setColorMap             },
	{"getZIndex",               l_getZIndex               },
	{"setVisible",              l_setVisible              },
	{"isVisible",               l_isVisible               },
	{"allowDesaturation",       l_allowDesaturation       },
	{"position",                l_position                },
	{"bounds",                  l_bounds                  },
	{"setValue",                l_setValue                },
	{"processKey",              l_processKey              },
	{"newColor",                l_newColor                },
	{"fixOffsets",              l_fixOffsets              },
	{"open",                    l_open                    },
	{"close",                   l_close                   },
	{"repaint",                 l_repaint                 },
	{"needsClear",              l_needsClear              },
	{"getTopWidget",            l_getTopWidget            },
	{"setHelp",                 l_setHelp                 },
	{"getHelp",                 l_getHelp                 },
	{"ignoreKey",               l_ignoreKey               },
	{"invalidate",              l_invalidate              },
	{"redraw",                  l_redraw                  },
	{"size",                    l_size                    },
	{NULL,                      NULL                      }
};

const struct luaL_Reg view_methods[] = {
	{"refresh",                   l_refresh            },
	{"restack",                   l_restack            },
	{NULL,                        NULL                 }
};



Wgt::Wgt(View* view)
{
	_view = view;
}

Wgt::~Wgt(){
}

void setField( lua_State *st, const std::string &index, const int &value ) {
	lua_pushstring(st, index.c_str());
	lua_pushnumber(st, value );
	lua_settable(st, -3);
}

void registerAlignments(lua_State *L)
{
	lua_newtable(L);
	setField(L, "vTop_hLeft",      (int)(alignment::vTop|alignment::hLeft));
	setField(L, "vTop_hCenter",    (int)(alignment::vTop|alignment::hCenter));
	setField(L, "vTop_hRight",     (int)(alignment::vTop|alignment::hRight));

	setField(L, "vCenter_hLeft",   (int)(alignment::vCenter|alignment::hLeft));
	setField(L, "vCenter_hCenter", (int)(alignment::vCenter|alignment::hCenter));
	setField(L, "vCenter_hRight",  (int)(alignment::vCenter|alignment::hRight));

	setField(L, "vBottom_hLeft",   (int)(alignment::vBottom|alignment::hLeft));
	setField(L, "vBottom_hCenter", (int)(alignment::vBottom|alignment::hCenter));
	setField(L, "vBottom_hRight",  (int)(alignment::vBottom|alignment::hRight));

	lua_setglobal(L, "Alignment" );
}

void Wgt::init(View* view, lua_State* L)
{
	_instance = new Wgt(view);

	luaL_register(L, "wgt",  wgt_methods);
	luaL_register(L, "view", view_methods);
	registerAlignments(L);

	_lua = L;
	
	wgt::window::init(L);
	wgt::bar::init(L);
	wgt::list::init(L);
	wgt::label::init(L);
	wgt::progressbar::init(L);

	wgt::button::init(L);
	wgt::buttonset::init(L);
	wgt::buttongrid::init(L);
	wgt::combobox::init(L);
	wgt::image::init(L);
	wgt::imageviewer::init(L);
	wgt::videoviewer::init(L);
	wgt::slidebar::init(L);
	wgt::textarea::init(L);
	wgt::verticalset::init(L);
	wgt::schedule::init(L);
	wgt::scroll::init(L);

	wgt::yesnodialog::init(L);
	wgt::pindialog::init(L);
	wgt::basicdialog::init(L);
}

void Wgt::fin()
{
	DEL(_instance);
}

Wgt* Wgt::getInstance()
{
	return _instance;
}

View* Wgt::view()
{
	return getInstance()->_view;
}

ColorId Wgt::newColor(int r, int g, int b){
	Color color(r, g, b);
	ColorId id = _view->addColor(color);
	return id;
}

void Wgt::setPosition(WidgetId w, int x, int y)
{
	WidgetPtr widget = _view->getWidgetById(w);
	widget->x(x);
	widget->y(y);
	widget->needsRepaint(true);
}

void Wgt::setBounds(WidgetId widg, int x, int y, int w, int h)
{
	WidgetPtr widget = _view->getWidgetById(widg);
	widget->x(x);
	widget->y(y);
	widget->w(w);
	widget->h(h);
	widget->needsRepaint(true);
}

void Wgt::size( WidgetId id, int &w, int &h ) {
	WidgetPtr widget = _view->getWidgetById(id);
	w = widget->w();
	h = widget->h();
}

void Wgt::ignoreKey(WidgetId w, util::key::type key, bool flag)
{
	WidgetPtr widget = _view->getWidgetById(w);
	widget->ignoreKey(key, flag);
}

void Wgt::setVisible(WidgetId w, bool visible, bool /* redrawOnlyThis */){
	WidgetPtr widget = _view->getWidgetById(w);
	widget->setVisible(visible);
	widget->needsRepaint(true);
}

bool Wgt::isVisible(WidgetId w)
{
	WidgetPtr widget = _view->getWidgetById(w);
 	return widget->isVisible();
}

void Wgt::setValue(WidgetId w, int value){
	WidgetPtr widget = _view->getWidgetById(w);
	widget->setValue(value);
}

int Wgt::processKey(WidgetId w, int key){
	WidgetPtr widget = _view->getWidgetById(w);
	if (widget) {
		bool redraw = widget->processKey(util::key::type(key));
		return redraw;
	} else {
		LERROR("Wgt", "processKey fail. widget not found");
	}
	return 0;
}

void Wgt::open(WidgetId w)
{
	WidgetPtr widget = _view->getWidgetById(w);
	widget->open();
	widget->needsRepaint(true);
}

void Wgt::close(WidgetId w)
{
	WidgetPtr widget = _view->getWidgetById(w);
	widget->close();
	if (_view->getTopWidget()) {
		_view->getTopWidget()->needsRepaint(true);
	}
}

void Wgt::repaint(WidgetId w)
{
	WidgetPtr widget = _view->getWidgetById(w);
	widget->needsRepaint(true);
}

void Wgt::invalidate(WidgetId w)
{
	WidgetPtr widget = _view->getWidgetById(w);
	widget->invalidate();
}

void Wgt::fixOffsets( WidgetId w)
{
	WidgetPtr widget = _view->getWidgetById(w);
	widget->fixOffsets();
}

void Wgt::setEnable( WidgetId w, bool enable ) {
	WidgetPtr widget = _view->getWidgetById(w);
	widget->enable(enable);
}

void Wgt::fixOffsets( WidgetId wID, WidgetId winID) {
	WidgetPtr widget = _view->getWidgetById(wID);
	WidgetPtr win = _view->getWidgetById(winID);
	widget->fixOffsets(win.get());
}

void Wgt::setHelp(WidgetId w, const std::string& help)
{
	WidgetPtr widget = _view->getWidgetById(w);
	widget->help(help);
}

const std::string& Wgt::getHelp(WidgetId w)
{
	WidgetPtr widget = _view->getWidgetById(w);
	return widget->help();
}

void Wgt::call(const std::string& f)
{
	luaz::lua::call(_lua, f.c_str());
}

void Wgt::call(const std::string& f, int n)
{
	luaz::lua::call(_lua, f.c_str(), n);
}

lua_State* Wgt::luaState() {
	return _lua;
}

}

