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

static void mpCallback( int *count ) {
	(*count)++;
}

TEST_F( MediaPlayerTest, streamsInfo_count ) {
	_mp->play("test.avi");
	
	ASSERT_EQ(0, _testData.nRefresh);
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(0, _testData.nRefresh);
	ASSERT_EQ(0, streams.videoCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( false );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, _testData.nRefresh);
	ASSERT_EQ(0, streams2.videoCount());
}

TEST_F( MediaPlayerTest, stream_info_before_play ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	const canvas::StreamsInfo &streams = _mp->streamsInfo();

	ASSERT_EQ(0, streams.videoCount());
	ASSERT_EQ(streams.currentVideo(), -1);

	ASSERT_EQ(0, streams.audioCount());
	ASSERT_EQ(streams.currentAudio(), -1);

	ASSERT_EQ(0, streams.subtitleCount());
	ASSERT_EQ(streams.currentSubtitle(), -1);

	ASSERT_EQ(0, parsed);
}

TEST_F( MediaPlayerTest, stream_info_before_parsed ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );
	_mp->play("test.avi");
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();

	ASSERT_EQ(0, streams.videoCount());
	ASSERT_EQ(streams.currentVideo(), -1);

	ASSERT_EQ(0, streams.audioCount());
	ASSERT_EQ(streams.currentAudio(), -1);

	ASSERT_EQ(0, streams.subtitleCount());
	ASSERT_EQ(streams.currentSubtitle(), -1);

	ASSERT_EQ(0, parsed);
}

TEST_F( MediaPlayerTest, stream_info_after_parsed ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );
	_mp->play("test.avi");
	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	ASSERT_EQ(1, parsed);

	const canvas::StreamsInfo &streams = _mp->streamsInfo();

	ASSERT_EQ(3, streams.videoCount());
	ASSERT_EQ(streams.currentVideo(), 0);

	ASSERT_EQ(2, streams.audioCount());
	ASSERT_EQ(streams.currentAudio(), 0);

	ASSERT_EQ(4, streams.subtitleCount());
	ASSERT_EQ(streams.currentSubtitle(), 0);
}

TEST_F( MediaPlayerTest, stream_info_after_parsed_empty ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );
	_mp->play("test.avi");
	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( false );
	ASSERT_EQ(1, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();

	ASSERT_EQ(0, streams.videoCount());
	ASSERT_EQ(streams.currentVideo(), -1);

	ASSERT_EQ(0, streams.audioCount());
	ASSERT_EQ(streams.currentAudio(), -1);

	ASSERT_EQ(0, streams.subtitleCount());
	ASSERT_EQ(streams.currentSubtitle(), -1);
}

TEST_F( MediaPlayerTest, has_video ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	
	ASSERT_EQ(0, streams.videoCount());

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_EQ(0, streams.videoCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	ASSERT_TRUE(streams2.videoCount() > 0);
}

TEST_F( MediaPlayerTest, has_video_empty ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	
	ASSERT_EQ(0, streams.videoCount());

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_EQ(0, streams.videoCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( false );
	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	ASSERT_EQ(0, streams2.videoCount());
}

TEST_F( MediaPlayerTest, video_stream_count ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();

	ASSERT_EQ(0, streams.videoCount());

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_EQ(0, streams.videoCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	ASSERT_EQ(3, streams2.videoCount());
}

TEST_F( MediaPlayerTest, video_stream_count_empty ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();

	ASSERT_EQ(0, streams.videoCount());

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_EQ(0, streams.videoCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( false );
	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	ASSERT_EQ(0, streams2.videoCount());
}

TEST_F( MediaPlayerTest, switch_video_stream ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchVideoStream(2));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchVideoStream(2));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(0, streams.currentVideo());
	ASSERT_EQ(1, parsed);
	ASSERT_TRUE(_mp->switchVideoStream(2));
	ASSERT_EQ(2, streams.currentVideo());
	ASSERT_EQ(1, _testData.videoSwitched);
}

TEST_F( MediaPlayerTest, switch_video_stream_invalid ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchVideoStream(2));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchVideoStream(2));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(0, streams.currentVideo());
	ASSERT_EQ(1, parsed);
	ASSERT_FALSE(_mp->switchVideoStream(5));
	ASSERT_EQ(0, streams.currentVideo());
	ASSERT_EQ(0, _testData.videoSwitched);
}

