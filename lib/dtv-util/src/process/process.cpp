/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "process.h"
#include "../log.h"
#include "../assert.h"
#include <string.h>
#include <stdlib.h>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#ifdef _WIN32
#include <windows.h>
#include <process.h>
#define strdup _strdup
#define CLOSE_PROCESS(p) { CloseHandle(p); p = INVALID_PROCESS_ID; }
#else
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#define CLOSE_PROCESS(p) { p = INVALID_PROCESS_ID; }
#endif

namespace util {

namespace fs = boost::filesystem;

Process::Process( const std::string &cmd ) {
	_pid = INVALID_PROCESS_ID;
	addParam(cmd);
}

Process::Process( const std::string &cmd, const Params &params )
{
	_pid = INVALID_PROCESS_ID;
	addParam(cmd);
	parameters(params);
}

Process::Process( const std::string &cmd, const Environment &env )
{
	_pid = INVALID_PROCESS_ID;
	addParam(cmd);
	_environment = env;
}

Process::Process( const std::string &cmd, const Params &params, const Environment &env )
{
	_pid = INVALID_PROCESS_ID;
	addParam(cmd);
	parameters(params);
	_environment = env;
}

/**
 * Destructor de la clase.
 */
Process::~Process()
{
	clearParams();
	clearEnvironment();
	if (_pid!=INVALID_PROCESS_ID) {
		LWARN("Process", "killing process on destructor");
		kill();
	}
}

/**
 * Comienza la ejecución del proceso.
 * @return True en caso de iniciar correctamente el proceso, False de lo contrario.
 */
bool Process::run() {
	DTV_ASSERT(_pid==INVALID_PROCESS_ID);
	bool result=false;

	if (fs::exists( _parameters[0] )) {
		std::string cmd="";
		BOOST_FOREACH(std::string param, _parameters) {
			cmd += param;
			cmd += " ";
		}
		LINFO("Process", "Spawn: %s", cmd.c_str());
		result = true;
	} else {
		LWARN("Process", "Spawn fails. Could not found file=%s", _parameters[0].c_str());
	}

	if (result) {
#ifdef _WIN32
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );

		std::string cmdLine;
		BOOST_FOREACH ( std::string param,  _parameters) {
			cmdLine += param;
			cmdLine += " ";
		}

		char** env = _environment.size()>0 ? makeEnv() : NULL;

		//	Start process.
		result = ::CreateProcess(
			NULL,           // No module name (use command line)
			(char *)cmdLine.c_str(),// Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			env,		// Use parent's environment block
			NULL,           // Use parent's starting directory
			&si,            // Pointer to STARTUPINFO structure
			&pi             // Pointer to PROCESS_INFORMATION structure
		) == TRUE;

		freeParams(env);
		if (!result) {
			LERROR("Process", "CreateProcess failed (%d)", GetLastError());
		} else {
			//	Save process
			_pid = pi.hProcess;

			//	Close process and thread handles.
			CloseHandle( pi.hThread );
		}
#else
		//	Fork process
		_pid = ::fork();
		if (_pid == 0) {	// child
			//	Close all descriptors!!!
			int maxfd=::sysconf(_SC_OPEN_MAX);
			for(int fd=3; fd<maxfd; fd++) {
				close(fd);
			}

			char **params = makeParams();

			//	Replace process
			if (_environment.size()>0) {
				char** env = makeEnv();
				result=::execve( params[0], params, env ) >= 0;
				freeParams( env );
			}
			else {
				result=::execv( params[0], params ) >= 0;
			}
			freeParams( params );

			if (!result) {
				LERROR("Process", "cannot execute process: cmd=%s", _parameters[0].c_str());
			} else {
				LINFO("Process", "Running process: cmd=%s", _parameters[0].c_str());
			}
		} else if (_pid < 0) { // failed to fork
			LERROR("Process", "cannot fork process");
			result=false;
		} else {
			LINFO("Process", "Fork process ok: pid=%d", _pid);
			result=true;
		}
#endif
	}

	return result;
}

#define CHECK_PERIOD 250

/**
 * Detiene la ejecución del proceso.
 * @param msTimeout Se especifica el tiempo limite antes de matarlo.
 */
