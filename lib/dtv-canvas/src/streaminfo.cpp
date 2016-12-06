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

#include "streaminfo.h"
#include <util/log.h>
#include <util/mcr.h>
#include <util/assert.h>

namespace canvas {

void StreamInfo::name( const std::string &name ) {
	_name = name;
}

std::string StreamInfo::name() const {
	return _name;
}

void StreamInfo::language( const std::string &lan ) {
	_language = lan;
}

std::string StreamInfo::language() const {
	return _language;
}

void StreamInfo::codecName( const std::string &codec ) {
	_codecName = codec;
}

std::string StreamInfo::codecName() const {
	return _codecName;
}

void VideoStreamInfo::bitrate( int bitrate ) {
	_bitrate = bitrate;
}

int VideoStreamInfo::bitrate() const {
	return _bitrate;
}

void VideoStreamInfo::size( const Size &size ) {
	_size = size;
}

Size VideoStreamInfo::size() const {
	return _size;
}

void VideoStreamInfo::framerate( int framerate ) {
	_framerate = framerate;
}

int VideoStreamInfo::framerate() const {
	return _framerate;
}

void VideoStreamInfo::interleaved( bool isInterleaved ) {
	_interleaved = isInterleaved;
}

bool VideoStreamInfo::isInterleaved() const {
	return _interleaved;
}

void VideoStreamInfo::aspectRatio( float aspect ) {
	_aspectRatio = aspect;
}

float VideoStreamInfo::aspectRatio() const {
	return _aspectRatio;
}

void AudioStreamInfo::bitrate( int bitrate ) {
	_bitrate = bitrate;
}

int AudioStreamInfo::bitrate() const {
	return _bitrate;
}

void AudioStreamInfo::channels( int channels ) {
	_channels = channels;
}

int AudioStreamInfo::channels() const {
	return _channels;
}

void AudioStreamInfo::samplerate( int samplerate ) {
	_samplerate = samplerate;
}

int AudioStreamInfo::samplerate() const {
	return _samplerate;
}

void AudioStreamInfo::bitspersample( int bps ) {
	_bitspersample = bps;
}

int AudioStreamInfo::bitspersample() const {
	return _bitspersample;
}

void SubtitleStreamInfo::encoding( const std::string &enc ) {
	_encoding = enc;
}

std::string SubtitleStreamInfo::encoding() const {
	return _encoding;
}

StreamsInfo::StreamsInfo()
{
	_currentAudio = -1;
	_currentVideo = -1;
	_currentSubtitle = -1;
	_parsed = false;
}

StreamsInfo::~StreamsInfo()
{
	reset();
}

int StreamsInfo::currentAudio() const {
	return _currentAudio;
}

void StreamsInfo::currentAudio( int id ) {
	LDEBUG("StreamsInfo", "Set current audio id=%d", id);
	size_t count = audioCount();
	if (id < (int)count) {
		_currentAudio = id;
	} else {
		LWARN("StreamsInfo", "Fail to set current audio, invalid id=%d, audio_count=%u", id, count);
	}
}

size_t StreamsInfo::audioCount() const {
	return audioInfo().size();
}

const std::vector<AudioStreamInfo*> &StreamsInfo::audioInfo() const {
	return _audioInfo;
}

int StreamsInfo::currentVideo() const {
	return _currentVideo;
}

void StreamsInfo::currentVideo( int id ) {
	LDEBUG("StreamsInfo", "Set current video id=%d", id);
	size_t count = videoCount();
	if (id < (int)count) {
		_currentVideo = id;
	} else {
		LWARN("StreamsInfo", "Fail to set current video, invalid id=%d, video_count=%u", id, count);
	}
}

size_t StreamsInfo::videoCount() const {
	return videoInfo().size();
}

const std::vector<VideoStreamInfo*> &StreamsInfo::videoInfo() const {
	return _videoInfo;
}

int StreamsInfo::currentSubtitle() const {
	return _currentSubtitle;
}

void StreamsInfo::currentSubtitle( int id ) {
	LDEBUG("StreamsInfo", "Set current subtitle id=%d", id);
	size_t count = subtitleCount();
	if (id < (int)count) {
		_currentSubtitle = id;
	} else {
		LWARN("StreamsInfo", "Fail to set current subtitle, invalid id=%d, subtitle_count=%u", id, count);
	}
}

size_t StreamsInfo::subtitleCount() const {
	return subtitleInfo().size();
}

const std::vector<SubtitleStreamInfo*> &StreamsInfo::subtitleInfo() const {
	return _subtitleInfo;
}

void StreamsInfo::parse() {
	LDEBUG("StreamsInfo", "Parse streams information!");
	reset();
	parseImpl();
	_parsed=true;
}

void StreamsInfo::refresh() {
	if (_parsed) {
		LDEBUG("StreamsInfo", "Refresh streams information!");
		refreshImpl();
	} else {
		LDEBUG("StreamsInfo", "Cannot refresh streams information, streams information is not parsed!");
	}
}

void StreamsInfo::reset() {
	LDEBUG("StreamsInfo", "Reset streams information!");
	CLEAN_ALL(AudioStreamInfo*, _audioInfo);
	CLEAN_ALL(VideoStreamInfo*, _videoInfo);
	CLEAN_ALL(SubtitleStreamInfo*, _subtitleInfo);
	_currentAudio = -1;
	_currentVideo = -1;
	_currentSubtitle = -1;
	_parsed=false;
	resetImpl();
}

void StreamsInfo::resetImpl() {
}

void StreamsInfo::addVideoInfo( VideoStreamInfo *sInfo ) {
	_videoInfo.push_back(sInfo);
	LTRACE("StreamsInfo", "Added VideoStreamInfo[%p], video_count=%u", sInfo, videoCount());
}

void StreamsInfo::addAudioInfo( AudioStreamInfo *sInfo ) {
	_audioInfo.push_back(sInfo);
	LTRACE("StreamsInfo", "Added AudioStreamInfo[%p], audio_count=%u", sInfo, audioCount());
}

void StreamsInfo::addSubtitleInfo( SubtitleStreamInfo *sInfo ) {
	_subtitleInfo.push_back(sInfo);
	LTRACE("StreamsInfo", "Added SubtitleStreamInfo[%p], subtitle_count=%u", sInfo, subtitleCount());
}

VideoStreamInfo *StreamsInfo::videoInfo( size_t id ) {
	DTV_ASSERT(id < videoCount());
	return _videoInfo.at(id);
}

AudioStreamInfo *StreamsInfo::audioInfo( size_t id ) {
	DTV_ASSERT(id < audioCount());
	return _audioInfo.at(id);
}

SubtitleStreamInfo *StreamsInfo::subtitleInfo( size_t id ) {
	DTV_ASSERT(id < subtitleCount());
	return _subtitleInfo.at(id);
}

const VideoStreamInfo *StreamsInfo::videoStreamInfo( int id/*=-1*/ ) const {
	if (id < 0) {
		id = currentVideo();
	}
	if (id >= 0 && id < (int)videoCount()) {
		const std::vector<VideoStreamInfo*> &tmp = videoInfo();
		return tmp.at(id);
	}
	return NULL;
}

const AudioStreamInfo *StreamsInfo::audioStreamInfo( int id/*=-1*/ ) const {
	if (id < 0) {
		id = currentAudio();
	}
	if (id >= 0 && id < (int)audioCount()) {
		const std::vector<AudioStreamInfo*> &tmp = audioInfo();
		return tmp.at(id);
	}
	return NULL;
}

const SubtitleStreamInfo *StreamsInfo::subtitleStreamInfo( int id/*=-1*/ ) const {
	if (id < 0) {
		id = currentSubtitle();
	}
	if (id >= 0 && id < (int)subtitleCount()) {
		const std::vector<SubtitleStreamInfo*> &tmp = subtitleInfo();
		return tmp.at(id);
	}
	return NULL;
}

}
