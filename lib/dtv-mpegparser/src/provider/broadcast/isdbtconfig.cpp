/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-mpegparser implementation.

    DTV-mpegparser is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-mpegparser is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-mpegparser.

    DTV-mpegparser es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-mpegparser se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "isdbtconfig.h"
#include "frontend.h"
#include <util/log.h>
#include <boost/lexical_cast.hpp>

//	VHF
#define FIRST_VHF_CHANNEL   7
#define LAST_VHF_CHANNEL    13
#define FIRST_VHF_FREQUENCY 177142

//	UHF
#define FIRST_UHF_CHANNEL   14
#define LAST_UHF_CHANNEL    69
#define FIRST_UHF_FREQUENCY 473142

namespace tuner {
namespace broadcast {

IsdbtConfig::IsdbtConfig()
{
	_frequency = -1;
}

IsdbtConfig::~IsdbtConfig()
{
}

void IsdbtConfig::frequency( unsigned int freq ) {
	_frequency = freq;
}

unsigned int IsdbtConfig::frequency() const {
	return _frequency;
}

std::string IsdbtConfig::name() const {
	return boost::lexical_cast<std::string>(frequency()/1000);
}

bool IsdbtConfig::start( Frontend *impl ) {
	return impl->start( this );
}

static void addChannel( std::vector<IsdbtConfig *> &chs, int firstFreq, int firstChannel, int lastChannel ) {
	IsdbtConfig *ch;
	for (int i=0;i<=lastChannel-firstChannel; i++) {
		ch = new IsdbtConfig();
		if (i == 36) {	//	Skip!
			continue;
		}
		ch->frequency( firstFreq+(i*6000) );
		chs.push_back( ch );
	}
}

void IsdbtConfig::loadDefaults( std::vector<IsdbtConfig *> &chs ) {
	LDEBUG( "Frontend", "Build ISDBT default frequenies list" );
	//	HIGH VHF Channels
	addChannel( chs, FIRST_VHF_FREQUENCY, FIRST_VHF_CHANNEL, LAST_VHF_CHANNEL );
	//	UHF Channels
	addChannel( chs, FIRST_UHF_FREQUENCY, FIRST_UHF_CHANNEL, LAST_UHF_CHANNEL );
}

}
}

