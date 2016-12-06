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
#include "view.h"
#include "widget/window.h"
#include "widget/borderedwindow.h"
#include "widget/label.h"
#include "widget/maskedlabel.h"
#include "widget/image.h"
#include "widget/imageviewer.h"
#include "widget/videoviewer.h"
#include "widget/list.h"
#include "widget/textarea.h"
#include "widget/button.h"
#include "widget/buttongrid.h"
#include "widget/bar.h"
#include "widget/combobox.h"
#include "widget/frame.h"
#include "widget/schedule.h"
#include "widget/schedulerow.h"
#include "widget/scrollbar.h"
#include "widget/scrollarrow.h"
#include "modules/wgt.h"
#include "modules/canvas.h"
#include <util/log.h>
#include <util/mcr.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/math/special_functions/round.hpp>

#define EMPTY_CC_W 225
#define EMPTY_CC_H  35

namespace wgt{
	namespace impl {
		View* instance = NULL;
	}

	void View::init(lua_State *L)
	{
		impl::instance = new View();

		wgt::Wgt::init(impl::instance, L);
	}
	void View::fin()
	{
		LDEBUG( "View", "Finalizing" );

		wgt::Wgt::fin();
		DEL(impl::instance);
	}

	View* View::instance() {
		return impl::instance;
	}

	View::View()
		: _cId(0)
		, _wId(0)
		, _fontFace("Tiresias")
		, _needsFullRedraw(true)
	{}

	View::~View(){}

	ColorId View::addColor(const Color& color){
		ColorId id = getNextColorId();
		_colors[id] = color;
		return id;
	}

	const std::string &View::fontFace() {
		return _fontFace;
	}

	WidgetId View::addWidget(WidgetPtr widget){
		WidgetId id = getNextWidgetId();
		_keyHandlingWidgets[id] = widget;
		widget->id(id);
		return id;
	}

	WidgetId View::getNextWidgetId(){
		return _wId++;
	}

	ColorId View::getNextColorId(){
		return _cId++;
	}

	Color  View::getColorById(ColorId id){
		return _colors[id];
	}

	WidgetPtr View::getWidgetById(WidgetId id){
		return _keyHandlingWidgets[id];
	}

	WidgetPtr View::getTopWidget() {

		if (_widgets.size()) {
			return _widgets.back();
		} else {
			return WidgetPtr();
		}
	}

	void View::sortWidgets() {
		std::sort(_widgets.begin(), _widgets.end(), Widget::compareWidgets);
	}

	void View::push(WidgetPtr widget){
		_needsFullRedraw = true;
		_widgets.push_back(widget);
		sortWidgets();
	}

	void View::pop(WidgetPtr widget){
		_needsFullRedraw = true;

		std::vector<WidgetPtr>::iterator it = std::find(_widgets.begin(), _widgets.end(), widget);
		if (it != _widgets.end()) {
			_widgetsToClear.push_back(*it);
			_widgets.erase(it);
			sortWidgets();
		} else {
			LDEBUG("View", "Try to pop an unpushed widget");
		}
	}

	void View::drawWidget(WidgetPtr widget) {
		if (widget->needsClear()) {
			LTRACE( "view", "Clear widget" );
			widget->clear(this);
		}
		if (widget->isVisible()) {
			widget->draw(this);
		}
	}

	void View::refresh() {
		if (_widgets.size()) {
			LTRACE("View", "Refresh");
			BOOST_FOREACH(WidgetPtr widget, _widgetsToClear) {
				clear(widget.get());
			}
			if (_needsFullRedraw) {
				LDEBUG("View", "Full redraw");
				BOOST_FOREACH(WidgetPtr widget, _widgets) {
					_desaturateColors = widget != getTopWidget() && widget->allowDesaturation();
					LTRACE("View", "Draw widget(%s) visible=%d", widget->widgetname().c_str(), widget->isVisible());
					drawWidget(widget);
				}
				_needsFullRedraw = false;
				_desaturateColors = false;
			} else {
				LTRACE("View", "Draw TOP widget(%s)", getTopWidget()->widgetname().c_str());
				_desaturateColors = false;
				drawWidget(getTopWidget());
			}
		} else {
			LTRACE("View", "Clear screen");
			BOOST_FOREACH(WidgetPtr widget, _widgetsToClear) {
				widget->needsClear(true);
				widget->clear(this);
			}
		}
		_widgetsToClear.clear();

		canvas::flush();
	}

