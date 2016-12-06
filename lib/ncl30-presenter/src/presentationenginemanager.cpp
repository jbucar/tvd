/******************************************************************************
 Este arquivo eh parte da implementacao do ambiente declarativo do middleware
 Ginga (Ginga-NCL).

 Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

 Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
 os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
 Software Foundation.

 Este programa eh distribuido na expectativa de que seja util, porem, SEM
 NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
 ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
 GNU versao 2 para mais detalhes.

 Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
 com este programa; se nao, escreva para a Free Software Foundation, Inc., no
 endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

 Para maiores informacoes:
 ncl @ telemidia.puc-rio.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://www.telemidia.puc-rio.br
 ******************************************************************************
 This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

 Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.
 Copyright 2010 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata


 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
 details.

 You should have received a copy of the GNU General Public License version 2
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

 For further information contact:
 ncl @ telemidia.puc-rio.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://www.telemidia.puc-rio.br
 *******************************************************************************/

#include "presentationenginemanager.h"
#include "../include/FormatterMediator.h"
#include "../include/adapters/FormatterPlayerAdapter.h"
#include "../include/privatebase/IPrivateBaseManager.h"
#include "generated/config.h"
#include <gingaplayer/system.h>
#include <gingaplayer/device.h>
#include <gingaplayer/player/settings.h>
#include <canvas/size.h>
#include <canvas/system.h>
#include <canvas/canvas.h>
#include <util/fs.h>
#include <util/keydefs.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/filesystem/operations.hpp>

using std::string;
using std::vector;
using std::map;

using ::br::pucrio::telemidia::ginga::ncl::FormatterMediator;
using ::br::pucrio::telemidia::ginga::ncl::PrivateBaseManager;
using ::br::pucrio::telemidia::ginga::ncl::adapters::FormatterPlayerAdapter;

namespace bfs = boost::filesystem;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {

PresentationEngineManager::PresentationEngineManager( player::System *sys ) :
		_sys( sys ),
		_formatter( NULL ),
		_paused( false ),
		_stopped( false ),
		_currentPrivateBaseId( "-1" ) {
	DTV_ASSERT( _sys );
}

PresentationEngineManager::~PresentationEngineManager() {
}

void PresentationEngineManager::setBaseId( const std::string &id ) {
	_currentPrivateBaseId = id;
}

//	Compare
static boost::condition_variable _stopWakeup;
static boost::mutex _stopMutex;

/**
 * Comienza la ejecución de un documento NCL.
 * @param file Ruta del documento NCL
 */
bool PresentationEngineManager::startDocument( const std::string &file ) {
	LDEBUG( "PresentationEngineManager", "Start document: doc=%s", file.c_str() );

	if (formatter()) {
		_sys->enqueue( boost::bind( &PresentationEngineManager::stopDocument, this ) );
		{ //	Wait for compare
			boost::unique_lock<boost::mutex> lock( _stopMutex );
			while (!_stopped) {
				_stopWakeup.wait( lock );
			}
			_stopped = false;
		}

	}

	//	Load settings module
	player::settings::load();

	initFormatter( file );
	if (!formatter()->parseDocument()) {
		LWARN( "PresentationEngineManager", "parseDocument fail" );
		return false;
	}

	{	//	Add fonts
		canvas::Canvas *c = mainScreen()->system()->canvas();

		{	//	Add ginga fonts
			std::string toolFonts = util::fs::make( util::fs::installDataDir(), "fonts" );
			c->addFontDirectory( toolFonts );
		}

		{	//	Add ncl fonts
			bfs::path p(file);
			c->addFontDirectory( p.parent_path().string() );
		}
	}

	//	Play formatter
	_sys->enqueue( boost::bind( &FormatterMediator::play, formatter() ) );

	return true;
}

/**
 * Detiene la ejecución de un documento NCL.
 */
void PresentationEngineManager::stopDocument() {
	LDEBUG( "PresentationEngineManager", "Stop document" );

	_stopMutex.lock();

	player::settings::unload();

	//	Stop formatter
	formatter()->stop();
	delFormatter();

	//	Clear custom font directories
	mainScreen()->system()->canvas()->clearFontDirectories();

	_stopped = true;
	_stopMutex.unlock();
	_stopWakeup.notify_one();
}

/**
 * Comienza la ejecución del sistema, realizando todas las tareas necesarias previas a correr un documento NCL.
 */
bool PresentationEngineManager::run() {
	if (!_sys->canRun()) {
		LWARN( "PresentationEngineManager", "no device configurated" );
		return false;
	}

	onStart();

	//	Run canvas loop
	_sys->run();

	mainScreen()->system()->canvas()->beginDraw();
	stopDocument();
	mainScreen()->system()->canvas()->endDraw();

	onStop();

	return true;
}

/**
 * Registra la función que manejará los eventos de teclado
 * @param callback función manejadora de eventos
 */
void PresentationEngineManager::onKeyPress( const KeyPressCallback &callback ) {
	FormatterPlayerAdapter::onKeyPress( callback );
	FormatterFocusManager::onKeyPress( callback );
}

//	Enqueue task into presentation thread
void PresentationEngineManager::enqueue( const boost::function<void( void )> &func ) {
	_sys->enqueue( func );
}

player::Device *PresentationEngineManager::mainScreen() const {
	return _sys->getDevice( "systemScreen(0)", 0 );
}

player::System *PresentationEngineManager::sys() {
	return _sys;
}

FormatterMediator *PresentationEngineManager::formatter() {
	return _formatter;
}

const std::string &PresentationEngineManager::baseId() {
	return _currentPrivateBaseId;
}

void PresentationEngineManager::initFormatter( const std::string &file ) {
	_formatter = new FormatterMediator( _currentPrivateBaseId, file, _sys );
}

void PresentationEngineManager::delFormatter() {
	DEL( _formatter );
}

void PresentationEngineManager::onStart() {
}

void PresentationEngineManager::onStop() {
}

}
}
}
}
}
