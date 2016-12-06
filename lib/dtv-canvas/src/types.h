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
#include "streaminfo.h"
#include <util/types.h>
#include <util/id/ident.h>
#include <util/keydefs.h>
#include <boost/function.hpp>
#include <boost/signals2.hpp>

namespace util {
namespace io {
	class Dispatcher;
}
}

namespace canvas {

class Factory;
class Audio;
class System;
class Rect;
class Point;
class Canvas;
class Color;
class Screen;
class Surface;
typedef Surface * SurfacePtr;
struct ImageDataType {
	Size size;
	unsigned char *data;
	int length;
	int bitsPerPixel;
	int bytesPerPixel;
	int stride;
	int dataOffset;
};
typedef struct ImageDataType ImageData;

//	Video
class Window;

#ifdef _WIN32
typedef HANDLE VideoDescription;
#else
typedef struct {
  void *display;
  int   screenID;
  unsigned long winID;
} VideoDescription;
#endif
class VideoOverlay;
typedef VideoOverlay *VideoOverlayPtr;

//	Volume
#define MIN_VOLUME     0
#define MAX_VOLUME     20
typedef long Volume;

//	Player
class Player;
class MediaPlayer;
typedef MediaPlayer *MediaPlayerPtr;

//	HTML
class WebViewer;

//	Events
template<typename T> class Event;
typedef boost::function<void (util::id::Ident &id)> EventCallback;

//	Input
#define LOW_INPUT_PRIORITY  1
#define MED_INPUT_PRIORITY  100
#define HI_INPUT_PRIORITY   1000
#define MAX_INPUT_PRIORITY  10000

class Input;
class InputManager;
namespace input {
	typedef void * ListenerID;

	struct ButtonEventS {
		unsigned int button;
		int x, y;
		bool isPress;
	};
	typedef ButtonEventS ButtonEvent;