	void View::clear(Widget* widget) {
		LTRACE("View", "Clear(%d, %d, %d, %d)", widget->x(), widget->y(), widget->w(), widget->h());
		canvas::clear(widget->x(), widget->y(), widget->w(), widget->h());
	}

	Color View::adjust(const Color& c)
	{
		if (c!=Color::None && _desaturateColors) {
			return c.toGrayscale();
		}
		return c;
	}

	void View::drawChildren(Window* window){
		LTRACE("View", "Window drawChildren");
		WidgetPtr active;

		BOOST_FOREACH(WidgetPtr widget, window->getChildren()) {
			if (widget) {
				if (widget->isActive()) {
					active = widget;
				} else {
					LTRACE("View", "Draw widget children(%s) needsRepaint(%d)", widget->widgetname().c_str(), widget->needsRepaint());
					drawWidget(widget);
				}
			}
		}
		if (active) {
			LTRACE("View", "Draw active children(%s) needsRepaint(%d)", active->widgetname().c_str(), active->needsRepaint());
			drawWidget(active);
		}

	}

	void View::draw(Frame* frame) {
		if (frame->needsRepaint()||_needsFullRedraw) {
			Color c = adjust( frame->getColor( "bgColor" ) );
			if (c != Color::None) {
				canvas::setColor(c.r(), c.g(), c.b());
				const canvas::Point &outPos = frame->outterPos();
				const canvas::Size &outSize = frame->outterSize();
				const canvas::Point &inPos = frame->innerPos();
				const canvas::Size &inSize = frame->innerSize();
				canvas::fillRect( outPos.x, outPos.y, outSize.w, inPos.y-outPos.y );
				canvas::fillRect( inPos.x+inSize.w, inPos.y, (outPos.x+outSize.w)-(inPos.x+inSize.w), inSize.h );
				canvas::fillRect( outPos.x, inPos.y+inSize.h, outSize.w, (outPos.y+outSize.h)-(inPos.y+inSize.h) );
				canvas::fillRect( outPos.x, inPos.y, inPos.x-outPos.x, inSize.h );
				canvas::clear( inPos.x, inPos.y, inSize.w, inSize.h );
			}
			if (_needsFullRedraw) {
				frame->needsRepaint(false);
			}
		}
		drawChildren(frame);
		frame->needsRepaint(false);
	}

	void View::draw( ScrollArrow *scroll ) {
		int leftW, leftH, rightW, rightH;
		canvas::imageSize(scroll->arrowLeft(), leftW, leftH);
		canvas::imageSize(scroll->arrowRight(), rightW, rightH);

		int y = scroll->y() + boost::math::iround(float(scroll->h() - rightH)/2.0f);
		canvas::drawImage( scroll->arrowLeft(), scroll->x(), y);
		canvas::drawImage( scroll->arrowRight(), scroll->x() + scroll->w() - rightW, y);
	}

	void View::draw( Scroll* scroll ) {
		if (scroll->needsRepaint() || _needsFullRedraw) {
			Color c = adjust( scroll->bgColor() );
			if (c != Color::None) {
				canvas::setColor(c.r(), c.g(), c.b());
				canvas::fillRect(scroll->x(), scroll->y(), scroll->w(), scroll->h());
			}
			if (_needsFullRedraw) {
				scroll->needsRepaint(false);
			}
		}

		this->draw(scroll->view());

		BOOST_FOREACH(WidgetPtr widget, scroll->getChildren()) {
			drawWidget(widget);
		}

		scroll->needsRepaint(false);
	}

	void View::draw(Window* window){
		if (window->needsRepaint()||_needsFullRedraw) {
			Color c = adjust( window->bgColor() );
			if (c != Color::None) {
				canvas::setColor(c.r(), c.g(), c.b());
				canvas::fillRect(window->x(), window->y(), window->w(), window->h());
			}
			if (_needsFullRedraw) {
				window->needsRepaint(false);
			}
		}

		drawChildren(window);
		window->needsRepaint(false);
	}

