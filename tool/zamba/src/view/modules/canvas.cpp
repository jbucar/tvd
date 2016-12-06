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

#include "canvas.h"
#include "../alignment.h"
#include "../view.h"
#include "generated/config.h"
#include <luaz/ui.h>
#include <luaz/lua.h>
#include <zapper/zapper.h>
#include <canvas/layer/layer.h>
#include <canvas/system.h>
#include <canvas/canvas.h>
#include <canvas/surface.h>
#include <canvas/fontinfo.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/math/special_functions/round.hpp>
#include <sstream>
#include <algorithm>
#include <stdlib.h>

namespace canvas {

namespace fs = boost::filesystem;

namespace impl {

	typedef std::map<char, int> FontCharSizeType;
	typedef std::map<int,FontCharSizeType> FontSizeType;
	typedef std::map<std::string,FontSizeType> FontCache;

	static FontCache fontSizes;
	static std::map<std::string, ::canvas::Surface *> images;
	static ::canvas::Surface *surface = NULL;

	static ::canvas::Canvas *canvas() {
		return surface->canvas();
	}

}

//	Aux methods
static ::canvas::Surface* loadImage(const std::string& path) {
	std::string tmp;
	fs::path imgPath(path);
	if (imgPath.has_parent_path()) {
		tmp = path;
	}
	else {
		fs::path tmpImg = luaz::lua::imagesPath();
		tmpImg /= path;
		tmp = tmpImg.string();
	}

	::canvas::Surface *img;
	std::map<std::string, ::canvas::Surface *>::const_iterator it=impl::images.find(tmp);
	if (it == impl::images.end()) {
		img=impl::canvas()->createSurfaceFromPath(tmp);
		if (img) {
			impl::images[tmp] = img;
		} else {
			LWARN( "modules::Canvas", "Failed to load image file: path=%s", tmp.c_str() );
		}
	}
	else {
		img = (*it).second;
	}
	return img;
}

static void setColor(const ::canvas::Color& color){
	impl::surface->setColor(color);
}

// static void readColor(lua_State* L, int ix, ::canvas::Color& color) {
// 	int rgb[3];
// 	lua_pushnil(L);

// 	for (int i=0; i < 3; ++i) {
// 		lua_next(L, ix);
// 		rgb[i] = luaL_checkint(L, -1);
// 		lua_pop(L, 1);
// 	}

// 	color.r = (util::BYTE) rgb[0];
// 	color.g = (util::BYTE) rgb[1];
// 	color.b = (util::BYTE) rgb[2];
// }

#define APPENDSPACE(line, words, word) \
	if ( word != words.back() ) { \
		line.append(" "); \
	}

