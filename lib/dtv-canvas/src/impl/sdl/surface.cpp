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

#include "surface.h"
#include "canvas.h"
#include "pngfuncs.h"
#include "../../rect.h"
#include "../../color.h"
#include "../../point.h"
#include "../../size.h"
#include "../../alignment.h"
#include "generated/config.h"
#include "gfx/SDL_gfxPrimitives.h"
#include "gfx/SDL_rotozoom.h"
#include "gfx/SDL_gfxBlitFunc.h"
#include <util/log.h>
#include <util/mcr.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>


namespace canvas {
namespace sdl {

Surface::Surface( Canvas *canvas, SDL_Surface *sdlSurface )
{
	init( canvas );
	DTV_ASSERT( sdlSurface );
	_surface = sdlSurface;
	canvas::Surface::clear();
}

Surface::Surface( Canvas *canvas, const canvas::Rect &rect )
	: canvas::Surface( rect )
{
	init( canvas );
	_surface = createSDLSurface( rect.w, rect.h ); 
	canvas::Surface::clear();
}

Surface::Surface( Canvas *canvas, const std::string &file )
{
	init( canvas );
	_surface = loadImage( file );
	if (!_surface) {
		throw std::runtime_error( "Cannot load image file!" );
	}
}

Surface::~Surface()
{
	freeFont();
	SDL_FreeSurface( _surface );
}

/****************************** Aux functions ********************************/

canvas::Canvas *Surface::canvas() const {
	return _canvas;
}

void Surface::init( Canvas *canvas ) {
	_canvas = canvas;
	_SDLfont = NULL;
	setScaledSize(getSize());
}

SDL_Rect Surface::toSDL( const canvas::Rect &src ) const {
	SDL_Rect rect;
	rect.x = src.x;
	rect.y = src.y;
	rect.w = src.w;
	rect.h = src.h;
	return rect;
}

SDL_Color Surface::toSDL( const canvas::Color &src ) const {
	SDL_Color color;
	color.r      = src.r;
	color.g      = src.g;
	color.b      = src.b;
	color.unused = src.alpha;
	return color;
}

static util::DWORD getpixel(SDL_Surface *surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	DTV_ASSERT( x < surface->w && y < surface->h );
	util::BYTE *p = (util::BYTE *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
		case 1:
			return *p;
			break;

		case 2:
			return *(util::WORD *)p;
			break;

		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;
			break;

		case 4:
			return *(util::DWORD *)p;
			break;

		default:
			return 0;       
	}
}

void putpixel(SDL_Surface *surface, int x, int y, util::DWORD pixel)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	util::BYTE *p = (util::BYTE *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
	case 1:
		*p = pixel;
		break;

	case 2:
		*(util::WORD *)p = pixel;
		break;

	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		} else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(util::DWORD *)p = pixel;
		break;
	}
}

bool Surface::saveAsImage( const std::string &file ) {
	bool success = true;
	
	int rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000; gmask = 0x00ff0000; bmask = 0x0000ff00; amask = 0x000000ff;
#else
	rmask = 0x000000ff; gmask = 0x0000ff00; bmask = 0x00ff0000; amask = 0xff000000;
#endif
	
	SDL_Surface *tmp = SDL_CreateRGBSurface(
		_surface->flags,
		_surface->w,
		_surface->h,
		_surface->format->BitsPerPixel,
		rmask,
		gmask,
		bmask,
		amask
	);
	//SDL_BlitSurface( _surface, NULL, tmp, NULL );
	
	
	canvas::Color c;
	for (int i=0; i < _surface->w; ++i ) {
		for (int j=0; j < _surface->h; ++j ) {
			//getPixelColor( canvas::Point(i,j), c);
			SDL_GetRGBA( getpixel( _surface, i, j ), _surface->format, &c.r, &c.g, &c.b, &c.alpha );
			//util::DWORD color = SDL_MapRGBA( tmp->format, c.r, c.g, c.b, c.alpha );
			util::DWORD color = SDL_MapRGBA( tmp->format, c.r, c.g, c.b, c.alpha );
			putpixel( tmp, i, j, color );
		}
	}
	
	if ( png_save_surface( file.c_str(), tmp ) < 0 ) {
		LWARN("sdl::Surface", "Can't save image as png, saving as bmp");
		SDL_SaveBMP( tmp, file.c_str() );
		success = false;
	}
	SDL_FreeSurface( tmp );
	return success;
}