	void View::draw(Button* but) {
		if (but->needsRepaint()||_needsFullRedraw) {
			Color c = adjust( but->bgColor() );
			ImagePtr icon = but->icon();
			if (c != Color::None) {
				canvas::setColor(c.r(), c.g(), c.b());
				canvas::fillRect(but->x(), but->y(), but->w(), but->h());
			}
			if (icon != NULL) {
				draw( but->icon().get() );
			}

			draw( but->label().get() );
			but->needsRepaint(false);
		}
	}

	void View::draw(BorderedWindow* borderedWindow){
		if (borderedWindow->needsRepaint()||_needsFullRedraw) {
			int x = borderedWindow->x();
			int y = borderedWindow->y();
			int w = borderedWindow->w();
			int h = borderedWindow->h();
			int border = borderedWindow->borderWidth();
			Color c =adjust( borderedWindow->getColor( "borderColor" ) );
			if (c != Color::None && border != 0) {
				canvas::setColor(c.r(), c.g(), c.b());
				//	BORDER
				canvas::fillRect(x, y, w, h);
			}

			//	BACKGROUND
			c = adjust( borderedWindow->getColor( "bgColor" ) );
			if (c != Color::None) {
				canvas::setColor(c.r(), c.g(), c.b());
				canvas::fillRect(x + border, y + border, w - border*2, h - border*2);
			}
			if (_needsFullRedraw) {
				borderedWindow->needsRepaint(false);
			}
		}
		drawChildren(borderedWindow);
		borderedWindow->needsRepaint(false);
	}

	void View::draw(Label* label){
		if (label->needsRepaint()||_needsFullRedraw) {
			Color c = adjust( label->getColor( "bgLabelColor" ) );

			if (c != Color::None) {
				canvas::setColor(c.r(), c.g(), c.b());
				canvas::fillRect(label->x(), label->y(), label->w(), label->h());
			}

			c = adjust( label->getColor( "textColor" ) );
			canvas::setColor(c.r(), c.g(), c.b());

			int size = label->fontSize();
			canvas::setFont( _fontFace.c_str(), size );

			std::string text = label->text();
			int w,h;
			canvas::textSize(text, w, h);
			std::pair<int,int> xy = align(label->alignment(), label->x(), label->y(), w, h, label->w(), label->h(), label->margin());

			canvas::text(xy.first, xy.second, (char*)(text.c_str()));
		}
		label->needsRepaint(false);
	}

	void View::draw(MaskedLabel* label){
		if (label->needsRepaint()||_needsFullRedraw) {
			Color c = adjust( label->getColor( "bgLabelColor" ) );

			if (c != Color::None) {
				canvas::setColor(c.r(), c.g(), c.b());
				canvas::fillRect(label->x(), label->y(), label->w(), label->h());
			}

			c = adjust( label->getColor( "textColor" ) );
			canvas::setColor(c.r(), c.g(), c.b());

			int size = label->fontSize();
			canvas::setFont( _fontFace.c_str(), size );

			std::string text = label->text();
			if (text[0] == '_' && text[1] == '_') {
				text.replace(0,2,"_ _");
			}
			int w,h;
			canvas::textSize(text, w, h);
			std::pair<int,int> xy = align(label->alignment(), label->x(),label->y(), w,h, label->w(), label->h(), label->margin());

			canvas::text(xy.first, xy.second, (char*)(text.c_str()));
		}
		label->needsRepaint(false);
	}

	const std::string& View::adjustedPath(Image* img)
	{
		return (_desaturateColors)? img->dimmedPath() :  img->path();
	}

	void View::draw(Image* image){
		if (image != NULL && !image->path().empty() && (image->needsRepaint()||_needsFullRedraw)) {
			int w, h;
			const char *path = adjustedPath(image).c_str();
			canvas::imageSize(path, w, h);
			int x = image->x() + (image->w() - w) / 2;
			int y = image->y() + (image->h() - h) / 2;
			canvas::drawImage(path, x, y, image->w(), image->h());
		}
		image->needsRepaint(false);
	}

	void View::draw(Bar* bar)
	{
		if (bar->needsRepaint()||_needsFullRedraw) {
			Color c = adjust( bar->getColor( "bgBarColor" ) );
			canvas::setColor(c.r(), c.g(), c.b());
			int value = bar->value();
			int minValue = bar->minValue();
			int maxValue = bar->maxValue();
			int p = (100*(value-minValue))/(maxValue-minValue);
			int w = bar->w() * p / 100;
			canvas::fillRect(bar->x(), bar->y(), w, bar->h());
		}
		bar->needsRepaint(false);
	}

