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

#include "pesfilter.h"
#include "provider.h"
#include "../../../demuxer/pes/pesdemuxer.h"
#include "../../../demuxer/ts.h"
#include <util/cfg/cfg.h>
#include <util/assert.h>
#include <util/log.h>
#include <sys/ioctl.h>
#include <linux/dvb/dmx.h>

namespace tuner {
namespace dvb {

PesFilter::PesFilter( pes::FilterParams *params, Provider *prov )
	: Filter(params->pid,0,prov), _type(params->type), _mode(params->mode), _pipe(params->pipe)
{
}

bool PesFilter::start() {
	struct dmx_pes_filter_params params;

	bool useGenericPES = util::cfg::getValue<bool>("tuner.provider.dvb.genericPES");

	//	Setup filter
	memset(&params, 0, sizeof(params));
	params.pid   = pid();
	params.pes_type = useGenericPES ? DMX_PES_OTHER : (dmx_pes_type_t)_type;
	params.input = DMX_IN_FRONTEND;
	params.flags = DMX_IMMEDIATE_START;

	if (_mode == pes::mode::pes) {
		DTV_ASSERT(_pipe);
		params.output = DMX_OUT_TAP;
	}
	else if (_mode == pes::mode::pvr) {
		DTV_ASSERT(!_pipe);
		params.output = DMX_OUT_TS_TAP;
	}
	else if (_mode == pes::mode::ts) {
		DTV_ASSERT(_pipe);
		params.output = DMX_OUT_TSDEMUX_TAP;
	}
	else if (_mode == pes::mode::decode) {
		params.output = DMX_OUT_DECODER;
		params.pes_type = (dmx_pes_type_t)_type;
	}
	else {
		return false;
	}

	if (_pipe &&
		(_type == pes::type::video || _type == pes::type::audio || _type == pes::type::pcr))
	{
		unsigned int bSize = util::cfg::getValue<int>("tuner.provider.dvb.buffer");
		if (!bSize) {
			bSize = TS_PACKET_SIZE*1024*4;
		}
		if (ioctl( fd(), DMX_SET_BUFFER_SIZE, bSize ) == -1) {
			LERROR( "PesFilter", "ioctl DMX_SET_BUFFER_SIZE failed: size=%u", bSize );
			return false;
		}
	}

	//	Start filter
	if (ioctl( fd(), DMX_SET_PES_FILTER, &params ) == -1) {
		LERROR( "PesFilter", "ioctl DMX_SET_PES_FILTER failed: err=%s", strerror(errno) );
		return false;
	}

	if (_pipe) {
		return startStream( _pipe );
	}
	return true;
}

}
}

