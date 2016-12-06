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

#include "surface.h"
#include "../../lua.h"
#include <canvas/fontinfo.h>
#include <canvas/canvas.h>
#include <canvas/surface.h>
#include <canvas/types.h>

#define SURFACE_TYPE "surface"

namespace luaz{
namespace canvas {

::canvas::Surface *checkSurface( lua_State *L, int pos ) {
	::canvas::Surface *surface = *(::canvas::Surface **) luaL_checkudata(L, pos, SURFACE_TYPE);
	return surface;
}


static ::canvas::FontInfo checkFont( lua_State *L, int pos=1 ) {
	::canvas::FontInfo font;
	int type = lua_type(L, pos);
	if ( type == LUA_TSTRING ) {
		std::string families = luaL_checkstring(L, pos++);
		size_t size = luaL_checkint(L, pos++);
		font = ::canvas::FontInfo(families.c_str(), size);
	} else {
		std::vector<std::string> families;
		luaz::lua::readList( L, pos++, families );
		size_t size = luaL_checkint(L, pos++);
		font = ::canvas::FontInfo(families, size);
	}

	if (lua_gettop(L) == ++pos) {
		font.bold(luaz::lua::checkBool(L, pos));
		if (lua_gettop(L) == ++pos) {
			font.italic(luaz::lua::checkBool(L, pos));
			if (lua_gettop(L) == ++pos) {
				font.smallCaps(luaz::lua::checkBool(L, pos));
			}
		}
	}

	return font;
}

static int pushRect( lua_State *L, const ::canvas::Rect &rect ) {
	lua_pushnumber( L, rect.x );
	lua_pushnumber( L, rect.y );
	lua_pushnumber( L, rect.w );
	lua_pushnumber( L, rect.h );
	return 4;
}

static int pushSize( lua_State *L, const ::canvas::Size &size ) {
	lua_pushnumber(L, size.w );
	lua_pushnumber(L, size.h );
	return 2;
}

static int pushPoint( lua_State *L, const ::canvas::Point &point ) {
	lua_pushnumber(L, point.x );
	lua_pushnumber(L, point.y );
	return 2;
}

static int pushFont( lua_State *L, const ::canvas::FontInfo &font ) {
	lua_pushstring(L, font.familiesAsString().c_str() );
	lua_pushnumber(L, font.size() );
	lua_pushboolean(L, font.bold() );
	lua_pushboolean(L, font.italic() );
	lua_pushboolean(L, font.smallCaps() );
	return 5;
}

static int pushColor( lua_State *L, const ::canvas::Color &color ) {
	lua_pushnumber(L, color.r);
	lua_pushnumber(L, color.g);
	lua_pushnumber(L, color.b);
	lua_pushnumber(L, color.alpha);
	return 4;
}

int pushSurface( lua_State *L, ::canvas::Surface *surface ) {
	::canvas::Surface **newSurface = (::canvas::Surface **) lua_newuserdata(L, sizeof(::canvas::Surface *) );

	{	//	Set metatable to the userdata
		luaL_getmetatable(L, SURFACE_TYPE );
		lua_setmetatable(L, -2);
	}

	*newSurface = surface;

	return 1;
}

static void checkRect( lua_State *L, int pos, ::canvas::Rect &rect ) {
	rect.x = luaL_checkint(L, pos++);
	rect.y = luaL_checkint(L, pos++);
	rect.w = luaL_checkint(L, pos++);
	rect.h = luaL_checkint(L, pos++);
}

static void checkColor( lua_State *L, int pos, ::canvas::Color &color ) {
	color.r = (util::BYTE) luaL_checknumber(L, pos++),
	color.g = (util::BYTE) luaL_checknumber(L, pos++),
	color.b = (util::BYTE) luaL_checknumber(L, pos++),
	color.alpha = (util::BYTE) luaL_checknumber(L, pos++);
}

static void checkPoint( lua_State *L, int pos, ::canvas::Point &point ) {
	point.x = luaL_checkint(L, pos++);
	point.y = luaL_checkint(L, pos++);
}

static int l_getFont( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	::canvas::FontInfo font = surface->getFont();
	pushFont( L, font );
	return 1;
}

static int l_setDefaultFont( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	surface->setDefaultFont();

	return 0;
}

static int l_setFont( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	::canvas::FontInfo font = checkFont( L, 2 );
	bool ret = surface->setFont( font );
	lua_pushboolean( L, ret );

	return 1;
}

static int l_getClip( lua_State *L ) {
	::canvas::Surface *surface = checkSurface(L);
	::canvas::Rect rect;
	checkRect( L, 2, rect );
	bool ret = surface->getClip( rect );
	lua_pushboolean( L, ret );

	return 1;
}

static int l_setClip( lua_State *L ) {
	::canvas::Surface *surface = checkSurface(L);
	::canvas::Rect rect;
	checkRect( L, 2, rect );
	bool ret = surface->setClip( rect );
	lua_pushboolean( L, ret );

	return 1;
}

static int l_drawLine( lua_State *L ) {
	::canvas::Surface *surface = checkSurface(L);

	int x1 = luaL_checkint(L, 2);
	int y1 = luaL_checkint(L, 3);
	int x2 = luaL_checkint(L, 4);
	int y2 = luaL_checkint(L, 5);

	bool ret = surface->drawLine( x1, y1, x2, y2 );

	lua_pushboolean( L, ret );

	return 1;
}

static int l_drawRect( lua_State *L ) {
	::canvas::Surface *surface=checkSurface(L);

	::canvas::Rect rect;
	checkRect( L, 2, rect );
	bool ret = surface->drawRect( rect );
	lua_pushboolean( L, ret );

	return 1;
}

static int l_fillRect( lua_State *L ) {
	::canvas::Surface *surface=checkSurface(L);

	::canvas::Rect rect;
	checkRect( L, 2, rect );
	bool ret = surface->fillRect( rect );
	lua_pushboolean( L, ret );

	return 1;
}

static int l_drawRoundRect( lua_State *L ) {
	::canvas::Surface *surface=checkSurface(L);

	::canvas::Rect rect;
	checkRect( L, 2, rect );

	int arcW = luaL_checkint(L, 6);
	int arcH = luaL_checkint(L, 7);

	bool ret = surface->drawRoundRect( rect, arcW, arcH );
	lua_pushboolean( L, ret );

	return 1;
}

static int l_fillRoundRect( lua_State *L ) {
	::canvas::Surface *surface=checkSurface(L);

	::canvas::Rect rect;
	checkRect( L, 2, rect );

	int arcW = luaL_checkint(L, 6);
	int arcH = luaL_checkint(L, 7);

	bool ret = surface->fillRoundRect( rect, arcW, arcH );
	lua_pushboolean( L, ret );

	return 1;
}

static int l_drawPolygon( lua_State *L ) {
	::canvas::Surface *surface = checkSurface(L);
	std::vector< ::canvas::Point > points;
	bool ret = false;

	lua_pushnil(L);
	while (lua_next(L, 2) != 0) {
		int x = luaL_checkint(L, -1);
		int y = luaL_checkint(L, -2);
		points.push_back( ::canvas::Point(x, y) );
		lua_pop(L, 2);
	}

	if (lua_gettop(L) == 2) {
		ret = surface->drawPolygon( points );
	} else if (lua_gettop(L) == 3) {
		bool closed = luaz::lua::checkBool(L, 3);
		ret = surface->drawPolygon( points, closed );
	}
	lua_pushboolean( L, ret );

	return 1;
}

static int l_fillPolygon( lua_State *L ) {
	::canvas::Surface *surface = checkSurface(L);
	std::vector< ::canvas::Point > points;

	lua_pushnil(L);
	while (lua_next(L, 2) != 0) {
		int x = luaL_checkint(L, -1);
		int y = luaL_checkint(L, -2);
		points.push_back( ::canvas::Point(x, y) );
		lua_pop(L, 2);
	}

	bool ret = surface->fillPolygon( points );
	lua_pushboolean( L, ret );

	return 1;
}

static int l_drawEllipse( lua_State *L ) {
	::canvas::Surface *surface = checkSurface(L);

	::canvas::Point point;
	checkPoint( L, 2, point );

	int w = luaL_checkint( L, 4 );
	int h = luaL_checkint( L, 5 );
	int arcW = luaL_checkint( L, 6 );
	int arcH = luaL_checkint( L, 7 );

	bool ret = surface->drawEllipse( point, w, h, arcW, arcH );
	lua_pushboolean( L, ret );

	return 1;
}

static int l_fillEllipse( lua_State *L ) {
	::canvas::Surface *surface = checkSurface(L);

	::canvas::Point point;
	checkPoint( L, 2, point );

	int w = luaL_checkint( L, 4 );
	int h = luaL_checkint( L, 5 );
	int arcW = luaL_checkint( L, 6 );
	int arcH = luaL_checkint( L, 7 );

	bool ret = surface->fillEllipse( point, w, h, arcW, arcH );
	lua_pushboolean( L, ret );

	return 1;
}

static int l_drawText( lua_State *L ) {
	bool ret = false;
	::canvas::Surface *surface = checkSurface(L);
	int x = luaL_checkint(L, 2);
	int y = luaL_checkint(L, 3);

	if( lua_gettop(L) == 4 ) {

		const char *txt = luaL_checkstring(L, 4);
		::canvas::Point p( x, y);
		ret = surface->drawText( p, txt );
	} else if ( lua_gettop(L) == 6 ) {
		int w = luaL_checkint(L, 4);
		int h = luaL_checkint(L, 5);
		::canvas::Rect rect( x, y, w, h);
		const char *txt = luaL_checkstring(L, 6);

		ret = surface->drawText( rect, txt );
	} else if ( lua_gettop(L) == 8 ) {
		int w = luaL_checkint(L, 4);
		int h = luaL_checkint(L, 5);
		const char *txt = luaL_checkstring(L, 6);
		const char *wrap = luaL_checkstring(L, 7);
		int spacing = luaL_checkint(L, 8);
		::canvas::Rect rect( x, y, w, h);

		bool byWord = strcmp(wrap, "byWord") == 0;

		ret = surface->drawText( rect, txt, byWord ? ::canvas::wrapByWord : ::canvas::wrapAnywhere, spacing );
	}
	lua_pushboolean( L, ret );

	return 1;
}

static int l_measureText( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );

