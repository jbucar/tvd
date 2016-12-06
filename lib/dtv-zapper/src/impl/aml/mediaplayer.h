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

#include <canvas/rect.h>
#include <canvas/mediaplayer.h>
#include <boost/thread.hpp>

typedef struct _GMainLoop GMainLoop;
typedef struct _GstElement GstElement;
typedef struct _GstBuffer GstBuffer;

namespace aml {

namespace ts {
class Player;
}

class MediaPlayer : public canvas::MediaPlayer {
public:
	explicit MediaPlayer( canvas::System *sys );
	virtual ~MediaPlayer();

	//	Feed data
	virtual void feed( util::Buffer *buf );

	//	Aux gstreamer
	void onRun();
	void endLoop();
	void onError( const std::string &errMsg );
	void onParsedEvent();
	void onBuffering( int percent );

protected:
	virtual bool startImpl( const std::string &url );
	virtual void stopImpl();
	virtual void pauseImpl( bool needPause );
	virtual bool parsedOnPlay() const;

	virtual void muteImpl( bool needsMute );
	virtual void volumeImpl( canvas::Volume vol );

	virtual void moveResizeImpl( const canvas::Rect &rect );

	//	Streams info implementation
	virtual canvas::StreamsInfo *createStreamsInfo();
	virtual bool switchVideoStreamImpl( int id );
	virtual bool switchAudioStreamImpl( int id );
	virtual bool switchSubtitleStreamImpl( int id );

private:
	GMainLoop *_loop;
	GstElement *_pipeline;
	boost::thread _thread;
	bool _needsMute;
	canvas::Volume _volume;
	ts::Player *_tsPlayer;
	bool _feed;
};

}