SDL_Surface* Surface::createSDLSurface( int width, int height ) {
	SDL_Surface *screen = SDL_GetVideoSurface();
	DTV_ASSERT( screen );
	SDL_Surface *sur = SDL_CreateRGBSurface(
		screen->flags,
		width,
		height,
		screen->format->BitsPerPixel,
		screen->format->Rmask,
		screen->format->Gmask,
		screen->format->Bmask,
		screen->format->Amask
	);
	if (!sur) {
		LERROR("sdl::Surface", "Can't create SDL_Surface: %s", SDL_GetError());
		throw std::runtime_error( "Cannot create SDL surface!" );
	}
	
	SDL_Surface *surAlpha = SDL_DisplayFormatAlpha ( sur );
	SDL_FreeSurface( sur );

	if (!surAlpha) {
		LERROR("sdl::Surface", "Can't set display format alpha: %s", SDL_GetError());
		throw std::runtime_error( "Cannot create SDL surface!" );
	}

	return surAlpha;
}

SDL_Surface* Surface::getContent() {
	return _surface;
}

/****************************** Size functions ********************************/
canvas::Size Surface::getSize() const {
	canvas::Size size;
	size.w = _surface->w;
	size.h = _surface->h;
	return size;
}

/****************************** Clip functions ********************************/

bool Surface::getClip( canvas::Rect &rect ) {
	SDL_Rect r;
	SDL_GetClipRect( _surface, &r );
	rect.x = r.x;
	rect.y = r.y;
	rect.w = r.w;
	rect.h = r.h;
	return true;
}

void Surface::setClipImpl( const canvas::Rect &rect ) {
	SDL_Rect r = toSDL( rect );
	SDL_SetClipRect( _surface, &r );
}

/****************************** Line functions ********************************/

void Surface::drawLineImpl( int x1, int y1, int x2, int y2 ) {
	const canvas::Color &color = getColor();
	lineRGBA( _surface, x1, y1, x2, y2, color.r, color.g, color.b, color.alpha );
}

/****************************** Rect functions ********************************/

void Surface::drawRectImpl( const canvas::Rect &rect ) {
    const canvas::Color &color = getColor();
    rectangleRGBA(_surface, rect.x, rect.y, rect.w + rect.x - 1, rect.h + rect.y - 1, color.r, color.g, color.b, color.alpha );
}

void Surface::fillRectImpl( const canvas::Rect &rect, bool /*blend*/ ) {
	const canvas::Color &color = getColor();
	boxRGBA(_surface, rect.x, rect.y, rect.x + rect.w - 1, rect.y + rect.h - 1, color.r, color.g, color.b, color.alpha );
}

/*************************** RoundRect functions ******************************/

void Surface::drawRoundRectImpl( const canvas::Rect &rect, int arcW, int arcH ) {
    const canvas::Color &color = getColor();
    roundedRectangleRGBA(
        _surface,
        rect.x,
        rect.y,
        rect.w + rect.x - 1,
        rect.h + rect.y - 1,
        (arcW + arcH) / 2,
        color.r,
        color.g,
        color.b,
        color.alpha
    );
}

void Surface::fillRoundRectImpl( const canvas::Rect &rect, int arcW, int arcH ) {
    const canvas::Color &color = getColor();
    roundedBoxRGBA(
        _surface,
        rect.x,
        rect.y,
        rect.w + rect.x - 1,
        rect.h + rect.y - 1,
        (arcW + arcH) / 2,
        color.r,
        color.g,
        color.b,
        color.alpha
    );
}