	const char *text = luaL_checkstring( L, 2 );
	if ( !text ) {
		return luaL_error( L, "Bad argument. The text argument must be a string." );
	}

	::canvas::Size size;
	surface->measureText( text, size );
	return pushSize( L, size );
}

static int l_fontAscent( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	int fontAscent = surface->fontAscent();

	lua_pushnumber( L, fontAscent );

	return 1;
}

static int l_fontDescent( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	int fontAscent = surface->fontDescent();

	lua_pushnumber( L, fontAscent );

	return 1;
}

static int l_drawImage( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );

	const char *fileName = luaL_checkstring( L, 2 );
	if ( !fileName ) {
		return luaL_error( L, "Bad argument. The file name argument must be a string." );
	}
	surface->drawImage( fileName );
	return 0;
}

static int l_blit( lua_State *L ) {
	bool ret = false;
	::canvas::Surface *surface = checkSurface(L);
	int x = luaL_checkint(L, 2);
	int y = luaL_checkint(L, 3);
	::canvas::Surface *srcSurface = checkSurface(L, 4);

	::canvas::Point p( x, y );

	if( lua_gettop(L) == 4 ) {

		ret = surface->blit( p, srcSurface );

	} else if ( lua_gettop(L) == 8 ) {
		int xRect = luaL_checkint(L, 5);
		int yRect = luaL_checkint(L, 6);
		int w = luaL_checkint(L, 7);
		int h = luaL_checkint(L, 8);
		::canvas::Rect rect( xRect, yRect, w, h);

		ret = surface->blit( p, srcSurface, rect );
	}
	lua_pushboolean( L, ret );

	return 1;
}

