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

#include "canvas.h"
#include "surface.h"
#include "window.h"
#include "fontmanager.h"
#include "text/font.h"
#include "impl/dummy/canvas.h"
#include "generated/config.h"
#if CANVAS_2D_USE_CAIRO
#	include "impl/cairo/canvas.h"
#endif
#if CANVAS_2D_USE_SKIA
#	include "impl/skia/canvas.h"
#endif
#if CANVAS_2D_USE_GL
#	include "impl/gl/canvas.h"
#endif
#include <util/cfg/configregistrator.h>
#include <util/mcr.h>
#include <util/log.h>
#include <util/assert.h>
#include <boost/foreach.hpp>
#include <boost/math/special_functions/round.hpp>
#include <limits.h>

#define LAYER(code)	  \
	{ \
		Window *w = win(); \
		Surface *layer = w->lockLayer(); \
		DTV_ASSERT(layer); \
		if (!_inTransaction) { flushImpl( layer ); } \
		code \
		w->unlockLayer( layer ); \
	}


namespace canvas {

REGISTER_INIT_CONFIG( gui_canvas ) {
	root().addNode( "canvas" )
#if CANVAS_2D_USE_CAIRO
	.addValue( "use", "Canvas to instance", std::string("cairo") )
#elif CANVAS_2D_USE_SKIA
	.addValue( "use", "Canvas to instance", std::string("skia") )
#elif CANVAS_2D_USE_GL
	.addValue( "use", "Canvas to instance", std::string("gl") )
#else
	.addValue( "use", "Canvas to instance", std::string("dummy") )
#endif

	.addValue( "joinPercentage", "Extra area that can be used when joining dirty regions", 0.2 )

	.addNode( "showBlits" )
		.addValue( "enabled", "Delimit blitted regions", false )
		.addValue( "track", "Number of flush to trak (max=5)", 5 );

	root()("canvas").addNode( "size" )
		.addValue( "width", "Canvas width", 720 )
		.addValue( "height", "Canvas height", 576 );
}

/**
 * Crea y retorna una instancia de Canvas usando la implementación seteada en el árbol de
 * configuración (ver módulo dtv-util).
 */
Canvas *Canvas::create() {
	const std::string &use = util::cfg::getValue<std::string>("gui.canvas.use");

	LINFO("Canvas", "Using canvas: %s", use.c_str());

#if CANVAS_2D_USE_CAIRO
	if (use == "cairo") {
		return new cairo::Canvas();
	}
#endif
#if CANVAS_2D_USE_SKIA
	if (use == "skia") {
		return new skia::Canvas();
	}
#endif
#if CANVAS_2D_USE_GL
	if (use == "gl") {
		return new gl::Canvas();
	}
#endif
	return new dummy::Canvas();
}

/**
 * Constructor base. Luego de crear una instancia es necesario llamar al método initialize() para poder utilizar la misma.
 */
Canvas::Canvas()
{
	_win = NULL;
	_fontManager = NULL;
	_inTransaction = 0;
	_nextGid = 0;
	_showBlittedRegions = false;
	_blittedRegionsSurface = NULL;
	_joinPercentage  = util::cfg::getValue<double>("gui.canvas.joinPercentage");
}

/**
 * Destructor base. Antes de destruir una instancia es necesario llamar al método finalize().
 */
Canvas::~Canvas()
{
	if (_surfaces.size()) {
		LWARN("Canvas", "not all surfaces destroyed: size=%d", _surfaces.size() );
	}
	DTV_ASSERT(!_win);
}

/**
 * Inicializa el estado del canvas. Este método debe ser llamado antes de llamar cualquier otro método.
 * @param win Instancia de la clase Window.
 * @return true en caso de que la inicialización se halla realizado correctamente, false en caso contrario.
 */
bool Canvas::initialize( Window *win ) {
	DTV_ASSERT(win);

	//	Get canvas size from config
	util::cfg::PropertyNode &sizeNode = util::cfg::get("gui.canvas.size");
	_size.w = sizeNode.get<int>( "width" );
	_size.h = sizeNode.get<int>( "height" );
	_dirtyRegions.push_back(Rect(Point(0,0),_size));

	//	Setup window
	if (!win->initCanvasLayer( this )) {
		LERROR( "Canvas", "Cannot setup canvas layer" );
		return false;
	}
	_win = win;
	_fontManager = FontManager::create();
	if (!_fontManager->initialize()) {
		LERROR("Canvas", "Cannot initialize font manager");
		return false;
	}

	if (!init()) {
		LERROR( "Canvas", "Cannot initialize canvas" );
		return false;
	}

	//	Create debug surfaces
	_showBlittedRegions = util::cfg::getValue<bool>("gui.canvas.showBlits.enabled");
	_nTrackedFlush = util::cfg::getValue<int>("gui.canvas.showBlits.track");
	if (_nTrackedFlush<1 || _nTrackedFlush>5) {
		LERROR( "Canvas", "Invalid showBlits.track value. Must be between 1 and 5" );
		return false;
	}
	if (_showBlittedRegions) {
		_blittedRegionsSurface = createSurface(size());
		_blittedRegionsSurface->setZIndex(INT_MAX);
		_blittedRegionsColors.push_back(Color(255,0,0));
		_blittedRegionsColors.push_back(Color(255,255,0));
		_blittedRegionsColors.push_back(Color(0,255,0));
		_blittedRegionsColors.push_back(Color(0,255,255));
		_blittedRegionsColors.push_back(Color(0,0,255));
	}

	return true;
}

/**
 * Finaliza el estado del canvas. Este método debe ser llamado antes de destruir el objeto.
 */
void Canvas::finalize() {
	fin();

	//	Cleanup layer
	_win->finCanvasLayer( this );
	_win = NULL;

	CLEAN_ALL( Font*, _fonts );

	_fontManager->finalize();
	DEL(_fontManager);

	if (_showBlittedRegions) {
		destroy(_blittedRegionsSurface);
	}
}

bool Canvas::init() {
	return true;
}

void Canvas::fin() {
}

/**
 * Crea una Surface del tamaño indicado en size, en la posición (0,0).
 */
Surface *Canvas::createSurface( const Size &size ) {
	Rect rect(0,0,size.w,size.h);
	return createSurface( rect );
}

/**
 * Crea una Surface del tamaño y en la posición indicados en rect.
 */
Surface *Canvas::createSurface( const Rect &rect ) {
	try {
		return addSurface( createSurfaceImpl( rect ) );
	} catch (...) {}
	return NULL;
}

/**
 * Crea una Surface a partir del archivo de imagen indicado en file, en la posición (0,0).
 */
Surface *Canvas::createSurfaceFromPath( const std::string &file ) {
	try {
		return addSurface( createSurfaceImpl( file ) );
	} catch (...) {}
	return NULL;
}

/**
 * Crea una Surface a partir de la imagen en memoria img, en la posición (0,0).
 */
Surface *Canvas::createSurface( ImageData *img ) {
	try {
		return createSurfaceImpl( img );
	} catch (...) {}
	return NULL;
}

/**
 * @return El tamaño del canvas en pixeles.
 */
const Size &Canvas::size() const {
	return _size;
}

Surface *Canvas::addSurface( Surface *surface ) {
	LTRACE("Canvas", "Add surface: surface=%p", surface );
	DTV_ASSERT(surface);
	_surfaces.push_back( surface );
	return surface;
}

/**
 * Destruye el objeto @b surface.
 * @param surface El @c Surface que será destruido.
 */
void Canvas::destroy( SurfacePtr &surface ) {
	LTRACE("Canvas", "Destroy surface: surface=%p", surface );
	DTV_ASSERT(surface);
	std::vector<Surface *>::iterator it=std::find(
		_surfaces.begin(), _surfaces.end(), surface );
	if (it != _surfaces.end()) {
		(*it)->markDirty();
		delete (*it);
		_surfaces.erase( it );
	} else {
		LERROR( "Canvas", "Surface to destroy not found: surface=%p", surface );
		DTV_ASSERT(false);
	}
	surface = NULL;
}

struct SortSurfaces {
	bool operator()( Surface *s1, Surface *s2 ) const {
		return s1->getZIndex() < s2->getZIndex();
	}
};

/**
 * @return La lista de regiones invalidadas que serán repintadas al llamarse al método flush().
 */
const std::vector<Rect> &Canvas::dirtyRegions() const {
	return _dirtyRegions;
}

void Canvas::addDirtyRegion( const Rect &rect ) {
	LTRACE("Canvas", "addDirtyRegion: rect(%d,%d,%d,%d)", rect.x, rect.y, rect.w, rect.h);
	_dirtyRegions.push_back( rect );

	//	Invalidate surfaces
	BOOST_FOREACH( Surface *surface, _surfaces ) {
		surface->invalidateRegion( rect );
	}
}

/**
 * Invalida la región indicada por rect para ser repintada. Al ejecutarse el método flush() todas las surfaces
 * marcadas como visibles, cuyos límites intersectan con dicha región, serán repintadas.
 * @param rect La región del @c Canvas a invalidar
 */
void Canvas::invalidateRegion( const Rect &rect ) {
	LTRACE("Canvas", "invalidateRegion: rect(%d,%d,%d,%d)", rect.x, rect.y, rect.w, rect.h);

	if (!rect.isEmpty()) {
		std::vector<Rect> toErase;
		std::vector<Rect> toAdd;

		BOOST_FOREACH( const Rect &dirtyRegion, _dirtyRegions ) {
			if (dirtyRegion.includes( rect )) {
				// If rect is already included in any dirty region return
				return;
			} else if ( rect.includes( dirtyRegion ) ) {
				// If rect includes a region, mark to remove it
				toErase.push_back( dirtyRegion );
			} else {
				Rect composedRect = rect.encompass(dirtyRegion);
				Rect intersection;
				rect.intersection(dirtyRegion, intersection);
				int extraJoinedArea = composedRect.area()-rect.area()-dirtyRegion.area()+intersection.area();
				int maxExtraJoinedArea = boost::math::iround(composedRect.area() * _joinPercentage);

				if (extraJoinedArea<=maxExtraJoinedArea) {
					// If the resulting join area of rect and dirtyRegion
					// is smaller enough, add the new composed rect
					toErase.push_back(dirtyRegion);
					toAdd.push_back(composedRect);
				} else if (intersection.area()>0) {
					// Substract dirtyRegion from rect and add resulting
					// rectangles that don't intersect with dirtyRegion
					if (rect.goesInto(dirtyRegion)) {
						toAdd.push_back(rect.cut(intersection));
					} else if (dirtyRegion.goesInto(rect)) {
						toErase.push_back(dirtyRegion);
						toAdd.push_back(dirtyRegion.cut(intersection));
						toAdd.push_back(rect);
					}
				}
			}
		}

		// Remove old regions
		BOOST_FOREACH( const Rect region, toErase ) {
			std::vector<Rect>::iterator it=std::find( _dirtyRegions.begin(), _dirtyRegions.end(), region );
			if (it != _dirtyRegions.end()) {
				_dirtyRegions.erase( it );
			}
		}

		if (toAdd.size()>0) {
			// If the original region has changed invalidate new regions
			BOOST_FOREACH( const Rect& r, toAdd ) {
				invalidateRegion(r);
			}
		} else {
			// else add the original region
			addDirtyRegion( rect );
		}
	}
}

/**
 * Compone todas las instancias de Surface creadas marcadas como visibles, generando la imagen
 * final que sera renderizada en la ventana.
 */
void Canvas::flush() {
	if (!_inTransaction) {
		Window *w = win();
		Surface *layer = w->lockLayer();
		DTV_ASSERT(layer);
		flushImpl( layer );
		w->unlockLayer( layer );
	}
}

/**
 * Inicia una transacción para dibujar. Pueden encadenarse múltiples llamados a beginDraw(). Todos los llamados a flush()
 * que se hagan entre un llamado a beginDraw() y el llamado a endDraw() correspondiente, serán omitidos.
 */
void Canvas::beginDraw() {
	LTRACE("Canvas", "Begin draw: transaction=%d", _inTransaction );
	_inTransaction++;
}

/**
 * Finaliza una transacción activa y en caso de no existir más transacciones activas, se ejecuta el método flush().
 */
void Canvas::endDraw() {
	LTRACE("Canvas", "End draw: transaction=%d", _inTransaction );
	if (_inTransaction > 0) {
		_inTransaction--;
		flush();
	}
}

void Canvas::flushImpl( Surface *layer ) {
	DTV_ASSERT(layer->getCompositionMode() == composition::source_over);
	std::vector<Surface *> toBlit;

	if (_showBlittedRegions && !_dirtyRegions.empty()) {
		_previousDirtyRegions.push_back(std::vector<Rect>(_dirtyRegions.begin(), _dirtyRegions.end()));
		_blittedRegionsSurface->autoFlush(false);
		if (_previousDirtyRegions.size()>(unsigned)_nTrackedFlush) {
			_previousDirtyRegions.pop_front();
		}
		_blittedRegionsSurface->clear();
		int i = _previousDirtyRegions.size();
		BOOST_FOREACH( std::vector<Rect> tmp,  _previousDirtyRegions ) {
			_blittedRegionsSurface->setColor(_blittedRegionsColors[--i]);
			BOOST_FOREACH( const Rect &dirtyRegion, tmp ) {
				_blittedRegionsSurface->drawRect( dirtyRegion );
			}
		}
		_blittedRegionsSurface->autoFlush(true);
	}

	// Check if any surface has changed size/location to mark dirty final path
	BOOST_FOREACH( Surface *surface, _surfaces ) {
		if (surface->boundsChanged()) {
			invalidateRegion( surface->getBounds() );
			surface->cleanBoundsChanged();
		}
	}

	// Check if any surface is dirty
	bool isDirty=false;
	BOOST_FOREACH( Surface *surface, _surfaces ) {
		// Check if surface is dirty
		isDirty |= surface->isDirty();
		if (surface->isDirty()) {
			if (surface->autoFlush() && surface->isVisible()) {
				toBlit.push_back( surface );
			}
			surface->clearDirty();
		}
	}

	// Need blit any surface?
	if (isDirty || !_dirtyRegions.empty()) {
		Rect blitRect;

		// Sort surfaces by zIndex
		std::stable_sort( toBlit.begin(), toBlit.end(), SortSurfaces() );

		LDEBUG("Canvas", "Flush: toBlit=%d, regions=%d", toBlit.size(), _dirtyRegions.size() );

		// For each dirty region
		BOOST_FOREACH( const Rect &dirtyRegion, _dirtyRegions ) {
			LTRACE("Canvas", "Dirty region: (%d, %d, %d, %d)", dirtyRegion.x, dirtyRegion.y, dirtyRegion.w, dirtyRegion.h );

			// Clear region
			Rect intersection;
			if (layer->getBounds().intersection( dirtyRegion, intersection )) {
				layer->clear( intersection );
			}

			// Blit each surface
			BOOST_FOREACH( Surface *surface, toBlit ) {
				// Need blit?
				if (surface->getDirtyRegion( dirtyRegion, blitRect )) {
					LTRACE("Canvas", "Surface to blit: %p, zIndex=%d", surface, surface->getZIndex() );

					// Set composition mode from surface
					layer->setCompositionMode( surface->flushCompositionMode() );

					// Blit dirty region
					Point point( blitRect.x, blitRect.y );
					Point surfacePos = surface->getLocation();
					blitRect.x -= surfacePos.x;
					blitRect.y -= surfacePos.y;
					layer->blit( point, surface, blitRect );
				}
			}
			layer->setCompositionMode( composition::source_over );
		}

		// Render layer
		win()->renderLayer( layer, _dirtyRegions );

		// Clean dirty regions
		_dirtyRegions.clear();
	}
}

/**
 * Vuelca en una imagen png indicada por el path @b file el contenido del canvas.
 * @param file Ruta absoluta que indica el nombre del archivo a generar.
 * @return true si la imagen se pudo generar correctamente, false en caso contrario.
 */
bool Canvas::saveAsImage( const std::string &file ) {
	bool result=false;
	LAYER( result=layer->saveAsImage( file ); );
	return result;
}

/**
 * Compara el contenido del canvas con la imagen indicada por @b file.
 * @param file Ruta absoluta de la imágen a comparar.
 * @return true si el contenido del canvas es igual a la imagen, false en caso contrario.
 */
bool Canvas::equalsImage( const std::string &file ) {
	bool result=false;
	LAYER( result=layer->equalsImage( file ); );
	return result;
}

Window *Canvas::win() const {
	return _win;
}

/**
 * Destruye todas las instancias de Surface creadas y limpia el canvas.
 */
void Canvas::reset() {
	CLEAN_ALL( Surface*, _surfaces );
	invalidateRegion(Rect(Point(0,0), _size));

	Window *w = win();
	Surface *layer = w->lockLayer();
	if (layer) {
		layer->clear();
		w->renderLayer( layer, _dirtyRegions );
		w->unlockLayer( layer );
	}
	_dirtyRegions.clear();
}

/**
 * Agrega el directorio pasado como parametro para la busqueda de fuentes.
 * @param dir Ruta absoluta a un directorio
 */
void Canvas::addFontDirectory( const std::string &dir ) {
	_fontManager->addFontDirectory( dir );
}

/**
 * Limpia la base de datos de busqueda de fuentes.
 */
void Canvas::clearFontDirectories() {
	_fontManager->clearFontDirectories();
}

/**
 * Busca una fuente con las características indicadas.
 * @param fontInfo Indica las características de la fuente a buscar (family,bold,italic,size).
 * @return El path absoluto a la fuente o un string vacio si se encontró ninguna fuente adecuada.
 */
Font *Canvas::loadFont( const FontInfo &fontInfo ) {
	std::string file = _fontManager->findFont( fontInfo );
	if (file.empty()) {
		LWARN("Canvas", "Fail to find font");
		return NULL;
	}

	BOOST_FOREACH( Font *font, _fonts ) {
		if ((font->filename()==file) && (font->size()==fontInfo.size())) {
			return font;
		}
	}

	Font *font = createFont( file, fontInfo.size() );
	if( !font->initialize() ) {
		LERROR("Canvas", "Fail to initialize font of size %d from %s", fontInfo.size(), file.c_str());
		DEL(font);
		return NULL;
	}
	_fonts.push_back( font );
	return font;
}

}
