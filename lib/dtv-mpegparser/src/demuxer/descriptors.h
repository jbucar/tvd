/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#pragma once

#include "../types.h"
#include <util/buffer.h>

#define DESC_PARSE( descs, tag, val ) descs.parse( tuner::desc::ident::tag, &tuner::desc::demuxer::fnc_ ## tag, val )

namespace tuner {
namespace desc {

namespace ident {

enum type {
	registration               =0x05,
	conditional_access         =0x09,
	language                   =0x0a,
	application_storage        =0x10,
	carousel_id                =0x13,
	association_tag            =0x14,
	network_name               =0x40,
	service_list               =0x41,
	service                    =0x48,
	linkage                    =0x4a,
	short_event                =0x4d,
	extended_event             =0x4e,
	component                  =0x50,
	stream_identifier          =0x52,
	content                    =0x54,
	parental_rating            =0x55,
	local_time_offset          =0x58,
	application_signalling     =0x6f,
	advanced_audio_coding      =0x7c,
	digital_copy_control       =0xc1,
	audio_component            =0xc4,
	data_content               =0xc7,
	video_decode               =0xc8,
	ts_information             =0xcd,
	logo_transmission          =0xcf,
	series_descriptor          =0xd5,
	event_group                =0xd6,
	component_group            =0xd9,
	content_availability       =0xde,
	terrestrial_delivery_system=0xfa,
	partial_reception          =0xfb,
	emergency_information      =0xfc,
	data_component             =0xfd,
	system_management          =0xfe
};

}	//	namespace ident

class Descriptors {
public:
	Descriptors();
	Descriptors( const Descriptors &other );
	~Descriptors();

	void operator+=( const Descriptors &other );
	size_t append( util::BYTE *ptr, size_t len );
	void addDescriptor( util::BYTE *ptr, size_t len );
	void clear();

	template<typename T>
	bool parse( util::BYTE tag, bool (*parser)( T &, util::BYTE * ), T &desc ) const;
	bool find( util::BYTE tag ) const;
	util::BYTE *get( util::BYTE tag, size_t &off ) const;
	size_t getTags( std::vector<util::BYTE> &tags ) const;

	bool empty() const;
	util::BYTE *bytes() const;
	size_t length() const;

	std::string show() const;
	std::string showBytes() const;

private:
	util::Buffer _buf;
};

template<typename T>
bool parseDescriptor( bool (*parser)( T &, util::BYTE * ), T &desc, util::BYTE *data ) {
	return (*parser)( desc, data );
}

template<typename T>
inline bool Descriptors::parse( util::BYTE tag, bool (*parser)( T &, util::BYTE * ), T &desc ) const {
	size_t off=0;
	util::BYTE *data;
	bool result=false;
	bool exit=false;
	while (!exit) {
		data = get( tag, off );
		if (data) {
			if (!parseDescriptor( parser, desc, data )) {
				return false;
			}
			result=true;
		}
		else {
			exit=true;
		}
	}
	return result;
}

template<typename T>
inline bool parseDesc( const Descriptors &gDesc, const Descriptors &aDesc, util::BYTE tag, bool (*parser)( T &, util::BYTE * ), T &desc ) {
	bool result=true;

	//	Get global descriptor
	bool gFind = gDesc.find( tag );

	//	Get specific descriptor
	bool sFind = aDesc.find( tag );

	if (sFind) {
		result=aDesc.parse( tag, parser, desc );
	}
	else if (gFind) {
		result=gDesc.parse( tag, parser, desc );
	}
	else {
		result=false;
	}

	return result;
}

}
}