static int l_scale( lua_State *L ) {
	bool ret = false;
	::canvas::Surface *surface = checkSurface(L);
	::canvas::Rect rect;
	checkRect( L, 2, rect );
	::canvas::Surface *srcSurface = checkSurface( L, 6 );


	if( lua_gettop(L) == 8 ) {
		bool flipw = luaz::lua::checkBool(L, 7);
		bool fliph = luaz::lua::checkBool(L, 8);
		ret = surface->scale( rect, srcSurface, flipw, fliph );

	} else if ( lua_gettop(L) == 12 ) {
		::canvas::Rect srcRect;
		checkRect( L, 7, srcRect );
		bool flipw = luaz::lua::checkBool(L, 11);
		bool fliph = luaz::lua::checkBool(L, 12);
		ret = surface->scale( rect, srcSurface, srcRect, flipw, fliph );
	}

	lua_pushboolean( L, ret );

	return 1;
}

static int l_rotate( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	int degrees = luaL_checkint( L, 2 );
	::canvas::Surface *retSurface = surface->rotate( degrees );
	lua_pushlightuserdata( L, retSurface );

	return 1;
}

static int l_resize( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	int w = luaL_checkint( L, 2 );
	int h = luaL_checkint( L, 3 );
	bool scale = true;
	if(lua_gettop(L) == 4) {
		scale = luaz::lua::checkBool(L, 4 );
	}
	::canvas::Size size( w, h );
	bool ret = surface->resize( size, scale );
	lua_pushboolean( L, ret );

	return 1;
}