	void splitLines( int w, const std::string &text, int borderWidth, int alignment, int margin, unsigned int maxLines, int fontSize, std::vector<std::string>& v_lines)
	{
		std::vector<std::string> explicitLines;
		// Split the explicit lines
		boost::split(explicitLines, text, boost::is_any_of("\n") );

		int maxSize = w - (2*borderWidth);
		if ((alignment & wgt::alignment::hLeft) || (alignment & wgt::alignment::hCenter)) {
			maxSize -= margin;
		}

		for (std::vector<std::string>::iterator it=explicitLines.begin(); it != explicitLines.end() ; ++it) {
			std::vector<std::string> words;
			std::string line;
			int textWidth, h;

			canvas::setFont(wgt::View::instance()->fontFace().c_str(), fontSize);

			boost::split( words, *it, boost::is_any_of(" ") );
			for (std::vector<std::string>::iterator it=words.begin(); it != words.end() && v_lines.size() < maxLines; ++it) {
				std::string word(*it);

				std::string tmp(line);
				tmp.append(word);
				APPENDSPACE(tmp, words, *it);

				canvas::textSize(tmp, textWidth, h);
				if (maxSize <= textWidth) {
					if ( v_lines.size() + 1 ==  maxLines ) {
						canvas::textSize("...", textWidth, h);
						if (textWidth <= maxSize) {
							if (line.size()) {
								std::string tmp(line);
								tmp.append("...");
								canvas::textSize(tmp, textWidth, h);
								if ( textWidth <= maxSize ) {
									line.append("...");
								} else {
									line.replace(line.size() - 4,  3, "...");
								}
							} else  {
								line.assign("...");
							}
						}
					}

					v_lines.push_back( line );

					line.assign(word);
					APPENDSPACE(line, words, *it);
				} else {
					line.assign(tmp);
				}
			}

			if (v_lines.size() < maxLines) {
				v_lines.push_back( line );
			}
			line.clear();
		}
	}

static void drawListCells(
	int x, int y,
	int rowH,
	int cell_padding,
	const ::canvas::Color& rowColor,
	const std::vector<int>& widths )
{
	for (size_t index=0; index<widths.size(); ++index) {
		setColor(rowColor);
		fillRect(x, y, widths[index], rowH);

		x += widths[index] + cell_padding;
	}
}

static void drawListItems(
	int x, int y,
	int rowH,
	int text_padding,
	int cell_padding,
	int fontSize,
	const ::canvas::Color& textColor,
	const std::vector<std::string>& values,
	const std::vector<int>& widths,
	ColumnMapping& images,
	const std::vector<wgt::Alignment> &alignments)
{
	int r_x = x;
	for (size_t index=0; index<values.size(); ++index) {
		setColor(textColor);

		std::string value = values[index];
		if (images.find(index) != images.end()) {
			if (images[index].find(value) != images[index].end()) {
				value = images[index][value];
			} else {
				value = "";
			}
		}

		if (value.find("img:", 0,4) == 0) { // if it starts with "img:"
			std::string path = value.substr(4,std::string::npos);
			::canvas::Surface *img = loadImage(path);
			::canvas::Size s = img->getSize();
			int alignedx = wgt::alignx(alignments[index], r_x, s.w, widths[index]);
			int alignedy = wgt::alignx(alignments[index], y, s.h, rowH);
			drawImage(path.c_str(), alignedx, alignedy);
		} else {
			int h = 0;
			int w = 0;
			textSize( value, w, h);
			int alignedx = wgt::alignx(alignments[index], r_x+text_padding, w, widths[index]-(text_padding*2));
			int alignedy = wgt::alignx(alignments[index], y+boost::math::iround((float)rowH*0.75f), h, rowH);

			std::vector<std::string> v_lines;
			splitLines( widths[index], value, 0, alignments[index], text_padding, 1, fontSize, v_lines );
			text( alignedx, alignedy, v_lines[0].c_str() );
		}
		r_x += widths[index] + cell_padding;
	}
}

static void drawListItems(
	int x, int y,
	int rowH,
	int text_padding,
	int cell_padding,
	int fontSize,
	const ::canvas::Color& textColor,
	const std::vector<std::string>& values,
	const std::vector<int>& widths,
	const std::vector<wgt::Alignment> &aligns)
{
	ColumnMapping images;
	drawListItems(x, y, rowH, text_padding, cell_padding, fontSize, textColor, values, widths, images, aligns);
}

static void loadImages() {
	fs::directory_iterator end_it;
	for (fs::directory_iterator it( luaz::lua::imagesPath() ); it != end_it; ++it) {
		if( !fs::is_directory(it->status()) && fs::extension(it->path()) == ".png" ) {
			loadImage( it->path().string() );
		}
	}
}

static void cacheFonts( const std::string &fontName ) {
	char charset[]  = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ÁÀÉÈÍÌÓÒÚÙÜÑáàéèíìóòúùüñ";
	char charset2[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~AAEEIIOOUUUNaaeeiioouuun";
	int  sizes[] = {14, 17, 22};

	int sizes_length = sizeof sizes/sizeof(int);
	int w,h;

	//	These widths belong to Tiresias font
	for (int s = 0; s < sizes_length; ++s) {
		int size = sizes[s];
		setFont(fontName.c_str(), size);
		for (int i = 0; charset[i] != '\0'; ++i) {
			std::string str = std::string("") + charset2[i];
			textSize(str, w, h);
			impl::fontSizes[fontName][size][charset[i]] = w;
		}
	}
}

//	API
void clear( int x, int y, int w, int h ) {
	impl::surface->clear(::canvas::Rect(x,y, w,h));
}

void fillRect( int x, int y, int w, int h ){
	impl::surface->fillRect(::canvas::Rect(x,y,w,h));
}

void drawRect(int x, int y, int w, int h){
	impl::surface->drawRect(::canvas::Rect(x,y, w,h));
}

void setColor(int r, int g, int b, int a /*=255*/) {
	setColor(::canvas::Color((util::BYTE)r,(util::BYTE)g,(util::BYTE)b,(util::BYTE)a) );
}

void setFont(const char* face, int size) {
	impl::surface->setFont(::canvas::FontInfo(face, size));
}

void text( int x, int y, const char *t ) {
	impl::surface->drawText(::canvas::Point(x,y), t);
}

int fontAscent() {
	return impl::surface->fontAscent();
}

int fontDescent() {
	return impl::surface->fontDescent();
}

void textSize(const std::string& text, int &w, int &base) {
	::canvas::Size sz;
	impl::surface->measureText(text, sz);
	w = sz.w;
	base = sz.h;
}

int textWidth( const std::string &fontName, const std::string& text, int size) {
	int width = 0;
	for (size_t i = 0; i < text.size(); ++i) {
		width += impl::fontSizes[fontName][size][text[i]];
	}
	return width;
}

int textWidth( const std::string &fontName, char c, int size) {
	return impl::fontSizes[fontName][size][c];
}

void fillPolygon(const std::vector<std::pair<int,int> >& vertices) {
	std::vector< ::canvas::Point > points;
	for(size_t i=0; i<vertices.size(); ++i) {
		points.push_back(::canvas::Point(vertices[i].first, vertices[i].second));
	}
	impl::surface->fillPolygon(points);
}

void drawImage( const std::string &path, int x, int y, int /*w*/, int /*h*/) {
	if (!path.empty()) {
		::canvas::Surface* img = loadImage(path);
		if (img) {
			::canvas::Point target(x,y);
			impl::surface->blit( target, img );
		}
	}
}

void imageSize( const std::string &path, int &w, int &h ) {
	::canvas::Surface* img = loadImage(path);
	::canvas::Size size(0,0);
	if (img) {
		size = img->getSize();
	}
	w = size.w;
	h = size.h;
}

void drawImageFullScreen(const char* path) {
	impl::surface->drawImage( std::string(path) );
}

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
)
{
	const int text_padding = 5;
	std::vector<wgt::Alignment> aligns;
	if (alignments.size()>0) {
		aligns.assign(alignments.begin(), alignments.end());
	} else {
		aligns.insert(aligns.begin(), headers.size(), wgt::alignment::None);
	}

	int listW = 0;
	BOOST_FOREACH(int width, cell_widths) {
		listW += width + rowSep;
	}

	// background
	const int listH = (rowCount+1)*(rowHeight+rowSep) + 2; // header + rows
	setColor(bgColor);
	fillRect(x,y, listW,listH);

	// header
	drawListCells(x, y, rowHeight, rowSep, headerColor, cell_widths);
	drawListItems(x, y, rowHeight, text_padding, rowSep, fontSize, textHeaderColor, headers, cell_widths, aligns);
	y += rowHeight + rowSep + 2;

	int r_y = y;
	int r_x = x;
	// fondo de las filas
	//for (int ix=from; ix<to; ++ix) {
	for (int ix=0; ix<rowCount; ++ix) {
		drawListCells(r_x, r_y, rowHeight, rowSep, (selected==ix+from)? selectedCellColor : cellColor, cell_widths);
		r_y += rowHeight + rowSep;
	}

	if (values.size()) {
		r_y = y;
		r_x = x;

		// texto de las filas
		for (int ix=from; ix<=to; ++ix) {
			const std::vector<std::string>& row = values[ix];
			drawListItems(r_x, r_y, rowHeight, text_padding, rowSep, fontSize, (selected==ix)? selectedTextColor : textColor, row, cell_widths, (selected==ix)? images_selected : images_normal, aligns);
			r_y += rowHeight + rowSep;
		}
	}
}

std::pair<int,int> screenSize() {
	const ::canvas::Size &size=impl::canvas()->size();
	return std::make_pair(size.w,size.h);
}

void getSize( int &w, int &h ) {
	const ::canvas::Size &size = impl::canvas()->size();
	w = size.w;
	h = size.h;
}

void flush() {
	impl::canvas()->flush();
}

//	LUA functions
static int l_size (lua_State* L) {
	const ::canvas::Size &size=impl::canvas()->size();
	lua_pushnumber(L, size.w );
	lua_pushnumber(L, size.h );
	return 2; // -> width, height
}

static int l_clear (lua_State* L) {
	int x = luaL_checkint(L,2);
	int y = luaL_checkint(L,3);
	int w = luaL_checkint(L,4);
	int h = luaL_checkint(L,5);
	clear(x,y,w,h);
	return 0;
}

static int l_flush(lua_State* /*L*/) {
	impl::canvas()->flush();
	return 0;
}

static int l_setFont( lua_State *L ) {
	int narg=2;
	const char *face = luaL_checkstring(L,narg++);
	int size = luaL_checkint(L,narg++);
	impl::surface->setFont(::canvas::FontInfo(face, size));
	return 0;
}

static int l_setColor( lua_State *L ) {
	int narg=2;
	int r = luaL_checkint(L,narg++);
	int g = luaL_checkint(L,narg++);
	int b = luaL_checkint(L,narg++);
	int a = luaL_checkint(L,narg++);
	setColor(r,g,b,a);
	return 0;
}

static int l_drawText( lua_State *L ) {
	int x, y, w, h, narg=2;
	bool boundedText = (lua_gettop(L) == 6);
	const char *txt = luaL_checkstring(L,narg++);

	x = luaL_checkint(L,narg++);
	y = luaL_checkint(L,narg++);

	if (boundedText) {
		w = luaL_checkint(L,narg++);
		h = luaL_checkint(L,narg++);
		impl::surface->drawText(::canvas::Rect(x,y,w,h), txt, ::canvas::wrapByWord);
	} else {
		text(x, y, (char *) txt);
	}

	return 0;
}

static int l_measureText( lua_State *L ) {
	int narg=2;
	const char *text = luaL_checkstring(L,narg++);
	if (text) {
		::canvas::Size sz;
		impl::surface->measureText(text, sz);
		lua_pushnumber(L, sz.w );
		lua_pushnumber(L, sz.h );
		return 2;
	}
	return 0;
}

static int l_drawImage( lua_State *L ) {
	int narg=2;
	const char *img = luaL_checkstring(L,narg++);
	int x = luaL_checkint(L,narg++);
	int y = luaL_checkint(L,narg++);
	drawImage( img, x, y, -1, -1 );
	return 0;
}

static int l_imageSize( lua_State *L ) {
	const char *path = luaL_checkstring(L,2);
	::canvas::Surface* img = loadImage(path);
	::canvas::Size tmp = img->getSize();
	lua_pushnumber(L, tmp.w );
	lua_pushnumber(L, tmp.h );
	return 2;
}

static int l_fillRect( lua_State *L ) {
	int narg=2;
	int x = luaL_checkint(L,narg++);
	int y = luaL_checkint(L,narg++);
	int w = luaL_checkint(L,narg++);
	int h = luaL_checkint(L,narg++);
	fillRect( x, y, w, h );
	return 0;
}

static int l_drawRect( lua_State *L ) {
	int narg=2;
	int x = luaL_checkint(L,narg++);
	int y = luaL_checkint(L,narg++);
	int w = luaL_checkint(L,narg++);
	int h = luaL_checkint(L,narg++);
	drawRect( x, y, w, h );
	return 0;
}

static int l_cacheFonts( lua_State *L ) {
	const char *fontName = luaL_checkstring(L,2);
	DTV_ASSERT(fontName);
	cacheFonts( fontName );
	return 0;
}

static const struct luaL_Reg canvas_methods[] = {
	{ "clear",            l_clear            },
	{ "size",             l_size             },
	{ "flush",            l_flush            },
	{ "drawImage",        l_drawImage        },
	{ "imageSize",        l_imageSize        },
	{ "drawText",         l_drawText         },
	{ "setColor",         l_setColor         },
	{ "setFont",          l_setFont          },
	{ "fillRect",         l_fillRect         },
	{ "drawRect",         l_drawRect         },
	{ "measureText",      l_measureText      },
	{ "cacheFonts",       l_cacheFonts       },
	{ NULL,               NULL               }
};

//	Module methods
bool init( zapper::Zapper *zapper ) {
	impl::surface = zapper->getLayer( zapper::layer::zapper )->getSurface();
	if (!impl::surface) {
		return false;
	}
	loadImages();
	return true;
}

void fin( void ) {
	typedef std::map<std::string, ::canvas::Surface *>::value_type vt;
	BOOST_FOREACH(vt pair, impl::images) {
		impl::canvas()->destroy(pair.second);
	}
	impl::surface = NULL;
}

void start( lua_State *lua ) {
	luaL_register( lua, "canvas", canvas_methods );
}



}

