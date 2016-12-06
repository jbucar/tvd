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
#include <vector>
#include <string>
#include <boost/thread.hpp>

namespace canvas {

class VideoOverlay;
typedef boost::function<void (bool begin)> OnResizeCallback;

/**
 * Esta clase representa una ventana del sistema, proveyendo funcionalidad para manejar la misma.
 */
class Window {
public:
	Window();
	virtual ~Window();

	//	Initialization
	bool initialize( System *sys );
	void finalize();

	//	Getters
	const std::string &title() const;
	const std::string &icon() const;

	//	Size methods
	const Size &size() const;
	virtual bool supportResize() const;
	void resize( const Size &newSize );
	OnResizeCallback onResizeCallback( const OnResizeCallback &callback );

	//	Full screen methods
	virtual bool supportFullScreen() const;
	void fullScreen( bool enable );
	bool isFullScreen() const;

	//	Iconify methods
	virtual bool supportIconify() const;
	void iconify( bool enable );
	bool isIconified() const;

	//	Embedded
	virtual bool supportEmbedded() const;
	const unsigned long &winID() const;

	//	Video overlays
	virtual bool supportVideoOverlay() const;
	VideoOverlay *createVideoOverlay( int zIndex=0 );
	void destroyVideoOverlay( VideoOverlayPtr &ptr );
	void restackOverlays();
	void updateBoundsOnOverlays();

	//	Canvas layer methods
	bool initCanvasLayer( Canvas *canvas );
	void finCanvasLayer( Canvas *canvas );
	bool haveCanvas() const;

	/**
	 * @return El @c Surface utilizado por la ventana para renderizar el contenido.
	 */
	virtual Surface *lockLayer()=0;
	virtual void unlockLayer( Surface *surface );
	const Rect &targetWindow() const;
	void renderLayer( Surface *surface, const std::vector<Rect> &dirtyRegions );
	void redraw();
	void redraw( const Rect &r );

	//	Video methods
	void moveVideo( const Point &point );
	void resizeVideo( const Size &size );
	virtual int getFormat( char *chroma, unsigned *width, unsigned *height, unsigned *pitches, unsigned *lines );
	virtual void cleanup();
	virtual void *allocFrame( void **pixels );
	virtual void freeFrame( void *frame );
	virtual void renderFrame( void *frame );

	//	Translation of coordenates
	bool translateToCanvas( int &x, int &y ) const;
	void translateToWindow( int &x, int &y, bool scale ) const;
	void scaleToWindow( int &w, int &h, bool scale ) const;
	const Size &getCanvasSize() const;;

	//	Instance creation
	static Window *create();

	//	Implementation
	void mainLoopThreadId( const boost::thread::id &id );
	const boost::thread::id &mainLoopThreadId() const;

protected:
	//	Initialization
	virtual bool init();
	virtual void fin();

	//	Canvas layer
	virtual bool initLayer( Canvas *canvas )=0;
	virtual void finLayer( Canvas *canvas )=0;

	//	Aux size
	void initSize();
	void setSize();
	virtual Size screenSize() const;
	virtual void setSizeImpl( const Size &size );
	virtual void onModeChanged( System *sys, mode::type videoMode );

	//	Aux full screen
	void setFullScreen( bool enable );
	virtual void fullScreenImpl( bool enable );

	//	Aux iconify
	virtual void iconifyImpl( bool enable );

	//	Aux coordenates
	void translateCoordenate( int &x, int &y, const Size &from, const Size &to ) const;
	void scaleBounds( Rect &bounds, const Size &from, const Size &to ) const;

	//	Aux layer methods
	virtual void renderLayerImpl( Surface *surface, const std::vector<Rect> &dirtyRegions )=0;

	//	Aux overlays
	const std::vector<VideoOverlayPtr> &overlays() const;
	virtual void updateOverlayStack();
	virtual VideoOverlay *createOverlayInstance( int zIndex ) const;

	//	Aux video
	const Rect &videoBounds() const;
	void updateVideoBounds( const Size &newSize );

private:
	std::string _title;
	std::string _icon;
	Size _size;
	Size _canvasSize;
	Rect _videoBounds;
	Rect _targetWindow;
	bool _isFullScreen;
	bool _isIconified;
	bool _canvasInitialized;
	std::vector<VideoOverlayPtr> _overlays;
	boost::thread::id _threadId;
	OnResizeCallback _resizeCallback;
	boost::signals2::connection _onModeChanged;
};

}
