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

//	Based in ARIB TR-B14 Version 2.8-E2
#define DATA_GROUP_HEADER     5

//	Caption data and data group identification (ARIB STD-B24 Volume 1, Table 9-2)
#define GROUP_ID(grp)         ((grp & 0xFC) >> 2)
#define GROUP_VERSION(grp)    (grp & 0x03)
#define GROUP_IS_CAPTION_MANAGEMENT(id) (id == 0x0 || id == 0x20)

//	Types of data unit (ARIB STD-B24 Volume 1, Table 9-11)
#define DATA_UNIT_STATEMENT   0x20
#define DATA_UNIT_GEOMETRIC   0x28
#define DATA_UNIT_SYNTHESIZED 0x2c
#define DATA_UNIT_DRCS_1B     0x30
#define DATA_UNIT_DRCS_2B     0x31
#define DATA_UNIT_COLOR_MAP   0x34
#define DATA_UNIT_BITMAP      0x35

#define DATA_UNIT_SIZE(res,ptr,off)	\
	{ BYTE *data=ptr+off; res = ((data[0] << 16) | (data[1] << 8) | (data[2])); off += 3; }