TEST_F( MediaPlayerTest, switch_video_stream_invalid2 ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchVideoStream(2));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchVideoStream(2));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(0, streams.currentVideo());
	ASSERT_EQ(1, parsed);
	ASSERT_FALSE(_mp->switchVideoStream(-1));
	ASSERT_EQ(0, streams.currentVideo());
	ASSERT_EQ(0, _testData.videoSwitched);
}

TEST_F( MediaPlayerTest, switch_video_stream_empty ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchVideoStream(2));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchVideoStream(2));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( false );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	
	ASSERT_EQ(-1, streams.currentVideo());
	ASSERT_EQ(1, parsed);
	ASSERT_FALSE(_mp->switchVideoStream(2));
	ASSERT_EQ(-1, streams.currentVideo());
	ASSERT_EQ(0, _testData.videoSwitched);
}

TEST_F( MediaPlayerTest, switch_video_stream_stopped ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchVideoStream(2));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchVideoStream(2));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(0, streams.currentVideo());
	ASSERT_EQ(1, parsed);
	_mp->stop();
	ASSERT_FALSE(_mp->switchVideoStream(2));
	ASSERT_EQ(-1, streams.currentVideo());
	ASSERT_EQ(0, _testData.videoSwitched);
}

TEST_F( MediaPlayerTest, switch_video_stream_paused ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchVideoStream(2));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchVideoStream(2));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(0, streams.currentVideo());
	ASSERT_EQ(1, parsed);
	_mp->pause();
	ASSERT_TRUE(_mp->switchVideoStream(2));
	ASSERT_EQ(2, streams.currentVideo());
	ASSERT_EQ(1, _testData.videoSwitched);
}

TEST_F( MediaPlayerTest, video_stream_info ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentVideo());
	ASSERT_EQ(0, streams.videoCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	const std::vector<canvas::VideoStreamInfo*> &vStreams = streams2.videoInfo();
	canvas::VideoStreamInfo *vInfo = vStreams.at(0);

	ASSERT_EQ( 7200000, vInfo->bitrate() );
	ASSERT_EQ( 2.4f, vInfo->aspectRatio() );
	ASSERT_EQ( 1920, vInfo->size().w );
	ASSERT_EQ( 800, vInfo->size().h );
	ASSERT_EQ( 30, vInfo->framerate() );
	ASSERT_TRUE( vInfo->isInterleaved() );
	ASSERT_STREQ( "Español", vInfo->language().c_str() );
	ASSERT_STREQ( "Video Test 1", vInfo->name().c_str() );
	ASSERT_STREQ( "MP4", vInfo->codecName().c_str() );

	vInfo = vStreams.at(1);
	ASSERT_EQ( 3600000, vInfo->bitrate() );
	ASSERT_EQ( 2.1f, vInfo->aspectRatio() );
	ASSERT_EQ( 720, vInfo->size().w );
	ASSERT_EQ( 576, vInfo->size().h );
	ASSERT_EQ( 25, vInfo->framerate() );
	ASSERT_TRUE( vInfo->isInterleaved() );
	ASSERT_STREQ( "English", vInfo->language().c_str() );
	ASSERT_STREQ( "Video Test 2", vInfo->name().c_str() );
	ASSERT_STREQ( "MPEG", vInfo->codecName().c_str() );
	
	vInfo = vStreams.at(2);
	ASSERT_EQ( 4700000, vInfo->bitrate() );
	ASSERT_EQ( 1.33f, vInfo->aspectRatio() );
	ASSERT_EQ( 320, vInfo->size().w );
	ASSERT_EQ( 240, vInfo->size().h );
	ASSERT_EQ( 60, vInfo->framerate() );
	ASSERT_FALSE( vInfo->isInterleaved() );
	ASSERT_STREQ( "Chinese", vInfo->language().c_str() );
	ASSERT_STREQ( "Video Test 3", vInfo->name().c_str() );
	ASSERT_STREQ( "AVI", vInfo->codecName().c_str() );
}

