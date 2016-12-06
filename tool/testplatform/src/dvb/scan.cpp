/*******************************************************************************

  Copyright (c) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata
  All rights reserved.

********************************************************************************

  Redistribution and use in source and binary forms, with or without 
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of its contributors may
      be used to endorse or promote products derived from this software
      without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

********************************************************************************

  La redistribución y el uso en las formas de código fuente y binario, con o sin
  modificaciones, están permitidos siempre que se cumplan las siguientes condiciones:
    * Las redistribuciones del código fuente deben conservar el aviso de copyright
      anterior, esta lista de condiciones y el siguiente descargo de responsabilidad.
    * Las redistribuciones en formato binario deben reproducir el aviso de copyright
      anterior, esta lista de condiciones y el siguiente descargo de responsabilidad
      en la documentación y/u otros materiales suministrados con la distribución.
    * Ni el nombre de los titulares de derechos de autor ni los nombres de sus
      colaboradores pueden usarse para apoyar o promocionar productos derivados de
      este software sin permiso previo y por escrito.

  ESTE SOFTWARE SE SUMINISTRA POR LIFIA "COMO ESTÁ" Y CUALQUIER GARANTÍA EXPRESA
  O IMPLÍCITA, INCLUYENDO, PERO NO LIMITADO A, LAS GARANTÍAS IMPLÍCITAS DE
  COMERCIALIZACIÓN Y APTITUD PARA UN PROPÓSITO DETERMINADO SON RECHAZADAS. EN
  NINGÚN CASO LIFIA SERÁ RESPONSABLE POR NINGÚN DAÑO DIRECTO, INDIRECTO, INCIDENTAL,
  ESPECIAL, EJEMPLAR O CONSECUENTE (INCLUYENDO, PERO NO LIMITADO A, LA ADQUISICIÓN
  DE BIENES O SERVICIOS; LA PÉRDIDA DE USO, DE DATOS O DE BENEFICIOS; O INTERRUPCIÓN
  DE LA ACTIVIDAD EMPRESARIAL) O POR CUALQUIER TEORÍA DE RESPONSABILIDAD, YA SEA
  POR CONTRATO, RESPONSABILIDAD ESTRICTA O AGRAVIO (INCLUYENDO NEGLIGENCIA O
  CUALQUIER OTRA CAUSA) QUE SURJA DE CUALQUIER MANERA DEL USO DE ESTE SOFTWARE,
  INCLUSO SI SE HA ADVERTIDO DE LA POSIBILIDAD DE TALES DAÑOS.

*******************************************************************************/

#include "scan.h"
#include <pvr/pvr.h>
#include <pvr/channels.h>
#include <pvr/channel.h>
#include <zapper/zapper.h>
#include <util/cfg/cfg.h>
#include <util/log.h>
#include <boost/bind.hpp>

namespace platform {
namespace dvb {

Scan::Scan( platform::ChannelInfo *chInfo )
	: Step( "scan", "Going to test channel scanning" ), _chInfo(chInfo)
{
	_pvr = NULL;
}

Scan::~Scan()
{
}

void Scan::setup() {
	util::cfg::setValue("zapper.showOneSeg", true);
	util::cfg::setValue("zapper.update.useOTA", true);
	util::cfg::setValue("zapper.mpeg.useEPG", false);
	util::cfg::setValue("zapper.mpeg.useGinga", false);
	util::cfg::setValue("zapper.mpeg.otaMakerId", 254 );
	util::cfg::setValue("zapper.mpeg.otaModelId", 254 );
	util::cfg::setValue("zapper.mpeg.otaVersion", 1 );
}

bool Scan::introduction() const {
	msg("It is necessary to broadcast the transport stream \"Test.ts\" to run this test");
	return true;
}

void Scan::start() {
	LDEBUG( "scan", "Begin scan step" );

	if (!_pvr) {
		_pvr = zapper()->pvr();
		if (!_pvr) {
			LERROR( "dvb::Scan", "Cannot found pvr" );
			fail();
			return;
		}

		_cScan = _pvr->onScanChanged().connect(boost::bind(&Scan::onScan, this, _1, _2));
	}

	msg("This may take several seconds");

	//	Start scan
	_pvr->startScan();
}

void Scan::stop() {
	if (_pvr) {
		_pvr->cancelScan();
	}
	_cScan.disconnect();
}

//	Implementation

void Scan::onScan( pvr::scan::Type scanState, const pvr::Pvr::ScanNetwork net ) {
	switch (scanState) {
		case pvr::scan::begin: {
			break;
		}
		case pvr::scan::network: {
			onNetwork(net.get());
			break;
		}
		case pvr::scan::end: {
			onEndScan();
			break;
		}
	};
}

void Scan::onNetwork( const std::string &net ) {
	std::string tmp("Scanning frequency: ");
	tmp += net;
	tmp += " Mhz";
	msg( tmp );
}

void Scan::onEndScan() {
	// Channels chs;
	pvr::Channels *chs = zapper()->pvr()->channels();
	if (!chs) {
		LERROR( "dvb::Scan", "Cannot found channel manager service" );
		fail();
		return;
	}

	{	//	Check services
		pvr::ChannelList chlist = chs->getAll();;

		//	Get channels count
		int count=0;
		while (_chInfo[count].name) {
				count++;
		}
		if (count <= 0) {
			LERROR( "dvb::Scan", "No channel information available" );
			fail();
			return;
		}

		int found=0;
		for (size_t i=0; i<chlist.size(); i++) {
			int j=0;
			while (_chInfo[j].name) {
				if (chlist[i]->name() == _chInfo[j].name) {
					_chInfo[j].id = chlist[i]->channelID();
					found++;
					break;
				}
				j++;
			}
		}

		if (found != count) {
			LERROR( "dvb::Scan", "Cannot found all services" );
			fail();
			return;
		}
	}
	
	success();
}

}
}

