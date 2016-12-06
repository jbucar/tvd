/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "../../surface.h"
#include "../../color.h"
#include <SDL/SDL_ttf.h>

struct SDL_Surface;
struct SDL_Rect;

namespace canvas {
namespace sdl {

class Canvas;
class Font;

class Surface : public canvas::Surface {
public:
	Surface( Canvas *canvas, SDL_Surface *sdl );
	Surface( Canvas *canvas, const Rect &rect );
	Surface( Canvas *canvas, const std::string &file );
	virtual ~Surface();

	virtual canvas::Canvas *canvas() const;

	virtual Size getSize() const;

	virtual bool getClip( Rect &rect );

	virtual int fontAscent();
	virtual int fontDescent();

	virtual bool setOpacity( util::BYTE alpha );
	virtual util::BYTE getOpacity() const;

	virtual void setCompositionMode( composition::mode mode );
	virtual composition::mode getCompositionMode();

	virtual bool saveAsImage( const std::string &file );
    
	SDL_Surface* getContent();

protected:
	virtual bool applyFont();

	//	Aux fonts
	void freeFont();
	void loadFont();
	TTF_Font *currentFont();

	//	Aux generic
	void init( Canvas *canvas );
	SDL_Surface* createSDLSurface( int width, int height );
	void swapSurface( SDL_Surface* );
	SDL_Surface* loadImage( const std::string &file );
	SDL_Rect  toSDL( const Rect &src ) const;
	SDL_Color toSDL( const Color &src ) const;
	const std::string getPathFont( const std::string &fontFace );
	void prepareToBlit( const Rect &target, SDL_Surface *srcSurface, const Rect &source );
	Size rawDrawText( const Point &pos, const std::string &text );

	virtual void setClipImpl( const Rect &rect );

	virtual void drawLineImpl( int x1, int y1, int x2, int y2 );

	virtual void drawRectImpl( const Rect &rect );
	virtual void fillRectImpl( const Rect &rect, bool blend );

	virtual void drawRoundRectImpl( const Rect &rect, int arcW, int arcH );
	virtual void fillRoundRectImpl( const Rect &rect, int arcW, int arcH );

	virtual void drawPolygonImpl( const std::vector<Point>& vertices, bool closed );
	virtual void fillPolygonImpl( const std::vector<Point>& vertices );

	virtual void drawEllipseImpl( const Point &center, int rw, int rh, int angStart, int angStop );
	virtual void fillEllipseImpl( const Point &center, int rw, int rh, int angStart, int angStop );
	
	virtual void blitImpl( const Point &target, canvas::Surface *srcSurface, const Rect &source );
	virtual void scaleImpl( const Rect &targetRect, canvas::Surface *srcSurface, const Rect &sourceRect );
	virtual canvas::Surface *rotateImpl( int degrees );

	virtual util::DWORD getPixel( const Point &pos );
	virtual void getPixelColorImpl( const Point &pos, Color &color );
	virtual void setPixelColorImpl( const Point &pos, const Color &color );

	virtual void clearImpl( const Rect &rect );

	virtual void drawTextImpl( const Point &pos, const std::string &text, int ascent );
	virtual void drawTextImpl( const Rect &rect, const std::string &text, Alignment alignment=alignment::None, int spacing=0 );
	virtual void measureTextImpl( const std::string &text, Size &size );

	virtual bool comparePixels( const Point &pos, canvas::Surface* image );

private:
	Canvas *_canvas;
	SDL_Surface *_surface;
	TTF_Font *_SDLfont;

	Surface() {}
};

}
}
