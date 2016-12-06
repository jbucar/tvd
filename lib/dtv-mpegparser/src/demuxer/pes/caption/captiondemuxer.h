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

#include "../pesdemuxer.h"
#include "captiondata.h"
#include <boost/function.hpp>

namespace tuner {
namespace arib {

class CaptionDemuxer : public PESDemuxer {
public:
	explicit CaptionDemuxer( bool isCC );
	virtual ~CaptionDemuxer();

	//  Caption Management notifications
	typedef boost::function<void ( Management * )> ManagementCallback;
	void onManagement( const ManagementCallback &callback );

	//  Caption Statement notifications
	typedef boost::function<void ( Statement * )> StatementCallback;
	void onStatement( const StatementCallback &callback );
	void filterGroup( BYTE groupID );

protected:
	typedef struct {
		BYTE groupID;
		QWORD pts;
		Buffer unit;
	} ParsedData;

	//	PESDemuxer virtual methods
	virtual bool checkSID( BYTE sID );
	virtual bool checkLen( size_t len ) const;
	virtual bool checkSynHeader( BYTE dataID, BYTE privateID ) const;
	virtual int parsePayload( BYTE *ptr, size_t len, size_t &off );

	//	Aux
	void parseDataGroup( BYTE *data, size_t len );
	void parseManagement( BYTE *packet, size_t len );
	void parseStatement( BYTE *packet, size_t len );
	bool parseDataUnit( BYTE *packet, size_t len );

private:
	bool _isCC;
	BYTE _groupID;
	ParsedData _parsed;
	ManagementCallback _onManagement;
	StatementCallback _onStatement;
};

}
}
