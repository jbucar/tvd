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

#include "window.h"
#include "screen.h"
#include "system.h"
#include "videooverlay.h"
#include "point.h"
#include "surface.h"
#include "canvas.h"
#include "impl/dummy/window.h"
#include "generated/config.h"
#if CANVAS_WINDOW_USE_FB
#	include "impl/fb/window.h"
#endif
#if CANVAS_WINDOW_USE_REMOTE
#include "remote/window.h"
#endif
#include <util/log.h>
#include <util/assert.h>
#include <util/cfg/configregistrator.h>
#include <boost/math/special_functions/round.hpp>

namespace canvas {

REGISTER_INIT_CONFIG( gui_window ) {
	root().addNode( "window" )
#if CANVAS_SYSTEM_USE_GTK
	.addValue( "use", "Window to instance", std::string("gtk") )
#else
	.addValue( "use", "Window to instance", std::string("dummy") )
#endif
	.addValue( "title", "Window title", std::string("Dummy") )
	.addValue( "icon", "Window icon", std::string("") )
	.addValue( "fullscreen", "Window fullscreen option", false )
	.addValue( "keepAspectRatio", "Keep aspect ratio from canvas to window", true )
	.addValue( "winID", "Window id", 0UL )
	.addNode( "size" )
		.addValue( "width", "Window width", 720 )
		.addValue( "height", "Window height", 576 );
}

/**
 * Crea una instancia de @c Window. usando la implementación indicada en el árbol de configuración (ver módulo @em dtv-util).
 * @return Una nueva instancia de @c Window.
 */
Window *Window::create() {
	const std::string &use = util::cfg::getValue<std::string>("gui.window.use");
	LINFO("Window", "Creating window: use=%s", use.c_str() );

#if CANVAS_WINDOW_USE_FB
	if (use == "fb") {
		return new fb::Window();
	}
#endif
#if CANVAS_WINDOW_USE_REMOTE
	if (use == "remote") {
		return new remote::Window();
	}
#endif

	return new dummy::Window();
}

/**
 * Constructor base. Luego de crear un instancia se debe llamar al método initialize().
 */
Window::Window()
{
	util::cfg::PropertyNode &root = util::cfg::get("gui.window");
	_title = root.get<std::string>("title");
	_icon = root.get<std::string>("icon");
	_isFullScreen = false;
	_isIconified = false;
	_canvasInitialized = false;
}

/**
 * Destructor base. Antes de destruir un instancia se debe llamar al método finalize().
 */
Window::~Window()
{
	if (_overlays.size()) {
		LWARN("Window", "not all video overlays destroyed: size=%d", _overlays.size() );
	}
}

/**
 * Inicializa el estado de la instancia.
 * @return true si la inicialización finalizó correctamente, false en caso contrario.
 */
bool Window::initialize( System *sys ) {
	if (winID() && !supportEmbedded()) {
		LERROR( "Window", "Window object not support embedded" );
		return false;
	}

	if (!init()) {
		LERROR( "Window", "Could not initialize window" );
		return false;
	}

	_onModeChanged = sys->screen()->onModeChanged().connect( boost::bind(&Window::onModeChanged,this,sys,_2) );

	return true;
}

bool Window::init() {
	initSize();
	return true;
}

/**
 * Finaliza la instancia.
 */
void Window::finalize() {
	DTV_ASSERT(!_canvasInitialized);

	//	Disconnect from screen
	_onModeChanged.disconnect();

	//	Finalize subclasses
	fin();

	//	Some checks!
	if (!_overlays.empty()) {
		LWARN( "Window", "Some windows overlays not destroyed" );
	}
}

void Window::fin() {
}

/**
 * @return El título que se muestra en la decoración de la ventana.
 */
const std::string &Window::title() const {
	return _title;
}

const std::string &Window::icon() const {
	return _icon;
}

/**
 * @return El tamaño de la ventana.
 */
const Size &Window::size() const {
	return _size;
}

OnResizeCallback Window::onResizeCallback( const OnResizeCallback &callback ) {
	OnResizeCallback tmp = _resizeCallback;
	_resizeCallback = callback;
	return tmp;
}

Size Window::screenSize() const {
	return Size(0,0);
}

void Window::initSize() {
	const bool &inFullScreen = util::cfg::getValue<bool>("gui.window.fullscreen");
	if (inFullScreen && supportFullScreen()) {
		setFullScreen( true );
	}

	setSize();
}

bool Window::supportResize() const {
	return false;
}

void Window::resize( const Size &newSize ) {
	if (supportResize()) {
		util::cfg::setValue<int>("gui.window.size.height", newSize.h );
		util::cfg::setValue<int>("gui.window.size.width", newSize.w );

		if (isFullScreen()) {
			setFullScreen( false );
		}

		setSize();
	}
	else {
		LWARN( "Window", "Window doesn't support resize");
	}
}

void Window::setSize() {
	Size size;

	if (isFullScreen()) {
		size = screenSize();
		DTV_ASSERT( size != Size(0,0) );
	}
	else {
		size.h = util::cfg::getValue<int>("gui.window.size.height");
		size.w = util::cfg::getValue<int>("gui.window.size.width");
	}

	LDEBUG( "Window", "Set size: old=(%d,%d), new=(%d,%d)",
		_size.w, _size.h, size.w, size.h );

	if (size != _size) {
		//	Notify to depends
		if (!_resizeCallback.empty()) {
			_resizeCallback( true );
		}

		updateVideoBounds( size );

		//	Set new size in the config
		_size = size;
		setSizeImpl( size );
		if (util::cfg::getValue<bool>("gui.window.keepAspectRatio")) {
			scaleBounds( _targetWindow, _canvasSize, _size );
		}
		else {
			_targetWindow = _size;
		}

		//	Update overlays bounds
		updateBoundsOnOverlays();

		//	Repaint
		redraw();

		//	Notify to depends
		if (!_resizeCallback.empty()) {
			_resizeCallback( false );
		}
	}
}

void Window::setSizeImpl( const Size & /*size*/ ) {
}

void Window::onModeChanged( System *sys, mode::type videoMode ) {
	Mode m = mode::get( videoMode );
	Rect newWindow( 0, 0, m.width, m.height);
	Rect current( 0, 0, size() );
	LDEBUG( "Window", "On mode changed: cur=(%d,%d), new=(%d,%d)", current.w, current.h, m.width, m.height );
	if ((isFullScreen() && newWindow != current) || !newWindow.includes(current)) {
		finCanvasLayer( sys->canvas() );
		initCanvasLayer( sys->canvas() );
		setSize();
	}
}

const Size &Window::getCanvasSize() const {
	DTV_ASSERT(_canvasSize.w > 0 && _canvasSize.h > 0);
	return _canvasSize;
}

bool Window::supportFullScreen() const {
	return false;
}

bool Window::isFullScreen() const {
	return _isFullScreen;
}

void Window::fullScreen( bool enable ) {
	if (supportFullScreen()) {
		LDEBUG( "Window", "fullScreen. oldState=%d newState=%d", _isFullScreen, enable );
		if (enable != _isFullScreen) {
			setFullScreen( enable );
			setSize();
		}
	} else {
		LWARN( "Window", "Window doesn't support fullScreen");
	}
}

void Window::setFullScreen( bool enable ) {
	fullScreenImpl( enable );
	_isFullScreen = enable;
}

void Window::fullScreenImpl( bool /*enable*/ ) {
}

//	Iconify methods
bool Window::supportIconify() const {
	return true;
}

bool Window::isIconified() const {
	return _isIconified;
}

void Window::iconify( bool enable ) {
	LDEBUG( "Window", "iconify. oldState=%d newState=%d", _isIconified, enable );
	if (supportIconify()) {
		if (enable != _isIconified) {
			//	Iconify/deicofiny the overlays
			for(size_t i=0; i < _overlays.size(); i++) {
				_overlays[i]->iconify( enable );
			}

			//	Implementation
			iconifyImpl( enable );
			_isIconified = enable;

			//	Raise to restore overlay stack
			if (!enable) {
				restackOverlays();
			}
		}
	}
	else {
		LWARN( "Window", "Window doesn't support iconify");
	}
}

void Window::iconifyImpl( bool /*enable*/ ) {
}

//	Canvas layer methods
bool Window::initCanvasLayer( Canvas *canvas ) {
	DTV_ASSERT(!_canvasInitialized);
	_canvasSize = canvas->size();
	if (!initLayer(canvas)) {
		_canvasSize = Size();
		return false;
	}
	canvas->invalidateRegion(Rect(Point(0,0),_canvasSize));
	_canvasInitialized = true;
	return true;
}

void Window::finCanvasLayer( Canvas *canvas ) {
	if (_canvasInitialized) {
		_canvasInitialized = false;
		finLayer( canvas );
		_canvasSize = Size();
	}
}

bool Window::haveCanvas() const {
	return _canvasInitialized;
}

//	Implementation
void Window::mainLoopThreadId( const boost::thread::id &id ) {
	_threadId = id;
}

const boost::thread::id &Window::mainLoopThreadId() const {
	return _threadId;
}

//	Embedded
const unsigned long &Window::winID() const {
	return util::cfg::getValue<unsigned long>("gui.window.winID");
}

bool Window::supportEmbedded() const {
	return false;
}

//	Overlays
bool Window::supportVideoOverlay() const {
	return false;
}

const std::vector<VideoOverlayPtr> &Window::overlays() const {
	return _overlays;
}

VideoOverlay *Window::createOverlayInstance( int /*zIndex*/ ) const {
	DTV_ASSERT(false);
	return NULL;
}

struct SortOverlays {
	bool operator()( VideoOverlay *vo1, VideoOverlay *vo2 ) const {
		return vo1->zIndex() < vo2->zIndex();
	}
};

void Window::restackOverlays() {
	if (supportVideoOverlay()) {
		LDEBUG( "Window", "Restack overlays: overlays=%d", _overlays.size() );
		//	Sort overlays by zIndex
		std::stable_sort( _overlays.begin(), _overlays.end(), SortOverlays() );

		updateOverlayStack();
	}
}

void Window::updateOverlayStack() {
}

void Window::updateBoundsOnOverlays() {
	LDEBUG( "Window", "Update bounds on overlays: overlays=%d", _overlays.size() );
	for (size_t i=0; i<_overlays.size(); i++) {
		_overlays[i]->updateBounds();
	}
}

/**
 * @return Una nueva instancia de @c VideoOverlay.
 */
VideoOverlay *Window::createVideoOverlay( int zIndex/*=0*/ ) {
	VideoOverlay *tmp=createOverlayInstance( zIndex );
	if (tmp) {
		_overlays.push_back( tmp );
	}
	return tmp;
}

/**
 * Destruye una instancia de @c VideoOverlay.
 * @param ptr La instancia de @c VideoOverlay a destruir.
 */
void Window::destroyVideoOverlay( VideoOverlayPtr &ptr ) {
	LDEBUG("Canvas", "Destroy overlay: overlay=%p", ptr );
	DTV_ASSERT(ptr);
	std::vector<VideoOverlay *>::iterator it=std::find(
		_overlays.begin(), _overlays.end(), ptr );
	if (it != _overlays.end()) {
		delete (*it);
		_overlays.erase( it );
	}
	ptr = NULL;

	// Redraw window
	redraw();
}

//	Coordenates
static inline void translate( int &x, int &y, const Size &from, const Size &to ) {
	x = x ? boost::math::iround( (double)(x * to.w) / (double)from.w ) : 0;
	y = y ? boost::math::iround( (double)(y * to.h) / (double)from.h ) : 0;
}

bool Window::translateToCanvas( int &x, int &y ) const {
	if (x >= _targetWindow.x && x <= _targetWindow.w &&
		y >= _targetWindow.y && y <= _targetWindow.h)
	{
		x -= _targetWindow.x;
		y -= _targetWindow.y;
		translate( x, y, Size(_targetWindow.w,_targetWindow.h), getCanvasSize() );
		return true;
	}
	return false;
}

void Window::translateToWindow( int &x, int &y, bool scale ) const {
	if (scale) {
		translate( x, y, getCanvasSize(), Size(_targetWindow.w,_targetWindow.h) );
		x += _targetWindow.x;
		y += _targetWindow.y;
	}
	else {
		translate( x, y, getCanvasSize(), Size(_size.w,_size.h) );
	}
}

void Window::scaleToWindow( int &w, int &h, bool scale ) const {
	if (scale) {
		translate( w, h, getCanvasSize(), Size(_targetWindow.w,_targetWindow.h) );
	}
	else {
		translate( w, h, getCanvasSize(), Size(_size.w,_size.h) );
	}
}

void Window::scaleBounds( Rect &bounds, const Size &from, const Size &to ) const {
	bounds = Rect( Point(0,0), to );
	float aw = float(from.w) / float(to.w);
	float ah = float(from.h) / float(to.h);
	if (aw>ah) {
		// Scale horizontaly and calculate height to mantain aspect.
		bounds.h = boost::math::iround(float(from.h * to.w) / float(from.w));
		bounds.y = boost::math::iround(float(to.h-bounds.h) / 2.0f);
	}
	else if (ah>aw) {
		// Scale verticaly and calculate width to mantain aspect.
		bounds.w = boost::math::iround(float(from.w * to.h) / float(from.h));
		bounds.x = boost::math::iround(float(to.w-bounds.w) / 2.0f);
	}
	//else { Same aspect => scale to window size. }
}

/**
 * Repinta la ventana por completo.
 */
void Window::redraw() {
	Rect r(0,0,_size.w,_size.h);
	redraw( r );
}

/**
 * Repinta la región de la ventana indicada en @b r.
 * @param r La región de la ventana que debe ser repintada.
 */
void Window::redraw( const Rect &r ) {
	if (_canvasInitialized) {
		std::vector<Rect> rects;
		rects.push_back( r );
		Surface *s = lockLayer();
		if (s) {
			renderLayer( s, rects );
			unlockLayer( s );
		}
	}
}

/**
 * Libera el bloqueo de @b surface.
 * @param surface El @c Surface obtenido mediante lockLayer().
 */
void Window::unlockLayer( Surface * /*surface*/ ) {
}

/**
 * Renderiza el contenido de @b surface en la ventana.
 * @param surface La @c Surface retornada por lockLayer().
 * @param dirtyRegions Las regiones del @c Surface que deben ser repintadas.
 */
void Window::renderLayer( Surface *surface, const std::vector<Rect> &dirtyRegions ) {
	DTV_ASSERT( mainLoopThreadId() == boost::thread::id() || mainLoopThreadId() == boost::this_thread::get_id() );
	renderLayerImpl( surface, dirtyRegions );
}

const Rect &Window::targetWindow() const {
	return _targetWindow;
}

//	Video methods
const Rect &Window::videoBounds() const {
	return _videoBounds;
}

void Window::updateVideoBounds( const Size &newSize ) {
	if (_size.w>0 && _size.h>0) {
		//	Reset video bounds
		Point tmpPos;
		tmpPos.x = boost::math::iround( (double)(_videoBounds.x * newSize.w) / (double) _size.w );
		tmpPos.y = boost::math::iround( (double)(_videoBounds.y * newSize.h) / (double) _size.h );

		Size tmpSize;
		tmpSize.w = boost::math::iround( (double)(_videoBounds.w * newSize.w) / (double) _size.w );
		tmpSize.h = boost::math::iround( (double)(_videoBounds.h * newSize.h) / (double) _size.h );

		_videoBounds = tmpPos;
		_videoBounds = tmpSize;
	} else {
		_videoBounds = newSize;
	}
}

void Window::moveVideo( const Point &point ) {
	_videoBounds = point;
}

void Window::resizeVideo( const Size &size ) {
	_videoBounds = size;
}

int Window::getFormat( char * /*chroma*/, unsigned * /*width*/, unsigned * /*height*/, unsigned * /*pitches*/, unsigned * /*lines*/ ) {
	return 0;
}

void Window::cleanup() {
}

void *Window::allocFrame( void ** /*pixels*/ ) {
	return NULL;
}

void Window::freeFrame( void * /*frame*/ ) {
}

void Window::renderFrame( void * /*frame*/ ) {
}

}
