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

#include "step.h"
#include "testlog.h"
#include <zapper/zapper.h>
#include <canvas/system.h>
#include <util/task/dispatcher.h>
#include <util/io/dispatcher.h>
#include <util/assert.h>
#include <util/log.h>
#include <stdio.h>

namespace platform {

Step::Step( const std::string &name, const std::string &desc )
	: _name(name), _desc(desc)
{
	_status = status::unexecuted;
	_zapper = NULL;
}

Step::~Step()
{
	DTV_ASSERT(!_zapper);
}

void Step::setup() {
}

void Step::initialize( zapper::Zapper *zapper ) {
	_zapper = zapper;
	_zapper->sys()->dispatcher()->registerTarget( this, "step" );
	init();
}

void Step::finalize() {
	//	Un-Register into dispatcher
	_zapper->sys()->dispatcher()->unregisterTarget( this );
	fin();
	_zapper = NULL;
}

void Step::init() {
}

void Step::fin() {
}

void Step::run() {
	//	Show status
	testlog::printGreen("[ RUN      ]");
	testlog::printStepName( _testName, _name );

	//	Describe text
	desc( _desc );

	//	Show introduction
	if (introduction()) {
		wait( boost::bind(&Step::doRun,this) );
	}
	else {
		doRun();
	}
}

void Step::end( status::type st ) {
	_zapper->sys()->post( this, boost::bind(&Step::doEnd, this, st) );
}

void Step::doRun() {
	_status = status::running;
	start();
}

void Step::cancel() {
	doEnd( status::unexecuted );
}

bool Step::introduction() const {
	return false;	//	No wait for user intervention
}

void Step::start() {
}

void Step::stop() {
}

void Step::onCommand( const OnCommandCallback &callback ) {
	_command = callback;
}

void Step::onKey( util::key::type key, bool isUp ) {
	LDEBUG( "Step", "On key: key=%d, isUp=%d", key, isUp );
	if (!_wait.empty()) {
		if (isUp) {
			_wait.clear();
			if (key == util::key::ok || key == util::key::enter) {
				doRun();
			}
			else if (key >= util::key::cursor_down && key <= util::key::cursor_up) {
				handleControl( key, isUp );
			}
		}
	}
	else if (!handleControl(key,isUp)) {
		handleKey( key, isUp );
	}
}

void Step::handleKey( util::key::type /*key*/, bool /*isUp*/ ) {
}

bool Step::acceptControl() {
	return (_status == status::waiting || _status == status::ended);
}

#define HANDLE_KEY(f)	\
	{ if (isUp) { f; } handled = true; }

bool Step::handleControl( util::key::type key, bool isUp ) {
	bool handled = false;

	if (acceptControl()) {
		if (key == util::key::escape || key == util::key::exit) {
			HANDLE_KEY( fail() );
		}
		else if (key == util::key::ok || key == util::key::enter) {
			HANDLE_KEY( success() );
		}
	}

	if (key >= util::key::cursor_down && key <= util::key::cursor_up) {
		if (isUp) {
			if (key == util::key::cursor_up) {
				skip( command::nextTest );
			}
			else if (key == util::key::cursor_down) {
				skip( command::previousTest );
			}
			else if (key == util::key::cursor_left) {
				if (_status == status::ended) {
					skip( command::repeat );
				} else {
					skip( command::previousStep );
				}
			}
			else if (key == util::key::cursor_right) {
				skip( command::nextStep );
			}
		}
		handled=true;
	}

	LDEBUG( "Step", "Handle control key: key=%d, isUp=%d, handled=%d",
		key, isUp, handled );

	return handled;
}

void Step::onButtonEvent( canvas::input::ButtonEvent * /*evt*/ ) {
}

void Step::stopTimer() {
	if (util::id::isValid(_timerID)) {
		_zapper->sys()->io()->stopTimer( _timerID );
		_timerID.reset();
	}
}

void Step::addTimer( int ms, const TimerCallback &callback ) {
	util::io::Callback fnc = boost::bind(callback);
	_timerID = _zapper->sys()->io()->addTimer( ms, fnc );
}

void Step::desc( const std::string &text ) const {
	testlog::printYellow("[          ] ");
	testlog::printCyan( text + "\n", NULL, true);
}

void Step::msg( const std::string &text, bool highlight/*=false*/ ) const {
	testlog::printYellow("[          ] ");
	testlog::printGray( text + "\n", NULL, highlight );
}

void Step::warn( const std::string &text ) const {
	testlog::printYellow( text + "\n", NULL, true );
}

void Step::end() {
	LDEBUG( "test", "End step!: step=%s", _name.c_str() );
	if (_status != status::failed) {
		msg( "Test ends. Result ? (OK=success, EXIT=failure)", true );
		_status = status::ended;
	}
}

void Step::success() {
	LDEBUG("test", "Success step!: step=%s", _name.c_str());
	execute(command::nextStep, status::succeeded);
}

void Step::fail() {
	LDEBUG("test", "Fail step !: step=%s", _name.c_str());
	execute(command::nextStep, status::failed);
}

void Step::skip( command::type cmd ) {
	LDEBUG("test", "Skip step !: step=%s", _name.c_str());
	execute(cmd, status::unexecuted);
}

void Step::execute( command::type cmd, status::type st ) {
	if (!_command.empty()) {
		_command(cmd, st);
	}
}

void Step::wait( const boost::function<void (void)> &fnc ) {
	LDEBUG( "test", "Wait for user" );
	msg("Press OK to continue", true);
	_status = status::waiting;
	_wait = fnc;
}

void Step::doEnd( status::type st ) {
	LDEBUG( "test", "doEnd" );
	_status = st;
	showResult();
	stopTimer();
	stop();
}

void Step::testName( const std::string &testName ) {
	_testName = testName;
}

zapper::Zapper *Step::zapper() const {
	return _zapper;
}

const std::string &Step::name() const {
	return _name;
}

const std::string &Step::desc() const {
	return _desc;
}

const status::type &Step::st() const {
	return _status;
}

void Step::showResult() const {
	switch (_status) {
		case status::unexecuted:
			testlog::printYellow("[ SKIPPED  ]");
			break;
		case status::failed:
			testlog::printRed("[  FAILED  ]");
			break;
		case status::succeeded:
			testlog::printGreen("[       OK ]");
			break;
		default: break;
	}
	testlog::printStepName(_testName, name());
}

}