TEST_F( MediaPlayerTest, video_stream_position ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentVideo());
	ASSERT_EQ(0, streams.videoCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	const canvas::VideoStreamInfo *vInfo = streams2.videoStreamInfo(2);
	
	ASSERT_EQ( 4700000, vInfo->bitrate() );
	ASSERT_EQ( 1.33f, vInfo->aspectRatio() );
	ASSERT_EQ( 320, vInfo->size().w );
	ASSERT_EQ( 240, vInfo->size().h );
	ASSERT_EQ( 60, vInfo->framerate() );
	ASSERT_FALSE( vInfo->isInterleaved() );
	ASSERT_STREQ( "Chinese", vInfo->language().c_str() );
	ASSERT_STREQ( "Video Test 3", vInfo->name().c_str() );
	ASSERT_STREQ( "AVI", vInfo->codecName().c_str() );
}

TEST_F( MediaPlayerTest, video_stream_position_default ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentVideo());
	ASSERT_EQ(0, streams.videoCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	const canvas::VideoStreamInfo *vInfo = streams2.videoStreamInfo();
	
	ASSERT_EQ( 7200000, vInfo->bitrate() );
	ASSERT_EQ( 2.4f, vInfo->aspectRatio() );
	ASSERT_EQ( 1920, vInfo->size().w );
	ASSERT_EQ( 800, vInfo->size().h );
	ASSERT_EQ( 30, vInfo->framerate() );
	ASSERT_TRUE( vInfo->isInterleaved() );
	ASSERT_STREQ( "Español", vInfo->language().c_str() );
	ASSERT_STREQ( "Video Test 1", vInfo->name().c_str() );
	ASSERT_STREQ( "MP4", vInfo->codecName().c_str() );
}

TEST_F( MediaPlayerTest, video_stream_position_negative ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentVideo());
	ASSERT_EQ(0, streams.videoCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	const canvas::VideoStreamInfo *vInfo = streams2.videoStreamInfo(-5);
	
	ASSERT_EQ( 7200000, vInfo->bitrate() );
	ASSERT_EQ( 2.4f, vInfo->aspectRatio() );
	ASSERT_EQ( 1920, vInfo->size().w );
	ASSERT_EQ( 800, vInfo->size().h );
	ASSERT_EQ( 30, vInfo->framerate() );
	ASSERT_TRUE( vInfo->isInterleaved() );
	ASSERT_STREQ( "Español", vInfo->language().c_str() );
	ASSERT_STREQ( "Video Test 1", vInfo->name().c_str() );
	ASSERT_STREQ( "MP4", vInfo->codecName().c_str() );
}

TEST_F( MediaPlayerTest, video_stream_position_invalid ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentVideo());
	ASSERT_EQ(0, streams.videoCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	const canvas::VideoStreamInfo *vInfo = streams2.videoStreamInfo(8);

	ASSERT_EQ( NULL, vInfo );
}

TEST_F( MediaPlayerTest, has_audio ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	
	ASSERT_EQ(0, streams.audioCount());

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_EQ(0, streams.audioCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	ASSERT_TRUE(streams2.audioCount() > 0);
}

TEST_F( MediaPlayerTest, has_audio_empty ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	
	ASSERT_EQ(0, streams.audioCount());

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_EQ(0, streams.audioCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( false );
	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	ASSERT_EQ(0, streams2.audioCount());
}

TEST_F( MediaPlayerTest, audio_stream_count ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	
	ASSERT_EQ(0, streams.audioCount());

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_EQ(0, streams.audioCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	ASSERT_EQ(2, streams2.audioCount());
}

TEST_F( MediaPlayerTest, switch_audio_stream ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchAudioStream(1));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchAudioStream(1));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(0, streams.currentAudio());
	ASSERT_EQ(1, parsed);
	ASSERT_TRUE(_mp->switchAudioStream(1));
	ASSERT_EQ(1, streams.currentAudio());
	ASSERT_EQ(1, _testData.audioSwitched);
}

TEST_F( MediaPlayerTest, switch_audio_stream_empty ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchAudioStream(1));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchAudioStream(1));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( false );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	
	ASSERT_EQ(-1, streams.currentAudio());
	ASSERT_EQ(1, parsed);
	ASSERT_FALSE(_mp->switchAudioStream(2));
	ASSERT_EQ(-1, streams.currentAudio());
	ASSERT_EQ(0, _testData.audioSwitched);
}

