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

#include "ppapi.h"
#include "key.h"
#include "system.h"
#include "../../inputmanager.h"
#include "generated/config.h"
#include <util/task/dispatcher.h>
#include <util/main.h>
#include <util/assert.h>
#include <util/cfg/configregistrator.h>
#include <util/string.h>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include "ppapi/c/pp_errors.h"
#include "ppapi/cpp/core.h"
#include "ppapi/cpp/fullscreen.h"
#include "ppapi/cpp/input_event.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/var.h"
#include <stdio.h>
#include <map>

namespace canvas {
namespace ppapi {

namespace impl {

const char* const kUsedInterfaces[] = {
  PPB_CORE_INTERFACE,
  PPB_CONSOLE_INTERFACE,
  NULL
};

}	//	namespace impl

class DTVInstance : public pp::Instance {
public:
	DTVInstance( PP_Instance instance );
	virtual ~DTVInstance();

	// pp::Instance implementation
	virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);
	virtual void DidChangeView(const pp::View& view);
	virtual bool HandleInputEvent(const pp::InputEvent& event);

	//	Implementation
	void attach( System *sys );
	void detach();

	pp::Size viewSize() const;

protected:
	void mainThread();

private:
	System *_sys;
	boost::thread _thread;
	boost::mutex _mutex;
	pp::Size _viewSize;
	std::map<std::string,std::string> _params;
};

DTVInstance::DTVInstance( PP_Instance instance )
	: pp::Instance(instance)
{
	fprintf( stderr, "[tvd] DTVIntance constructor: instance=%d\n", instance );

	_sys = NULL;
	RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE);
	RequestInputEvents(PP_INPUTEVENT_CLASS_KEYBOARD);
}

DTVInstance::~DTVInstance()
{
	fprintf( stderr, "[tvd] DTVIntance destructor\n" );

	//	Force exit
	_mutex.lock();
	if (_sys) {
		_sys->dispatcher()->post( _sys, boost::bind(&System::exit,_sys) );
	}
	_mutex.unlock();

	//	Wait tool thread
	_thread.join();
}

bool DTVInstance::Init(uint32_t argc, const char *argn[], const char *argv[]) {
	fprintf( stderr, "[tvd] Init: argc=%d\n", argc );

	{	//	Check interfaces
		size_t i=0;
		while (impl::kUsedInterfaces[i]) {
			if (!pp::Module::Get()->GetBrowserInterface(impl::kUsedInterfaces[i])) {
				fprintf( stderr, "[tvd] PPAPI Interface not found: iface=%s\n", impl::kUsedInterfaces[i] );
				return false;
			}
			i++;
		}
	}

	//	Find "ppapi_args" parameter
	std::string requiredParams;
	for (uint32_t i=0; i<argc; i++) {
		if (!strcmp("ppapi_args",argn[i])) {
			requiredParams = argv[i];
			break;
		}
	}
	if (requiredParams.empty()) {
		fprintf( stderr, "[tvd] PPAPI tool args 'ppapi_args' not found\n" );
		return false;
	}

	{	//	Copy required parameters
		//	Get list of required parameters
		std::vector<std::string> tokens;
		boost::split( tokens, requiredParams, boost::is_any_of( "," ) );

		//	Copy all parameters
		_params.clear();
		for (uint32_t i=0; i<argc; i++) {
			//	Is required?
			BOOST_FOREACH( const std::string &param, tokens ) {
				if (!strncmp(argn[i],param.c_str(),param.length())) {
					_params[argn[i]] = argv[i];
				}
			}
		}

		//	Check all
		BOOST_FOREACH( const std::string &param, tokens ) {
			if (_params.find( param ) == _params.end()) {
				fprintf( stderr, "[tvd] PPAPI requiered parameter not found: param=%s\n", param.c_str() );
				return false;
			}
		}
	}

	return true;
}

pp::Size DTVInstance::viewSize() const {
	return _viewSize;
}

void DTVInstance::DidChangeView( const pp::View& view ) {
	bool mustLaunch = _viewSize.width() == 0;
	_viewSize = view.GetRect().size();
	if (mustLaunch) {
		//	Create thread for tool
		_thread = boost::thread( boost::bind( &DTVInstance::mainThread, this ) );
	}
	fprintf( stderr, "[tvd] Did change view: size=(%d,%d)\n", _viewSize.width(), _viewSize.height() );
}

