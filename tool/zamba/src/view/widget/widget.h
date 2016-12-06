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

#ifndef WIDGET_H
#define WIDGET_H

#pragma once

#include "nav.h"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <vector>
#include <map>
#include <string>

#define DEF_SHARED(Class) typedef boost::shared_ptr<Class> Class##Ptr

typedef int WidgetId;

namespace wgt{

class View;
class Widget;
class Color;

typedef std::map<std::string, Color> colorMap_t;

DEF_SHARED(Widget);

class Widget : public NavController
	, public boost::enable_shared_from_this<Widget>
{
	public:
		Widget(int x, int y, int w, int h, const colorMap_t &aMap);
		virtual ~Widget();
		virtual void draw(View* view) = 0;
		
		virtual int x();
		int y();
		int w();
		int h();
		unsigned short zIndex();
		bool isVisible();
		bool isActive();
		bool needsClear();
		bool needsRepaint();
		bool isEnabled();
		void enable(bool enabled=true);
		Widget* parent();
		const colorMap_t &colorMap();
		const Color getColor( const std::string &component );
		void setColor( const std::string &component, const Color &aColor );

		virtual void x(int x);
		void y(int y);
		void w(int w);
		void h(int h);
		void colorMap( const colorMap_t &colorMap);
		void zIndex(unsigned short zIndex);
		virtual void setVisible(bool visible);
		virtual void setValue  (int value);
		virtual void setText   (const std::string& text);
		virtual void needsRepaint(bool flag);
		void needsClear(bool flag);
		
		void clear(View* view);

		virtual void fixOffsets(Widget* w=0);
		virtual void activate(bool b, bool fromTop=true);

		virtual bool allowDesaturation();
		virtual void allowDesaturation( bool allow );

		virtual void open();
		virtual void close();

		const std::string& help();
		void help(const std::string& h);

		void id(WidgetId id);
		WidgetId id();

		void parent(Widget* w);
		
		virtual void updatePreviousActiveChild();
		virtual void rawNeedsRepaint(bool flag);

		const std::string& widgetname();
		void widgetname(const std::string& n);

		void invalidate();

		static bool compareWidgets(WidgetPtr w1, WidgetPtr w2);

	protected:
		int _x;
		int _y;
		int _w;
		int _h;
		unsigned short _zIndex;
		bool _visible;
		bool _needsClear;
		bool _needsRepaint;
		bool _active;
		bool _enabled;


		std::string _help;

		WidgetId _id;

		Widget* _parent;

		std::string _widgetname;
		colorMap_t _colorMap;

		virtual const std::string& currentHelp();

	private:
		bool _allowDesaturation;

};

}

#endif // WIDGET_H
