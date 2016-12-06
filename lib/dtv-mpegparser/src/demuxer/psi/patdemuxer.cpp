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
#include "patdemuxer.h"
#include "psi.h"
#include <util/log.h>
#include <set>

#define NETWORK_ID_PROGRAM 0

namespace tuner {

PATDemuxer::PATDemuxer( WORD pid )
: PSIDemuxer( pid )
{
	_nitPid  = TS_PID_NULL;
}

PATDemuxer::~PATDemuxer()
{
}

//	Getters
ID PATDemuxer::tableID() const {
	return PSI_TID_PAT;
}

bool PATDemuxer::supportCache() const {
	return true;
}

//  Signals
void PATDemuxer::onParsed( const ParsedCallback &callback ) {
    _onParsed = callback;
}

//  TSSectionDemuxer Virtual method
DWORD PATDemuxer::frequency() const {
	return 500;
}

void PATDemuxer::onSection( TableInfo * /*ver*/, BYTE *section, size_t len ) {
	//  Parse the programs
	for (size_t x=PSI_PAYLOAD_OFFSET; x<len; x+=4) {
		Pat::ProgramInfo info;
		info.program = GET_WORD( section+x );
		info.pid     = GET_PID( section+x+2 );
		_programs.push_back( info );
	}
}

void PATDemuxer::onComplete( TableInfo * /*ver*/, BYTE *section, size_t /*len*/ ) {
	//	Sanity check 
	check();
	
	//  PSI_CUSTOM = Transport Stream ID
	Pat *pat = new Pat( _info.current(), PSI_EXTENSION(section), _nitPid, _programs );
	notify<Pat>( _onParsed, pat );
}

TableInfo *PATDemuxer::getTable( BYTE * /*section*/ ) {
	return &_info;
}

void PATDemuxer::onVersionChanged( void ) {
	_nitPid = TS_PID_NULL;	
	_programs.clear();
}

//  Aux
void PATDemuxer::check() {
	std::set<ID> progs;
	Pat::Programs::iterator it=_programs.begin();
	
	while (it != _programs.end()) {
		bool erase = true;
		const Pat::ProgramInfo &prog = (*it);
		
		//	Check NIT
		if (prog.program == NETWORK_ID_PROGRAM) {
			if (_nitPid == TS_PID_NULL && checkPID( prog.pid, TS_PID_NIT )) {
				_nitPid = prog.pid;
			}
		}
		else {
			//	Check repeated programs and if PID is valid
			std::pair<std::set<ID>::iterator,bool> ret = progs.insert( prog.program );
			if (ret.second && checkPID( prog.pid )) {
				erase=false;
			}
		}

		//	If need remove program ...
		if (erase) {
			if (prog.program != NETWORK_ID_PROGRAM) {
				LWARN( "PATDemuxer", "program removed: id=%04x, pid=%04x", prog.program, prog.pid );
			}
			it=_programs.erase( it );
		}
		else {
			it++;
		}
	}
}

}
