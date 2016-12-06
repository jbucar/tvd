/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "system.h"
#include "device.h"
#include "player/settings.h"
#include "player/keys.h"
#include <canvas/inputmanager.h>
#include <canvas/system.h>
#include <canvas/canvas.h>
#include <util/task/dispatcher.h>
#include <util/io/dispatcher.h>
#include <util/assert.h>
#include <util/mcr.h>

namespace player {

/**
 * Construye un objeto @c System.
 */
System::System()
{
	keys::init();
}

/**
 * Destruye un objeto @c System.
 */
System::~System()
{
	destroy();
	keys::fin();
}

/**
 * @return True si el @c System puede iniciar, false caso contrario.
 * @note Para que pueda iniciar es necesario que haya al menos un device.
 */
bool System::canRun() const {
	return !_devices.empty();
}

/**
 * Comienza a monitorear la ocurrencia de eventos, inicia al @c canvas::System y a los dispositivos.
 */
void System::run() {
	//	Setup reserve keys
	canvasSystem()->input()->onReservationChanged().connect( boost::bind(&System::onReserveKeys,this,_1) );

	//	Setup devices
	BOOST_FOREACH( Device *dev, _devices ) {
		dev->start();
	}

	//	Run canvas
	canvasSystem()->run();

	//	Stop devices
	BOOST_FOREACH( Device *dev, _devices ) {
		dev->stop();
	}
}

/**
 * Provoca la finalización del @c System.
 */
void System::exit() {
	//	Stop canvas
	canvasSystem()->exit();
}

/**
 * Agrega un dispositivo a la lista dispositivos del @c System.
 * @param dev Dispositivo a agregarse.
 */
void System::addDevice( Device *dev ) {
	_devices.push_back( dev );
	dev->system()->dispatcher()->registerTarget( this, "player::System" );
}

/**
 * Destruye un dispositivo.
 * @param dev Dispositivo a destruir.
 * @note El dispositivo debe estar en la lista de dispositivos del @c System.
 */
void System::destroy( Device *dev ) {
	std::vector<Device *>::iterator it=std::find(
		_devices.begin(), _devices.end(), dev );
	if (it != _devices.end()) {
		destroyDevice( dev );
		_devices.erase( it );
	}
}

/**
 * Destruye todos los dispositivos de la lista de dispositivos del @c System.
 */
void System::destroy() {
	BOOST_FOREACH( Device *dev, _devices ) {
		destroyDevice( dev );
	}
	_devices.clear();
}

void System::destroyDevice( Device *dev ) {
	dev->system()->dispatcher()->unregisterTarget( this );
	dev->finalize();
	delete dev;
}

template<typename T>
static Device *findDevice( const std::vector<Device *> &devs, T cmp, int num ) {
	std::vector<Device *>::const_iterator it=devs.begin();
	int i=0;
	while (it != devs.end()) {
		if (cmp((*it))) {
			if (num == i) {
				return (*it);
			}
			else {
				i++;
			}
		}
		it++;
	}
	return NULL;
}

/**
 * Retorna un dispositivo del @c System.
 * @param device Nombre del dispositivo a retornar.
 * @param devNumber Número del dispositivo.
 * @return Puntero al dispositivo requerido, o NULL si éste no existe.
 * @note @em device puede tomar los valores de systemScreen o systemAudio.
 */
Device *System::getDevice( const std::string &device, int devNumber ) const {
	//	SystemScreen
	size_t found = device.find( "systemScreen" );
	if (found != std::string::npos) {
		return findDevice( _devices, boost::bind(&Device::haveVideo,_1), devNumber );
	}

	//	SystemAudio
	found = device.find( "systemAudio" );
	if (found != std::string::npos) {
		return findDevice( _devices, boost::bind(&Device::haveAudio,_1), devNumber );
	}

	return NULL;
}

void System::onRunTask( canvas::Canvas *canvas, const boost::function<void (void)> &task ) {
	canvas->beginDraw();
	task();
	canvas->endDraw();
}

/**
 * Encola una tarea para ser ejecutada en el hilo del loop principal del @c canvas::System.
 * @param func Puntero a la función con la tarea a encolar.
 */
void System::enqueue( const boost::function<void (void)> &func ) {
	canvasSystem()->post( this, boost::bind(&System::onRunTask,this,canvasSystem()->canvas(),func) );
}

/**
 * Despacha un evento de teclado.
 * @param key El código de la tecla que fue pulsada o soltada.
 * @param isUp True si el evento se generó por la pulsación de una tecla, false caso contrario.
 * @return True si se ha tenido éxito despachando el evento de teclado, false caso contrario.
 */
bool System::dispatchKey( ::util::key::type key, bool isUp ) {
	return canvasSystem()->input()->dispatchKey( key, isUp );
}

/**
 * Si existe un listener con el id proporcionado se configura a éste para maneje que las teclas pasadas como parámetro,
 * si no existe se lo crea.
 * @param id Número identificador del objeto Listener.
 * @param callback Función que se ejecutará cuándo alguna de las teclas reservadas sean presionadas.
 * @param keys Conjunto de teclas que serán tenidas en cuenta por el listener cuándo sean presionadas.
 * @return True si se ha tenido éxito agregando y/o configurando al listener, false caso contrario.
 */
bool System::addInputListener( canvas::input::ListenerID id, const canvas::input::Callback &callback, const ::util::key::Keys &keys ) {
	canvasSystem()->input()->reserveKeys( id, callback, keys );
	return true;
}

/**
 * Elimina a un objeto listener. Esto implica que las teclas reservadas por este listener ya no serán atendidas.
 * @param id Número identificador del objeto Listener.
 * @return True si se ha tenido éxito eliminando al objeto listener, false caso contrario.
 */
bool System::delInputListener( canvas::input::ListenerID id ) {
	canvasSystem()->input()->releaseKeys( id );
	return true;
}

void System::dispatchButton( canvas::input::ButtonEvent *evt ) {
	canvasSystem()->input()->dispatchButton( evt );
}

boost::signals2::connection System::addButtonListener(const ButtonCallback &callback) {
	return canvasSystem()->input()->onButton().connect( callback );
}

canvas::System *System::canvasSystem() const {
	DTV_ASSERT( _devices[0] );
	return _devices[0]->system();
}

void System::onReserveKeys( const ::util::key::Keys &keys ) {
	BOOST_FOREACH( Device *dev, _devices ) {
		dev->onReserveKeys( keys );
	}
}

//	Editing command
/**
 * Configura que función será llamada cuándo se produzca un editing command.
 * @param callback Función que será llamada.
 */
void System::onEditingCommand( const OnEditingCommand &callback ) {
	_onEditingCommand = callback;
}

/**
 * Despacha un evento de editing command.
 * @param event Evento a despachar.
 */
void System::dispatchEditingCommand( connector::EditingCommandData *event ) {
	if (!_onEditingCommand.empty()) {
		_onEditingCommand( event );
	}
}

//	Timers
/**
 * Registra un timer, que causará la ejecución de @em callback luego de transcurridos @em ms milisegundos.
 * @return El id del timer registrado.
 */
util::id::Ident System::registerTimer( ::util::DWORD ms, const canvas::EventCallback &callback ) {
	return canvasSystem()->io()->addTimer( ms, callback );
}

/**
 * Cancela un timer que ha sido registrado con anterioridad.
 * @em timerID Número de identificación del timer que se desea cancelar.
 */
void System::unregisterTimer( util::id::Ident &timerID ) {
	canvasSystem()->io()->stopTimer( timerID );
}

/**
 * Cuándo se produzca un evento de entrada/salida en el descriptor @em fd se ejecutará la función @em callback.
 * @return Número identificador del IO.
 */
util::id::Ident System::addIO( util::DESCRIPTOR fd, const canvas::EventCallback &callback ) {
	return canvasSystem()->io()->addIO(fd, callback);
}

/**
 * Detiene el monitoreo de un descriptor, esto es, que futuros eventos que alteren al descriptor no causarán
 * la ejecución de ninguna función.
 * @param id Número identificador del IO.
 */
void System::stopIO( util::id::Ident &id ) {
	canvasSystem()->io()->stopIO( id );
}

//	Socket events
/**
 * Agrega un nuevo socket para ser escuchado.
 * @param sock Estructura que representa a un socket.
 * @param callback Función que se ejecutará cuando el socket reciba un evento.
 * @return Identificador del socket que escucha los cambios en el descriptor.
 */
util::id::Ident System::addSocket( util::SOCKET sock, const canvas::EventCallback &callback ) {
	return canvasSystem()->io()->addSocket(sock, callback);
}

/**
 * Detiene el monitoreo de determinado socket.
 * @param id Identificador del socket que se desea detener.
 */
void System::stopSocket( util::id::Ident &id ) {
	canvasSystem()->io()->stopSocket( id );
}


}