static int l_setCompositionMode( lua_State * L ) {
	::canvas::Surface *surface = checkSurface( L );
	int mode = luaL_checkint( L, 2 );
	surface->setCompositionMode( (::canvas::composition::mode)mode );
	return 0;
}

static int l_getCompositionMode( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	::canvas::composition::mode mode = surface->getCompositionMode();
	lua_pushinteger( L, mode );

	return 1;
}

static int l_getPixelColor( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	::canvas::Point point;
	::canvas::Color color;
	checkPoint( L, 2, point );
	checkColor( L, 4, color );
	surface->getPixelColor( point, color );

	return 0;
}

static int l_setPixelColor( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	::canvas::Point point;
	::canvas::Color color;
	checkPoint( L, 2, point );
	checkColor( L, 4, color );
	surface->setPixelColor( point, color );

	return 0;
}

static int l_pixels( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	util::DWORD *pixels = surface->pixels();
	lua_pushlightuserdata( L, pixels );

	return 1;
}

static int l_saveAsImage( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	std::string filename = luaL_checkstring( L, 2 );
	bool ret = surface->saveAsImage( filename );
	lua_pushboolean( L, ret );

	return 1;
}

static int l_equalsImage( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	std::string filename = luaL_checkstring( L, 2 );
	bool ret = surface->equalsImage( filename );
	lua_pushboolean( L, ret );

	return 1;
}

static int l_flush( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	surface->flush();

	return 0;
}

static int l_autoFlush( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );

	if( lua_gettop(L) == 1 ) {
		lua_pushboolean( L, surface->autoFlush() ? 1 : 0 );
	} else if( lua_gettop(L) == 2 ) {
		surface->autoFlush( luaz::lua::checkBool( L, 2 ) );
	}

	return 0;
}


static int l_flushCompositionMode( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );

	if( lua_gettop(L) == 1 ) {
		int composition = surface->flushCompositionMode();
		lua_pushinteger( L, composition );
	} else if( lua_gettop(L) == 2 ) {
		int composition = luaL_checkint( L, 2 );
		surface->flushCompositionMode( (::canvas::composition::mode)composition );
	}
	return 0;
}

static int l_markDirty( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );

	if( lua_gettop(L) == 1 ) {
		surface->markDirty();
	} else if( lua_gettop(L) == 5 ) {
		::canvas::Rect rect;
		checkRect( L, 2, rect );
		surface->markDirty( rect );
	}
	return 0;
}

static int l_isDirty( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	bool isDirty = surface->isDirty();
	lua_pushboolean( L, isDirty );

	return 1;
}

static int l_clearDirty( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	surface->clearDirty();

	return 0;
}

static int l_getDirtyRegion( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	::canvas::Rect dirtyRegion, blitRect;
	checkRect( L, 2, dirtyRegion );
	surface->getDirtyRegion( dirtyRegion, blitRect );
	return pushRect( L, blitRect );
}

static int l_invalidateRegion( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	::canvas::Rect rect;
	checkRect( L, 2, rect );
	surface->invalidateRegion( rect );

	return 0;
}

static int l_setZIndex( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	int zIndex = luaL_checkint( L, 2 );
	surface->setZIndex( zIndex );

	return 0;
}

static int l_getZIndex( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	int zIndex = surface->getZIndex();
	lua_pushinteger( L, zIndex);

	return 1;
}

static int l_setOpacity( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	util::BYTE alpha = (util::BYTE) luaL_checkint( L, 2 );
	bool ret = surface->setOpacity( alpha );
	lua_pushboolean( L, ret );

	return 1;
}

static int l_getOpacity( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	util::BYTE alpha = surface->getOpacity();
	lua_pushinteger( L, alpha );

	return 1;
}

static int l_setColor( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	::canvas::Color col;
	checkColor( L, 2, col );
	surface->setColor( col );

	return 0;
}

static int l_getColor( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	::canvas::Color col = surface->getColor();
	return pushColor( L, col );
}

static int l_clear( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );

	if( lua_gettop(L) == 1 ) {
		surface->clear();
	} else if( lua_gettop(L) == 5 ) {
		::canvas::Rect rect;
		checkRect( L, 2, rect );
		surface->clear( rect );
	}

	return 0;
}

