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

#include "types.h"
#include "point.h"
#include "color.h"
#include "fontinfo.h"
#include "size.h"
#include "rect.h"
#include <vector>
#include <string>

namespace canvas {

class Font;
class GlyphRun;

/**
 * Representa un superficie donde se pueden realizar dibujos de figuras y texto en 2D y renderizado de imágenes.
 * Cada una de las instancias creadas son compuestas en un Canvas al llamarse al método Canvas::flush()
 */
class Surface {
public:
	explicit Surface( const Point &pos );
	virtual ~Surface();

	static void destroy( Surface *s );

	const FontInfo &getFont() const;
	void setDefaultFont();
	bool setFont( const FontInfo &font );

	/**
	 * Actualiza @b rect con los valores correspondientes a la región de clip del @c Surface.
	 * @param[out] rect Región de clip del @c Surface
	 * @return true si pudo obtener el clip correctamente, false en caso contrario.
	 */
	virtual bool getClip( Rect &rect )=0;
	bool setClip( const Rect &rect );

	bool drawLine( int x1, int y1, int x2, int y2 );

	bool drawRect( const Rect &rect );
	bool fillRect( const Rect &rect );

	bool drawRoundRect( const Rect &rect, int arcW, int arcH );
	bool fillRoundRect( const Rect &rect, int arcW, int arcH );

	bool drawPolygon( const std::vector<Point>& vertices, bool closed=true );
	bool fillPolygon( const std::vector<Point>& vertices );

	bool drawEllipse( const Point &center, int rw, int rh, int angStart, int angStop );
	bool fillEllipse( const Point &center, int rw, int rh, int angStart, int angStop );

	//	Note: The y-position is used as the baseline of the font.
	bool drawText( const Point &pos, const std::string &text );

	//	Note: The y-coordinate of rectangle is used as the top of the font.
	bool drawText( const Rect &rect, const std::string &text, WrapMode wrapper=wrapAnywhere, int spacing=0 );

	bool measureText( const std::string &text, Size &size );

	int fontAscent();
	int fontDescent();

	void drawImage( const std::string &filename );

	bool blit( const Point &targetPoint, Surface *srcSurface );
	bool blit( const Point &targetPoint, Surface *srcSurface, const Rect &sourceRect );

	bool scale( const Rect &targetRect, Surface *srcSurface, bool flipw=false, bool fliph=false );
	bool scale( const Rect &targetRect, Surface *srcSurface, const Rect &sourceRect, bool flipw=false, bool fliph=false );

	Surface *rotate( int degrees );

	bool resize( const Size &size, bool scaleContent=false );

	void setCompositionMode( composition::mode mode );
	composition::mode getCompositionMode();

	bool getPixelColor( const Point &pos, Color &color );
	bool setPixelColor( const Point &pos, const Color &color );
	virtual util::DWORD *pixels();

	/**
	 * Exporta el contenido actual del Surface a un archivo png.
	 * @param file El nombre del archivo a generar.
	 * @return true si pudo generar el archivo correctamente, false en caso contrario.
	 */
	virtual bool saveAsImage( const std::string &file )=0;
	virtual bool equalsImage( const std::string &file );

	void flush();
	void autoFlush( bool inCanvasFlush );
	bool autoFlush() const;

	void flushCompositionMode( composition::mode mode );
	composition::mode flushCompositionMode();

	void markDirty();
	void markDirty( const Rect &rect );
	bool isDirty() const;
	void clearDirty();
	bool getDirtyRegion( const Rect &dirtyRegion, Rect &blitRect );
	void invalidateRegion( const Rect &rect );

	void setZIndex( int zIndex );
	int getZIndex() const;

	/*
	 * Setea la opacidad del @c Surface con el valor indicado en @b alpha. El valor de @b alpha debe estar en el rango de 0 a 255,
	 * donde 0 indica complemtamente transparente y 255 indica completamente opaca.
	 * Este valor es utilizado en los métodos de composición como blit() y scale() y es compuesto con el valor del alpha de cada pixel
	 * según la función de composición seteada con setCompositionMode(). La opacidad inicial del Surface es 255.
	 * @param alpha La opacidad a setear en el @c Surface.
	 * @return true si pudo setear la opacidad correctamente, false en caso contrario
	 */
	virtual bool setOpacity( util::BYTE alpha )=0;