void Process::kill( int msTimeout/*=0*/ ) {
	LINFO("Process", "Kill process: pid=%d, timeout=%d", _pid, msTimeout);

	//	Wait for process to finish msTimeout
	bool running=isRunning();
	int transcurred = 0;
	while (running && msTimeout > transcurred) {
		boost::this_thread::sleep( boost::posix_time::milliseconds( CHECK_PERIOD ) );
		transcurred += CHECK_PERIOD;
		running=isRunning();
	}

	//	Process's run yet?
	if (running) {
#ifdef _WIN32
		::TerminateProcess( _pid, 10000 );
#else
		// Send kill
		::kill( _pid, SIGKILL );
#endif
	}

	//	Wait for process
	wait();
}

void Process::wait() {
	checkStatus(true);
}

/**
 * @return El pid del proceso.
 */
PROCESS_ID Process::pid() const {
	return _pid;
}

/**
 * @return Si esta en ejecución o no.
 */
bool Process::isRunning() {
	return checkStatus(false);
}

//	Params
/**
 * Agrega nuevos parámetros.
 * @param params Un vector con parámetros a insertar.
 */
void Process::parameters( const Params &params ) {
	_parameters.insert( _parameters.end(), params.begin(), params.end() );
}

/**
 * Agrega un parámetro nuevo.
 * @param param Un string con el nuevo parámetro a agregar.
 */
void Process::addParam( const std::string &param ) {
	_parameters.push_back(param);
}

/**
 * Limpia los parámetros.
 */
void Process::clearParams() {
	std::string cmd = _parameters[0];
	_parameters.clear();
	addParam(cmd);
}

/**
 * Agrega una variable de entorno.
 * @param var Un string con el formato variable=valor.
 */
void Process::addToEnvironment( const std::string &var ) {
	std::vector<std::string> result;
	boost::algorithm::split(result, var, boost::algorithm::is_any_of("="));
	DTV_ASSERT(result.size()==2);

	addToEnvironment(result[0], result[1]);
}

/**
 * Agrega una variable de entorno.
 * @param key Un string con el nombre de la variable.
 * @param value El valor a asignar a la variable key.
 */
void Process::addToEnvironment( const std::string &key, const std::string &value ) {
	std::string var = _environment[key];
	if (!var.empty()) {
		var += PATH_SEPARATOR;
	}
	var += value;
	_environment[key] = var;
}

/**
 * Limpia las variables de entorno.
 */
void Process::clearEnvironment() {
	_environment.clear();
}

bool Process::checkStatus( bool wait ) {
	bool result=false;
	if (_pid != INVALID_PROCESS_ID) {
		//	Check if process was ended!
		bool end;
#ifdef _WIN32
		end=WaitForSingleObject( _pid, wait ? INFINITE : 0 ) == WAIT_OBJECT_0;
#else
		end=::waitpid( _pid, NULL, wait ? 0 : WNOHANG ) == _pid;
#endif
		//	Process isn't running?
		if (end) {
			CLOSE_PROCESS(_pid);
		}
		else {
			result=true;
		}
	}
	return result;
}

char **Process::makeParams() const {
	char **cmd = (char **)calloc( _parameters.size()+1, sizeof(char *) );

	int i=0;
	BOOST_FOREACH(std::string param, _parameters) {
		cmd[i] = strdup(param.c_str());
		i++;
	}
	cmd[i] = NULL;

	return cmd;
}

char** Process::makeEnv() const {
	char **cmd;

	size_t size = _environment.size();
	cmd = (char **)calloc( size+1, sizeof(char *) );

	int i=0;
	typedef std::pair<std::string,std::string> EnvironVar;
	BOOST_FOREACH(  EnvironVar var, _environment ) {
		std::string str;
		str += var.first.c_str();
		str += "=";
		str += var.second.c_str();
		cmd[i] = strdup(str.c_str());
		i++;
	}
	cmd[i] = NULL;

	return cmd;
}

void Process::freeParams( char **params ) {
	int i=0;
	if (params) {
		while (params[i]) {
			LDEBUG("Process", "Free param[%d] = '%s'", i, params[i]);
			free( params[i] );
			i++;
		}
		free( params );
	}
}

}
