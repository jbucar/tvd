/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-luaz implementation.

    DTV-luaz is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-luaz is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-luaz.

    DTV-luaz es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-luaz se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "tuner.h"
#include "../lua.h"
#include <zapper/zapper.h>
#include <pvr/pvr.h>
#include <util/log.h>
#include <cmath>

namespace luaz {
namespace tuner {

namespace impl {
	static pvr::Pvr *pvr = NULL;
	static boost::signals2::connection cScan;

	void scanChanged( lua_State *st, pvr::scan::Type scanState, const pvr::Pvr::ScanNetwork net ) {
		static int sCurNetwork = 0;

		LDEBUG( "luaz", "Scan changed: state=%d", scanState );
		switch (scanState) {
			case pvr::scan::begin: {
				sCurNetwork = 0;
				lua::call( st, "zapperBeginScan" );
				break;
			}
			case pvr::scan::network: {
				size_t networks = pvr->networks();
				int porcentage = floor( (sCurNetwork*100)/(double)networks );
				lua::call( st, "zapperScanNetwork", net.get().c_str(), porcentage );
				sCurNetwork++;
				break;
			}
			case pvr::scan::end: {
				sCurNetwork = 0;
				lua::call( st, "zapperEndScan" );
				break;
			}
		};
	}
}

static int l_isScanning( lua_State *L ) {
	bool isScanning = impl::pvr->isScanning();
	lua_pushboolean( L, isScanning );
	return 1;
}

static int l_startScan( lua_State * /*L*/ ) {
	impl::pvr->startScan();
	return 0;
}

static int l_cancelScan( lua_State * /*L*/ ) {
	impl::pvr->cancelScan();
	return 0;
}

static const struct luaL_Reg tuner_methods[] = {
	{ "isScanning", l_isScanning },
	{ "startScan",  l_startScan  },
	{ "cancelScan", l_cancelScan },
	{ NULL,         NULL         }
};

//	Public methods
void initialize( zapper::Zapper *zapper ) {
	impl::pvr = zapper->pvr();
}

void finalize() {
	impl::pvr = NULL;
}

void start( lua_State *st ) {
	boost::function<void (pvr::scan::Type,const pvr::Pvr::ScanNetwork)> fnc = boost::bind(&impl::scanChanged,st,_1,_2);
	impl::cScan = impl::pvr->onScanChanged().connect(fnc);
	luaL_register( st, "tuner", tuner_methods );
}

void stop() {
	impl::cScan.disconnect();
}

}
}

