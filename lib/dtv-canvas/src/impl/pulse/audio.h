/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "../../audio.h"
#include <pulse/volume.h>
#include <pulse/context.h>

struct pa_context;
struct pa_threaded_mainloop;

namespace canvas {
namespace pulse {

class Audio : public canvas::Audio {
public:
	Audio();
	virtual ~Audio();

	//	Standby/wakeup
	virtual void standby();
	virtual void wakeup();

	//	Audio control
	void onNotification();
	bool ignoreNotifications();

protected:
	virtual bool init();
	virtual void fin();
	virtual bool open( const std::string &dev );
	virtual void close();
	
	virtual bool muteImpl( bool needsMute );
	virtual bool volumeImpl( Volume vol );
	virtual bool channelImpl( audio::channel::type channel );

	bool setVolume( pa_volume_t vol );
	bool setupContext();
	bool waitForOperation( pa_operation *op, const char *msg=NULL ) const;

	//devices
	virtual bool supportMultipleDevices();
	bool enumDevices();
	void onDeviceChanged();

private:
	pa_threaded_mainloop *_mainloop;
	pa_context *_context;
	std::string _sink;
	pa_volume_t _oldVolume;
	bool _oldMute;
	bool _ignoreNotifications;
};

}
}
