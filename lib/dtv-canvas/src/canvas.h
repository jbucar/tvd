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
#include "rect.h"
#include "size.h"
#include "fontinfo.h"
#include <string>
#include <vector>
#include <list>
#include <string>

namespace canvas {

class Window;
class FontManager;
class Font;

/**
 * Clase abstracta que representa un lienzo. El mismo permite componer varias instancias de la clase 
 * Surface para obtener el resultado final que sera renderizado en la instancia de la clase Window pasada 
 * como parámetro al inicializar el canvas.
 */
class Canvas {
public:
	Canvas();
	virtual ~Canvas();

	// Initialization
	bool initialize( Window *win );
	void finalize();

	// Surface creation
	Surface *createSurface( const Size &size );
	Surface *createSurface( const Rect &rect );
	Surface *createSurfaceFromPath( const std::string &file );
	Surface *createSurface( ImageData *img );
	void destroy( SurfacePtr &surface );

	// Font methods
	void addFontDirectory( const std::string &dir );
	void clearFontDirectories();
	Font *loadFont( const FontInfo &font );

	// Getters
	const Size &size() const;
	
	/**
	 * @return el nombre de la implementación (subclase)
	 */
	virtual std::string name()=0;

	// Flush
	void flush();
	void beginDraw();
	void endDraw();
	void invalidateRegion( const Rect &rect );
	const std::vector<Rect> &dirtyRegions() const;

	// Store
	bool saveAsImage( const std::string &file );
	bool equalsImage( const std::string &file );

	void reset();
	static Canvas *create();

protected:
	virtual bool init();
	virtual void fin();

	virtual Surface *createSurfaceImpl( ImageData *img )=0;
	virtual Surface *createSurfaceImpl( const Rect &rect )=0;
	virtual Surface *createSurfaceImpl( const std::string &file )=0;

	void flushImpl( Surface *layer );
	Surface *addSurface( Surface *surface );
	void addDirtyRegion( const Rect &rect );

	// Getters
	Window *win() const;

	virtual Font *createFont( const std::string &filename, size_t size )=0;

private:
	Size _size;
	FontManager *_fontManager;
	std::vector<Font*> _fonts;
	Window *_win;
	int _inTransaction;
	std::vector<Rect> _dirtyRegions;
	std::vector<Surface *> _surfaces;
	int _nextGid;
	double _joinPercentage;

	bool _showBlittedRegions;
	int _nTrackedFlush;
	Surface *_blittedRegionsSurface;
	std::vector<Color> _blittedRegionsColors;
	std::list< std::vector<Rect> > _previousDirtyRegions;
};

}

