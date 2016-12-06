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
#include <boost/thread.hpp>
#include <string>

namespace canvas {

class System;

namespace ffmpeg {

class PacketQueue;

class Stream {
public:
	Stream( System *sys, stream::type type );
	virtual ~Stream();

	bool open( AVFormatContext *context );
	void close();
	bool isOpen() const;
	int index() const;

	void start();
	void stop();

	bool push( struct AVPacket *pkt );
	bool isFull() const;
	double clock() const;

	stream::type type() const;

protected:
	//	Aux codec
	bool openCodec( AVCodecContext *avctx );
	virtual bool initCodec( AVCodecContext *avctx, AVCodec *codec );
	virtual void finCodec( AVCodecContext *avctx );

	//	Aux streams
	virtual void beginProcess();
	virtual void processPkt( AVFrame *frame, AVPacket *pkt );
	virtual void endProcess();
	AVStream *stream() const;
	AVFormatContext *context() const;
	double updatePTS( AVFrame *frame );

	//	Aux
	void processData();
	System *sys() const;

private:
	System *_sys;
	stream::type _type;
	bool _initialized;
	AVFormatContext *_context;

	//	Stream
	int _index;
	AVStream *_stream;
	PacketQueue *_queue;

	//	Codec
	std::string _codecName;

	//	Clock
	double _currentPtsDrift;

	//	Thread
	boost::thread _thread;
};

}
}