TEST_F( MediaPlayerTest, switch_audio_stream_invalid ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchAudioStream(1));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchAudioStream(1));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(0, streams.currentAudio());
	ASSERT_EQ(1, parsed);
	ASSERT_FALSE(_mp->switchAudioStream(2));
	ASSERT_EQ(0, streams.currentAudio());
	ASSERT_EQ(0, _testData.audioSwitched);
}

TEST_F( MediaPlayerTest, switch_audio_stream_stopped ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchAudioStream(1));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchAudioStream(1));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(0, streams.currentAudio());
	ASSERT_EQ(1, parsed);
	_mp->stop();
	ASSERT_FALSE(_mp->switchAudioStream(1));
	ASSERT_EQ(-1, streams.currentAudio());
	ASSERT_EQ(0, _testData.audioSwitched);
}

TEST_F( MediaPlayerTest, switch_audio_stream_paused ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchAudioStream(1));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchAudioStream(1));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(0, streams.currentAudio());
	ASSERT_EQ(1, parsed);
	_mp->pause();
	ASSERT_TRUE(_mp->switchAudioStream(1));
	ASSERT_EQ(1, streams.currentAudio());
	ASSERT_EQ(1, _testData.audioSwitched);
}

TEST_F( MediaPlayerTest, audio_stream_info ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentAudio());
	ASSERT_EQ(0, streams.audioCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	const std::vector<canvas::AudioStreamInfo*> &aStreams = streams2.audioInfo();
	canvas::AudioStreamInfo *aInfo = aStreams.at(0);

	ASSERT_EQ( 256000, aInfo->bitrate() );
	ASSERT_EQ( 2, aInfo->channels() );
	ASSERT_EQ( 44100, aInfo->samplerate() );
	ASSERT_EQ( 16, aInfo->bitspersample() );
	ASSERT_STREQ( "Español", aInfo->language().c_str() );
	ASSERT_STREQ( "Audio Test 1", aInfo->name().c_str() );
	ASSERT_STREQ( "MP3", aInfo->codecName().c_str() );

	aInfo = aStreams.at(1);
	ASSERT_EQ( 128000, aInfo->bitrate() );
	ASSERT_EQ( 2, aInfo->channels() );
	ASSERT_EQ( 44100, aInfo->samplerate() );
	ASSERT_EQ( 24, aInfo->bitspersample() );
	ASSERT_STREQ( "English", aInfo->language().c_str() );
	ASSERT_STREQ( "Audio Test 2", aInfo->name().c_str() );
	ASSERT_STREQ( "AAC", aInfo->codecName().c_str() );
}

TEST_F( MediaPlayerTest, audio_stream_position ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentAudio());
	ASSERT_EQ(0, streams.audioCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	const canvas::AudioStreamInfo *aInfo = streams2.audioStreamInfo(1);

	ASSERT_EQ( 128000, aInfo->bitrate() );
	ASSERT_EQ( 2, aInfo->channels() );
	ASSERT_EQ( 44100, aInfo->samplerate() );
	ASSERT_EQ( 24, aInfo->bitspersample() );
	ASSERT_STREQ( "English", aInfo->language().c_str() );
	ASSERT_STREQ( "Audio Test 2", aInfo->name().c_str() );
	ASSERT_STREQ( "AAC", aInfo->codecName().c_str() );
}

TEST_F( MediaPlayerTest, audio_stream_position_default ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentAudio());
	ASSERT_EQ(0, streams.audioCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	const canvas::AudioStreamInfo *aInfo = streams2.audioStreamInfo();
	
	ASSERT_EQ( 256000, aInfo->bitrate() );
	ASSERT_EQ( 2, aInfo->channels() );
	ASSERT_EQ( 44100, aInfo->samplerate() );
	ASSERT_EQ( 16, aInfo->bitspersample() );
	ASSERT_STREQ( "Español", aInfo->language().c_str() );
	ASSERT_STREQ( "Audio Test 1", aInfo->name().c_str() );
	ASSERT_STREQ( "MP3", aInfo->codecName().c_str() );
}

