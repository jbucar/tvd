/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "canvas.h"
#include "surfacewrapper.h"
#include "../lua.h"
#include "../../luaplayer.h"
#include <util/mcr.h>
#include <util/assert.h>
#include <util/log.h>
#include <canvas/system.h>
#include <canvas/canvas.h>
#include <canvas/surface.h>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/math/special_functions/round.hpp>
#include <string>
#include <string.h>


#define LUA_CANVAS  "lua_canvas_module"
#define CANVAS_TYPE "lua.canvas"

namespace player {
namespace mcanvas {

namespace fs = boost::filesystem;

//	Aux implementation for a l_drawPolygon function
namespace impl {
	struct PolygonDrawer {
		PolygonDrawer( const std::string &mode, SurfaceWrapper *wrapper) : _mode(mode), _surface(wrapper->surface()) {}
		~PolygonDrawer() {}

		void addPoint( const canvas::Point &point) {
			_points.push_back(point);
		}

		int draw( lua_State *L ) const {
			DTV_ASSERT(_surface);
			int res = 0;
			if (!_points.empty()) {
				if (_mode == "close") {
					_surface->drawPolygon(_points);
				} else if (_mode == "fill") {
					_surface->fillPolygon(_points);
				} else {
					_surface->drawPolygon(_points, false);
				}
			} else {
				res = luaL_error(L, "The polygon should have vertices.");
			}
			return res;
		}

		const std::string _mode;
		std::vector<canvas::Point> _points;
		::canvas::Surface *_surface;
		