	typedef boost::function<void (util::key::type key, bool isUp)> Callback;
	typedef boost::function<void (const util::key::Keys &keys)> ReservedKeysCallback;
	typedef boost::signals2::signal<void (util::key::type key, bool isUp)> KeySignal;
	typedef boost::signals2::signal<util::key::type (util::key::type key)> KeyTranslator;
	typedef boost::signals2::signal<void (ButtonEvent *ev)> ButtonSignal;
	typedef boost::signals2::signal<void (const util::key::Keys &keys)> KeyReservationChangedSignal;
	void dispatchButtonEvent( System *sys, ButtonEvent *ev );
}

namespace composition {

enum mode {
	source_over,
	destination_over,
	clear,
	source,
	destination,
	source_in,
	destination_in,
	source_out,
	destination_out,
	source_atop,
	destination_atop,
	x_or,
	plus,
	multiply,
	screen,
	overlay,
	darken,
	lighten,
	color_dodge,
	color_burn,
	hard_light,
	soft_light,
	difference,
	exclusion
};

}	// namespace composition

namespace connector {

#define CONNECTOR_LIST(fnc) \
	fnc(vga,VGA) \
	fnc(composite,Video Compuesto) \
	fnc(svideo,S-Video) \
	fnc(component,Por Componentes) \
	fnc(dvi,DVI) \
	fnc(hdmi,HDMI)

#define DO_ENUM_CONNECTOR(n,i) n,
enum type {
	unknown,
	CONNECTOR_LIST(DO_ENUM_CONNECTOR)
	LAST_CONNECTOR
};
#undef DO_ENUM_CONNECTOR

// Returns the connector (tv out) name
const char *name( type conn );

}	// namespace connector

typedef struct {
	const char *name;
	int x;
	int y;
} Aspect;

namespace aspect {

#define ASPECT_RATIO_LIST(fnc)					\
	fnc(1_1,1,1)								\
	fnc(4_3,4,3)								\
	fnc(14_9,14,9)								\
	fnc(16_9,16,9)

#define DO_ENUM_ASPECT(n,w,h) a##n,
enum type {
	unknown,
	automatic,
	ASPECT_RATIO_LIST(DO_ENUM_ASPECT)
	LAST_ASPECT
};
#undef DO_ENUM_ASPECT

// Signal aspect mode changes
typedef boost::signals2::signal<void (type)> ChangedSignal;
// Returns the aspect mode
Aspect &get( type vAspect );
type get( float ratio );
type get( const VideoStreamInfo &vInfo );

}	// namespace aspect

namespace scan {

enum type {
	unknown,
	p,
	i
};

}	// namespace scan

typedef struct {
	const char *name;
	const char *mode;
	int width;
	int height;
	int frequency;
	scan::type type;
} Mode;

namespace mode {

#define MODE_LIST(fnc)							\
	fnc(240p_60,320,240,60,p)					\
	fnc(480p_60,640,480,60,p)					\
	fnc(480p_72,640,480,72,p)					\
	fnc(480p_75,640,480,75,p)					\
	fnc(480p_85,640,480,85,p)					\
	fnc(480i_60,640,480,60,i)					\
	fnc(480i_72,640,480,72,i)					\
	fnc(480i_75,640,480,75,i)					\
	fnc(480i_85,640,480,85,i)					\
	fnc(800_600_60,800,600,60,p)				\
	fnc(800_600_72,800,600,72,p)				\
	fnc(800_600_75,800,600,75,p)				\
	fnc(800_600_85,800,600,85,p)				\
	fnc(1024_768_60,1024,768,60,p)				\
	fnc(1024_768_72,1024,768,72,p)				\
	fnc(1024_768_75,1024,768,75,p)				\
	fnc(1024_768_85,1024,768,85,p)				\
	fnc(576p_50,720,576,50,p)					\
	fnc(576i_50,720,576,50,i)					\
	fnc(576i_25,720,576,25,i)					\
	fnc(NTSC,720,480,60,i)						\
	fnc(720p_50,1280,720,50,p)					\
	fnc(720p_60,1280,720,60,p)					\
	fnc(720i_50,1280,720,50,i)					\
	fnc(720i_60,1280,720,60,i)					\
	fnc(1080p_24,1920,1080,24,p)				\
	fnc(1080p_25,1920,1080,25,p)				\
	fnc(1080p_30,1920,1080,30,p)				\
	fnc(1080p_50,1920,1080,50,p)				\
	fnc(1080p_60,1920,1080,60,p)				\
	fnc(1080i_25,1920,1080,25,i)				\
	fnc(1080i_30,1920,1080,30,i)				\
	fnc(1080i_50,1920,1080,50,i)				\
	fnc(1080i_60,1920,1080,60,i)

#define DO_ENUM_MODES(n,w,h,f,t) m##n,
enum type {
	unknown,
	MODE_LIST(DO_ENUM_MODES)
	LAST_MODE
};
#undef DO_ENUM_MODES

// Signal mode changes
typedef boost::signals2::signal<void (connector::type,type)> ChangedSignal;

//	Returns the mode
Mode &get( type vMode );
type get( int w, int h, int f, scan::type t );
type get( const VideoStreamInfo &vInfo );

}	// namespace mode

typedef unsigned int (*WrapMode)( const std::string &word, unsigned int breakPos );

inline unsigned int wrapByWord( const std::string &/*word*/, unsigned int /*breakPos*/ ) {
	return 0;
}

inline unsigned int wrapAnywhere( const std::string &/*word*/, unsigned int breakPos ) {
	return breakPos;
}

namespace audio {

#define STEP_VOLUME    1
#define DEFAULT_VOLUME 10

struct DeviceInfoS {
	std::string name;
	std::string displayName;
	std::string displayDescription;
};
typedef struct DeviceInfoS DeviceInfo;
typedef std::vector<DeviceInfo> Devices;


class Stream;
typedef std::vector<Stream *> Streams;

typedef struct {
	const char *name;
	int count;
	int lfe;
} Channel;

namespace channel {

#define AUDIO_CHANNEL_LIST(fnc)	  \
	fnc(mono,     Mono,     1, 0)	\
	fnc(stereo,   Estéreo,  2, 0)	\
	fnc(surround, Surround, 5, 1)

#define DO_ENUM_AUDIO_CHANNEL(n,c,k,l) n,
enum type {
	unknown,
	AUDIO_CHANNEL_LIST(DO_ENUM_AUDIO_CHANNEL)
	LAST_CHANNEL
};
#undef DO_ENUM_AUDIO_CHANNEL

}	//	namespace channel

//	Returns the audio channel info
Channel &getChannelInfo( channel::type ch );
channel::type get( const AudioStreamInfo &aInfo );

}	// namespace audio

}	//namespace canvas