TEST_F( MediaPlayerTest, audio_stream_position_negative ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentAudio());
	ASSERT_EQ(0, streams.audioCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	const canvas::AudioStreamInfo *aInfo = streams2.audioStreamInfo(-5);
	
	ASSERT_EQ( 256000, aInfo->bitrate() );
	ASSERT_EQ( 2, aInfo->channels() );
	ASSERT_EQ( 44100, aInfo->samplerate() );
	ASSERT_EQ( 16, aInfo->bitspersample() );
	ASSERT_STREQ( "Español", aInfo->language().c_str() );
	ASSERT_STREQ( "Audio Test 1", aInfo->name().c_str() );
	ASSERT_STREQ( "MP3", aInfo->codecName().c_str() );
}

TEST_F( MediaPlayerTest, audio_stream_position_invalid ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentAudio());
	ASSERT_EQ(0, streams.audioCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	const canvas::AudioStreamInfo *aInfo = streams2.audioStreamInfo(8);

	ASSERT_EQ( NULL, aInfo );
}

TEST_F( MediaPlayerTest, has_subtitle ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	
	ASSERT_EQ(0, streams.subtitleCount());

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_EQ(0, streams.subtitleCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	ASSERT_TRUE(streams2.subtitleCount() > 0);
}

TEST_F( MediaPlayerTest, has_subtitle_empty ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	
	ASSERT_EQ(0, streams.subtitleCount());

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_EQ(0, streams.subtitleCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( false );
	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	ASSERT_EQ(0, streams2.subtitleCount());
}

TEST_F( MediaPlayerTest, subtitle_stream_count ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	
	ASSERT_EQ(0, streams.subtitleCount());

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_EQ(0, streams.subtitleCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	ASSERT_EQ(4, streams2.subtitleCount());
}

TEST_F( MediaPlayerTest, switch_subtitle_stream ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchSubtitleStream(1));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchSubtitleStream(1));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(0, streams.currentSubtitle());
	ASSERT_EQ(1, parsed);
	ASSERT_TRUE(_mp->switchSubtitleStream(1));
	ASSERT_EQ(1, streams.currentSubtitle());
	ASSERT_EQ(1, _testData.subtitleSwitched);
}

TEST_F( MediaPlayerTest, switch_subtitle_stream_invalid ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchSubtitleStream(1));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchSubtitleStream(1));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(0, streams.currentSubtitle());
	ASSERT_EQ(1, parsed);
	ASSERT_FALSE(_mp->switchSubtitleStream(8));
	ASSERT_EQ(0, streams.currentSubtitle());
	ASSERT_EQ(0, _testData.subtitleSwitched);
}

TEST_F( MediaPlayerTest, switch_subtitle_stream_invalid2 ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchSubtitleStream(1));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchSubtitleStream(1));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(0, streams.currentSubtitle());
	ASSERT_EQ(1, parsed);
	ASSERT_FALSE(_mp->switchSubtitleStream(-1));
	ASSERT_EQ(0, streams.currentSubtitle());
	ASSERT_EQ(0, _testData.subtitleSwitched);
}

TEST_F( MediaPlayerTest, switch_subtitle_stream_empty ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchSubtitleStream(1));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchSubtitleStream(1));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( false );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentSubtitle());
	ASSERT_EQ(1, parsed);
	ASSERT_FALSE(_mp->switchSubtitleStream(1));
	ASSERT_EQ(-1, streams.currentSubtitle());
	ASSERT_EQ(0, _testData.subtitleSwitched);
}

TEST_F( MediaPlayerTest, switch_subtitle_stream_stopped ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchSubtitleStream(2));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchSubtitleStream(2));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(0, streams.currentSubtitle());
	ASSERT_EQ(1, parsed);
	_mp->stop();
	ASSERT_FALSE(_mp->switchSubtitleStream(2));
	ASSERT_EQ(-1, streams.currentSubtitle());
	ASSERT_EQ(0, _testData.subtitleSwitched);
}

TEST_F( MediaPlayerTest, switch_subtitle_stream_paused ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(_mp->switchSubtitleStream(2));

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(_mp->switchSubtitleStream(2));

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(0, streams.currentSubtitle());
	ASSERT_EQ(1, parsed);
	_mp->pause();
	ASSERT_TRUE(_mp->switchSubtitleStream(2));
	ASSERT_EQ(2, streams.currentSubtitle());
	ASSERT_EQ(1, _testData.subtitleSwitched);
}

