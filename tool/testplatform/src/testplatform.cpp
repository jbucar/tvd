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

#include "testplatform.h"
#include "testlog.h"
#include "test.h"
#include "input/map.h"
#include "canvas/composition.h"
#include "canvas/format.h"
#include "mixer/volume.h"
#include "mixer/mute.h"
#include "mixer/output.h"
#include "display/modes.h"
#include "display/aspect.h"
#include "mediaplayer/mediaend.h"
#include "mediaplayer/volume.h"
#include "mediaplayer/format.h"
#include "mediaplayer/resize.h"
#include "mediaplayer/mute.h"
#include "dvb/scan.h"
#include "dvb/video.h"
#include "dvb/download.h"
#include "dvb/signal.h"
#include "generated/config.h"
#include <zapper/zapper.h>
#include <canvas/inputmanager.h>
#include <canvas/system.h>
#include <util/serializer/xml.h>
#include <util/task/dispatcher.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <util/fs.h>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <stdio.h>

CREATE_TOOL(TestPlatform, TESTPLATFORM);

#define INVALID_INDEX -1

#define CFG_DIRECTORY "config"
#define CFG_VALUE value
#define ADD_STEPS( testName, file, step, append ) \
{ \
	std::string filename = util::fs::make(util::fs::installDataDir(), CFG_DIRECTORY, file); \
	if (boost::filesystem::exists(filename)) { \
		platform::Test *test = getTest(testName); \
		if (test) { \
			std::vector<std::string> values; \
			util::serialize::XML ser(filename); \
			if (!ser.load(values)) { \
				LWARN("TestPlatform", "Fail to load steps for %s from %s", testName, file); \
			}\
			BOOST_FOREACH( const std::string &CFG_VALUE, values ) { \
				test->addStep( step, append ); \
			} \
		} \
	} \
}

#define CURRENT( t, s )	\
	platform::Test *t = test(); DTV_ASSERT(t); \
	platform::Step *s = step(); DTV_ASSERT(s);

static platform::ChannelInfo _chInfo[] = {
	{ "SD - Prueba OTA", -1, 0xf1a0, 2101, true },
	{ "HD - Prueba OTA", -1, 0xf1a1, 2102, false },
	{ NULL, -1, 0, 0, false }
};

TestPlatform::TestPlatform()
{
	_zapper = NULL;
	_testIndex = INVALID_INDEX;
	_stepIndex = -1;

	{	//	Setup input test
		platform::Test *test = addTest( "input" );
		test->addStep( new platform::input::Map() );
	}

	{	//  Setup canvas test
		platform::Test *test = addTest( "canvas" );
		test->addStep( new platform::canvas::Composition() );
	}

	{	//	Mediaplayer
		platform::Test *test = addTest( "mediaplayer" );
		test->addStep( new platform::mediaplayer::MediaEnd() );
		test->addStep( new platform::mediaplayer::Volume() );
		test->addStep( new platform::mediaplayer::Resize() );
		test->addStep( new platform::mediaplayer::Mute(true) );
		test->addStep( new platform::mediaplayer::Mute(false) );
	}

	{	//	Setup mixer test
		platform::Test *test = addTest( "mixer" );
		test->addStep( new platform::mixer::Volume() );
		test->addStep( new platform::mixer::Output( canvas::audio::channel::mono ) );
		test->addStep( new platform::mixer::Output( canvas::audio::channel::stereo ) );
		test->addStep( new platform::mixer::Mute(true) );
		test->addStep( new platform::mixer::Mute(false) );
	}

	{	//	Setup DVB test
		platform::Test *test = addTest( "dvb" );
		test->addStep( new platform::dvb::Scan( _chInfo ) );
		test->addStep( new platform::dvb::Download( _chInfo ) );
		test->addStep( new platform::dvb::Signal(false) );
		test->addStep( new platform::dvb::Signal(true) );
		test->addStep( new platform::dvb::Video() );
	}

	{	//	Display
		platform::Test *test = addTest( "display" );
		test->addStep( new platform::display::Aspect(canvas::aspect::a4_3) );
		test->addStep( new platform::display::Aspect(canvas::aspect::a16_9) );
	}

}