bool DTVInstance::HandleInputEvent(const pp::InputEvent& event) {
	bool handled=false;

	_mutex.lock();
	if (_sys) {
		handled = true;
		switch (event.GetType()) {
			case PP_INPUTEVENT_TYPE_MOUSEDOWN: {
				pp::MouseInputEvent mouse_event(event);
				break;
			}
			case PP_INPUTEVENT_TYPE_MOUSEMOVE: {
				pp::MouseInputEvent mouse_event(event);
				break;
			}
			case PP_INPUTEVENT_TYPE_KEYDOWN: {
				pp::KeyboardInputEvent key_event(event);
				util::key::type k = mapKey(key_event);
				_sys->dispatcher()->post( _sys, boost::bind(&canvas::InputManager::dispatchKey,_sys->input(),k,false) );
				break;
			}
			case PP_INPUTEVENT_TYPE_KEYUP: {
				pp::KeyboardInputEvent key_event(event);
				util::key::type k = mapKey(key_event);
				_sys->dispatcher()->post( _sys, boost::bind(&canvas::InputManager::dispatchKey,_sys->input(),k,true) );
				break;
			}
			case PP_INPUTEVENT_TYPE_CHAR: {
				//	Ignore
				break;
			}
			default: {
				fprintf( stderr, "[tvd] Event not handled: type=%d\n", event.GetType() );
				handled=false;
				break;
			}
		}
	}
	_mutex.unlock();

	return handled;
}

void DTVInstance::attach( System *sys ) {
	_mutex.lock();
	DTV_ASSERT(!_sys);
	_sys = sys;
	_mutex.unlock();
}

void DTVInstance::detach() {
	_mutex.lock();
	DTV_ASSERT(_sys);
	_sys = NULL;
	_mutex.unlock();
}

#define COPY_PARAM(p) argv[i++] = strdup(p);
void DTVInstance::mainThread() {
	size_t i=0;
	char **argv = (char **)malloc( sizeof(const char *) * ((_params.size()*2)+4) );

	COPY_PARAM( "ppapi" );

	std::string inst = util::format( "gui.ppapi.instance=%d", pp_instance() );
	COPY_PARAM( "--set" );
	COPY_PARAM( inst.c_str() );

	std::map<std::string,std::string>::const_iterator it=_params.begin();
	while (it != _params.end()) {
		std::string param = "--";
		param += it->first;
		COPY_PARAM( param.c_str() );
		COPY_PARAM( it->second.c_str() );
		it++;
	}
	argv[i++] = NULL;

	fprintf( stderr, "[tvd] Run main tool loop\n" );
	util::main::run_tvd_tool( i-1, argv );
	fprintf( stderr, "[tvd] stop main tool loop\n" );

	for (size_t x=0; x<i; x++) {
		free( argv[x] );
	}
	free( argv );
}

// This object is the global object representing this plugin library as long
// as it is loaded.
class DTVModule : public pp::Module {
public:
	DTVModule() : pp::Module() {}
	virtual ~DTVModule() {}

	virtual pp::Instance* CreateInstance(PP_Instance instance) {
		return new DTVInstance(instance);
	}
};

pp::Instance *getInstance( PP_Instance inst ) {
	return pp::Module::Get()->InstanceForPPInstance( inst );
}

pp::Size getViewSize( PP_Instance inst ) {
	DTVInstance *i = (DTVInstance *)getInstance( inst );
	if (i) {
		return i->viewSize();
	}
	return pp::Size(720, 576);
}

bool attach( PP_Instance inst, System *sys ) {
	DTVInstance *i = (DTVInstance *)getInstance( inst );
	if (i) {
		i->attach( sys );
		return true;
	}
	return false;
}

void detach( PP_Instance inst ) {
	DTVInstance *i = (DTVInstance *)getInstance( inst );
	if (i) {
		i->detach();
	}
}

}	//	namespace pppai
}	//	namespace canvas

//	-------- MAIN --------------
namespace pp {

// Factory function for your specialization of the Module object.
Module* CreateModule() {
	return new canvas::ppapi::DTVModule();
}

}	// namespace pp

