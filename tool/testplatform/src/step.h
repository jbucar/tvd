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

#pragma once

#include "types.h"
#include <util/id/ident.h>
#include <boost/function.hpp>
#include <string>

namespace platform {

class Step {
public:
	Step( const std::string &name, const std::string &desc );
	virtual ~Step();

	//	Methods
	virtual void setup();
	void initialize( zapper::Zapper *zapper );
	void finalize();
	void run();
	void end( status::type st );
	void cancel();
	void showResult() const;

	//	Key handling
	void onKey( util::key::type key, bool isUp );
	void onButtonEvent( canvas::input::ButtonEvent *evt );

	//	Command callback
	typedef boost::function<void ( command::type, status::type )> OnCommandCallback;
	void onCommand( const OnCommandCallback &callback );

	//	Setters
	void testName( const std::string &testName );

	//	Getters
	const status::type &st() const;
	const std::string &name() const;
	const std::string &desc() const;


protected:
	void doRun();

	virtual bool introduction() const;
	virtual void init();
	virtual void fin();
	virtual void start();
	virtual void stop();

	//	key handling
	virtual void handleKey( util::key::type key, bool isUp );
	virtual bool acceptControl();
	bool handleControl( util::key::type key, bool isUp );

	//	Timers
	typedef boost::function<void (void)> TimerCallback;
	void addTimer( int ms, const TimerCallback &callback );
	void stopTimer();

	//	Text
	void desc( const std::string &text ) const;
	void msg( const std::string &text, bool highlight=false ) const;
	void warn( const std::string &text ) const;

	//	Methods
	void end();
	void success();
	void fail();
	void skip( command::type cmd );
	typedef boost::function<void (void)> WaitMethod;
	void wait( const WaitMethod &fnc );

	zapper::Zapper *zapper() const;

private:
	void doEnd( status::type status );
	void execute( command::type cmd, status::type status );

	zapper::Zapper *_zapper;
	std::string _name;
	std::string _testName;
	std::string _desc;
	OnCommandCallback _command;
	status::type _status;
	util::id::Ident _timerID;
	WaitMethod _wait;

};

}

