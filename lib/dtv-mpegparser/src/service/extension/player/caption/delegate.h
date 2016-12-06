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

#pragma once

#include "../delegate.h"
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <list>

//#define DEBUG_CAPTION

namespace tuner {

namespace stream {
	class Pipe;
	class BasicPipe;
}

namespace arib {
	class CaptionData;
	typedef CaptionData Statement;
	class Management;
	class CaptionDemuxer;
}

namespace player {
namespace es {
namespace cc {

namespace pt = boost::posix_time;

class Viewer;

class Delegate : public player::Delegate {
public:
	explicit Delegate( Viewer *view );
	virtual ~Delegate();

	//	Streams
	virtual StreamInfo *canPlay( const ElementaryInfo &info, ID tag ) const;
	virtual void start();
	virtual void stop();

protected:
	virtual bool init( );
	virtual void fin();
	virtual pes::mode::type mode() const;
	virtual stream::Pipe *pipe( pes::type::type esType ) const;

	void onShow( const std::string &data );

	//	Aux ARIB parsing
	void processUnit( const util::Buffer *buf );
	void endLine();
	int processControlCode( util::BYTE *data, int len );
	int processCSI( util::BYTE *data, int len );
	int processChar( util::BYTE *data, int len );

	//	Commands
	void setCharacterSmallSize();
	void setCharacterMiddleSize();
	void setCharacterNormalSize();
	void setActivePosition( int p1, int p2 );
	void setActivePositionForward();
	void setActivePositionBackward();
	void setActivePositionDown();
	void setActivePositionUp();
	void setColour( int p1 );
	void setForegroundColour( util::BYTE r, util::BYTE g, util::BYTE b, util::BYTE alpha=255 );
	void setBackgroundColour( util::BYTE r, util::BYTE g, util::BYTE b, util::BYTE alpha=255 );

	//	CSI commands
	void setWritingFormat( int *params, int count );
	void setWritingForm( int form );
	void setDisplayPosition( int horizontal, int vertical );
	void setDisplayFormat( int horizontal, int vertical );
	void setCharacterComposition( int horizontal, int vertical );
	void setHorizontalSpacing( int value );
	void setVerticalSpacing( int value );
	void setRasterColour( int value );

	//	Aux demuxer
	void onManagement( tuner::arib::CaptionDemuxer *demux, tuner::arib::Management *mgt );
	void onStatement( tuner::arib::Statement *st );
	void playCaptions();

private:
	Viewer *_view;
	tuner::stream::BasicPipe *_pipe;
	boost::mutex _mutex;
	boost::thread _thread;
	bool _exit;
	bool _running;
	std::vector<std::string> _lines;
	std::string _text;
	std::list<tuner::arib::Statement *> _sts;
#ifdef DEBUG_CAPTION
	std::vector<std::string> _debugData;
#endif
	player::StreamInfo *_superImposeStream;
};

}	//	namespace cc
}	//	namespace es
}	//	namespace player
}	//	namespace tuner

