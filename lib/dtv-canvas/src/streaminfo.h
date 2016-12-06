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

#include "size.h"
#include <string>
#include <vector>

namespace canvas {

class StreamInfo {
public:
	virtual ~StreamInfo() {}

	void name( const std::string &name );
	std::string name() const;

	void language( const std::string &lan );
	std::string language() const;

	void codecName( const std::string &codec );
	std::string codecName() const;

private:
	std::string _name;
	std::string _language;
	std::string _codecName;
};

class VideoStreamInfo : public StreamInfo {
public:
	virtual ~VideoStreamInfo() {}

	void bitrate( int bitrate );
	int bitrate() const;

	void size( const Size &size );
	Size size() const;

	void framerate( int framerate );
	int framerate() const;

	void interleaved( bool isInterleaved );
	bool isInterleaved() const;

	void aspectRatio( float aspect );
	float aspectRatio() const;

private:
	int _bitrate;
	Size _size;
	float _aspectRatio;
	int _framerate;
	bool _interleaved;
};

class AudioStreamInfo : public StreamInfo {
public:
	virtual ~AudioStreamInfo() {}

	void bitrate( int bitrate );
	int bitrate() const;

	void channels( int channels );
	int channels() const;

	void samplerate( int samplerate );
	int samplerate() const;

	void bitspersample( int bps );
	int bitspersample() const;

private:
	int _bitrate;
	int _channels;
	int _samplerate;
	int _bitspersample;
};

class SubtitleStreamInfo : public StreamInfo {
public:
	virtual ~SubtitleStreamInfo() {}

	void encoding( const std::string &enc );
	std::string encoding() const;

private:
	std::string _encoding;
};

/**
 * Clase usada por MediaPlayer para retornar información de los streams en reproducción.
 */
class StreamsInfo {
public:
	StreamsInfo();
	virtual ~StreamsInfo();

	//	Getters
	virtual int currentAudio() const;
	size_t audioCount() const;
	virtual const std::vector<AudioStreamInfo*> &audioInfo() const;
	const AudioStreamInfo *audioStreamInfo( int id=-1 ) const;

	virtual int currentVideo() const;
	size_t videoCount() const;
	virtual const std::vector<VideoStreamInfo*> &videoInfo() const;
	const VideoStreamInfo *videoStreamInfo( int id=-1 ) const;

	virtual int currentSubtitle() const;
	size_t subtitleCount() const;
	virtual const std::vector<SubtitleStreamInfo*> &subtitleInfo() const;
	const SubtitleStreamInfo *subtitleStreamInfo( int id=-1 ) const;

	//	Implementation. Stream info modification
	void parse();
	void refresh();
	void reset();
	void currentAudio( int id );
	void currentVideo( int id );
	void currentSubtitle( int id );

protected:
	virtual void parseImpl()=0;
	virtual void refreshImpl()=0;
	virtual void resetImpl();

	void addVideoInfo( VideoStreamInfo *sInfo );
	void addAudioInfo( AudioStreamInfo *sInfo );
	void addSubtitleInfo( SubtitleStreamInfo *sInfo );
	VideoStreamInfo *videoInfo( size_t id );
	AudioStreamInfo *audioInfo( size_t id );
	SubtitleStreamInfo *subtitleInfo( size_t id );

private:
	int _currentAudio;
	int _currentVideo;
	int _currentSubtitle;
	std::vector<AudioStreamInfo*> _audioInfo;
	std::vector<VideoStreamInfo*> _videoInfo;
	std::vector<SubtitleStreamInfo*> _subtitleInfo;
	bool _parsed;
};

}