TEST_F( MediaPlayerTest, subtitle_stream_info ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentSubtitle());
	ASSERT_EQ(0, streams.subtitleCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	const std::vector<canvas::SubtitleStreamInfo*> &sStreams = streams2.subtitleInfo();
	canvas::SubtitleStreamInfo *sInfo = sStreams.at(0);

	ASSERT_STREQ( "ISO-8859-1", sInfo->encoding().c_str() );
	ASSERT_STREQ( "Español", sInfo->language().c_str() );
	ASSERT_STREQ( "Subtitle Test 1", sInfo->name().c_str() );
	ASSERT_STREQ( "SUB", sInfo->codecName().c_str() );

	sInfo = sStreams.at(1);
	ASSERT_STREQ( "ISO-8859-2", sInfo->encoding().c_str() );
	ASSERT_STREQ( "English", sInfo->language().c_str() );
	ASSERT_STREQ( "Subtitle Test 2", sInfo->name().c_str() );
	ASSERT_STREQ( "SRT", sInfo->codecName().c_str() );
	
	sInfo = sStreams.at(2);
	ASSERT_STREQ( "ISO-8859-3", sInfo->encoding().c_str() );
	ASSERT_STREQ( "Chinese", sInfo->language().c_str() );
	ASSERT_STREQ( "Subtitle Test 3", sInfo->name().c_str() );
	ASSERT_STREQ( "SRT", sInfo->codecName().c_str() );
	
	sInfo = sStreams.at(3);
	ASSERT_STREQ( "ISO-8859-4", sInfo->encoding().c_str() );
	ASSERT_STREQ( "Arabic", sInfo->language().c_str() );
	ASSERT_STREQ( "Subtitle Test 4", sInfo->name().c_str() );
	ASSERT_STREQ( "SRT", sInfo->codecName().c_str() );
}

TEST_F( MediaPlayerTest, subtitle_stream_info_empty ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentSubtitle());
	ASSERT_EQ(0, streams.subtitleCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( false );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	ASSERT_EQ(0, streams2.subtitleCount());
	const std::vector<canvas::SubtitleStreamInfo*> &sStreams = streams2.subtitleInfo();
	ASSERT_EQ(0, sStreams.size());
}

TEST_F( MediaPlayerTest, subtitle_stream_position ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentSubtitle());
	ASSERT_EQ(0, streams.subtitleCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	const canvas::SubtitleStreamInfo *sInfo = streams2.subtitleStreamInfo(1);

	ASSERT_STREQ( "ISO-8859-2", sInfo->encoding().c_str() );
	ASSERT_STREQ( "English", sInfo->language().c_str() );
	ASSERT_STREQ( "Subtitle Test 2", sInfo->name().c_str() );
	ASSERT_STREQ( "SRT", sInfo->codecName().c_str() );
}

TEST_F( MediaPlayerTest, subtitle_stream_position_default ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentSubtitle());
	ASSERT_EQ(0, streams.subtitleCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	const canvas::SubtitleStreamInfo *sInfo = streams2.subtitleStreamInfo();
	
	ASSERT_STREQ( "ISO-8859-1", sInfo->encoding().c_str() );
	ASSERT_STREQ( "Español", sInfo->language().c_str() );
	ASSERT_STREQ( "Subtitle Test 1", sInfo->name().c_str() );
	ASSERT_STREQ( "SUB", sInfo->codecName().c_str() );
}

TEST_F( MediaPlayerTest, subtitle_stream_position_negative ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentSubtitle());
	ASSERT_EQ(0, streams.subtitleCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	const canvas::SubtitleStreamInfo *sInfo = streams2.subtitleStreamInfo(-5);
	
	ASSERT_STREQ( "ISO-8859-1", sInfo->encoding().c_str() );
	ASSERT_STREQ( "Español", sInfo->language().c_str() );
	ASSERT_STREQ( "Subtitle Test 1", sInfo->name().c_str() );
	ASSERT_STREQ( "SUB", sInfo->codecName().c_str() );
}

TEST_F( MediaPlayerTest, subtitle_stream_position_invalid ) {
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentSubtitle());
	ASSERT_EQ(0, streams.subtitleCount());

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );

	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(1, parsed);
	const canvas::SubtitleStreamInfo *sInfo = streams2.subtitleStreamInfo(8);

	ASSERT_EQ( NULL, sInfo );
}