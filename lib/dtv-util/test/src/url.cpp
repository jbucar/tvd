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

#include <gtest/gtest.h>
#include "../../src/url.h"

TEST( Url, basic_parse ) {
	std::string params="service://videoID=10,videoPID=11,audioID=12,audioPID=13,pcrPID=14";
	util::Url url( params );

	ASSERT_TRUE( url.type() == "service" );
	int videoID=0, videoPID=0, audioID=0, audioPID=0, pcrPID=0;
	ASSERT_TRUE( url.getParam( "videoID", videoID ) );
	ASSERT_TRUE( videoID == 10 );
	ASSERT_TRUE( url.getParam( "videoPID", videoPID ) );
	ASSERT_TRUE( videoPID == 11 );
	ASSERT_TRUE( url.getParam( "audioID", audioID ) );
	ASSERT_TRUE( audioID == 12 );
	ASSERT_TRUE( url.getParam( "audioPID", audioPID ) );
	ASSERT_TRUE( audioPID == 13 );
	ASSERT_TRUE( url.getParam( "pcrPID", pcrPID ) );
	ASSERT_TRUE( pcrPID == 14 );		
}