/**************************** Polygon functions ******************************/

//     TODO: open polygon
void Surface::drawPolygonImpl(const std::vector<Point>& vertices, bool /*closed*/) {
    const canvas::Color &color = getColor();

    Sint16* vx = new Sint16[vertices.size()];
    Sint16* vy = new Sint16[vertices.size()];
    for(size_t i=0; i<vertices.size(); ++i) {
        const Point& point = vertices[i];
        vx[i] = point.x;
        vy[i] = point.y;
    }
    polygonRGBA(_surface, vx,vy, vertices.size(), color.r,color.g,color.b,color.alpha);
    delete[] vx;
    delete[] vy;
}

void Surface::fillPolygonImpl(const std::vector<Point>& vertices) {
    const canvas::Color &color = getColor();

    Sint16* vx = new Sint16[vertices.size()];
    Sint16* vy = new Sint16[vertices.size()];
    for(size_t i=0; i<vertices.size(); ++i) {
        const Point& point = vertices[i];
        vx[i] = point.x;
        vy[i] = point.y;
    }
    filledPolygonRGBA(_surface, vx,vy, vertices.size(), color.r,color.g,color.b,color.alpha);
    delete[] vx;
    delete[] vy;
}

/**************************** Ellipse functions ******************************/

void Surface::drawEllipseImpl( const canvas::Point &center, int rw, int rh, int /*angStart*/, int /*angStop*/ ) {
    const canvas::Color &color = getColor();
    ellipseRGBA(
        _surface,
        center.x,
        center.y,
        rw,
        rh,
        color.r,
        color.g,
        color.b,
        color.alpha
    );
}

void Surface::fillEllipseImpl( const canvas::Point &center, int rw, int rh, int /*angStart*/, int /*angStop*/ ) {
    const canvas::Color &color = getColor();
    filledEllipseRGBA(
        _surface,
        center.x,
        center.y,
        rw,
        rh,
        color.r,
        color.g,
        color.b,
        color.alpha
    );
}

/****************************** Font functions ********************************/

const std::string Surface::getPathFont( const std::string &fontFace ) {
    boost::filesystem::path pathGinga( DATA_PREFIX_PATH );
    boost::filesystem::path pathGingaFonts( pathGinga / "share" / "ginga" / "font" );
    DTV_ASSERT( boost::filesystem::exists( pathGingaFonts ) );

    boost::filesystem::path pathFontFace( pathGingaFonts / fontFace );
    pathFontFace.replace_extension( ".ttf" );
    if( boost::filesystem::exists( pathFontFace ) ) {
        return pathFontFace.string();
    }

    boost::filesystem::path pathFontDefault( pathGingaFonts / DEFAULT_FONT );
    pathFontDefault.replace_extension( ".ttf" );
    DTV_ASSERT( boost::filesystem::exists( pathFontDefault ) );
    return pathFontDefault.string();
}

void Surface::freeFont() {
	if (_SDLfont) {
		TTF_CloseFont( _SDLfont );
		_SDLfont = NULL;
	}
}

void Surface::loadFont() {
	//	Initialize TTF
	if (TTF_WasInit() == 0) {
		if (TTF_Init() == -1 ) {
			LERROR("sdl::Surface", "Can't Initialize TTF: %s", TTF_GetError());
			return;
		}
	}

	//	Create font
	const canvas::Font &font = getFont();
	std::vector<std::string> families = font.families();
	BOOST_FOREACH(std::string family, families) {
		_SDLfont = TTF_OpenFont( getPathFont( family ).c_str(), font.size() );
		if (_SDLfont) {
			break;
		}
	}
	if (!_SDLfont) {
		LERROR("sdl::Surface", "Unable to load font: %s", TTF_GetError());
		return;
	}
	LINFO("sdl::Surface", "Using font %s", TTF_FontFaceFamilyName(_SDLfont));

	{	//	Setup font stype
		int style = 0;
		if (font.bold()) {
			style |= TTF_STYLE_BOLD;
		}	
		if (font.italic()) {
			style |= TTF_STYLE_ITALIC;
		}
		if (font.smallCaps()) {
			LWARN("sdl::Surface", "Small-caps not implemented yet");
		}
		TTF_SetFontStyle( _SDLfont, style );
	}
}