	::canvas::Color View::toLuaColor(const Color& color)
	{
		Color c = adjust(color);
		return ::canvas::Color((util::BYTE)c.r(), (util::BYTE)c.g(), (util::BYTE)c.b());
	}

	void View::draw(List* list)
	{
		if (list->needsRepaint()||_needsFullRedraw) {
			canvas::setFont(_fontFace.c_str(), list->fontSize());
			::canvas::Color bgColor           = toLuaColor( list->getColor( "bgColor" ) );
			::canvas::Color cellColor         = toLuaColor( list->getColor( "cellColor" ) );
			::canvas::Color selectedCellColor = toLuaColor( list->getColor( "selectedCellColor") );
			::canvas::Color textColor         = toLuaColor( list->getColor( "textColor" ) );
			::canvas::Color selectedTextColor = toLuaColor( list->getColor( "selectedTextColor" ));
			::canvas::Color headerColor       = toLuaColor( list->getColor( "headerColor" ) );
			::canvas::Color textHeaderColor   = toLuaColor( list->getColor( "textHeaderColor" ) );
			::canvas::Color scrollColor       = toLuaColor( list->getColor( "scrollColor" ) );
			::canvas::Color scrollBgColor     = toLuaColor( list->getColor( "scrollBgColor" ) );
			canvas::drawList(
					list->x(), list->y()
					, list->cellWidths()
					, list->rowHeight()
					, list->rowSep()
					, list->cellHeaders()
					, list->contents()
					, list->cellAlignments()
					, list->fontSize()
					, bgColor, cellColor, selectedCellColor
					, textColor, selectedTextColor
					, headerColor, textHeaderColor
					, list->current()
					, list->from(), list->to(), list->visible()
					, list->images_normal()
					, list->images_selected()
					);
			this->draw( list->scrollBar() );
		}
		list->needsRepaint(false);
	}

	void View::draw( Scrollbar *scrollbar ) {
		int upW, upH, downW, downH;
		canvas::imageSize(scrollbar->arrowUp(), upW, upH);
		canvas::imageSize(scrollbar->arrowDown(), downW, downH);

		bool check = true;
		check &= upW>0;
		check &= upH>0;
		check &= downW>0;
		check &= downH>0;

		if (check) {
			Color c = adjust(scrollbar->bgColor());
			canvas::setColor(c.r(), c.g(), c.b());
			canvas::fillRect(scrollbar->scroll.x, scrollbar->scroll.y, scrollbar->scroll.w, scrollbar->scroll.h);

			int arrowXCentered = scrollbar->scroll.x+boost::math::iround(float(scrollbar->scroll.w-downW)/2.0f);
			canvas::drawImage( scrollbar->arrowUp(), arrowXCentered, scrollbar->scroll.y+scrollbar->scroll.margin );
			canvas::drawImage( scrollbar->arrowDown(), arrowXCentered, scrollbar->scroll.y+scrollbar->scroll.h-scrollbar->scroll.margin-downH );

			c = adjust(scrollbar->scrollColor());
			canvas::setColor(c.r(), c.g(), c.b());

			int fixedScrollH = scrollbar->scroll.h - upH - downH - (scrollbar->scroll.margin*2);
			int fixedH = boost::math::iround( float(fixedScrollH * scrollbar->handle.h) / float(scrollbar->scroll.h) );
			int fixedY = scrollbar->scroll.y + scrollbar->scroll.margin + upH + boost::math::iround( float(fixedScrollH * (scrollbar->handle.y-scrollbar->scroll.y)) / float(scrollbar->scroll.h) );
			canvas::fillRect(scrollbar->handle.x, fixedY, scrollbar->handle.w, fixedH);

		} else {
			LWARN("View", "Fail to draw scrollbar");
		}
	}

	void View::fitToSize(int text_w, int text_h, int margin, int &x, int &w, int &h)
	{
		if (text_w == 0) {
			text_w = EMPTY_CC_W;
			text_h = EMPTY_CC_H;
		}

		std::pair<int,int> size = canvas::screenSize();
		x = (size.first  - text_w)/2;
		w = text_w + 2*margin;
		h = text_h;
	}

