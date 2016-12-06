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

#include "../alignment.h"
#include <canvas/color.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <string>
#include <vector>
#include <map>

typedef struct lua_State lua_State;

namespace canvas {
	class Canvas;
}

namespace zapper {
	class Zapper;
}

namespace canvas {

bool init( zapper::Zapper *mgr );
void fin( void );
void start( lua_State *lua );

void flush();
void splitLines( int w, const std::string &text, int borderWidth, int alignment, int margin, unsigned int maxLines, int fontSize, std::vector<std::string>& v_lines);
void getSize( int &w, int &h );
void drawRect(int x, int y, int w, int h);
void fillRect(int x, int y, int w, int h);
void clear(int x, int y, int w, int h);
void setColor(int r, int g, int b, int a=255);
void text( int x, int y, const char *t );
int fontAscent();
int fontDescent();
void setFont(const char* face, int size);
void textSize(const std::string& text, int &w, int &base);
int textWidth( const std::string &fontName, const std::string& text, int size );
int textWidth( const std::string &fontName, char c, int size );
void drawImage( const std::string &path, int x, int y, int w=-1, int h=-1);
void imageSize( const std::string &path, int &w, int &h );
void drawImageFullScreen(const char* path);

typedef std::map<size_t, std::map<std::string,std::string> > ColumnMapping;
void drawList(
		  int x, int y
		, const std::vector<int>& cell_widths
		, int rowHeight
		, int rowSep
		, const std::vector<std::string>& headers
		, const std::vector<std::vector<std::string> >& values
		, const std::vector<wgt::Alignment> &alignments
		, int fontSize
		, const ::canvas::Color& bgColor
		, const ::canvas::Color& cellColor
		, const ::canvas::Color& selectedCellColor
		, const ::canvas::Color& textColor
		, const ::canvas::Color& selectedTextColor
		, const ::canvas::Color& headerColor
		, const ::canvas::Color& textHeaderColor
		, int selected
		, int from
		, int to
		, int rowCount
		, ColumnMapping& images_normal
		, ColumnMapping& images_selected
	);

std::pair<int,int> screenSize();
void fillPolygon(const std::vector<std::pair<int,int> >& vertices);

void str_list_to_time_period_list(std::vector<std::string>& source, std::vector<std::pair<boost::posix_time::ptime,boost::posix_time::ptime> >& dest);


}

