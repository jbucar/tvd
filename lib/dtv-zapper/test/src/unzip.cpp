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

#include "generated/config.h"

#if ZAPPER_LOGOS_USE_NETWORK
#include "util.h"
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

namespace zapper {
	bool unzipFile( const std::string &parent, const std::string &recvZipFile );
}

namespace fs = boost::filesystem;

const char *logos_filenames[] = {
				"logos/logos.xml", "logos/images/telefe.jpg", "logos/images/a24.jpg",
				"logos/images/canal9larioja.jpg","logos/images/tateti.jpg","logos/images/pakapaka.jpg",
				"logos/images/senalu.jpg", "logos/images/canal12posadas.jpg", "logos/images/acuamayor.jpg",
				"logos/images/castv.jpg", "logos/images/telesur.jpg", "logos/images/cn23.jpg",
				"logos/images/arpeggio.jpg", "logos/images/digo.jpg", "logos/images/viajar.jpg",
				"logos/images/telemax.jpg", "logos/images/acuafederal.jpg", "logos/images/acequiatv.jpg",
				"logos/images/incaatv.jpg", "logos/images/universidad.jpg", "logos/images/amarica.jpg",
				"logos/images/stars.jpg", "logos/images/g360.jpg", "logos/images/c5n.jpg",
				"logos/images/construir.jpg", "logos/images/canal10cordoba.jpg", "logos/images/canal9.jpg",
				"logos/images/vivra.jpg", "logos/images/canal26.jpg", "logos/images/canal7jujuy.jpg",
				"logos/images/lapachotv.jpg", "logos/images/deportv.jpg", "logos/images/encuentro.jpg",
				"logos/images/canal5rosario.jpg", "logos/images/canal3santarosa.jpg", "logos/images/cba24n.jpg",
				"logos/images/tecnopolis.jpg", "logos/images/tvpublica.jpg"
};

TEST( UnzipTest, unzip_file ) {
	std::string filename = util::getTestResource("logos.zip");

	fs::path dstName = "/tmp";
	dstName /= "unzip_file%%%%%%%%";
	dstName = fs::unique_path(dstName);

	ASSERT_TRUE( zapper::unzipFile(dstName.string(), filename) );
	for(unsigned int i=0; i < sizeof(logos_filenames) / sizeof(logos_filenames[0]); ++i) {
		fs::path logos_filename = dstName;
		logos_filename /= logos_filenames[i];
		ASSERT_TRUE( fs::exists( logos_filename ));
	}

	fs::remove_all(dstName);
}
#endif