TTF_Font *Surface::currentFont() {
	if (!_SDLfont) {
		loadFont();
		DTV_ASSERT(_SDLfont);
	}
	return _SDLfont;
}

bool Surface::applyFont() {
	freeFont();
	return true;
}

/****************************** Text functions ********************************/

canvas::Size Surface::rawDrawText( const canvas::Point &pos, const std::string &text ) {
	SDL_Color color = toSDL( getColor() );
	SDL_Surface *tmp = TTF_RenderUTF8_Blended( currentFont(), text.c_str(), color );
	canvas::Rect clip;
	getClip( clip );
	int start_x = std::max( pos.x, clip.x );
	int start_y = std::max( pos.y, clip.y );
	int end_x = std::min( pos.x+ tmp->w, clip.x+clip.w );
	int end_y = std::min( pos.y+ tmp->h, clip.y+clip.h );
	canvas::Rect target = canvas::Rect( start_x, start_y , end_x-start_x, end_y-start_y );
	canvas::Rect source = canvas::Rect( 0, 0, tmp->w, tmp->h );
	prepareToBlit( target, tmp, source );
	SDL_Rect rect = toSDL( target );
	SDL_BlitSurface( tmp, NULL, _surface, &rect );
	canvas::Size s = canvas::Size( tmp->w, tmp->h );
	SDL_FreeSurface( tmp );
	return s;
}

//	Note: The y-position is used as the baseline of the font.
void Surface::drawTextImpl( const canvas::Point &pos, const std::string &text, int ascent ) {
    Size realSize = rawDrawText( canvas::Point( pos.x, pos.y - ascent + 1), text);
    //	Since measureText is not accurate in SDL, cannot assert drawing inside the surface.
    DTV_ASSERT( pointInBounds( canvas::Point( pos.x+realSize.w-1, pos.y-ascent+realSize.h ) ));
}

//Note: The y-coordinate of rectangle is used as the top of the font.
void Surface::drawTextImpl( const canvas::Rect &rect, const std::string &text, canvas::Alignment /*alignment*/, int spacing ) {
	canvas::Rect oldClip, clip;
	getClip( oldClip );
	bool intersects = oldClip.intersection( rect, clip );
	if ( intersects ) {
	setClip( clip );
		std::vector<std::string> lineas;
		std::vector<std::string> strs;
		canvas::Size tam;
		
		boost::split(lineas, text, boost::is_any_of("\n"));
		measureText(" ",tam);
		int lineY = 0;
		for(size_t i=0;i< lineas.size();++i){            
			if(lineas.at(i).empty()){            
				lineY += tam.h + spacing;
			}else{  
				boost::split(strs, lineas.at(i), boost::is_any_of(" "));               
				bool force = true;
				bool ok = true;
				size_t k= 0;
				std::string line = "";
				canvas::Size lineSize;
				std::string word;
				while(ok){
					if (strs.at(k).empty()){
						word = " ";
					}else{
						word = (line.empty() ? "" : " ") +strs.at(k);
					}
					measureText( line+word, lineSize );
					if ( lineSize.w <= rect.w ) {
						line += word;
						force = false;
						k++;
					}else{
						measureText(strs.at(k),lineSize);
						if(lineSize.w > rect.w){
							k++;
							line += word;
							force = false;
						}else{
							ok=false;
						}
					}
					if(k == strs.size()){
						ok = false;
					}
				}
				if ( force ) {
					line += word;
					canvas::Size s = rawDrawText( canvas::Point( rect.x, rect.y+lineY ), line);
					lineY += s.h + spacing;
					line="";
				} else {
					canvas::Size s = rawDrawText( canvas::Point( rect.x, rect.y+lineY ), line);
					lineY += s.h + spacing;
					line=word.substr(1);
					measureText( line, lineSize );
				}
				if (k < strs.size()){
					std::string generarpalabra;
					size_t i2;
					for (i2=k;i2<strs.size()-1;i2++){
						generarpalabra += strs.at(i2)+" ";
					}
					generarpalabra += strs.at(i2)+"";
					std::string vacio = "" ;
					if(i+1 == lineas.size()){
						lineas.push_back(vacio);
					}else{
						lineas.insert(lineas.begin()+i+1,1,vacio);
					}
					lineas.at(i+1)=generarpalabra+lineas.at(i+1);
				}
				force = true;
			}
		}
		setClip( oldClip );
	}
}

