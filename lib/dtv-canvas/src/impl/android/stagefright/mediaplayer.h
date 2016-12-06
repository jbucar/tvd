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

#include "../../../mediaplayer.h"

class ANativeWindow;
class _jobject;
typedef _jobject* jobject;
struct _JNIEnv;
typedef _JNIEnv JNIEnv;

namespace util {
	class Buffer;
}

namespace canvas {

class System;
namespace gl { class Canvas; class Surface; }

namespace android {
namespace stagefright {

class AVFormatSource;

class MediaPlayer : public canvas::MediaPlayer {
public:
	MediaPlayer( canvas::System *sys );
	virtual ~MediaPlayer();

	//	Initialization
	virtual bool initialize( const Rect &rect, const std::string &url );
	virtual bool initialize( const std::string &url );
	virtual void finalize();

	//	Stream operations
	virtual void play();
	virtual void stop();
	virtual void pause( bool needPause );

	//	Volume
	virtual void mute( bool needMute );

	//	Position/size
	virtual void setFullScreen();
	virtual void move( const Point &point );
	virtual void resize( const Size &size );

	//	Feed data
	virtual void feed( util::Buffer *buf );

protected:
	//	Aux volume
	virtual void setVolume( Volume vol );
	virtual Volume getVolume() const;

	void process();

private:
	JNIEnv *_jni;
	canvas::gl::Canvas *_canvas;
	canvas::gl::Surface *_surface;
	AVFormatSource *_src;	
	jobject _jniSurfaceTexture;
	jobject _jniSurface;	
	ANativeWindow *_window;
};

}
}
}

