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

#include "types.h"
#include "system.h"
#include "inputmanager.h"
#include "window.h"
#include <cmath>

namespace canvas {

namespace input {

void dispatchButtonEvent( System *sys, ButtonEvent *evt ) {
	if (sys->window()->translateToCanvas( evt->x, evt->y )) {
		sys->input()->dispatchButton( evt );
	}
}

}

namespace connector {

#define DO_CREATE_ST_CONNECTORS(n,i) #i,
static const char *connectors[] = {
	"Unknown",
	CONNECTOR_LIST(DO_CREATE_ST_CONNECTORS)
	NULL
};
#undef DO_CREATE_ST_CONNECTORS

const char *name( type conn ) {
	if (conn >= LAST_CONNECTOR) {
		conn=unknown;
	}
	return connectors[conn];
}

}	// namespace connector

namespace aspect {

//	Aspect ratio
#define DO_CREATE_ST_ASPECTS(n,x,y) { #x ":" #y, x, y },
static Aspect aspects[] = {
	{ "Unknown", 0, 0 },
	{ "Auto", -1, -1 },
	ASPECT_RATIO_LIST(DO_CREATE_ST_ASPECTS)
	{ NULL, 0, 0 }
};
#undef DO_CREATE_ST_ASPECTS

Aspect &get( type vAspect ) {
	if (vAspect >= LAST_ASPECT) {
		vAspect=unknown;
	}
	return aspects[vAspect];
}

type get( float ratio ) {
	type aspect = (type)(unknown+2);
	type bestAspect = unknown;

	// This initialization is because of sar (sample aspect ratio) value is not mathematicaly perfect
	float minDiff = 0.05f;

	while (aspect<LAST_ASPECT) {
		Aspect &a = get(aspect);
		float r = float(a.x) / float(a.y);
		float diff = std::abs(r-ratio);
		if (diff<minDiff) {
			minDiff = diff;
			bestAspect = aspect;
		}
		aspect = type(aspect+1);
	}
	return bestAspect;
}

type get( const VideoStreamInfo &vInfo ) {
	float aspect = vInfo.aspectRatio();
	if (aspect <= 0.0f) {
		Size tmp = vInfo.size();
		aspect = (float) tmp.w / (float) tmp.h;
	}
	return get(aspect);
}

}	// namespace aspect

namespace mode {

//	Modes
#define DO_NAME(s) #s
#define DO_CREATE_ST_MODES(n,w,h,f,t) { DO_NAME(w##x##h##t) "@" #f, DO_NAME(h##t), w, h, f, scan::t },
static Mode modes[] = {
	{ "Unknown", "Unknown", 0, 0, 0, scan::unknown },
	MODE_LIST(DO_CREATE_ST_MODES)
	{ NULL, NULL, 0, 0, 0, scan::unknown }
};
#undef DO_CREATE_ST_MODES

//	Public methods
Mode &get( type vMode ) {
	if (vMode >= LAST_MODE) {
		vMode=unknown;
	}
	return modes[vMode];
}

#define MODE_SIZE_MAX_DIFF 10
type get( int w, int h, int f, scan::type t ) {
	type mode = (type)(unknown+1);
	while (mode<LAST_MODE) {
		Mode &m = get(mode);
		bool check=true;
		check &= abs(m.width-w) <= MODE_SIZE_MAX_DIFF;
		check &= abs(m.height-h) <= MODE_SIZE_MAX_DIFF;
		check &= (m.frequency == f);
		check &= (m.type == t);
		if (check) {
			return mode;
		}
		mode = (type)(mode+1);
	}
	return unknown;
}
#undef MODE_SIZE_MAX_DIFF

type get( const VideoStreamInfo &vInfo ) {
	Size s = vInfo.size();
	return get(s.w, s.h, vInfo.framerate(), vInfo.isInterleaved() ? scan::i : scan::p );
}

}	// namespace mode

namespace audio {

#define DO_ENUM_AUDIO_CHANNEL(n,c,k,l) { #c, k, l },
static Channel channels[] = {
	{ "Unknown", 0, 0 },
	AUDIO_CHANNEL_LIST(DO_ENUM_AUDIO_CHANNEL)
	{ NULL, 0, 0 }
};
#undef DO_ENUM_AUDIO_CHANNEL

Channel &getChannelInfo( channel::type ch ) {
	if (ch >= channel::LAST_CHANNEL) {
		ch=channel::unknown;
	}
	return channels[ch];
}

channel::type get( const AudioStreamInfo &aInfo ) {
	channel::type type = channel::unknown;
	int nChannels = aInfo.channels();
	if (nChannels==1) {
		type = channel::mono;
	}
	else if (nChannels==2) {
		type = channel::stereo;
	}
	else if (nChannels>2) {
		type = channel::surround;
	}
	return type;
}

}	//	namespace audio

}	// namespace canvas
