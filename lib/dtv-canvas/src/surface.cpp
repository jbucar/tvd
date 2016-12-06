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
#include "rect.h"
#include "text/font.h"
#include "text/glyphrun.h"
#include "canvas.h"
#include "system.h"
#include "types.h"
#include <util/log.h>
#include <util/mcr.h>
#include <util/assert.h>
#include <boost/math/special_functions/round.hpp>


namespace canvas {

/**
 * Destruye el Surface @b s eliminando la referencia al mismo del canvas a partir del cual fue creado.
 * @param s El surface a destruir
 */
void Surface::destroy( Surface *s ) {
	Canvas *canvas = s->canvas();
	canvas->destroy( s );
}

/**
 * Crea un Surface en la posición indicada por @b pos
 * @param pos La posición del surface relativa al canvas ( x=0, y=0 es la esquina superior izquierda )
 */
Surface::Surface( const Point &pos )
	: _pos(pos)
{
	_font = NULL;
	_visible = true;
	_zIndex = 1;
	_dirty = false;
	_autoFlush = false;
	_mode = composition::source_over;
	_flushMode = composition::source_over;
	_boundsChanged = false;
}

Surface::Surface()
{
	_font = NULL;
	_visible = true;
	_zIndex = 1;
	_dirty = false;
	_autoFlush = false;
	_mode = composition::source_over;
	_flushMode = composition::source_over;	
	_boundsChanged = false;
}

/**
 * Destructor base. No debe ser utilizado directamente para destruir una @c Surface,
 * para esto se debe llamar al método estático destroy().
 */
Surface::~Surface()
{
	// Font is owned and freed by Canvas
	_font = NULL;
}

void Surface::markDirtySurface( const Rect &rect ) {
    const Point &point=getLocation();
    const Size &size=getSize();
    Rect copy;
    copy.x = point.x + rect.x;
    copy.y = point.y + rect.y;
    copy.w = std::min(std::max(size.w-rect.x,0), rect.w);
    copy.h = std::min(std::max(size.h-rect.y,0), rect.h);
    markDirty( copy );
}

Surface *Surface::createSimilar( const Rect &rect ) {
	Surface *tmp = canvas()->createSurface( rect );
	if (tmp) {
		tmp->setVisible( _visible );
		tmp->setZIndex( _zIndex );
		tmp->autoFlush( _autoFlush );
		tmp->setFont( _fontInfo );
		tmp->setColor( _color );
		tmp->setCompositionMode( _mode );
		tmp->flushCompositionMode( _flushMode );
		tmp->setOpacity( getOpacity() );
		tmp->markDirty();
	}
	return tmp;
}

/**
 * @return La fuente usada para dibujar texto.
 */
const FontInfo &Surface::getFont() const {
	return _fontInfo;
}

/**
 * Setea como fuente actual la fuente por defecto.
 */
void Surface::setDefaultFont() {
	setFont( FontInfo() );
}

/**
 * Setea como fuente actual la indicada por @b font
 * @param font La fuente a usar para el dibujado de texto
 * @return true si la fuente pudo ser seteada correctamente, false en caso contrario.
 */
bool Surface::setFont( const FontInfo &font ) {
	bool result = true;
	if (_fontInfo != font) {
		_fontInfo = font;
		if (hackDejaVuFont()) {
			_fontInfo.fixDejaVuFont();
		}
		result = applyFont();
	}
	return result;
}

bool Surface::applyFont() {
	_font = canvas()->loadFont( _fontInfo );
	if (!_font) {
		LWARN("Surface", "Fail to apply font");
		return false;
	}
	return true;
}

bool Surface::hackDejaVuFont() const {
	return true;
}

/****************************** Clip functions ********************************/

/**
 * Setea el clip del @c Surface a la región indicada en @b rect, restringiendo el área de dibujado del @c Surface a dicha región.
 * @param rect La región de clip a setear en el @c Surface
 * @return true si pudo setear la propiedad correctamente, false en caso contrario.
 */
bool Surface::setClip( const Rect &rect ) {
    bool check=true;

    check &= pointInBounds( canvas::Point(rect.x, rect.y) );
    check &= pointInBounds( canvas::Point(rect.x+rect.w-1, rect.y+rect.h-1) );
    check &= ( rect.w>0 && rect.h>0 );

    if (check) {
        setClipImpl( rect );
    } else {
        LWARN("Surface", "setClip fail. Invalid bounds rect(x=%d y=%d w=%d h=%d)", rect.x, rect.y, rect.w, rect.h);
    }
    return check;
}

/****************************** Line functions ********************************/

/**
 * Dibuja una línea desde el punto representado por (@b x1,@b y1) hasta el punto representado por (@b x2,@b y2).
 * @param x1 Coordenada horizontal del primer punto
 * @param y1 Coordenada vertical del primer punto
 * @param x2 Coordenada horizontal del segundo punto
 * @param y2 Coordenada vertical del segundo punto
 * @return true si pudo dibujar la línea correctamente, false en caso contrario.
 */
bool Surface::drawLine( int x1, int y1, int x2, int y2 ) {
    bool check=true;

    check &= pointInBounds( canvas::Point(x1, y1) );
    check &= pointInBounds( canvas::Point(x2, y2) );

    if (check) {
        drawLineImpl( x1, y1, x2, y2 );
        int min_x = std::min(x1, x2);
        int min_y = std::min(y1, y2);
        int max_x = std::max(x1, x2);
        int max_y = std::max(y1, y2);

        markDirtySurface( canvas::Rect( min_x, min_y, max_x-min_x+1, max_y-min_y+1 ) );
    } else {
        LWARN("Surface", "drawLine fail. Invalid bounds (x1=%d y1=%d) (x2=%d y2=%d)", x1, y1, x2, y2);
    }

    return check;
}

/****************************** Rect functions ********************************/

/**
 * Dibuja el contorno del rectángulo indicado por @b rect.
 * @param rect Rectángulo a dibujar.
 * @return true si pudo dibujar el rectángulo correctamente, false en caso contrario.
 */
bool Surface::drawRect( const Rect &rect ) {
    bool check=true;

    check &= pointInBounds( canvas::Point(rect.x, rect.y) );
    check &= pointInBounds( canvas::Point(rect.x+rect.w-1, rect.y+rect.h-1) );
    check &= (rect.w>=0 && rect.h>=0);

    if (check) {
        if (rect.w>0 && rect.h>0) {
            drawRectImpl(rect);
            markDirtySurface( rect );
        }
    } else {
        LWARN("Surface", "drawRect fail. Invalid bounds rect(x=%d y=%d w=%d h=%d)", rect.x, rect.y, rect.w, rect.h);
    }

    return check;
}

/**
 * Dibuja el rectángulo indicado por @b rect pintando su interior.
 * @param rect Rectángulo a dibujar.
 * @return true si pudo dibujar el rectángulo correctamente, false en caso contrario.
 */
bool Surface::fillRect( const Rect &rect ) {
    bool check=true;

    check &= pointInBounds( canvas::Point(rect.x, rect.y) );
    check &= pointInBounds( canvas::Point(rect.x+rect.w-1, rect.y+rect.h-1) );
    check &= (rect.w>=0 && rect.h>=0);

    if (check) {
        if (rect.w>0 && rect.h>0) {
            fillRectImpl( rect );
            markDirtySurface( rect );
        }
    } else {
        LWARN("Surface", "fillRect fail. Invalid bounds rect(x=%d y=%d w=%d h=%d)", rect.x, rect.y, rect.w, rect.h);
    }

    return check;
}

/*************************** RoundRect functions ******************************/

/**
 * Dibuja el contorno del rectángulo indicado por @b rect con bordes redondeados.
 * La curva del borde comienza a la distancia del vértice indicada por @b arcW (horizontal) y @b arcH (vertical).
 * @param rect Rectángulo a dibujar.
 * @param arcW Distancia horizontal desde un vertice hasta el comienzo del borde redondeado.
 * @param arcH Distancia vertical desde un vertice hasta el comienzo del borde redondeado.
 * @return true si pudo dibujar el rectángulo con bordes redondeados correctamente, false en caso contrario.
 */
bool Surface::drawRoundRect( const Rect &rect, int arcW, int arcH ) {
    bool check=true;

    check &= pointInBounds( canvas::Point(rect.x, rect.y) );
    check &= pointInBounds( canvas::Point(rect.x+rect.w-1, rect.y+rect.h-1) );
    check &= (rect.w>=0 && rect.h>=0);
    check &= (arcW > -1 && arcH > -1);

    if (check) {
        if (rect.w>0 && rect.h>0) {
            if (arcW==0 && arcH==0) {
                drawRectImpl( rect );
            } else {
                drawRoundRectImpl(rect, arcW, arcH);
            }
            markDirtySurface( rect );
        }
    } else {
        LWARN("Surface", "drawRoundRect fail. Invalid bounds rect(x=%d y=%d w=%d h=%d, arcW=%d, arcH=%d)", rect.x, rect.y, rect.w, rect.h, arcW, arcH);
    }

    return check;
}

/**
 * Dibuja el rectángulo indicado por @b rect con bordes redondeados pintando su interior.
 * La curva del borde comienza a la distancia del vértice indicada por @b arcW (horizontal) y @b arcH (vertical).
 * @param rect Rectángulo a dibujar.
 * @param arcW Distancia horizontal desde un vertice hasta el comienzo del borde redondeado.
 * @param arcH Distancia vertical desde un vertice hasta el comienzo del borde redondeado.
 * @return true si pudo dibujar el rectángulo con bordes redondeados correctamente, false en caso contrario.
 */
bool Surface::fillRoundRect( const Rect &rect, int arcW, int arcH ) {
    bool check=true;

    check &= pointInBounds( canvas::Point(rect.x, rect.y) );
    check &= pointInBounds( canvas::Point(rect.x+rect.w-1, rect.y+rect.h-1) );
    check &= (rect.w>=0 && rect.h>=0);
    check &= (arcW > -1 && arcH > -1);

    if (check) {
        if (rect.w>0 && rect.h>0) {
            if (arcW==0 && arcH==0) {
                fillRectImpl( rect );
            } else {
                fillRoundRectImpl(rect, arcW, arcH);
            }
            markDirtySurface( rect );
        }
    } else {
	    LWARN("Surface", "fillRoundRect fail. Invalid bounds rect(x=%d y=%d w=%d h=%d, arcW=%d, arcH=%d)",
		    rect.x, rect.y, rect.w, rect.h, arcW, arcH);
    }

    return check;
}

/**************************** Polygon functions ******************************/

/**
 * Dibuja un polígono compuesto por los puntos indicados en @b vértices.
 * Si @b closed es true el polígono se cierra uniendo el último punto con el primero, de lo contrario el polígono se dibuja abierto.
 * @param vertices Collección de vértices que conforman el polígono.
 * @param closed Indica si el último vértice debe unirse con el primero.
 * @return true si pudo dibujar el polígono correctamente, false en caso contrario.
 */
bool Surface::drawPolygon( const std::vector<Point>& vertices, bool closed/*=true*/ ) {
    bool check=true;

    check &= vertices.size()>2;

    canvas::Size size = getSize();
    int minX = size.w;
    int minY = size.h;
    int maxX = 0;
    int maxY = 0;
    for(size_t i=0; i<vertices.size() && check; ++i) {
        const Point& p = vertices[i];
        check &= pointInBounds(p);

        if (p.x < minX) {
            minX = p.x;
        }
        if (p.x > maxX) {
            maxX = p.x;
        }
        if (p.y < minY) {
            minY = p.y;
        }
        if (p.y > maxY) {
            maxY = p.y;
        }
    }

    if (check) {
        drawPolygonImpl( vertices, closed );
        markDirtySurface( canvas::Rect(minX, minY, maxX - minX + 1, maxY - minY + 1) );
    } else {
        LWARN("Surface", "drawPolygon fail. Invalid vertices");
    }
    return check;
}

/**
 * Dibuja un polígono cerrado compuesto por los puntos indicados en @b vértices pintando su interior.
 * @param vertices Collección de vértices que conforman el polígono.
 * @return true si pudo dibujar el polígono correctamente, false en caso contrario.
 */
bool Surface::fillPolygon( const std::vector<Point>& vertices ) {
    bool check=true;

    check &= vertices.size()>2;

    canvas::Size size = getSize();
    int minX = size.w;
    int minY = size.h;
    int maxX = 0;
    int maxY = 0;
    for(size_t i=0; i<vertices.size() && check; ++i) {
        const Point& p = vertices[i];
        check &= pointInBounds(p);

        if (p.x < minX) {
            minX = p.x;
        }
        if (p.x > maxX) {
            maxX = p.x;
        }
        if (p.y < minY) {
            minY = p.y;
        }
        if (p.y > maxY) {
            maxY = p.y;
        }
    }

    if (check) {
        fillPolygonImpl( vertices );
        markDirtySurface( canvas::Rect(minX, minY, maxX - minX + 1, maxY - minY + 1) );
    } else {
        LWARN("Surface", "fillPolygon fail. Invalid vertices");
    }
    return check;
}

/**************************** Ellipse functions ******************************/

/**
 * Dibuja una elipse con centro en el punto indicado en @b center con radio horizontal @b rw y radio vertical @b rh.
 * Si los ángulos indicados por @b angStart y @b angStop son diferentes, solo se dibujará la porción de la elipse ubicada entre dichos ángulos.
 * @param center El centro del polígono.
 * @param rw El radio horizontal.
 * @param rh El radio vertical.
 * @param angStart El ángulo a partir del cual se comienza a dibujar el polígono.
 * @param angStop El ángulo donde se termina de dibujar el polígono.
 * @return true si pudo dibujar la elipse correctamente, false en caso contrario.
 */
bool Surface::drawEllipse( const Point &center, int rw, int rh, int angStart, int angStop ) {
    bool check=true;

    check &= pointInBounds( canvas::Point(center.x - rw, center.y) );
    check &= pointInBounds( canvas::Point(center.x + rw, center.y) );
    check &= pointInBounds( canvas::Point(center.x, center.y - rh) );
    check &= pointInBounds( canvas::Point(center.x, center.y + rh) );
    check &= angStart>=0;
    check &= angStop>=angStart;
    check &= angStop<=360;

    if (check) {
        if (rw>0 && rh>0) {
            drawEllipseImpl( center, rw, rh, angStart, (angStop==0) ? 360 : angStop );
            markDirtySurface(canvas::Rect(center.x - rw, center.y - rh, rw*2+1, rh*2+1));
        }
    } else {
        LWARN("Surface", "drawEllipse fail. Invalid bounds center(x=%d y=%d) rw=%d rh=%d angStart=%d angStop=%d", center.x, center.y, rw, rh, angStart, angStop);
    }
    return check;
}

/**
 * Dibuja una elipse con centro en el punto indicado en @b center con radio horizontal @b rw y radio vertical @b rh pintando su interior.
 * Si los ángulos indicados por @b angStart y @b angStop son diferentes, solo se dibujará la porción de la elipse ubicada entre dichos ángulos.
 * @param center El centro del polígono.
 * @param rw El radio horizontal.
 * @param rh El radio vertical.
 * @param angStart El ángulo a partir del cual se comienza a dibujar el polígono.
 * @param angStop El ángulo donde se termina de dibujar el polígono.
 * @return true si pudo dibujar la elipse correctamente, false en caso contrario.
 */
bool Surface::fillEllipse( const Point &center, int rw, int rh, int angStart, int angStop ) {
    bool check=true;

    check &= pointInBounds( canvas::Point(center.x - rw, center.y) );
    check &= pointInBounds( canvas::Point(center.x + rw, center.y) );
    check &= pointInBounds( canvas::Point(center.x, center.y - rh) );
    check &= pointInBounds( canvas::Point(center.x, center.y + rh) );
    check &= angStart>=0;
    check &= angStop>=angStart;
    check &= angStop<=360;

    if (check) {
        if (rw>0 && rh>0) {
            fillEllipseImpl(center, rw, rh, angStart, (angStop==0) ? 360 : angStop);
            markDirtySurface(canvas::Rect(center.x - rw, center.y - rh, rw*2+1, rh*2+1));
        }
    } else {
        LWARN("Surface", "fillEllipse fail. Invalid bounds center(x=%d y=%d) rw=%d rh=%d angStart=%d angStop=%d", center.x, center.y, rw, rh, angStart, angStop);
    }
    return check;
}

const Font *Surface::font() const {
	return _font;
}

/*
 * @return El ascendente en pixeles, esto es la longitud entre la línea base y la altura máxima de un carácter, teniendo en cuenta la fuente actual.
 */
int Surface::fontAscent() {
	int ascent = 0;
	if (_font || applyFont()) {
		ascent = _font->ascent();
	}
	return ascent;
}

/*
 * @return El descendente en pixeles, esto es la longitud máxima que un carácter puede utilizar por debajo de la línea base, teniendo en cuenta la fuente actual.
 */
int Surface::fontDescent() {
	int descent = 0;
	if (_font || applyFont()) {
		descent = _font->descent();
	}
	return descent;
}

unsigned char Surface::toUnicode( const std::string &string, size_t &pos ) const {
	unsigned char tmp = string[pos];
	if (tmp==0xc2) {
		return string[++pos];
	} else if (tmp==0xc3) {
		return (unsigned char)(string[++pos] + 64);
	} else {
		return tmp;
	}
}

bool Surface::isWhiteSpace( char c ) const {
	return c==' ' || c=='\t' || c=='\n';
}

/**
 * Dibuja el texto indicado en @b text, a partir de las coordenadas indicadas en @b pos.
 * La coordenada pos.y indica la línea base para dibujar el texto, es decir la posición del renglón.
 * Para dibujar el texto es tenido en cuenta el caracteres de escape '\t' (tabulación).
 * @param pos La coordenada donde comienza a dibujarse el texto (línea base).
 * @param text El texto a dibujar.
 * @return true si pudo dibujar el texto correctamente, false en caso contrario.
 */
bool Surface::drawText( const Point &pos, const std::string &text ) {
	int ascent = fontAscent();
	bool check = pointInBounds(Point(pos.x, pos.y-ascent));

	int descent = fontDescent();
	check &= pointInBounds(Point(pos.x, pos.y+descent));

	if (check) {
		if (text.length()>0) {
			Point start(pos.x, pos.y-ascent);
			Size size = drawTextImpl(text, start, Point(getSize().w, pos.y+descent), canvas::wrapAnywhere, 0 );
			markDirtySurface( Rect(start, size) );
		}
	} else {
		LWARN("Surface", "drawText fail. Invalid bounds pos(x=%d y=%d) ascent=%d descent=%d", pos.x, pos.y, ascent, descent);
	}
	return check;
}

/**
 * Dibuja el texto indicado en @b text, dentro de la región indicada en @b rect cortando el texto segun
 * lo retornado por el método wrap del wrapper pasado por parametro.
 * El texto que no entre en la región indicada en @b rect es descartado.
 * Para dibujar el texto son tenidos en cuenta los caracteres de escape '\n' y '\t' (nueva línea y tabulación respectivamente).
 * @param rect La región dentro de la cual se dibujará el texto.
 * @param text El texto a dibujar.
 * @param wrapper El método wrap de este objeto indica como se debe cortar el texto cuando se llega al límite de la región.
 * @param spacing El espacio a agregar al interlineado.
 * @return true si pudo dibujar el texto correctamente, false en caso contrario.
 */
bool Surface::drawText( const Rect &rect, const std::string &text, WrapMode wrapper/*=wrapAnywhere*/, int spacing/*=0*/ ) {
	bool check=true;
	check &= pointInBounds(Point(rect.x, rect.y));
	check &= pointInBounds(Point(std::max(rect.x+rect.w-1,0), std::max(rect.y+rect.h-1,0)));
	check &= spacing >=0;

	if (check) {
		if (text.length()>0) {
			Point start(rect.x, rect.y);
			Point max(rect.x+rect.w, rect.y+rect.h);
			drawTextImpl( text, start, max, wrapper, spacing );
			markDirtySurface( rect );
		}
	} else {
		LWARN("Surface", "drawText fail. Invalid parameters. bounds=(x=%d,y=%d,w=%d,h=%d) spacing=%d", rect.x, rect.y, rect.w, rect.h, spacing);
	}
	return check;
}

#define ADVANCE_LINE \
			maxW = std::max(maxW, pen.x-pos.x); \
			pen.x = pos.x; \
			pen.y += _font->height()+spacing; \
			currentWord = ""; \
			currentPos = 0; \
			lastLineMeasured = true;

Size Surface::drawTextImpl( const std::string &text, const Point &pos, const Point &max, WrapMode wrapper, int spacing ) {
	if (!_font) {
		applyFont();
	}
	DTV_ASSERT(_font);

	Point pen(pos.x, pos.y+_font->ascent());
	int maxW=0;
	std::string currentWord = "";
	unsigned int currentPos = 0;
	bool lastLineMeasured=false;
	int fDescent = _font->descent();

	GlyphRun *gr = createGlyphRun( text, pos );

	size_t length = text.length();
	for (size_t i=0; i<length; i++) {
		unsigned char c = toUnicode(text, i);
		if( c == '\n' ) {
			ADVANCE_LINE;
		}
		else if( c=='\t' || c==' ') {
			const Glyph *glyph = _font->getGlyph(' ');
			DTV_ASSERT(glyph);
			pen.x += (c==' ') ? glyph->advance : glyph->advance * 4;
			currentWord = "";
			currentPos = 0;
		}
		else {
			const Glyph *glyph = _font->getGlyph(c);
			if (glyph) {
				if (pen.x+glyph->advance > max.x) {
					for ( unsigned int j=i; (j<length) && !isWhiteSpace(text[j]); j++) {
						currentWord += text[j];
					}
					unsigned int breakPos = wrapper(currentWord, currentPos);
					if (breakPos<currentPos) {
						int cant = currentPos - breakPos;
						i -= cant+1;
						gr->backward(cant);
						ADVANCE_LINE;
						continue;
					}
					else if (breakPos>currentPos) {
						LWARN("Surface", "Invalid text break position, fail to draw complete text");
						break;
					}
					ADVANCE_LINE;
				}
				if (pen.y+fDescent-1>max.y) {
					break;
				}
				currentPos++;
				currentWord += c;

				gr->processGlyph(glyph, pen);
				pen.x += glyph->advance;
				lastLineMeasured=false;
			} else {
				LWARN("Surface", "Fail to get glyph for code: 0x%X", c);
			}
		}
	}
	maxW = std::max(maxW, pen.x-pos.x);
	if (!lastLineMeasured) {
		pen.y += _font->height();
	}

	renderText(gr, pos);
	DEL(gr);

	return Size(maxW, pen.y-(pos.y+_font->ascent()));
}
#undef ADVANCE_LINE

/**
 * Actualiza @b size con el alto y ancho en pixeles que ocuparía dibujar el texto indicado en @b text, con la fuente actual.
 * @param text El texto a medir.
 * @param[out] size El tamaño de renderizado del texto indicado en @b text
 * @return true si pudo calcular el tamaño correctamente, false en caso contrario.
 */
bool Surface::measureText( const std::string &text, Size &size ) {
	if (!_font) {
		applyFont();
	}

	size.h = 0;
	size.w = 0;
	int currW = 0;
	int nLines = 1;

	size_t len = text.length();
	for (size_t i=0; i<len; i++) {
		char c = toUnicode( text, i );
		if (c=='\n') {
			size.w = std::max( size.w, currW );
			currW = 0;
			nLines++;
		} else if (c=='\t') {
			currW += _font->getGlyph(' ')->advance * 4;
		} else {
			const canvas::Glyph *glyph = _font->getGlyph( c );
			if (glyph) {
				currW += glyph->advance;
			}
		}
	}
	size.w = std::max( size.w, currW );
	size.h = _font->height() * nLines;

	return true;
}

/*********************** Image functions **************************************/

void Surface::drawImage( const std::string &filename ) {
	// Create temporary surface from file
	Surface* img = canvas()->createSurfaceFromPath( filename );

	Size size = getSize();

	const Size &imgSize = img->getSize();

	// determine which axis to scale the image, and the new size
	float h_ratio = (float)imgSize.w / (float)size.w;
	float v_ratio = (float)imgSize.h / (float)size.h;
	if (h_ratio > 1.0 || v_ratio > 1.0) {
		h_ratio = v_ratio = std::max(h_ratio, v_ratio);
	}
	int newImgW = boost::math::iround((float)imgSize.w / h_ratio);
	int newImgH = boost::math::iround((float)imgSize.h / v_ratio);

	// clear the screen
	setColor( Color(0,0,0, 255) );
	fillRect( Rect(0, 0, size.w, size.h) );

	// show the image centered and resized on the screen
	const int x = (size.w - newImgW)/2;
	const int y = (size.h - newImgH)/2;
	scale( Rect(x,y,newImgW,newImgH), img );
	canvas()->destroy( img );
}

/*********************** Pixel manipulation functions *************************/

/**
 * Actualiza @b color con el color del pixel ubicado en la posición indicada en @b pos.
 * @param pos La posición del pixel del cual se desea obtener el color.
 * @param[out] color El color del pixel en la posicion @b pos
 * @return true si pudo obtener el color del pixel correctamente, falso en caso contrario.
 */
bool Surface::getPixelColor( const Point &pos, Color &color ) {
	bool check=true;
	check &= pointInBounds( pos );
	if (check) {
		getPixelColorImpl(pos, color);
	} else {
		LWARN("Surface", "getPixelColor fail. Invalid bounds pos(x=%d y=%d)", pos.x, pos.y);
	}
	return check;
}

/**
 * Setea el color del pixel ubicado en la posición indicada en @b pos con el color indicado en @b color.
 * @param pos  La posición del pixel a pintar.
 * @param color El color con el cual se pintará el pixel.
 * @return true si se seteó el color del pixel correctamente, falso en caso contrario.
 */
bool Surface::setPixelColor( const Point &pos, const Color &color ) {
    bool check=true;

    check &= pointInBounds( pos );
    if (check) {
        setPixelColorImpl(pos, color);
        canvas::Rect rect(pos.x, pos.y, 1, 1);
        markDirtySurface(rect);
    } else {
        LWARN("Surface", "setPixelColor fail. Invalid bounds pos(x=%d y=%d)", pos.x, pos.y);
    }
    return check;
}

/**
 * Si la propiedad de autoFlush es true, efectúa el pintado del canvas, desencadenando la composición de todas las @c Surface
 * y el renderizado en la ventana, en caso contrario el llamado a este método no tiene ningún efecto.
 */
void Surface::flush() {
	if (_autoFlush) {
		canvas()->flush();
	}
}

/**
 * Setea la propiedad de autoFlush con el valor indicado en @b inCanvasFlush.
 * Cuando la propiedad de autoFlush es true, cada vez que se efectúa un flush en el @c Canvas, la Surface sera compuesta con el resto de
 * las Surface que tengan su propiedad de autoFlush en true. Aquellas @c Surface con la propiedad de autoFlush en false son ignoradas.
 * @param inCanvasFlush Indica si el @c Surface debe ser compuesto al ejecutarse Canvas::flush().
 */
void Surface::autoFlush( bool inCanvasFlush ) {
	_autoFlush=inCanvasFlush;
	setAutoFlushImpl( inCanvasFlush );
	markDirty();
}

void Surface::setAutoFlushImpl( bool /*inCanvasFlush*/ ) {
}

/**
 * @return El valor de la propiedad de autoFlush actual.
 */
bool Surface::autoFlush() const {
	return _autoFlush;
}

/**
 * Setea la función de composición utilizada para componer la @c Surface en el canvas al momento de ejecutarse Canvas::flush().
 * @param mode La función de composicion a utilizar al ejecutarse Canvas::flush().
 */
void Surface::flushCompositionMode( composition::mode mode ) {
	_flushMode = mode;
}

/**
 * @return La función de composición actual utilizada para componer la @c Surface en el canvas al ejecutarse Canvas::flush().
 */
composition::mode Surface::flushCompositionMode() {
	return _flushMode;
}

/**
 * Si la propiedad de autoFlush es true, marca el @c Surface completo como sucio, para ser repintado al ejecutarse Canvas::flush().
 */
void Surface::markDirty() {
	Rect dirty = getBounds();
	markDirty( dirty );
}

/**
 * Si la propiedad de autoFlush es true, marca la región del @c Surface indicada en @b rect como sucia, para ser repintada
 * al ejecutar Canvas::flush().
 * @param rect La región del @c Surface que se marcará como sucia.
 */
void Surface::markDirty( const Rect &rect ) {
	if (_autoFlush && !_boundsChanged) {
		canvas()->invalidateRegion( rect );
		_dirty=true;
	}
}

/**
 * Marca la surface como limpia.
 */
void Surface::clearDirty() {
	_dirty = false;
}

/**
 * Marca el Surface como sucio si la propiedad de autoFlush es true y la región ocupada por el @c Surface coincide
 * en algún punto con la región indicada en @b rect.
 * @param rect La región del @c Canvas que será repintada.
 */
void Surface::invalidateRegion( const Rect &rect ) {
	if (_autoFlush && !_dirty) {
		Rect bounds = getBounds();
		if (rect.intersects( bounds )) {
			_dirty=true;
		}
	}
}

/**
 * Actualiza @b blitRect con la intersección entre la región ocupada por el @c Surface y la región indicada en @b dirtyRegion.
 * @param dirtyRegion Región marcada como sucia en el @c Canvas.
 * @param[out] blitRect Intersección entre la región @b dirtyRegion y el @c Surface
 * @return true si la región ocupada por el Surface intersecta al menos en un punto con la región indicada en @b dirtyRegion.
 */
bool Surface::getDirtyRegion( const Rect &dirtyRegion, Rect &blitRect ) {
	Rect bounds = getBounds();
	return dirtyRegion.intersection( bounds, blitRect );
}

/**
 * @return true si el @c Surface esta marcado como sucio, false en caso contrario.
 */
bool Surface::isDirty() const {
	return _dirty;
}

/**
 * Setea el valor de la propiedad ZIndex del @c Surface con el valor indicado en @b zIndex.
 * Las @c Surface con menor ZIndex son compuestas primero en el canvas al realizarse el flush(),
 * quedando por debajo de las surface con mayor zIndex.
 * @param zIndex El valor de Z-Index a setear.
 */
void Surface::setZIndex( int zIndex ) {
	if (_zIndex != zIndex) {
		_zIndex = zIndex;
		setZIndexImpl( zIndex );
		markDirty();
	}
}

void Surface::setZIndexImpl( int /*zIndex*/ ) {
}

/**
 * @return El valor actual de la propiedad ZIndex del @c Surface.
 */
int Surface::getZIndex() const {
	return _zIndex;
}

/**
 * Setea el color utilizado en los métodos de dibujado del @c Surface al valor indicado en @b color.
 * El color inicial es negro 100\% transparente, esto es un valor de RGBA=(0,0,0,0).
 * @param color El color que se usará en los métodos de dibujado.
 */
void Surface::setColor( const Color &color ) {
	_color = color;
	setColorImpl( _color );
}

void Surface::setColorImpl( Color &/*color*/ ) {
}

/**
 * @return El color actual utilizado en los métodos de dibujado del @c Surface.
 */
const Color &Surface::getColor() const {
    return _color;
}

/**
 * Limpia el contenido del @c Surface, seteando todos sus pixeles al valor RGBA=(0,0,0,0).
 * @return true si pudo realizar la operación correctamente, false en caso contrario.
 */
bool Surface::clear() {
	Size size = getSize();
	Rect bounds(0,0,size.w,size.h);
	return clear( bounds );
}

/**
 * Limpia el contenido del @c Surface ubicado en la región indicada en @b rect, seteando todos sus pixeles al valor RGBA=(0,0,0,0).
 * @param rect La region del @c Surface que será limpiada
 * @return true si pudo realizar la operación correctamente, false en caso contrario.
 */
bool Surface::clear( const Rect &rect ) {
	bool check=true;

	check &= pointInBounds( Point(rect.x, rect.y) );
	check &= pointInBounds( Point(rect.x+rect.w-1, rect.y+rect.h-1) );
	check &= (rect.w>0 && rect.h>0);

	if (check) {
		composition::mode oldMode = getCompositionMode();
		setCompositionMode(composition::clear);
		fillRectImpl(rect);
		setCompositionMode(oldMode);
		markDirtySurface( rect );
	} else {
		LWARN("Surface", "clear fail. Invalid bounds rect(x=%d y=%d w=%d h=%d)", rect.x, rect.y, rect.w, rect.h);
	}
	return check;
}

/**
 * Setea la propiedad de visibilidad del @c Surface al valor indicado en @b visible.
 * Si la propiedad de visibilidad es true, el @c Surface es compuesto al realizarse el flush() del canvas,
 * en caso contrario el Surface no es compuesto.
 * @param visible Booleano que indica si el @c Surface es visible o no
 */
void Surface::setVisible( bool visible ) {
	if (visible != _visible) {
		setVisibleImpl( visible );
		markDirty();
	}
	_visible = visible;
}

void Surface::setVisibleImpl( bool /*visible*/ ) {
}

/**
 * @return El valor actual de la propiedad de visibilidad del @c Surface.
 */
bool Surface::isVisible() const {
	return _visible;
}

/**
 * Setea la posición del @c Surface al punto indicado en @b point.
 * @param point Posición de la esquina superior izquierda del @c Surface relativo al @c Canvas
 */
void Surface::setLocation( const Point &point ) {
	markDirty();
	_pos.x = point.x;
	_pos.y = point.y;
	_boundsChanged = true;
	setLocationImpl( point );
}

void Surface::setLocationImpl( const Point &/*point*/ ) {
}

/**
 * @return La posición actual del @c Surface relativo al @c Canvas.
 */
const Point &Surface::getLocation() const {
	return _pos;
}

/**
 * @return La región ocupada por el @c Surface relativa al @c Canvas.
 */
Rect Surface::getBounds() const {
	Point pos = getLocation();
	Size size = getSize();
	Rect bounds(pos.x, pos.y, size.w, size.h);
	return bounds;
}

/**
 * Compara el contenido actual del @c Surface con el contenido de la imagen indicada en @b file.
 * param file Ruta absoluta de la imagen a comparar.
 * @return true si las imágenes coinciden, false en caso contrario.
 */
bool Surface::equalsImage( const std::string &file ) {
	Surface* image = canvas()->createSurfaceFromPath( file );

	if (!image) {
		return false;
	}
	Size s_size = getSize();
	Size i_size = image->getSize();

	if (s_size.w != i_size.w || s_size.h != i_size.h) {
		canvas()->destroy( image );
		return false;
	} else {
		for (int i = 0; i < s_size.w; ++i) {
			for (int j = 0; j < s_size.h; ++j) {
				if (!comparePixels(Point(i,j), image)) {
					canvas()->destroy( image );
					return false;
				}
			}
		}
	}
	canvas()->destroy( image );
	return true;
}

#define THRESHOLD 20
bool Surface::comparePixels( const Point &pos, Surface *image ) {
	util::DWORD p1, p2;
	bool check=true;

	p1 = getPixel(pos);
	p2 = image->getPixel(pos);
	if (p1!=p2) {
		canvas::Color c1, c2;
		getPixelColor(pos, c1);
		image->getPixelColor(pos, c2);
		if (!c1.equals(c2, THRESHOLD ) ) {
			LINFO("Surface", "Difference found in pixel (%d, %d): s_color=(%d,%d,%d,%d), i_color=(%d,%d,%d,%d).",
				pos.x, pos.y, c1.r, c1.g, c1.b, c1.alpha, c2.r, c2.g, c2.b, c2.alpha );
			check=false;
		}
	}
	return check;
}

/**
 * @param point Coordenada a chequear si se encuentra dentro del área del @c Surface
 * @return true si la coordenada indicada en @b point esta ubicada dentro de la región ocupada por el @c Surface, false en caso contrario.
 */
bool Surface::pointInBounds( const Point &point ) const {
	Size size = getSize();
	bool check_x = point.x > -1 && point.x < size.w;
	bool check_y = point.y > -1 && point.y < size.h;
	return check_x && check_y;
}

/**
 * Compone @b srcSurface en el punto indicado en @b targetPoint, usando la función de composición indicada con setCompositionMode()
 * @param targetPoint Coordenada relativa al @c Surface donde será compuesto @b srcSurface.
 * @param srcSurface El @c Surface a componer.
 * @return true si pudo componer el @c Surface correctamente, false en caso contrario.
 */
bool Surface::blit( const Point &targetPoint, Surface *srcSurface ) {
	const Size &s=srcSurface->getSize();
	Rect srcRect(0,0,s.w,s.h);
	return blit( targetPoint, srcSurface, srcRect );
}

/**
 * Compone la región de @b srcSurface indicada en @b sourceRect en el punto indicado en @b targetPoint,
 * usando la función de composición indicada con setCompositionMode()
 * @param targetPoint Coordenada relativa al @c Surface donde será compuesto @b srcSurface.
 * @param srcSurface El @c Surface a componer.
 * @param source La región de @b srcSurface a componer.
 * @return true si pudo componer la Surface correctamente, false en caso contrario.
 */
bool Surface::blit( const Point &targetPoint, Surface *srcSurface, const Rect &source ) {
	bool check=true;

	check &= (source.w>=0 && source.h>=0);
	check &= srcSurface->pointInBounds( canvas::Point(source.x, source.y) );

	int source_x2 = source.x+(source.w?source.w-1:0);
	int source_y2 = source.y+(source.h?source.h-1:0);
	check &= srcSurface->pointInBounds( canvas::Point(source_x2, source_y2) );

	if (check) {
		blitImpl(targetPoint, srcSurface, source);
		markDirtySurface( Rect( targetPoint.x, targetPoint.y, source.w, source.h ) );
	} else {
		const Rect &bounds=getBounds();
		LWARN("Surface", "blit fail. Invalid bounds: bounds=(%d,%d,%d,%d), targetPoint=(x=%d y=%d) source=(x=%d y=%d w=%d h=%d)",
			bounds.x, bounds.y, bounds.w, bounds.h,
			targetPoint.x, targetPoint.y,
			source.x, source.y, source.w, source.h);
	}
	return check;
}

/**
 * Compone @b srcSurface en la región indicada en @b targetRect escalandola de ser necesario y usando la función de composición indicada con
 * setCompositionMode(). @b flipw y @b fliph indican si además, srcSurface debe ser espejada horizontal y/o verticalmente al momento de componerla.
 * @param targetRect La región del @c Surface donde será compuesto @b srcSurface.
 * @param srcSurface El @c Surface a componer.
 * @param flipw Indica si @b srcSurface debe ser espejado horizontalmente.
 * @param fliph Indica si @b srcSurface debe ser espejado verticalmente.
 * @return true si pudo componer la Surface correctamente, false en caso contrario.
 */
bool Surface::scale( const Rect &targetRect, Surface *srcSurface, bool flipw /*=false*/, bool fliph /*=false*/ ) {
	const Size &s=srcSurface->getSize();
	Rect srcRect(0,0,s.w,s.h);
	return scale( targetRect, srcSurface, srcRect, flipw, fliph );
}

/**
 * Compone la región de @b srcSurface indicada en @b sourceRect, en la región indicada en @b targetRect escalandola de ser necesario
 * y usando la función de composición indicada con setCompositionMode().
 * @b flipw y @b fliph indican si además, srcSurface debe ser espejada horizontal y/o verticalmente al momento de componerla.
 * @param targetRect La región del @c Surface donde será compuesto @b srcSurface.
 * @param srcSurface El @c Surface a componer.
 * @param source La región de @b srcSurface a componer.
 * @param flipw Indica si @b srcSurface debe ser espejado horizontalmente.
 * @param fliph Indica si @b srcSurface debe ser espejado verticalmente.
 * @return true si pudo componer la Surface correctamente, false en caso contrario.
 */
bool Surface::scale( const Rect &targetRect, Surface *srcSurface, const Rect &source, bool flipw /*=false*/, bool fliph /*=false*/ ) {
	bool check=true;

	check &= (source.w>0 && source.h>0);
	check &= srcSurface->pointInBounds( canvas::Point(source.x, source.y) );
	
	int source_x2 = source.x+(source.w?source.w-1:0);
	int source_y2 = source.y+(source.h?source.h-1:0);
	check &= srcSurface->pointInBounds( canvas::Point(source_x2, source_y2) );

	check &= (targetRect.w>0 && targetRect.h>0);
	check &= pointInBounds( canvas::Point(targetRect.x, targetRect.y) );

	source_x2 = targetRect.x+(targetRect.w?targetRect.w-1:0);
	source_y2 = targetRect.y+(targetRect.h?targetRect.h-1:0);
	check &= pointInBounds( canvas::Point(source_x2, source_y2) );

	if (check) {
		scaleImpl( targetRect, srcSurface, source, flipw, fliph );
		markDirtySurface( targetRect );
	} else {
		const Rect &bounds=getBounds();
		LWARN("Surface", "scale fail. Invalid bounds: bounds=(%d,%d,%d,%d), target=(%d,%d,%d,%d) source=(%d,%d,%d,%d)",
			bounds.x, bounds.y, bounds.w, bounds.h,
			targetRect.x, targetRect.y, targetRect.w, targetRect.h,
			source.x, source.y, source.w, source.h);
	}
	return check;
}

/**
 * Rota la surface según los grados indicados en @b degrees (@b degrees debe ser un múltiplo de 90).
 * @param degrees Los grados a rotar el @c Surface.
 * @return Una nueva Surface, similar a la actual pero con el contenido rotado.
 */
Surface *Surface::rotate( int degrees ) {
	Surface *newSurface = NULL;
	if ((degrees%90) != 0) {
		LWARN("Surface", "rotate fail. Degrees must be a multiple of 90 (degrees=%d)", degrees);
	} else {
		while (degrees<0) {
			degrees = degrees + 360;
		}
		newSurface = rotateImpl( degrees%360 );
	}
	return newSurface;
}

/**
 * Cambia el tamaño del @c Surface a @b size. Si @b scaleContent es true el contenido actual es escalado al nuevo tamaño,
 * en caso contrario el contenido actual se descarta.
 * @param size El nuevo tamaño.
 * @param scaleContent Indica si el contenido actual debe ser escalado al nuevo tamaño o ser descartado.
 * @return true si pudo cambiar el tamaño correctamente, false en caso contrario.
 */
bool Surface::resize( const Size &size, bool scaleContent/*=false*/ ) {
	bool check = size.w>0 && size.h>0;
	if (check) {
		markDirty();
		_boundsChanged = true;
		resizeImpl(size, scaleContent);
	} else {
		LWARN("Surface", "resize fail. New size must be greater than 0. newSize=(%d,%d)", size.w, size.h);
	}
	return check;
}

/**
 * Setea la función de composición a utilizar en métodos de composición como blit() y scale().
 * @param mode La función de composición a utilizar.
 */
void Surface::setCompositionMode( composition::mode mode ) {
	switch (mode) {
		case composition::source_over:
		case composition::source:
		case composition::clear:
			_mode = mode;
			setCompositionModeImpl( mode );
			break;
		default:
			LWARN("Surface", "setCompositionMode fail. Composition mode not suported!");
			break;
	}
}

/**
 * @return La función de composición actual utilizada en métodos de composición como blit() y scale().
 */
composition::mode Surface::getCompositionMode() {
	return _mode;
}

bool Surface::boundsChanged() const {
	return _boundsChanged;
}

void Surface::cleanBoundsChanged() {
	_boundsChanged = false;
}

util::DWORD *Surface::pixels() {
	return NULL;
}

}