void Surface::measureTextImpl( const std::string &text, canvas::Size &size ) {
	TTF_SizeUTF8( currentFont(), text.c_str(), &size.w, &size.h );
}

int Surface::fontAscent() {
	return TTF_FontAscent( currentFont() );
}

int Surface::fontDescent() {
	return abs( TTF_FontDescent( currentFont() ) );
}

/**************************** Opacity functions ******************************/

bool Surface::setOpacity( util::BYTE alpha ) {
	int res = SDL_gfxSetAlpha( _surface, alpha); //SDL_SetAlpha( _surface, SDL_SRCALPHA, alpha );
	markDirty();
	return res != -1;
}

util::BYTE Surface::getOpacity() const {
	//TODO
	return 1;
}

/****************************** Blit functions ********************************/

void Surface::prepareToBlit( const canvas::Rect &target, SDL_Surface *srcSurface, const canvas::Rect &source ) {
	if (_surface->format->Amask == srcSurface->format->Amask && _surface->format->Amask != 0 ) {
		canvas::Color destColor, sourceColor;
		canvas::Point p;
		for ( int i = 0; i < target.w; ++i ) {
			for ( int j = 0; j < target.h; ++j ) {
				p = canvas::Point(i,j);
				DTV_ASSERT( pointInBounds( p ) );
				int dest_x = target.x + i;
				int dest_y = target.y + j;
				int src_x = source.x + i;
				int src_y = source.y + j;
				SDL_GetRGBA( getpixel( _surface, dest_x, dest_y ), _surface->format, &destColor.r, &destColor.g, &destColor.b, &destColor.alpha );
				SDL_GetRGBA( getpixel( srcSurface, src_x, src_y ), srcSurface->format, &sourceColor.r, &sourceColor.g, &sourceColor.b, &sourceColor.alpha );
				util::DWORD opaque = SDL_MapRGBA( _surface->format, destColor.r, destColor.g, destColor.b, std::max( destColor.alpha, sourceColor.alpha ) );
				putpixel( _surface, dest_x, dest_y, opaque );
			}
		}
	}
	
}

void Surface::blitImpl( const canvas::Point &target, canvas::Surface *srcSurface, const canvas::Rect &source ) {

	Surface *src = dynamic_cast<Surface *>(srcSurface);
	SDL_Rect SDL_target = { target.x, target.y, 0, 0 };
	SDL_Rect SDL_source = toSDL( source );
	canvas::Rect clip;
	getClip( clip );
	int x_start = std::max(clip.x, target.x);
	int y_start = std::max(clip.y, target.y);
	int x_end   = std::min(clip.x+clip.w, target.x+source.w);
	int y_end   = std::min(clip.y+clip.h, target.y+source.h);
	SDL_BlitSurface( src->getContent(), &SDL_source, _surface, &SDL_target );
	prepareToBlit( canvas::Rect(x_start, y_start, x_end-x_start, y_end-y_start), src->getContent(), source );
}

