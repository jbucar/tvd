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

#include "widget.h"

#include "../view.h"
#include "util/log.h"

namespace wgt{

int wcount = 0;

Widget::Widget(int x, int y, int w, int h, const colorMap_t &aMap)
	: _x(x)
	, _y(y)
	, _w(w)
	, _h(h)
	, _zIndex(1)
	, _visible(true)
	, _needsClear(false)
	, _needsRepaint(false)
	, _active(false)
	, _enabled(true)
	, _help("")
	, _parent(0)
	, _colorMap(aMap)
	, _allowDesaturation(true)
{}

Widget::~Widget() {}

//getters
int Widget::x(){
	return _x;
}
int Widget::y(){
	return _y;
}
int Widget::w(){
	return _w;
}
int Widget::h(){
	return _h;
}

const colorMap_t &Widget::colorMap() {
	return _colorMap;
}

unsigned short Widget::zIndex(){
	return _zIndex;
}

bool Widget::isVisible(){
	return _visible && (!_parent || _parent->isVisible());
}

bool Widget::isActive(){
	return _active;
}

bool Widget::needsClear() {
	return _needsClear;
}

bool Widget::needsRepaint() {
	return _needsRepaint || (_parent && _parent->needsRepaint());
}

Widget* Widget::parent()
{
	return _parent;
}

bool Widget::isEnabled()
{
	return _enabled;
}

void Widget::enable(bool enabled/*=true*/)
{
	_enabled = enabled;
}

const Color Widget::getColor( const std::string &component ) {
	return _colorMap[component];
}

void Widget::setColor( const std::string &component, const Color &aColor ) {
	_colorMap[component] = aColor;
}

//setters
void Widget::x(int x){
	_x = x;
}
void Widget::y(int y){
	_y = y;
}
void Widget::w(int w){
	_w = w;
}
void Widget::h(int h){
	_h = h;
}
void Widget::zIndex(unsigned short zIndex){
	_zIndex = zIndex;
}

void Widget::colorMap( const colorMap_t &colorMap ) {
	_colorMap = colorMap;
}

void Widget::setVisible(bool visible){
	_visible = visible;
}

void Widget::needsRepaint(bool flag) {
	_needsRepaint = flag;
}

void Widget::needsClear(bool flag) {
	_needsClear = flag;
}

void Widget::setValue(int /*value*/){
}

void Widget::setText(const std::string& /*text*/){
}

void Widget::clear(View* view){
	if (_visible || _needsClear) {
		_needsClear = false;
		view->clear(this);
	}
}

void Widget::fixOffsets(Widget* window)
{
	if (window) {
		_x += window->x();
		_y += window->y();
	}
}

void Widget::activate(bool b, bool /* fromTop */)
{
	_active = b;
	needsRepaint(true);
}

void Widget::open()
{
	if (!_visible) {
		LDEBUG("Widget", "Open(%s)", widgetname().c_str());
		setVisible(true);
		View::instance()->push(shared_from_this());
	}
}
void Widget::close()
{
	if (_visible) {
		setVisible(false);
		View::instance()->pop(shared_from_this());
	}
}


const std::string& Widget::help()
{
	return currentHelp(); //_help;
}

void Widget::help(const std::string& h)
{
	_help = h;
}

const std::string& Widget::currentHelp()
{
	return _help;
}

void Widget::id(WidgetId id)
{
	_id = id;
}

WidgetId Widget::id()
{
	return _id;
}

void Widget::parent(Widget* w)
{
	_parent = w;
}

void Widget::updatePreviousActiveChild(){}
void Widget::rawNeedsRepaint(bool /* flag */){}

void Widget::widgetname(const std::string& n)
{
	_widgetname = n;
}
const std::string& Widget::widgetname()
{
	return _widgetname;
}

bool Widget::allowDesaturation() {
	if (_parent) {
		return _parent->allowDesaturation() || _allowDesaturation;
	} else {
		return _allowDesaturation;
	}
}

void Widget::allowDesaturation( bool allow ) {
	_allowDesaturation = allow;
}


void Widget::invalidate() {
	View::instance()->invalidate(shared_from_this());
}

bool Widget::compareWidgets(WidgetPtr w1, WidgetPtr w2){
	return w1->zIndex() < w2->zIndex();
}

}

