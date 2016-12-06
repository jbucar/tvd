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

#include "webviewer.h"
#include "../../surface.h"
#include "../../canvas.h"
#include "../../system.h"
#include <util/task/dispatcher.h>
#include <util/log.h>
#include <util/assert.h>
#include <util/registrator.h>
#include <util/mcr.h>
#include <boost/bind.hpp>


#define REQUIRE_UI_THREAD()   DTV_ASSERT(CefCurrentlyOn(TID_UI));
#define REQUIRE_IO_THREAD()   DTV_ASSERT(CefCurrentlyOn(TID_IO));

namespace canvas {
namespace cef {

// Handler for off-screen rendering windows.
class WebHandler : public CefClient,
                         public CefLifeSpanHandler,
                         public CefRequestHandler,
                         public CefRenderHandler
{
public:
	WebHandler( WebViewer *view )
		: _view(view)
	{
	}

	virtual ~WebHandler() 
	{
	}

	// CefClient methods
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() { return this; }
	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() { return this; }
	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() { return this; }

	// CefLifeSpanHandler methods
	virtual void OnAfterCreated( CefRefPtr<CefBrowser> browser ) {
		REQUIRE_UI_THREAD();
		_view->setupBrowser( browser );
	}

	//	CefRequestHandler methods
	virtual bool OnBeforeResourceLoad(
		CefRefPtr<CefBrowser> /*browser*/,
		CefRefPtr<CefRequest> request,
		CefString& /*redirectUrl*/,
		CefRefPtr<CefStreamReader>& /*resourceStream*/,
		CefRefPtr<CefResponse> /*response*/,
		int /*loadFlags*/ )
	{
		REQUIRE_IO_THREAD();
		std::string url = request->GetURL();
		LDEBUG("cef::WebViewer", "request url=%s", url.c_str() );
		return false;
	}

	//	CefRenderHandler methods
	virtual void OnPaint(
		CefRefPtr<CefBrowser> /*browser*/,
		PaintElementType /*type*/,
		const RectList& /*dirtyRects*/,
		const void* buffer)
	{
		REQUIRE_UI_THREAD();
		_view->paint( (const char *)buffer );
	}

private:
	WebViewer *_view;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(ClientOSRPlugin);
};

bool WebViewer::init() {
	// Initialize CEF.
	CefSettings settings;
	settings.multi_threaded_message_loop = true;
	settings.log_severity = LOGSEVERITY_ERROR;
	// settings.log_severity = LOGSEVERITY_VERBOSE;
	// CefString(&settings.log_file) = "chrome.log";
	CefRefPtr<CefApp> app;
	CefInitialize(settings, app);
	return true;
}

void WebViewer::fin() {
	// Shut down CEF.
	CefShutdown();
}

REGISTER_INIT( cef ) {
	WebViewer::init();
}

REGISTER_FIN( cef ) {
	WebViewer::fin();
}

WebViewer::WebViewer( System *sys, Surface *surface )
	: canvas::WebViewer( surface ), _sys(sys)
{
	_buf = NULL;
	_bufSize = 0;
	_valid = false;
}

WebViewer::~WebViewer( void )
{
	free(_buf);
}

void WebViewer::setupBrowser( const CefRefPtr<CefBrowser> &browser ) {
	_browser = browser;
	resize( surface()->getSize() );
}

//	Widget
void WebViewer::resize( const Size &size ) {
	_mutex.lock();
	_size = size;
	int tmp = bitmapSize();
	if (_bufSize < tmp) {
		free(_buf);
		_bufSize = tmp;
		_buf = (char *)malloc( _bufSize );
	}
	_valid = false;
	_mutex.unlock();
	_browser->SetSize( PET_VIEW, _size.w, _size.h );
}

int WebViewer::bitmapSize() const {
	return _size.w * _size.h * 4;
}

void WebViewer::paint( const char *buf ) {
	//	Copy memory
	_mutex.lock();
	memcpy( _buf, buf, bitmapSize() );
	_valid = true;
	_mutex.unlock();
	
	//	Enqueue paint
	_sys->dispatcher()->post( this, boost::bind(&WebViewer::draw,this) );
}

void WebViewer::draw() {
	_mutex.lock();
	if (_valid) {
		canvas::ImageData img;
		memset(&img,0,sizeof(canvas::ImageData));

		//	Setup data
		img.size = _size;
		img.length = bitmapSize();
		img.data = (unsigned char *)_buf;
		img.bitsPerPixel = 32;
		img.bytesPerPixel = 4;
		img.stride = _size.w*4;
		img.dataOffset = 0;	

		//	Create surface from data
		canvas::Surface *sur=_sys->canvas()->createSurface( &img );
		if (sur) {
			surface()->blit( canvas::Point(0,0), sur );
			DEL( sur );
			_sys->canvas()->flush();
		}
	}
	_mutex.unlock();
}

bool WebViewer::load( const std::string &file ) {
	//	In player thread	
	LINFO("cef::WebViewer", "load: uri=%s", file.c_str());

	_sys->dispatcher()->registerTarget( this, "canvas::cef::WebViewer" );	

	//	Create the off-screen rendering window.
	CefWindowInfo windowInfo;
	CefBrowserSettings settings;
	settings.history_disabled = true;
	settings.local_storage_disabled = true;
	settings.databases_disabled = true;
	settings.application_cache_disabled = true;
	windowInfo.SetAsOffScreen(NULL);
	windowInfo.SetTransparentPainting(TRUE);
	bool result=CefBrowser::CreateBrowser( 
		windowInfo, 
		new WebHandler( this ),
		file,
		settings );
	if (result) {
		return true;
	}
	return false;
}

void WebViewer::stop() {
	_sys->dispatcher()->unregisterTarget( this );
	
	//	--	In player thread	   
	LINFO("cef::WebViewer", "stop");
	_browser->CloseBrowser();
	_browser = NULL;
}

}
}