TestPlatform::~TestPlatform()
{
	DTV_ASSERT( !_zapper );
	CLEAN_ALL( platform::Test *, _tests );
}

//	Configuration
void TestPlatform::registerProperties( util::cfg::cmd::CommandLine &cmd ) {
	Tool::registerProperties(cmd);

	std::string defTest;
	for (size_t i=0; i<_tests.size(); i++) {
		defTest += _tests[i]->name();
		defTest += ",";
	}

	defTest = defTest.substr(0, defTest.size()-1);
	registerProp("test", "", defTest, cmd);

	registerProp("file", "", "", cmd);
	registerProp("volumeDuration", "", 2, cmd);
	registerProp("volumeStep", "", 25, cmd);
}

int TestPlatform::run( util::cfg::cmd::CommandLine & /*cmd*/ ) {
#ifdef NDEBUG
	//	Set log level
	util::log::setLevel( "all", "all", "error" );
#endif

	{	//	Register tests
		std::vector<std::string> tests;
		boost::split(tests, getCfg<std::string>("test"), boost::is_any_of(","));
		BOOST_FOREACH( const std::string &test, tests ) {
			enableTest(test);
		}
	}

	{	// Add steps from config
		ADD_STEPS("canvas", "canvas.imageformats.xml", new platform::canvas::Format(CFG_VALUE), false);
		ADD_STEPS("mediaplayer", "mediaplayer.formats.video.xml", new platform::mediaplayer::Format(CFG_VALUE, true), false);
		ADD_STEPS("mediaplayer", "mediaplayer.formats.sound.xml", new platform::mediaplayer::Format(CFG_VALUE, false), false);
		ADD_STEPS("display", "display.connectors.composite.xml", new platform::display::Mode("composite", CFG_VALUE), true);
		ADD_STEPS("display", "display.connectors.component.xml", new platform::display::Mode("component", CFG_VALUE), true);
		ADD_STEPS("display", "display.connectors.hdmi.xml", new platform::display::Mode("hdmi", CFG_VALUE), true);
		ADD_STEPS("display", "display.connectors.vga.xml", new platform::display::Mode("vga", CFG_VALUE), true);
		ADD_STEPS("display", "display.connectors.svideo.xml", new platform::display::Mode("svideo", CFG_VALUE), true);
		ADD_STEPS("display", "display.connectors.dvi.xml", new platform::display::Mode("dvi", CFG_VALUE), true);
	}

	//	Setup steps
	BOOST_FOREACH( platform::Test *test, _tests ) {
		test->setup();
	}

	{	//	Remove db
		std::string dbDir = util::cfg::getValue<std::string>("zapper.db");
		if (dbDir.empty()) {
			dbDir = util::fs::stateDir();
		}
		LINFO( "TestPlatform", "Clean Zapper DB directory=%s", dbDir.c_str() );
		util::safeCleanDirectory( dbDir );
	}

	//	Create zapper
	_zapper = zapper::Zapper::create( "Test platform" );
	if (!_zapper) {
		LERROR( "TestPlatform", "Cannot create zapper object" );
		return false;
	}

	//	Initialize zapper
	if (!_zapper->initialize()) {
		return false;
	}

	//	Initialize steps
	BOOST_FOREACH( platform::Test *test, _tests ) {
		test->initialize( _zapper );
	}

	//	Register into dispatcher
	_zapper->sys()->dispatcher()->registerTarget( this, "testplatform" );

	//	Start zapper
	_zapper->start();

	//	Reserve keys
	reserveKeys();

	printIntroduction();

	//	Run tests
	if (nextTest()) {
		//	Run zapper loop
		_zapper->run();

		//	Cancel current step if there is
		cancel();

		showFinalResults();
	}

	//	Clean keys
	cleanKeys();

	//	Finalize steps
	BOOST_FOREACH( platform::Test *test, _tests ) {
		test->finalize();
	}

	//	Un-Register into dispatcher
	_zapper->sys()->dispatcher()->unregisterTarget( this );

	//	Stop zapper
	_zapper->stop();

	//	Finalize zapper!
	_zapper->finalize();
	DEL(_zapper);

	return 0;
}

