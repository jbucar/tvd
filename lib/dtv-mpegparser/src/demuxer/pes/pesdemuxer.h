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

namespace util {
	class Buffer;
}

namespace tuner {

/**
 * Esta clase es la encargada de demultiplexar un PES (packetized elementary stream)
 */
class PESDemuxer {
public:
	PESDemuxer( pes::type::type pesType );
	virtual ~PESDemuxer();

	//	Getters
	pes::type::type type() const;

	//  Parser operations
	void parse( BYTE *tsPayload, size_t len );
	void parse( BYTE *tsPayload, size_t len, bool start );

protected:
	//	Virtual methods
	virtual bool checkSID( BYTE sID );
	virtual bool checkLen( size_t len ) const;
	virtual bool checkSynHeader( BYTE dataID, BYTE privateID ) const;
	virtual int parsePayload( BYTE *ptr, size_t len, size_t &off );

	//	Aux
	bool fillData( int minSize, bool fill, BYTE *ptr, size_t len, size_t &ptrOff );
	bool fillData( int minSize, BYTE *data, int &dataOff, BYTE *ptr, size_t len, size_t &ptrOff );
	bool fillPayload( BYTE *ptr, size_t len, size_t &ptrOff );
	QWORD pts() const;
	QWORD dts() const;
	BYTE *data() const;
	int dataOff() const;

private:
	//	Demux
	pes::type::type _pesType;

	//	Parser
	int _state;
	util::BYTE *_data;
	int _dataOff;

	//	PES fields
	BYTE _fields[3];
	QWORD _pts;
	QWORD _dts;

	util::DWORD _sync;
	util::BYTE _sID;
	int _packetLen;
	int _payloadSize;
	util::BYTE _syncHeaderLen;
};

}