static int l_setVisible( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	surface->setVisible( luaz::lua::checkBool(L, 2) );
	return 0;
}

static int l_isVisible( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	bool visible = surface->isVisible();
	lua_pushboolean( L, visible);

	return 1;
}

static int l_getBounds( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	::canvas::Rect rect = surface->getBounds();
	return pushRect( L, rect );
}

static int l_setLocation( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	int x = luaL_checkint( L, 2 );
	int y = luaL_checkint( L, 3 );
	::canvas::Point p( x, y );
	surface->setLocation( p );

	return 0;
}

static int l_getLocation( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	::canvas::Point p = surface->getLocation();
	return pushPoint( L, p );
}

static int l_getSize( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	::canvas::Size s = surface->getSize();
	return pushSize( L, s );
}

static int l_pointInBounds( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	int x = luaL_checkint( L, 2 );
	int y = luaL_checkint( L, 3 );
	::canvas::Point p( x, y );
	bool ret = surface->pointInBounds( p );
	lua_pushboolean( L, ret );

	return 1;
}

static int l_boundsChanged( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	bool ret = surface->boundsChanged();
	lua_pushboolean( L, ret );

	return 1;
}

static int l_cleanBoundsChanged( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	surface->cleanBoundsChanged();

	return 0;
}

static const struct luaL_Reg surface_methods[] = {
	{ "getFont",                l_getFont               },
	{ "setDefaultFont",         l_setDefaultFont        },
	{ "setFont",                l_setFont               },
	{ "getClip",                l_getClip               },
	{ "setClip",                l_setClip               },
	{ "drawLine",               l_drawLine              },
	{ "drawRect",               l_drawRect              },
	{ "fillRect",               l_fillRect              },
	{ "drawRoundRect",          l_drawRoundRect         },
	{ "fillRoundRect",          l_fillRoundRect         },
	{ "drawPolygon",            l_drawPolygon           },
	{ "fillPolygon",            l_fillPolygon           },
	{ "drawEllipse",            l_drawEllipse           },
	{ "fillEllipse",            l_fillEllipse           },
	{ "drawText",               l_drawText              },
	{ "measureText",            l_measureText           },
	{ "fontAscent",             l_fontAscent            },
	{ "fontDescent",            l_fontDescent           },
	{ "drawImage",              l_drawImage             },
	{ "blit",                   l_blit                  },
	{ "scale",                  l_scale                 },
	{ "rotate",                 l_rotate                },
	{ "resize",                 l_resize                },
	{ "setCompositionMode",     l_setCompositionMode    },
	{ "getCompositionMode",     l_getCompositionMode    },
	{ "getPixelColor",          l_getPixelColor         },
	{ "setPixelColor",          l_setPixelColor         },
	{ "pixels",                 l_pixels                },
	{ "saveAsImage",            l_saveAsImage           },
	{ "equalsImage",            l_equalsImage           },
	{ "flush",                  l_flush                 },
	{ "autoFlush",              l_autoFlush             },
	{ "flushCompositionMode",   l_flushCompositionMode  },
	{ "markDirty",              l_markDirty             },
	{ "isDirty",                l_isDirty               },
	{ "clearDirty",             l_clearDirty            },
	{ "getDirtyRegion",         l_getDirtyRegion        },
	{ "invalidateRegion",       l_invalidateRegion      },
	{ "setZIndex",              l_setZIndex             },
	{ "getZIndex",              l_getZIndex             },
	{ "setOpacity",             l_setOpacity            },
	{ "getOpacity",             l_getOpacity            },
	{ "setColor",               l_setColor              },
	{ "getColor",               l_getColor              },
	{ "clear",                  l_clear                 },
	{ "setVisible",             l_setVisible            },
	{ "isVisible",              l_isVisible             },
	{ "getBounds",              l_getBounds             },
	{ "setLocation",            l_setLocation           },
	{ "getLocation",            l_getLocation           },
	{ "getSize",                l_getSize               },
	{ "pointInBounds",          l_pointInBounds         },
	{ "boundsChanged",          l_boundsChanged         },
	{ "cleanBoundsChanged",     l_cleanBoundsChanged    },
	{ NULL,                     NULL                    }
};

int luaopen_surface(lua_State *L ) {
	luaL_newmetatable(L, SURFACE_TYPE);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_openlib(L, NULL, surface_methods, 0);
	return 1;
}

}//	End canvas namespace
}//	End luaz namespace