//	Aux tests
platform::Test *TestPlatform::addTest( const std::string &name ) {
	platform::Test *test = new platform::Test( name );
	_tests.push_back(test);
	return test;
}

platform::Test *TestPlatform::getTest( const std::string &name ) const {
	platform::Tests::const_iterator it=std::find_if(
		_tests.begin(),
		_tests.end(),
		boost::bind(&platform::Test::name,_1) == name
	);
	return it != _tests.end() ? (*it) : NULL;
}

void TestPlatform::enableTest( const std::string &name ) {
	platform::Test *test = getTest( name );
	if (test) {
		test->enable();
	}
}

platform::Test *TestPlatform::test() const {
	if (_testIndex >= 0) {
		return _tests[_testIndex];
	}
	return NULL;
}

platform::Step *TestPlatform::step() const {
	platform::Test *t = test();
	if (t && _stepIndex >= 0) {
		const platform::Steps &steps = t->steps();
		return steps[_stepIndex];
	}
	return NULL;
}

int TestPlatform::next( int initial, int inc, int size ) {
	initial += inc;
	if (initial >= 0 && initial < size) {
		return initial;
	}
	return INVALID_INDEX;
}

bool TestPlatform::nextTest( int inc/*=1*/ ) {
	//	Reset step index
	_stepIndex = -1;

	//	Find next test enabled
	int s = (int)_tests.size();
	_testIndex = next( _testIndex, inc, s );
	while (_testIndex != INVALID_INDEX && !_tests[_testIndex]->canRun()) {
		_testIndex = next( _testIndex, inc, s );
	}

	if (_testIndex != INVALID_INDEX) {
		return nextStep( _tests[_testIndex] );
	} else { // Quit
		LINFO( "TestPlatform", "Post quit");
		_zapper->sys()->post( this, boost::bind(&TestPlatform::quit,this) );
	}

	return false;
}

bool TestPlatform::nextStep( platform::Test *test, int inc/*=1*/ ) {
	const platform::Steps &steps = test->steps();
	int s = (int)steps.size();
	_stepIndex = next( _stepIndex, inc, s );
	if (_stepIndex >= 0) {
		LINFO( "TestPlatform", "Post test: %s::%s",	test->name().c_str(), steps[_stepIndex]->name().c_str() );
		_zapper->sys()->post( this, boost::bind(&TestPlatform::runStep,this) );
		return true;
	}
	else {
		return nextTest(inc);
	}
}

void TestPlatform::runStep() {
	//	Get current test and step (It must exist!)
	CURRENT( t, s );
	LINFO( "TestPlatform", "Run test: %s::%s", t->name().c_str(), s->name().c_str() );

	//	Run
	s->onCommand( boost::bind(&TestPlatform::onCommandStep,this,_1, _2) );
	s->run();
}

void TestPlatform::cancel() {
	//	Stop current step
	platform::Step *s = step();
	if (s) {
		s->cancel();
	}
}

#define CHANGE_STEP( action ) \
	{ cur->end(st); action; }

#define ENSURE_CHANGE_STEP( action ) \
	{if (_testIndex != 0) { CHANGE_STEP(action) }}

