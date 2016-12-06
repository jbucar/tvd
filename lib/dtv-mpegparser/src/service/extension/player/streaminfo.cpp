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

#include "streaminfo.h"
#include "../../../demuxer/descriptors/generic.h"
#include "../../../demuxer/descriptors/demuxers.h"
#include <boost/foreach.hpp>

namespace tuner {
namespace player {

StreamInfo::StreamInfo( const std::string &name, pes::type::type esType, util::BYTE streamType )
	: _name(name), _esType(esType), _type(streamType)
{
}

StreamInfo::~StreamInfo()
{
}

pes::type::type StreamInfo::esType() const {
	return _esType;
}

util::BYTE StreamInfo::type() const {
	return _type;
}

const std::string &StreamInfo::name() const {
	return _name;
}

bool StreamInfo::check( const ElementaryInfo &info, ID tag ) const {
	if (_type != info.streamType) {
		return false;
	}

	if (!checkTag( tag )) {
		return false;
	}

	return checkAux( info );
}

bool StreamInfo::checkAux( const ElementaryInfo & /*info*/ ) const {
	return true;
}

bool StreamInfo::checkTag( ID tag ) const {
	bool valid=_tags.empty() ? true : false;
	BOOST_FOREACH( const Tag &t, _tags ) {
		if (t.first <= tag && tag <= t.second) {
			valid=true;
			break;
		}
	}
	return valid;
}

void StreamInfo::addTag( ID first, ID last ) {
	_tags.push_back( std::make_pair( first, last ) );
}

#define ADD_STREAM( name, t, n )	  \
	{ StreamInfo *si = new StreamInfo( name, t, n ); tmp.push_back( si ); }

#define ADD_STREAM_CHECK( name, t, n, check )	  \
	{ StreamInfo *si = new StreamInfoImpl<check>( name, t, n ); tmp.push_back( si ); }

static void mpegVideoStreams( std::vector<StreamInfo *> &tmp ) {
	ADD_STREAM( "MPEG-1 Video", pes::type::video, 0x01 );
	ADD_STREAM( "MPEG-2 Video", pes::type::video, 0x02 );
}

struct AC3StreamChecker {
	bool operator()( StreamInfo *s, const ElementaryInfo &info ) const {
		//	Check registration descriptor (ISO 13818-1 2.6.8)
		desc::FormatIdentifier id;
		if (DESC_PARSE( info.descriptors, registration, id )) {
			//	Check via format_identifiers (see http://www.smpte-ra.org/mpegreg/mpegreg.html)
			//	Is AC-3?
			if (id.format == 0x41432D33) {
				return true;
			}
		}

		//	AC-3 stream types (ATSC/DVB) A/52:2010 A3. Generic Identification of an AC-3 STREAM
		util::BYTE tag=0;
		if (s->type() == 0x81) {
			//	Check System A (ATSC)
			tag = 0x81;
		}
		else if (s->type() == 0x6) {
			//	Check System B (DVB)
			tag = 0x6A;
		}

		if (tag) {
			return info.descriptors.find( tag );
		}

		return false;
	}
};

struct MpegSubtitleStreamChecker {
	bool operator()( StreamInfo *s, const ElementaryInfo &info ) const {
		if (s->type() == 0x6) {
			return info.descriptors.find( 0x59 );
		}
		return false;
	}
};

static void mpegAudioStreams( std::vector<StreamInfo *> &tmp ) {
	ADD_STREAM( "MPEG-1 Audio", pes::type::audio, 0x03 );
	ADD_STREAM( "MPEG-1 Audio", pes::type::audio, 0x04);
	ADD_STREAM( "MPEG-2 AAC Audio", pes::type::audio, 0x0F );
	ADD_STREAM( "AC3 Audio", pes::type::audio, 0x81 );
	ADD_STREAM_CHECK( "AC3 Audio", pes::type::audio, 0x6, AC3StreamChecker );
}

static void mpegSubtitleStreams( std::vector<StreamInfo *> &tmp ) {
	ADD_STREAM_CHECK( "MPEG Subtitle", pes::type::subtitle, 0x06, MpegSubtitleStreamChecker );
}

static void isdbtVideoStreams( std::vector<StreamInfo *> &tmp ) {
	//	H.264 video
	StreamInfo *i = new StreamInfo( "H.264 Video", pes::type::video, 0x1b );
	i->addTag( 0x00, 0x0F );
	i->addTag( 0x81, 0x82 );
	i->addTag( 0x00, 0x00 );
	tmp.push_back( i );
}

static void isdbtAudioStreams( std::vector<StreamInfo *> &tmp ) {
	//	MPEG-4 AAC Audio (LATM, 48Khz)
	StreamInfo *i = new StreamInfo( "MPEG-4 AAC Audio (LATM, 48Khz)", pes::type::audio, 0x11 );
	i->addTag( 0x10, 0x2F );
	i->addTag( 0x83, 0x86 );  //	24Khz
	i->addTag( 0x90, 0x91 );  //	16Khz
	i->addTag( 0x90, 0x91 );  //	16Khz
	i->addTag( 0x00, 0x00 );
	tmp.push_back( i );
}

struct AribClosedCaptionStreamChecker {
	bool operator()( StreamInfo * /*s*/, const ElementaryInfo &info ) const {
		//	ABNT NBR 15608-3:2008 22.3.3.3
		desc::DataComponentDescriptor desc;
		if (DESC_PARSE(info.descriptors, data_component, desc )) {
			return desc.codingMethodID == 0x8;
		}

		return false;
	}
};

StreamInfo *aribClosedCaption() {
	//	ARIB STD-B24 Caption
	StreamInfo *i = new StreamInfoImpl<AribClosedCaptionStreamChecker>( "ARIB STD-B24 Caption", pes::type::subtitle, 0x06 );
	i->addTag( 0x30, 0x37 );
	i->addTag( 0x87, 0x87 );
	i->addTag( 0x00, 0x00 );
	return i;
}

StreamInfo *aribSuperImpose() {
	//	ARIB STD-B24 Caption
	StreamInfo *i = new StreamInfoImpl<AribClosedCaptionStreamChecker>( "ARIB STD-B24 Superimpose", pes::type::subtitle, 0x06 );
	i->addTag( 0x38, 0x3F );
	i->addTag( 0x88, 0x88 );
	return i;
}

void defaultsStreams( std::vector<StreamInfo *>  &vec ) {
	//	MPEG streams
	mpegVideoStreams( vec );
	mpegAudioStreams( vec );
	mpegSubtitleStreams( vec );

	//	ISDBT streams
	isdbtVideoStreams( vec );
	isdbtAudioStreams( vec );
	vec.push_back( aribClosedCaption() );
}

}
}

