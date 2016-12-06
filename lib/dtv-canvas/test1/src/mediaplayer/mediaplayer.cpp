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

#include "mediaplayer.h"

static canvas::VideoStreamInfo *createVinfo( int bitrate, float aspect, int w, int h, int framerate, bool interleaved, const std::string &lan, const std::string &name, const std::string &codec ) {
	canvas::VideoStreamInfo *sInfo = new canvas::VideoStreamInfo();
	sInfo->name( name );
	sInfo->language( lan );
	sInfo->bitrate( bitrate );
	sInfo->aspectRatio( aspect );
	sInfo->size( canvas::Size( w, h ) );
	sInfo->framerate( framerate );
	sInfo->interleaved( interleaved );
	sInfo->codecName( codec );
	return sInfo;
}

static canvas::AudioStreamInfo *createAinfo( int bitrate, int channels, int samplerate, int bitspersample, const std::string &lan, const std::string &name, const std::string &codec ) {
	canvas::AudioStreamInfo *sInfo = new canvas::AudioStreamInfo();
	sInfo->name( name );
	sInfo->language( lan );
	sInfo->bitrate( bitrate );
	sInfo->channels( channels );
	sInfo->samplerate( samplerate );
	sInfo->bitspersample( bitspersample );
	sInfo->codecName( codec );
	return sInfo;
}

static canvas::SubtitleStreamInfo *createSinfo( const std::string &lan, const std::string &name, const std::string &enc, const std::string &codec ) {
	canvas::SubtitleStreamInfo *sInfo = new canvas::SubtitleStreamInfo();
	sInfo->name( name );
	sInfo->language( lan );
	sInfo->encoding( enc );
	sInfo->codecName( codec );
	return sInfo;
}

OkStreamsInfo::OkStreamsInfo( TestData &data )
	: _testData(data)
{
	_doParse = false;
}

OkStreamsInfo::~OkStreamsInfo()
{
}

void OkStreamsInfo::refreshImpl() {
	_testData.nRefresh++;
}

void OkStreamsInfo::parseImpl() {
	if (_doParse) {
		addVideoInfo( createVinfo( 7200000, 2.4f, 1920, 800, 30, true, "Español", "Video Test 1", "MP4" ) );
		addVideoInfo( createVinfo( 3600000, 2.1f, 720, 576, 25, true, "English", "Video Test 2", "MPEG" ) );
		addVideoInfo( createVinfo( 4700000, 1.33f, 320, 240, 60, false, "Chinese", "Video Test 3", "AVI" ) );
		addAudioInfo( createAinfo( 256000, 2, 44100, 16, "Español", "Audio Test 1", "MP3" ) );
		addAudioInfo( createAinfo( 128000, 2, 44100, 24, "English", "Audio Test 2", "AAC" ) );
		addSubtitleInfo( createSinfo( "Español", "Subtitle Test 1", "ISO-8859-1", "SUB" ) );
		addSubtitleInfo( createSinfo( "English", "Subtitle Test 2", "ISO-8859-2", "SRT" ) );
		addSubtitleInfo( createSinfo( "Chinese", "Subtitle Test 3", "ISO-8859-3", "SRT" ) );
		addSubtitleInfo( createSinfo( "Arabic", "Subtitle Test 4", "ISO-8859-4", "SRT" ) );
		currentVideo( 0 );
		currentAudio( 0 );
		currentSubtitle( 0 );
	}
}

void OkStreamsInfo::doParse( bool parse ) {
	_doParse = parse;
}

void OkMediaPlayer::signalCompleted() {
	onMediaCompleted();
};

void OkMediaPlayer::signalMediaChanged( bool addStreams/*=false*/ ) {
	static_cast<OkStreamsInfo*>(sInfo())->doParse( addStreams );
	onMediaParsed();
}

void OkMediaPlayer::signalError( const std::string &err ) {
	onMediaError( err );
}

bool OkMediaPlayer::startImpl( const std::string &/*url*/ ) {
	return true;
}

void OkMediaPlayer::stopImpl() {
}

void OkMediaPlayer::pauseImpl( bool needPause ) {
	_testData.nPause += needPause ? 1 : -1;
}

void OkMediaPlayer::muteImpl( bool needsMute ) {
	_testData.nMuted += needsMute ? 1 : -1;
}

void OkMediaPlayer::volumeImpl( canvas::Volume /*vol*/ ) {
	_testData.nVol++;
}

void OkMediaPlayer::moveResizeImpl( const canvas::Rect &rect ) {
	_testData.nMoveResize++;
	_testData.bounds = rect;
}

bool OkMediaPlayer::switchVideoStreamImpl( int /*id*/ ) {
	_testData.videoSwitched++;
	return true;
}

bool OkMediaPlayer::switchAudioStreamImpl( int /*id*/ ) {
	_testData.audioSwitched++;
	return true;
}

bool OkMediaPlayer::switchSubtitleStreamImpl( int /*id*/ ) {
	_testData.subtitleSwitched++;
	return true;
}
