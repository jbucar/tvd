/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-pvr implementation.

    DTV-pvr is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-pvr is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-pvr.

    DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "show.h"
#include "control.h"
#include "../time.h"
#include <mpegparser/demuxer/psi/eit.h>
#include <mpegparser/demuxer/descriptors/demuxers.h>
#include <util/log.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace pvr {
namespace dvb {

category::Type parseContent( const std::string &spec, const tuner::desc::Descriptors &descs );

Show::Show()
{
}

Show::Show( tuner::eit::Event *evt, const std::string &spec )
	: _descs( evt->descs )
{
	std::string name;
	std::string description;

	{	//	Parse short event descriptor
		tuner::desc::ShortEventDescriptor desc;
		if (DESC_PARSE( evt->descs, short_event, desc )) {
			name = desc.event;
			description = desc.text;
		}
	}

	{	//	Parse extended event descriptor
		std::string eText;
		tuner::desc::ExtendedEventDescriptor desc;
		if (DESC_PARSE( evt->descs, extended_event, desc )) {
			BOOST_FOREACH(tuner::desc::ExtendedEvent d, desc) {
				eText += d.text;
			}
		}

		if (!eText.empty() && !description.empty()) {
			description += "\n";
		}

		description += eText;
	}

	this->name( name );
	this->description( description );

	try {	//	Get time range
		bpt::ptime start_time = evt->startTime.get();
		bpt::ptime end_time = time::clock()->localTime(start_time) + evt->duration.getTime();
		bpt::time_period timeRange = bpt::time_period(start_time, end_time);
		this->timeRange( timeRange );
	} catch (...) {
		LWARN( "dvb", "Invalid startTime/duration in event: eventID=%d", evt->eventID );
	}

	//	Setup parental control
	parentalControl( Control::parse( evt->descs ) );

	//	Parse category
	category( parseContent(spec, evt->descs) );
}

Show::~Show()
{
}

bool Show::audioLanguage( int tag, std::string &lang ) {
	tuner::desc::AudioComponentDescriptor desc;
	if (DESC_PARSE( _descs, audio_component, desc )) {
		BOOST_FOREACH( const tuner::desc::AudioComponent &audio, desc ) {
			if (audio.componentTag == tag) {
				lang = audio.languages[0].name();
				return true;
			}
		}
	}
	return false;
}

}
}

