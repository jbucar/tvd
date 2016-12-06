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

#include "modes.h"
#include <canvas/screen.h>
#include <canvas/system.h>
#include <zapper/zapper.h>
#include <util/string.h>
#include <util/log.h>
#include <boost/lexical_cast.hpp>
#include <string>

namespace platform {
namespace display {

namespace impl {

	canvas::connector::type getConnector( const std::string &name ) {
		if (name == "composite") {
			return canvas::connector::composite;
		}
		else if (name == "svideo") {
			return canvas::connector::svideo;
		}
		else if (name == "component") {
			return canvas::connector::component;
		}
		else if (name == "hdmi") {
			return canvas::connector::hdmi;
		}
		else if (name == "dvi") {
			return canvas::connector::dvi;
		}
		else if (name == "vga") {
			return canvas::connector::vga;
		}
		return canvas::connector::unknown;
	}

	struct FinderByName {
		FinderByName( const std::string &name ) : _name(name) {};

		bool operator()( const canvas::mode::type &modeType ) {
			const canvas::Mode &mode = canvas::mode::get( modeType );
			return (_name == mode.name);
		}

		const std::string &_name;
	};

	std::string makeDescription( const std::string &connector, const std::string &mode ) {
		const std::string &conn = canvas::connector::name( impl::getConnector(connector) );
		return util::format("Going to test %s resolution over %s", mode.c_str(), conn.c_str());
	}

	std::string makeName( const std::string &connector, const std::string &mode ) {
		return util::format("mode.%s.%s", connector.c_str(), mode.c_str());
	}

}

Mode::Mode( const std::string &connector, const std::string &mode )
	: Step(impl::makeName( connector, mode ), impl::makeDescription( connector, mode )), 
	_connector(impl::getConnector(connector)),
	_mode(mode)
{
}

Mode::~Mode()
{
}

void Mode::start() {
	::canvas::Screen *screen = zapper()->sys()->screen();
	if (!screen) {
		LERROR( "screen::Mode", "Cannot found screen" );
		return;
	}

	std::vector<canvas::connector::type> connectors = screen->supportedConnectors();
	std::vector<canvas::connector::type>::iterator conn_it = std::find(connectors.begin(), connectors.end(), _connector);
	if (conn_it == connectors.end()) {
		LERROR( "screen::Mode", "Connector not supported" );
		fail();	
	} else {
		std::vector<canvas::mode::type> modes = screen->supportedModes(*conn_it);
		std::vector<canvas::mode::type>::iterator mode_it = std::find_if(modes.begin(), modes.end(), impl::FinderByName(_mode));
		if (mode_it == modes.end()) {
			LERROR( "screen::Mode", "Mode not supported" );
			fail();
		} else {
			//	Set mode
			if (!screen->mode( *conn_it, *mode_it )) {
				LERROR( "screen::Mode", "Cannot set mode" );
				fail();
			}
		}
	}

	end();
}

}
}