	void View::draw(TextArea* textArea) {
		if (textArea->needsRepaint()||_needsFullRedraw) {
			Color c = Color::None;
			int size = textArea->fontSize();
			canvas::setFont(_fontFace.c_str(), size);

			std::vector<std::string> v_lines;
			canvas::splitLines(textArea->w(), textArea->text(), textArea->borderWidth(), textArea->alignment(), textArea->margin(), textArea->lines(), textArea->fontSize(), v_lines);

			int margin = textArea->margin();
			int leading = textArea->leading();
			int text_h = v_lines.size() * (5+size + leading);
			int alignment = textArea->alignment();
			int x = 0;
			int y = aligny(alignment, textArea->y(), text_h, textArea->h(), textArea->margin());

			std::vector<std::pair<int, int> > positions;
			int maxW = 0;

			for (size_t i = 0; i < v_lines.size(); ++i) {
				int h,text_w = 0;
				canvas::textSize(v_lines[i], text_w, h);
				maxW = std::max(text_w, maxW);
				x = alignx(alignment, textArea->x(), text_w, textArea->w(), margin);
				positions.push_back(std::make_pair(x, y + i*(5+size + leading)));
			}

			int tx, tw, th;
			if (textArea->fitToText()) {
				fitToSize(maxW, v_lines.size()*(5+size + leading) + 2*margin+ size, margin, tx, tw, th);
			}else{
				tx = textArea->x();
				tw = textArea->w();
				th = textArea->h();
			}

			int borderWidth = textArea->borderWidth();
			if (borderWidth && textArea->getColor( "borderColor" ) != Color::None) {
				Color c = adjust( textArea->getColor( "borderColor" ) );
				canvas::setColor(c.r(), c.g(), c.b());
				canvas::fillRect(tx, textArea->y(), tw, th);
			}

			c = adjust( textArea->bgColor() );
			if (c != Color::None) {
				canvas::setColor(c.r(), c.g(), c.b());
				canvas::fillRect(tx + borderWidth, textArea->y() + borderWidth, tw - borderWidth*2, th - borderWidth*2);
			}

			c = adjust( textArea->textColor() );
			canvas::setColor(c.r(), c.g(), c.b());

			for (size_t i = 0; i < v_lines.size(); ++i) {
				std::pair<int, int> point = positions[i];
				canvas::text(point.first, point.second, (char*)(v_lines[i].c_str()));
			}

		}
		textArea->needsRepaint(false);
	}

	void View::draw( ComboBox *combo ) {
		if (combo->Widget::needsRepaint()||_needsFullRedraw){
			if (!combo->isEnabled()) {
				combo->combo()->setColor( "borderColor", combo->getColor("disabledColor") );
			}
			draw(static_cast<Window*>(combo));
		}
		combo->needsRepaint(false);
	}

	void View::draw(ImageViewer* iv)
	{
		ImagePtr image = iv->currentImage();
		canvas::drawImageFullScreen(image->path().c_str());
	}

	void View::draw(VideoViewer* /*vv*/)
	{
		int w,h;
		canvas::getSize( w, h );
		canvas::clear(0, 0, w, h );
	}

	void View::draw(ScheduleRow* row)
	{
		if (row->needsRepaint() || _needsFullRedraw) {
			row->title()->y(row->y());
			row->title()->draw(this);

			BOOST_FOREACH(const ScheduleRow::Cell& cell, row->cells()) {
				if (cell._widget->isVisible()) {
					cell._widget->needsRepaint(true);
					cell._widget->y(row->y());
					cell._widget->draw(this);
				}
			}
		}
		row->needsRepaint(false);
	}

	void View::draw(Schedule* schedule)
	{
		if (schedule->needsRepaint() || _needsFullRedraw) {
			Color c = adjust( schedule->getColor( "bgColor" ) );
			if (c != Color::None) {
				canvas::setColor(c.r(), c.g(), c.b());
				canvas::fillRect(schedule->x(), schedule->y(), schedule->w(), schedule->h());
			}
			schedule->header()->draw(this);
		}

		for (unsigned int i=schedule->scrollbar()->from(); i <= schedule->scrollbar()->to(); ++i) {
			ScheduleRowPtr row = schedule->getRow(i);
			row->draw(this);
		}

		schedule->needsRepaint(false);
	} // draw

	void View::invalidate(WidgetPtr widget) {
		if (widget != getTopWidget()) {
			_needsFullRedraw = true;
		}
	}

} // wgt
