/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include <string>

#define SCHEMA_TYPES_LIST(fnc)	\
	fnc(unknown,unknown) \
	fnc(file,file) \
	fnc(http,http) \
	fnc(https,https) \
	fnc(rtsp,rtsp) \
	fnc(rtp,rtp) \
	fnc(mirror,ncl-mirror) \
	fnc(sbtvd,sbtvd-ts)

#define PLAYER_TYPES_LIST(fnc) \
	fnc(ncl) \
	fnc(image) \
	fnc(text) \
	fnc(lua) \
	fnc(video) \
	fnc(audio) \
	fnc(html) \
	fnc(anime)
	
#define MIME_TYPES_LIST(fnc) \
	fnc(html,  htm,  TEXT/HTML) \
	fnc(html,  html, TEXT/HTML) \
	fnc(text,  txt,  TEXT/PLAIN) \
	fnc(text,  css,  TEXT/CSS) \
	fnc(text,  xml,  TEXT/XML) \
	fnc(text,  srt,  TEXT/SRT) \
	fnc(image, bmp,  IMAGE/BMP) \
	fnc(image, png,  IMAGE/PNG) \
	fnc(image, gif,  IMAGE/GIF) \
	fnc(image, jpg,  IMAGE/JPEG) \
	fnc(image, jpeg, IMAGE/JPEG) \
	fnc(audio, wav,  AUDIO/BASIC) \
	fnc(audio, ac3,  AUDIO/AC3) \
	fnc(audio, mpa,  AUDIO/MPA) \
	fnc(audio, mp3,  AUDIO/MP3) \
	fnc(audio, mp2,  AUDIO/MP2) \
	fnc(video, mp4,  VIDEO/MPEG4) \
	fnc(video, mpg,  VIDEO/MPEG) \
	fnc(video, mpv,  VIDEO/MPV) \
	fnc(video, ts,   VIDEO/MPEG) \
	fnc(video, mts,  VIDEO/MPEG) \
	fnc(video, mpeg, VIDEO/MPEG) \
	fnc(video, mov,  VIDEO/QUICKTIM) \
	fnc(video, avi,  VIDEO/X-MSVIDEO) \
	fnc(lua,   lua,  APPLICATION/X-GINGA-NCLUA) \
	fnc(lua,   lua,  APPLICATION/X-NCL-NCLUA) \
	fnc(ncl,   ncl,  APPLICATION/X-GINGA-NCL) \
	fnc(ncl,   ncl,  APPLICATION/X-NCL-NCL)

namespace player {

namespace schema {
#define DO_ENUM_SCHEMA_TYPES(n,t) n,
enum type {
	SCHEMA_TYPES_LIST(DO_ENUM_SCHEMA_TYPES)
	LAST
};
#undef DO_ENUM_SCHEMA_TYPES

const char *getName( type schType );

}	//	namespace schema

namespace type {

#define DO_ENUM_PLAYER_TYPES(t) t,
enum type {
	unknown,
	PLAYER_TYPES_LIST(DO_ENUM_PLAYER_TYPES)
	LAST
};
#undef DO_ENUM_PLAYER_TYPES

const char *getName( type player );
const char *getExtensionFromMime( const std::string &mimeType );
type fromBody( const std::string &body );
type fromMime( const std::string &mimeType );
type get( schema::type schema, const std::string &body, const std::string &mimeType );

} //	namespace schema

namespace url {

//	Parse url into schema and body
void parse( const std::string &url, schema::type &schema, std::string &body );

}	//	namespace url

}

