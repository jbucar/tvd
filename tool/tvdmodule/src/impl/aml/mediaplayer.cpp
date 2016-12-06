/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "mediaplayer.h"
#include <aml/aml.h>
#include <aml/ts.h>
#include <util/url.h>
#include <util/log.h>

namespace aml {

MediaPlayer::MediaPlayer()
{
	_isPlaying = false;
	_ts = new ts::Player();
}

MediaPlayer::~MediaPlayer()
{
	delete _ts;
}

bool MediaPlayer::init() {
	LDEBUG( "aml", "Mediaplayer init" );
	return _ts->init();
}

void MediaPlayer::fin() {
	LDEBUG( "aml", "Mediaplayer fin" );
	_ts->fin();
}

void MediaPlayer::play( const std::string &data ) {
	util::Url url(data);

	stop();

	if (url.type() == "srvdtv") {
		ts::Params params;
		params.videoPID = -1;
		params.audioPID = -1;

		{	//	Video
			int pid=-1;
			int type=-1;
			if (url.getParam( "videoPID", pid ) &&
				url.getParam( "videoType", type ))
			{
				params.videoPID = pid;
				params.videoType = type;
			}
		}

		{	//	Audio
			int pid=-1;
			int type=-1;
			if (url.getParam( "audioPID", pid ) &&
				url.getParam( "audioType", type ))
			{
				params.audioPID = pid;
				params.audioType = type;
			}
		}

		{	//	PCR pid
			int pid=-1;
			if (url.getParam( "pcrPID", pid )) {
				params.pcrPID = pid;
			}
		}

		_isPlaying = _ts->play( params );
		if (!_isPlaying) {
			LERROR( "aml", "Cannot play url: data=%s", data.c_str() );
		}
	}
	else {
		LERROR( "aml", "Cannot play url: data=%s", data.c_str() );
	}
}

void MediaPlayer::stop() {
	if (_isPlaying) {
		_ts->stop();
		_isPlaying = false;
	}
}

bool MediaPlayer::getVideoInfo( int &w, int &h, int &fps ) {
	if (_isPlaying) {
		_ts->getVideoInfo( w, h, fps );
	}
	return false;
}

bool MediaPlayer::getAudioInfo( int &nChannels, int &rate ) {
	if (_isPlaying) {
		_ts->getAudioInfo( nChannels, rate );
	}
	return false;
}

}
