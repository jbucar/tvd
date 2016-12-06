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
#include "../../src/any.h"
#include "../../src/types.h"
#include "../../src/fs.h"
#include <gtest/gtest.h>

TEST( Paths, uniqueFile ) {
	std::string format = "temp_";
	std::string uniqueFile = util::mkTempFileName("temp_%%%");
	std::string uniqueFile2 = util::mkTempFileName("temp_%%%");

	ASSERT_TRUE(strncmp(format.c_str(),uniqueFile.c_str(),format.length()) == 0);
	ASSERT_FALSE(strncmp(format.c_str(),uniqueFile.c_str(),uniqueFile.length()) == 0);
	ASSERT_FALSE(uniqueFile.compare(uniqueFile2) == 0);
	
	uniqueFile = util::mkTempFileName("%%%_temp");
	uniqueFile2 = util::mkTempFileName("%%%_temp");

	ASSERT_TRUE(uniqueFile.compare(3,5,"_temp") == 0);
	ASSERT_TRUE(uniqueFile2.compare(3,5,"_temp") == 0);
	ASSERT_FALSE(uniqueFile.compare(uniqueFile2) == 0);
	
	uniqueFile = util::mkTempFileName("%te%mp%");
	uniqueFile2 = util::mkTempFileName("%te%mp%");
	
	ASSERT_TRUE(uniqueFile.compare(1,2,"te") == 0);
	ASSERT_TRUE(uniqueFile.compare(4,2,"mp") == 0);
	
	ASSERT_TRUE(uniqueFile2.compare(1,2,"te") == 0);
	ASSERT_TRUE(uniqueFile2.compare(4,2,"mp") == 0);
	
	ASSERT_FALSE(uniqueFile.compare(uniqueFile2) == 0);

}

