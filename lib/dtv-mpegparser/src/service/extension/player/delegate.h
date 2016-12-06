/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "types.h"

namespace tuner {
namespace stream { class Pipe; }
namespace player {

class Player;

class Delegate {
public:
	Delegate();
	virtual ~Delegate();

	virtual const std::string url() const;

	//	Initialization
	bool initialize( Player *, pes::type::type esType );
	void finalize();

	//	Start/stop
	virtual void start();
	virtual void stop();

	//	Streams
	virtual StreamInfo *canPlay( const ElementaryInfo &info, ID tag ) const;
	virtual bool startStream( ID pid, pes::type::type esType );
	virtual void stopStream( ID pid );

protected:
	//	Initialization
	virtual bool init();
	virtual void fin();
	virtual pes::mode::type mode() const;
	virtual stream::Pipe *pipe( pes::type::type esType ) const;
	virtual const std::string urlType() const;

	Player *player() const;
	pes::type::type esType() const;

private:
	Player *_player;
	pes::type::type _esType;
};

class AvDelegate : public Delegate {
public:
	AvDelegate();
	virtual ~AvDelegate();

protected:
	virtual pes::mode::type mode() const;
	virtual const std::string urlType() const;
};

class ForwardDelegate : public Delegate {
public:
	explicit ForwardDelegate( Delegate *dlg );
	virtual ~ForwardDelegate();

	//	Streams
	virtual bool startStream( ID pid, pes::type::type esType );
	virtual void stopStream( ID pid );

private:
	Delegate *_dlg;
};

}
}