void Surface::scaleImpl( const Rect &targetRect, canvas::Surface *srcSurface, const Rect &sourceRect ) {
	Surface *src = dynamic_cast<Surface *>(srcSurface);
	SDL_Rect SDL_target = { target.x, target.y, 0, 0 };
	SDL_Rect SDL_source = toSDL( source );
	canvas::Rect clip;
	getClip( clip );
	int x_start = std::max(clip.x, target.x);
	int y_start = std::max(clip.y, target.y);
	int x_end   = std::min(clip.x+clip.w, target.x+source.w);
	int y_end   = std::min(clip.y+clip.h, target.y+source.h);
	SDL_BlitSurface( src->getContent(), &SDL_source, _surface, &SDL_target );
	prepareToBlit( canvas::Rect(x_start, y_start, x_end-x_start, y_end-y_start), src->getContent(), source );
}

canvas::Surface *Surface::rotateImpl( int degrees ) {
	// TODO:
	return NULL;
}

/***************************** Image functions *******************************/

SDL_Surface* Surface::loadImage( const std::string &file ) {
	SDL_Surface *tmp =  IMG_Load( file.c_str() ) ;
	
	if (!tmp) {
		return NULL;
	}
       
	SDL_Surface *loadedImage =SDL_DisplayFormatAlpha( tmp );
	if (!loadedImage) {
		LERROR("sdl::Surface", "Can't load image %s: %s", file.c_str(), IMG_GetError());
	}
	return loadedImage;
}

/*********************** Pixel manipulation functions *************************/

#define THRESHOLD 20
bool Surface::comparePixels( const Point &pos, canvas::Surface* image ) {
    bool check=true;

    canvas::Color c1, c2;
    getPixelColor(pos, c1);
    image->getPixelColor(pos, c2);
    if (!c1.equals(c2, THRESHOLD ) ) {
        LINFO("sdl::Surface", "Difference found in pixel (%d, %d): s_color=(%d,%d,%d,%d), i_color=(%d,%d,%d,%d)",
            pos.x, pos.y, c1.r, c1.g, c1.b, c1.alpha, c2.r, c2.g, c2.b, c2.alpha );
        check=false;
    }
    return check;
}

util::DWORD Surface::getPixel( const Point &pos ) {
	return getpixel(_surface, pos.x, pos.y);
}

void Surface::getPixelColorImpl( const canvas::Point &pos, canvas::Color &color ) {
	util::DWORD pixel = getpixel( _surface, pos.x, pos.y );
	SDL_GetRGBA( pixel, _surface->format, &color.r, &color.g, &color.b, &color.alpha );
}

void Surface::setPixelColorImpl( const canvas::Point &pos, const canvas::Color &color ) {
	util::DWORD pixel = SDL_MapRGBA( _surface->format, color.r, color.g, color.b, color.alpha );
	putpixel( _surface, pos.x, pos.y, pixel );
}

/************************ Composition mode functions **************************/

void Surface::setCompositionMode( canvas::composition::mode mode ){
	//	Other modes are not supported in SDL.
	DTV_ASSERT( mode == canvas::composition::source_over );
}

canvas::composition::mode Surface::getCompositionMode() {
	//	Other modes are not supported in SDL.
	return canvas::composition::source_over;
}

/************************ Clear functions **************************/

void Surface::clearImpl( const canvas::Rect &rect ) {
	const canvas::Color &c = getColor();
	canvas::Rect clip;
	getClip( clip );
	canvas::Size size = getSize();
	int x_start = std::max(clip.x, rect.x);
	int y_start = std::max(clip.y, rect.y);
	int x_end   = std::min(std::min(clip.x+clip.w, rect.x+rect.w), size.w);
	int y_end   = std::min(std::min(clip.y+clip.h, rect.y+rect.h), size.h);
	for ( int i=x_start; i < x_end ; ++i ) {
		for ( int j=y_start; j < y_end; ++j ) {
			util::DWORD pixel = SDL_MapRGBA( _surface->format, c.r, c.g, c.b, c.alpha /*255*/ );
			putpixel( _surface, i, j, pixel );
		}
	}
}

}
}
