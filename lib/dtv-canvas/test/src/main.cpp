/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "system.h"
#include "../../src/system.h"
#include "generated/config.h"
#include <util/registrator.h>
#include <util/cfg/cfg.h>
#include <util/mcr.h>
#include <util/log.h>
#include <util/main.h>
#include <gtest/gtest.h>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#ifdef _WIN32
#	ifdef _DEBUG
#		pragma comment(lib,"gtestd.lib")
#	else
#		pragma comment(lib,"gtest.lib")
#	endif // _DEBUG
#endif

void getSystems( std::vector<std::string> &systems ) {
	UNUSED(systems);

#if CANVAS_SYSTEM_USE_GTK
	systems.push_back("gtk");
#endif // GTK

#if CANVAS_SYSTEM_USE_X11
	systems.push_back("x11");
#endif // X11

#if CANVAS_SYSTEM_USE_SDL
	systems.push_back("sdl");
#endif // SDL

#if CANVAS_SYSTEM_USE_QT
	systems.push_back("qt");
#endif // QT

#if CANVAS_SYSTEM_USE_DIRECTFB
	systems.push_back("directfb");
#endif // DIRECTFB

}

void runAllTests( int *result ) {
	*result = RUN_ALL_TESTS();
	System::getSystem()->post( NULL, boost::bind(&canvas::System::exit,System::getSystem()) );
}

int main( int argc, char **argv ) {
	int result = 0;
	int partialResult = 0;

	testing::InitGoogleTest( &argc, argv );
	util::main::init("canvas-test");
	util::log::setLevel("canvas", "all", "trace");

	std::vector<std::string> systems;
	std::string error = "Using systems: ";
	getSystems( systems );

	BOOST_FOREACH( std::string name, systems ) {
		if (impl::init( name )) {
			System::getSystem()->post( NULL, boost::bind( &runAllTests, &partialResult ) );
			System::getSystem()->run();

			util::log::flush();
			impl::fin();

			if (partialResult!=0) {
				error += name;
				error += " ";
				result += partialResult;
				partialResult = 0;
			}
		}
		else {
			printf("[canvastest] Cannot initialize system: name=%s\n", name.c_str() );
			result += 1;
		}
	}

	util::main::fin();

	if (result!=0) {
		printf("[canvastest] Some tests failed!!! ( %s)\n", error.c_str());
	} else {
		printf("[canvastest] All tests passed!!!\n");
	}

	return result;
}
