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

#include "signal.h"
#include <zapper/zapper.h>
#include <zapper/player.h>
#include <util/string.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/bind.hpp>

#define CHECK_NUMBER 5

namespace platform {
namespace dvb {

Signal::Signal( bool checkGood )
	: Step( checkGood ? "signal.good" : "signal.bad", "Going to test signal intensity" ), _checkGood(checkGood)
{
	_pvr = NULL;
	_checks = 0;
	_fails = 0;
}

Signal::~Signal()
{
}

bool Signal::introduction() const {
	std::string text;
	if (!_checkGood) {
		text = "Disconnect the antenna from the receiver and press any key to continue";
	}
	else {
		text = "Connect the antenna from the receiver and press any key to continue";
	}
	msg( "" );
	msg( text );
	return true;
}

void Signal::start() {
	_checks = 0;
	_fails = 0;

	_pvr = zapper()->pvr();
	if (!_pvr) {
		LERROR( "dvb::Signal", "Cannot found Pvr" );
		fail();
		return;
	}

	checkSignal();
}

void Signal::stop() {
	_pvr = NULL;
}

bool Signal::testSignal() {
	int signal=0, snr=0;
	zapper()->player()->status( signal, snr );
	bool good = signal > 0 && snr > 0;
	msg( util::format( "Checking for %s signal intensity... (signal=%d, snr=%d)",
			_checkGood ? "good" : "poor", signal, snr ) );
	return _checkGood ? good : !good;
}

void Signal::checkSignal() {
	DTV_ASSERT(_pvr);
	std::string tmp;


	//	Test signal
	if (!testSignal()) {
		_fails++;
	}

	_checks++;
	if (_checks < CHECK_NUMBER) {
		addTimer( 1000, boost::bind(&Signal::checkSignal,this) );
	}
	else {
		//	End test
		if (_fails > 0) {
			LERROR("dvb::Signal", "The signal intensity verification is not working properly: checks=%d, fails=%d", _checks, _fails );
			fail();
		}
		else {
			success();
		}
	}
}

}
}