void TestPlatform::onCommandStep( platform::command::type cmd, platform::status::type st ) {
	//	Get current test and step (It must exist!)
	CURRENT( t, cur );

	LDEBUG( "TestPlatform", "Command from test: %s::%s cmd=%d",
		t->name().c_str(), cur->name().c_str(), cmd);

	switch (cmd) {
		case platform::command::repeat: {
			CHANGE_STEP(nextStep( t, 0 ));
			break;
		}
		case platform::command::nextStep: {
			CHANGE_STEP(nextStep( t ));
			break;
		}
		case platform::command::previousStep: {
			ENSURE_CHANGE_STEP(nextStep(t, -1));
			break;
		}
		case platform::command::nextTest: {
			CHANGE_STEP(nextTest());
			break;
		}
		case platform::command::previousTest: {
			ENSURE_CHANGE_STEP(nextTest(-1));
			break;
		}
	}
};

void TestPlatform::quit() {
	_zapper->exit(0);
}

void TestPlatform::printIntroduction() {
	testlog::printGray("============================================================================\n", NULL, true);
	testlog::printGray("| TestPlatform tool\n", NULL, true);

	testlog::printGray("| \tPress OK to finish a test successfully\n", NULL, true);

	testlog::printGray("| \tPress EXIT to finish a test unsuccessfully\n", NULL, true);

	testlog::printGray("| \tPress cursors to go to the next/previous step or next/previous test\n", NULL, true );

	testlog::printGray("============================================================================\n\n", NULL, true);
}

void TestPlatform::reserveKeys() {
	{	//	Reserve keys
		util::key::Keys keys;
#define SAVE_KEY( c, n, u ) keys.insert( static_cast<util::key::type>(util::key::c) );
		UTIL_KEY_LIST(SAVE_KEY);
#undef SAVE_KEY
		canvas::InputManager *input = _zapper->sys()->input();
		input->reserveKeys( this, boost::bind(&TestPlatform::onKey,this,_1,_2), keys, 1 );
		input->onButton().connect( boost::bind(&TestPlatform::onButtonEvent,this,_1) );
	}
}

void TestPlatform::cleanKeys() {
	_zapper->sys()->input()->releaseKeys( this );
}

void TestPlatform::onKey( util::key::type key, bool isUp ) {
	platform::Step *s = step();
	if (s) {
		s->onKey( key, isUp );
	}
}

void TestPlatform::onButtonEvent( ::canvas::input::ButtonEvent *evt ) {
	platform::Step *s = step();
	if (s) {
		s->onButtonEvent( evt );
	}
}

void TestPlatform::showFinalResults() {
	const std::string &output = getCfg<std::string>("file");
	FILE *file = NULL;
	if (!output.empty()) {
		file = fopen( output.c_str(), "w" );
	}

	int testsRunned=0;
	int stepsSkipped=0, stepsSuccess=0, stepsFailed=0;
	BOOST_FOREACH( platform::Test *test, _tests ) {
		test->stepsStatistics( stepsSkipped, stepsSuccess, stepsFailed );
		if (test->enabled()) {
			testsRunned++;
		}
	}
	int nRun = stepsSuccess + stepsFailed;

	testlog::printGreen( "[==========]", file );
	fprintf( file ? file : stdout, " %d step(s) from %d tests ran\n", nRun, testsRunned );

	testlog::printGreen( "[  PASSED  ]", file );
	fprintf( file ? file : stdout, " %d step(s).\n", stepsSuccess );

	if (stepsSkipped) {
		testlog::printYellow( "[ SKIPPED  ]", file );
		fprintf( file ? file : stdout, " %d step(s).\n", stepsSkipped );
	}

	if (stepsFailed) {
		testlog::printRed( "[  FAILED  ]", file );
		fprintf( file ? file : stdout, " %d step(s), listed below:\n", stepsFailed);
		BOOST_FOREACH(  platform::Test *test, _tests ) {
			BOOST_FOREACH( platform::Step *step, test->steps() ) {
				if (step->st() == platform::status::failed) {
					testlog::printRed( "[  FAILED  ]", file );
					fprintf( file ? file : stdout, " %s.%s (%s)\n", test->name().c_str(), step->name().c_str(), step->desc().c_str());
				}
			}
		}
	}

	if (file) {
		fclose( file );
	}
}