	/*
	 * @return el valor de opacidad actual del @c Surface.
	 */
	virtual util::BYTE getOpacity() const=0;

	void setColor( const Color &color );
	const Color &getColor() const;

	bool clear();
	bool clear( const Rect &rect );

	void setVisible( bool visible );
	bool isVisible() const;

	Rect getBounds() const;

	void setLocation( const Point &point );
	const Point &getLocation() const;

	/**
	 * @return El tamaño actual del @c Surface.
	 */
	virtual Size getSize() const=0;

	/**
	 * @return El @c Canvas a partir del cual fue creado el @c Surface.
	 */
	virtual Canvas *canvas() const=0;
	bool pointInBounds( const Point &point ) const;

	bool boundsChanged() const;
	void cleanBoundsChanged();

protected:
	virtual Surface *createSimilar( const Rect &rect );

	void markDirtySurface( const Rect &rect );
	bool applyFont();
	virtual bool hackDejaVuFont() const;
	virtual void setLocationImpl( const Point &point );
	virtual void setVisibleImpl( bool visible );
	virtual void setZIndexImpl( int zIndex );
	virtual void setColorImpl( Color &color );
	virtual void setAutoFlushImpl( bool inCanvasFlush );
	virtual void setClipImpl( const Rect &rect )=0;

	virtual void drawLineImpl( int x1, int y1, int x2, int y2 )=0;

	virtual void drawRectImpl( const Rect &rect )=0;
	virtual void fillRectImpl( const Rect &rect )=0;

	virtual void drawRoundRectImpl( const Rect &rect, int arcW, int arcH )=0;
	virtual void fillRoundRectImpl( const Rect &rect, int arcW, int arcH )=0;

	virtual void drawPolygonImpl( const std::vector<Point>& vertices, bool closed )=0;
	virtual void fillPolygonImpl( const std::vector<Point>& vertices )=0;

	virtual void drawEllipseImpl( const Point &center, int rw, int rh, int angStart, int angStop )=0;
	virtual void fillEllipseImpl( const Point &center, int rw, int rh, int angStart, int angStop )=0;
	
	virtual void blitImpl( const Point &target, Surface *srcSurface, const Rect &source )=0;
	virtual void scaleImpl( const Rect &targetRect, Surface *srcSurface, const Rect &sourceRect, bool flipw=false, bool fliph=false )=0;

	virtual Surface *rotateImpl( int degrees )=0;
	virtual void resizeImpl( const Size &size, bool scaleContent=false )=0;

	virtual void getPixelColorImpl( const Point &pos, Color &color )=0;
	virtual void setPixelColorImpl( const Point &pos, const Color &color )=0;

	const Font *font() const;

	// DrawText implementation
	Size drawTextImpl( const std::string &text, const Point &pos, const Point &max, WrapMode wrapper, int spacing );
	virtual GlyphRun *createGlyphRun( const std::string &text, const Point &pos )=0;
	virtual void renderText( GlyphRun *glyphRun, const Point &pos )=0;

	// drawText helpers
	unsigned char toUnicode( const std::string &string, size_t &pos ) const;
	bool isWhiteSpace( char c ) const;

	virtual bool comparePixels( const Point &pos, Surface* image );
	virtual util::DWORD getPixel( const Point &pos )=0;

	virtual void setCompositionModeImpl( composition::mode /*mode*/ ) {}

private:
	int _zIndex;
	bool _visible;
	bool _dirty;
	bool _autoFlush;
	bool _boundsChanged;
	Color _color;
	Point _pos;
	FontInfo _fontInfo;
	Font *_font;
	composition::mode _mode;
	composition::mode _flushMode;

	Surface();
};

}