		private:
			PolygonDrawer operator=( const PolygonDrawer &/*drawer*/ ) { return *this; }
	};
}

//	Aux
static inline canvas::Canvas *getCanvas( SurfaceWrapper *wrapper ) {
	return wrapper->surface()->canvas();
}

static int pushSize( lua_State *L, const canvas::Size &size ) {
	lua_pushnumber(L, size.w );
	lua_pushnumber(L, size.h );
	return 2;
}

static int pushRect( lua_State *L, const canvas::Rect &rect ) {
	lua_pushnumber( L, rect.x );
	lua_pushnumber( L, rect.y );
	lua_pushnumber( L, rect.w );
	lua_pushnumber( L, rect.h );
	return 4;
}

static void checkRect( lua_State *L, int pos, canvas::Rect &rect ) {
	rect.x = luaL_checkint(L, pos++);
	rect.y = luaL_checkint(L, pos++);
	rect.w = luaL_checkint(L, pos++);
	rect.h = luaL_checkint(L, pos++);
}

static int pushColor( lua_State *L, const canvas::Color &color ) {
	lua_pushnumber(L, color.r);
	lua_pushnumber(L, color.g);
	lua_pushnumber(L, color.b);
	lua_pushnumber(L, color.alpha);
	return 4;
}

static void checkColor( lua_State *L, int pos, canvas::Color &color ) {
	color.r = (util::BYTE) luaL_checknumber(L, pos++),
	color.g = (util::BYTE) luaL_checknumber(L, pos++),
	color.b = (util::BYTE) luaL_checknumber(L, pos++),
	color.alpha = (util::BYTE) luaL_checknumber(L, pos++);
}

static void checkPoint( lua_State *L, int pos, canvas::Point &point ) {
	point.x = luaL_checkint(L, pos++);
	point.y = luaL_checkint(L, pos++);
}

static SurfaceWrapper *checkSurface( lua_State *L, int pos=1 ) {
	void *ud = luaL_checkudata( L, pos, CANVAS_TYPE );
	luaL_argcheck(L, ud != NULL, 1, "'canvas' expected" );
	return *((SurfaceWrapperPtr *)ud);
}

static void buildLuaFontStyle( std::string &style, bool bold, bool italics ) {
	if (bold) {
		if (italics) {
			style = "bold-italic";
		} else {
			style = "bold";
		}
	} else {
		if (italics) {
			style = "italic";
		} else {
			style = "normal";
		}
	}
}

//	API
static int l_new( lua_State *L ) {
	//	Parameters: canvas [string | (width,height)] -> canvas
	SurfaceWrapper *wrapper=checkSurface(L);
	canvas::Surface *surface;
	
	//	Get canvas module from stack
	Module *module = Module::get( L );
	if (!module) {
		return luaL_error( L, "Invalid canvas module\n" );
	}

	int type = lua_type(L, 2);
	switch (type) {
		case LUA_TSTRING: {
			const char *img = luaL_checkstring(L, 2);
			if (!img) {
				return luaL_error(L, "The path of an image must be a string\n");
			}

			std::string path = module->getImagePath( img );
			if (!fs::exists( path )) {
				return luaL_error(L, "The image does not exists\n");
			}

			surface=getCanvas(wrapper)->createSurfaceFromPath( path );

			break;
		}
		case LUA_TNUMBER: {
			canvas::Rect bounds( 0, 0, luaL_checkint(L, 2), luaL_checkint(L, 3) );
			if (bounds.w <= 0 || bounds.h <= 0) {
				return luaL_error(L, "Invalid size of canvas\n");
			}
			surface=getCanvas(wrapper)->createSurface( bounds );
			break;
		}
		default: {
			surface=NULL;
			break;
		}
	};

	if (surface!=NULL) {
		return module->createSurface(surface);
	} else {
		return luaL_error(L, "The parameter to create a new canvas must be the width and height of this or the path of an image\n");
	}
}

static int l_attrSize( lua_State *L ) {
	//	Parameters: canvas -> width height
	SurfaceWrapper *wrapper=checkSurface(L);
	return pushSize( L, wrapper->surface()->getSize() );
}

static int l_attrColor( lua_State *L ) {
	//	Method:
	//	a) SET: canvas R G B A
	//	b) SET: canvas cl_name
	//	c) GET: canvas -> R G B A
	SurfaceWrapper *wrapper=checkSurface(L);

	if (lua_gettop(L) == 1) {
		//	(c)
		return pushColor( L, wrapper->surface()->getColor() );
	}
	else {
		//	Set
		if (lua_type(L, 2) == LUA_TNUMBER) {
			//	(a)
			::canvas::Color color;
			checkColor( L, 2, color );
			wrapper->surface()->setColor( color );
		}
		else {
			//	(b)
			const char *textColor=luaL_checkstring(L, 2);
			if (!textColor) {
				return luaL_error(L, "The color must be a string or a RGBA color\n");
			}

			//	Transform string color to RGB
			::canvas::Color color;
			if (!::canvas::color::get( textColor, color )) {
				return luaL_error(L, "The string must be a NCL defined color\n");
			}

			//	Try set color
			wrapper->surface()->setColor( color );
		}

		return 0;
	}
}

static int l_attrFont( lua_State *L ) {
	//	a) canvas:attrFont() -> face: string; size: number; style: string
	//	b) canvas:attrFont( face: string; size: number; style: string )
	SurfaceWrapper *wrapper=checkSurface(L);

	if (lua_gettop(L) == 1) {
		//	(a)
		std::string luastyle;
		::canvas::FontInfo font = wrapper->surface()->getFont();
		
		buildLuaFontStyle( luastyle, font.bold(), font.italic() );
		
		std::string face = font.familiesAsString();
		lua_pushstring( L, face.c_str() );
		lua_pushnumber( L, font.size() );
		lua_pushstring( L, luastyle.c_str() );
		return 3;
	}
	else {
		//	(b)
		const char *face=luaL_checkstring(L,2);
		if (!face) {
			return luaL_error(L, "The font must be a string\n");
		}
		
		int size = luaL_checkint(L, 3);
		
		const char *style=luaL_optstring(L,4,"normal");
		if (!style) {
			return luaL_error(L,"The style is not supported\n" );
		}

		//	Check style
		if (strcmp("normal",style) &&
			strcmp("bold",style) &&
			strcmp("italic",style) &&
			strcmp("bold-italic",style) &&
			strcmp("nil",style))
		{
			return luaL_error(L, "The font styles only can take one of the following values: normal, bold, bold-italic, italic, nil\n");
		}
		
		{	//	parse font lua style 
			std::string luastyle(style);
			bool sty  = std::string::npos != luastyle.find("italic");
			bool bold = std::string::npos != luastyle.find("bold");
			bool variant = false; //variant = "normal" | "small-caps"
			::canvas::FontInfo font( face, size, bold, sty, variant );
			if (!wrapper->surface()->setFont( font )) {
				return luaL_error(L, "The font has not been set\n");
			}
		}

		return 0;
	}
}

static int l_attrClip( lua_State *L ) {
	//	a) canvas:attrClip () -> x, y, width, height: number
	//	b) canvas:attrClip (x, y, width, height: number)
	SurfaceWrapper *wrapper=checkSurface(L);

	if (lua_gettop(L) == 1) {
		//	(a)
		::canvas::Rect rect;
		if (!wrapper->surface()->getClip( rect )) {
			return luaL_error(L, "There is no set clipping area\n");
		}
		return pushRect( L, rect );
	}
	else {
		//	(b)
		::canvas::Rect rect;
		checkRect( L, 2, rect );
		if (!wrapper->surface()->setClip( rect )) {
			return luaL_error(L, "The clipping area has not been set\n");
		}

		return 0;
	}
}

static int l_attrCrop( lua_State *L ) {
	//	a) canvas:attrCrop () -> x, y, w, h: number	
	//	b) canvas:attrCrop (x, y, w, h: number)
	SurfaceWrapper *wrapper=checkSurface(L);

	if (lua_gettop(L) == 1) {
		//	(a)
		const canvas::Rect &rect = wrapper->surface()->getBounds();
		return pushRect( L, rect );
	} else {
		//	(b)		
		//	Get canvas module from stack
		Module *module = Module::get( L );
		if (!module) {
			return luaL_error( L, "Invalid canvas module\n" );
		}

		if (wrapper->isPrimary()) {
			return luaL_error( L, "Cannot change the crop attribute to main canvas\n" );
		}		

		//	Check rectangle
		canvas::Rect rect;
		checkRect( L, 2, rect );

		canvas::Surface *newSurface;
		
		{	//	Create new surface
			if ((rect.w < 0) || (rect.h < 0)){
				return luaL_error( L, "Cannot create surface from rect. Width and height must be greater than 0\n" );
			}
			newSurface=getCanvas(wrapper)->createSurface( canvas::Rect(0,0,rect.w,rect.h) );
			if (!newSurface) {
				return luaL_error( L, "Cannot create surface from rect\n" );
			}
		}

		//	Blit rectangle to new surface
		if (!newSurface->blit( canvas::Point(0,0), wrapper->surface(), rect )) {
			canvas::Surface::destroy(newSurface);
			return luaL_error( L, "Cannot blit rect to new surface\n" );
		}

		wrapper->replaceSurface( newSurface );
		return 0;
	}	
}

static int l_attrFlip( lua_State *L ) {
	//	a) canvas:attrFlip () -> horiz, vert: boolean	
	//	b) canvas:attrFlip ( horiz, vert: boolean)
	SurfaceWrapper *wrapper=checkSurface(L);
	
	//	Get canvas module from stack
	Module *module = Module::get( L );
	if (!module) {
		return luaL_error( L, "Invalid canvas module\n" );
	}

	if (wrapper->isPrimary()) {
		return luaL_error( L, "Cannot change the flip attribute to main canvas\n" );
	}		

	if (lua_gettop(L) == 1) {
		//	(a)
		lua_pushboolean( L, wrapper->isFlippedH() );
		lua_pushboolean( L, wrapper->isFlippedV() );
		return 2;
	} else {
		//	(b)
		bool horiz;
		bool vert;
		if (lua_isboolean(L,2) && lua_isboolean(L,3)){
			horiz = lua_toboolean(L,2) != 0;
			vert = lua_toboolean(L,3) != 0;
		} else {
			return luaL_error( L, "Invalid arguments\n" );
		}
		wrapper->setFlip(horiz, vert);
		return 0;
	}	
}

static int l_attrOpacity( lua_State *L ) {
// 	a) canvas:attrOpacity (opacity: number)
// 	b) canvas:attrOpacity () -> opacity: number

	SurfaceWrapper *wrapper = checkSurface(L);

	if (lua_gettop(L) == 1) {
		lua_pushnumber(L, wrapper->surface()->getOpacity());
		return 1;
	} else {
		//	Get canvas module from stack
		Module *module = Module::get( L );
		if (!module) {
			return luaL_error( L, "Invalid canvas module\n" );
		}

		if (wrapper->isPrimary()) {
			return luaL_error( L, "Cannot change opacity to main canvas\n" );
		}

		int x = luaL_checkint(L, 2);
		if (x < 0 || x > 255){
			return luaL_error(L, "Opacity value must be between 0 and 255");
		}

		wrapper->surface()->setOpacity( (util::BYTE) x );
		return 0;		
	}
}

static int l_attrRotation( lua_State *L ) {
// 	a) canvas:attrRotation (degrees: number)
// 	b) canvas:attrRotation () -> degrees: number

	SurfaceWrapper *wrapper=checkSurface(L);

	if (lua_gettop(L) == 1) {
		//	(a)
		lua_pushnumber(L, wrapper->rotation());
		return 1;
	} else {
		//	(b)

		//	Get canvas module from stack
		Module *module = Module::get( L );
		if (!module) {
			return luaL_error( L, "Invalid canvas module\n" );
		}

		if (wrapper->isPrimary()) {
			return luaL_error( L, "Cannot change the rotation attribute to main canvas\n" );
		}

		int degrees = luaL_checkint(L, 2);
		if (degrees%90 == 0) {
			wrapper->rotation(degrees);
		}
		return 0;
	}
}

static int l_attrScale( lua_State *L ) {
	// attrScale( w, h: number | boolean )
	if (lua_gettop(L) == 3) {
		SurfaceWrapper *wrapper=checkSurface(L);

		//	Get canvas module from stack
		Module *module = Module::get( L );
		if (!module) {
			return luaL_error( L, "Invalid canvas module\n" );
		}
		if (wrapper->isPrimary()) {
			return luaL_error( L, "Cannot scale the main canvas\n" );
		}

		canvas::Size s = wrapper->surface()->getSize();
		int w, h;
		double fw, fh;

		if (lua_isboolean(L, 2)==1) {
			if (lua_toboolean(L, 2)==0) {
				return luaL_error( L, "Boolean parameter must be true\n" );
			}
			if (lua_isnumber(L, 3)==1) {
				h = luaL_checkint(L, 3);
				if (h>0) {
					fw = fh = ((double)h) / ((double)s.h);
				} else {
					return luaL_error( L, "Height must be greater than 0\n" );
				}
			} else {
				return luaL_error( L, "At least one argument must be a number\n" );
			}
		} else if (lua_isboolean(L, 3)==1) {
			if (lua_toboolean(L, 3)==0) {
				return luaL_error( L, "Boolean parameter must be true\n" );
			}
			if (lua_isnumber(L, 2)==1) {
				w = luaL_checkint(L, 2);
				if (w>0) {
					fw = fh = ((double)w) / ((double)s.w);
				} else {
					return luaL_error( L, "Height must be greater than 0\n" );
				}
			} else {
				return luaL_error( L, "At least one argument must be a number\n" );
			}
		} else {
			w = luaL_checkint(L, 2);
			h = luaL_checkint(L, 3);
			if (w>0 && h>0) {
				fw = ((double)w) / ((double)s.w);
				fh = ((double)h) / ((double)s.h);
			} else {
				return luaL_error( L, "Height and width must be greater than 0\n" );
			}
		}
		wrapper->setScaledSize( canvas::Size(boost::math::iround(s.w*fw), boost::math::iround(s.h*fh)) );
		return 0;
	} else {
		return luaL_error( L, "Wrong number of parameters\n" );
	}
}

static int l_drawLine( lua_State *L ) {
	//	canvas:drawLine (x1, y1, x2, y2: number)
	if (lua_gettop(L) == 5) {
		SurfaceWrapper *wrapper=checkSurface(L);

		int x1 = luaL_checkint(L, 2);
		int y1 = luaL_checkint(L, 3);
		int x2 = luaL_checkint(L, 4);
		int y2 = luaL_checkint(L, 5);

		wrapper->surface()->drawLine( x1, y1, x2, y2 );
		return 0;
	} else {
		return luaL_error( L, "Wrong number of parameters\n" );
	}
}

static int l_drawRect( lua_State *L ) {
	//	canvas:drawRect (mode: string; x, y, width, height: number) mode = [fill|frame]
	SurfaceWrapper *wrapper=checkSurface(L);
	
	const char *mode = luaL_checkstring(L,2);
	if (!mode) {
		return luaL_error(L, "Bad argument. The first argument must be a string.");
	}

	::canvas::Rect rect;
	checkRect( L, 3, rect );

	if (!strcmp( "fill", mode )) {
		wrapper->surface()->fillRect( rect );
	}
	else if (!strcmp( "frame", mode )) {
		wrapper->surface()->drawRect( rect );
	}
	else {
		return luaL_error(L, "The first argument is invalid. Options: fill | frame");
	}
	
	return 0;
}

static int l_drawRoundRect( lua_State *L ) {
	//	canvas:drawRoundRect (mode: string; x, y, width, height, arcWidth, arcHeight: number) mode = [fill|frame]
	SurfaceWrapper *wrapper=checkSurface(L);
	
	const char *mode = luaL_checkstring(L,2);
	if (!mode) {
		return luaL_error(L, "Bad argument. The first argument must be a string.");
	}

	::canvas::Rect rect;
	checkRect( L, 3, rect );
	
	int arcW = luaL_checkint(L, 7);
	int arcH = luaL_checkint(L, 8);

	if (!strcmp( "fill", mode )) {
		wrapper->surface()->fillRoundRect( rect, arcW, arcH );
	}
	else if (!strcmp( "frame", mode )) {
		wrapper->surface()->drawRoundRect( rect, arcW, arcH );
	}
	else {
		return luaL_error(L, "The first argument is invalid. Options: fill | frame.");
	}
	return 0;	
}

//	Aux l_drawPolygon function
static int l_auxPolygonDrawing( lua_State *L ) {
	//	gets the drawer instance from the lua stack
	impl::PolygonDrawer **drawer = (impl::PolygonDrawer **)lua_touserdata(L, lua_upvalueindex(1));

	if (lua_gettop(L) == 2) {
		::canvas::Point point;
		//	gets the point passed by parameter
		checkPoint( L, 1, point );

		(*drawer)->addPoint(point);

		// push the drawer instance in the stack
		lua_pushlightuserdata(L, drawer);

		// 	push the return function in the stack
		lua_pushcclosure( L, l_auxPolygonDrawing, 1 );
		return 1;
	} else {
		int res = (*drawer)->draw(L);
		DEL(*drawer);
		return res;
	}
}

static int l_drawPolygon( lua_State *L ) {
	SurfaceWrapper *wrapper = checkSurface(L);

	if (lua_gettop(L) == 2) {
		const char *mode = luaL_checkstring(L,2);
		if (!mode) {
			return luaL_error(L, "Bad argument. The argument must be a string.");
		}

		if ((strcmp(mode, "open") != 0) && (strcmp(mode, "fill") != 0) && (strcmp(mode, "close") != 0) ) {
			return luaL_error(L, "Bad argument. The draw mode must be open, close or fill.");
		}

		impl::PolygonDrawer **drawer = (impl::PolygonDrawer **) lua_newuserdata(L, sizeof(impl::PolygonDrawer *));
		*drawer = new impl::PolygonDrawer( mode, wrapper );
		/* set its metatable */
		luaL_getmetatable(L, "Aux.Drawer");
		lua_setmetatable(L, -2);

		//	 push the return function
		lua_pushcclosure( L, l_auxPolygonDrawing, 1 );
		return 1;
	} else {
		return luaL_error(L, "No argument. To draw a polygon one of the following draw modes must be specified : fill, close, open.");
	}
}

static int l_drawEllipse( lua_State *L ) {
	SurfaceWrapper *wrapper=checkSurface(L);

	const char *mode = luaL_checkstring(L,2);
	if (!mode) {
		return luaL_error(L, "Bad argument. The first argument must be a string.");
	}

	::canvas::Point point;
	checkPoint( L, 3, point );

	int w    =  boost::math::iround(luaL_checknumber(L, 5)/2);
	int h    =  boost::math::iround(luaL_checknumber(L, 6)/2);
	int arcW = luaL_checkint(L, 7);
	int arcH = luaL_checkint(L, 8);

	if (!strcmp( "fill", mode )) {
		wrapper->surface()->fillEllipse( point, w, h, arcW, arcH );
	}
	else if (!strcmp( "arc", mode )) {
		wrapper->surface()->drawEllipse( point, w, h, arcW, arcH );
	}
	else {
		return luaL_error(L, "The first argument is invalid. Options: fill | arc");
	}
	
	return 0;	
}

static int l_drawText( lua_State *L ) {
	//	canvas:drawText (x,y: number; text: string)
	SurfaceWrapper *wrapper=checkSurface(L);
	
	canvas::Point point;
	checkPoint( L, 2, point );
	
	const char *text = luaL_checkstring(L,4);
	if (!text) {
		return luaL_error(L, "Bad argument. The third argument must be a string.");
	}

	std::string str( text );
	Player::convertText( str );
	point.y += wrapper->surface()->fontAscent();
	wrapper->surface()->drawText( point, str );
	return 0;
}

static int l_clear( lua_State *L ) {
	//	(a) canvas:clear ()	
	//	(b) canvas:clear (x, y, w, h: number)
	SurfaceWrapper *wrapper=checkSurface(L);
	wrapper->surface()->setCompositionMode( canvas::composition::source );

	if (lua_gettop(L) == 1) {
		//	(a)
		canvas::Size size = wrapper->surface()->getSize();
		wrapper->surface()->fillRect(canvas::Rect(0,0,size.w,size.h));
	} else {
		//	(b)
		canvas::Rect rect;
		checkRect( L, 2, rect );
		wrapper->surface()->fillRect(rect);
	}
	wrapper->surface()->setCompositionMode( canvas::composition::source_over );
	return 0;
}

static int l_flush( lua_State *L ) {
	//	canvas:flush ()
	Module *module = Module::get( L );
	if (!module) {
		return luaL_error( L, "Invalid canvas module\n" );
	}

	SurfaceWrapper *wrapper=checkSurface(L);
	if (wrapper->isPrimary()) {
		module->flushPrimary();
	}
	return 0;
}

static int l_compose( lua_State *L ) {
	//	(a) canvas:compose (x, y: number; src: canvas )	
	//	(b) canvas:compose (x, y: number; src: canvas; src_x, src_y, src_width, src_height: number )
	SurfaceWrapper *wrapper=checkSurface(L);
	
	canvas::Point point;
	checkPoint( L, 2, point );

	SurfaceWrapper *srcWrapper=checkSurface(L,4);
	
	if (!srcWrapper) {
		return luaL_error(L, "The source is invalid");
	}

	canvas::Surface *surface;
	if (srcWrapper->rotation()!=0) {
		surface = srcWrapper->surface()->rotate(srcWrapper->rotation());
	} else {
		surface = srcWrapper->surface();
	}

	if (lua_gettop(L) == 4) {
		//	(a)
		if (srcWrapper->needScale()) {
			wrapper->surface()->scale( canvas::Rect(point, srcWrapper->getScaledSize()),
						    surface,
						    srcWrapper->isFlippedH(),
						    srcWrapper->isFlippedV() );
		} else {
			wrapper->surface()->blit( point, surface );
		}
	} else {
		//	(b)
		canvas::Rect rect;
		checkRect( L, 5, rect );
		if (srcWrapper->needScale()) {
			wrapper->surface()->scale( canvas::Rect(point, srcWrapper->getScaledSize()),
						    surface,
						    rect,
						    srcWrapper->isFlippedH(),
						    srcWrapper->isFlippedV() );
		} else {
			wrapper->surface()->blit( point, surface, rect );
		}
	}
	if (surface != srcWrapper->surface()) {
		canvas::Surface::destroy( surface );
	}
	return 0;
}

static int l_pixel( lua_State *L ) {
	//	(a) canvas:pixel (x, y: number) -> R, G, B, A: number
	//	(b) canvas:pixel (x, y, R, G, B, A: number)
	SurfaceWrapper *wrapper=checkSurface(L);
	
	::canvas::Point point;
	checkPoint( L, 2, point );
	
	if (lua_gettop(L) == 3) {
		//	(a)
		::canvas::Color color;
		if (!wrapper->surface()->getPixelColor( point, color )) {
			return luaL_error(L, "Bad argument. Invalid point ");
		}
		return pushColor( L, color );
	}
	else {
		//	(b)
		::canvas::Color color;
		checkColor( L, 4, color );
		wrapper->surface()->setPixelColor( point, color );
		return 0;
	}
}

static int l_measureText( lua_State *L ) {
	//	canvas:measureText (text: string) -> dx, dy: number
	SurfaceWrapper *wrapper=checkSurface(L);
	
	const char *text = luaL_checkstring(L,2);
	if (!text) {
		return luaL_error(L, "Bad argument. The text argument must be a string.");
	}

	::canvas::Size size;
	if (!wrapper->surface()->measureText( text, size )) {
		return luaL_error(L, "The text is invalid.");

	}
	return pushSize( L, size );
}

static const struct luaL_Reg canvas_methods[] = {
	{ "new",           l_new           },
	{ "attrSize",      l_attrSize      },
	{ "attrColor",     l_attrColor     },
	{ "attrFont",      l_attrFont      },
	{ "attrClip",      l_attrClip      },
	{ "attrCrop",      l_attrCrop      },
	{ "attrFlip",      l_attrFlip      },
	{ "attrOpacity",   l_attrOpacity   },
	{ "attrRotation",  l_attrRotation  },
	{ "attrScale",     l_attrScale     },
	{ "drawLine",      l_drawLine      },
	{ "drawRect",      l_drawRect      },
	{ "drawRoundRect", l_drawRoundRect },
	{ "drawPolygon",   l_drawPolygon   },
	{ "drawEllipse",   l_drawEllipse   },
	{ "drawText",      l_drawText      },
	{ "flush",         l_flush         },
	{ "clear",         l_clear         },
	{ "compose",       l_compose       },
	{ "pixel",         l_pixel         },
	{ "measureText",   l_measureText   },
	{ NULL,            NULL            }
};

/*******************************************************************************
*	Canvas module
*******************************************************************************/

Module::Module( LuaPlayer *player, lua_State *st )
	: _player(player), _lua( st )
{
	//	Store module into stack
	lua::storeObject( st, this, LUA_CANVAS );

	//	Register new type and set the metadata
	luaL_newmetatable(st, CANVAS_TYPE);
	lua_pushvalue(st, -1);
	lua_setfield(st, -2, "__index");
	luaL_register(st, NULL, canvas_methods);
	lua_pop(st, 1);
	lua_pushnil(st);
}

Module::~Module()
{
}

void Module::start( canvas::Surface *surface ) {
	_surface = surface;
	canvas::Surface *main = surface->canvas()->createSurface(surface->getBounds());

	//	Create main canvas
	createSurface(main, true);
	lua_setglobal(_lua, "canvas");
}

void Module::stop() {
	LINFO("canvas::Module", "Stop");
	CLEAN_ALL( SurfaceWrapper* , _surfaces );
}

int Module::createSurface( canvas::Surface *surface, bool isPrimary/*=false*/ ) {
	SurfaceWrapperPtr *newSurface = (SurfaceWrapperPtr *)lua_newuserdata(_lua, sizeof(SurfaceWrapperPtr) );
	SurfaceWrapper *wrapper = new SurfaceWrapper( surface, isPrimary );
	*newSurface = wrapper;
	surface->setColor(canvas::Color(0,0,0,255));
	luaL_getmetatable(_lua, CANVAS_TYPE );
	lua_setmetatable(_lua, -2);
	_surfaces.push_back( wrapper );
	return 1;
}

std::string Module::getImagePath( const std::string &image ) {
	//	Make asbsolute path and create the image surface from file
	fs::path absPath( _player->rootPath() );
	absPath /= image;
	return absPath.string();
}

Module *Module::get( lua_State *st ) {
	return lua::getObject<Module>(st,LUA_CANVAS);
}

void Module::flushPrimary() {
	_surface->setCompositionMode( canvas::composition::source );
	_surface->blit( canvas::Point(0,0), _surfaces[0]->surface() );
	_surface->setCompositionMode( canvas::composition::source_over );
	_player->update();
}

}
}


