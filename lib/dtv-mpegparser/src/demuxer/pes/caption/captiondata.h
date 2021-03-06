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

  Este archivo es parte de la implementaci�n de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los t�rminos de la Licencia P�blica General Reducida GNU como es publicada por la
  Free Software Foundation, seg�n la versi�n 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANT�A; ni siquiera la garant�a impl�cita de COMERCIALIZACI�N o ADECUACI�N
  PARA ALG�N PROP�SITO PARTICULAR. Para m�s detalles, revise la Licencia P�blica
  General Reducida GNU.

    Ud. deber�a haber recibido una copia de la Licencia P�blica General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#pragma once

#include "../types.h"
#include "../../text.h"
#include <util/buffer.h>
#include <vector>

namespace tuner {
namespace arib {

class CaptionData {
public:
	CaptionData( BYTE groupID, QWORD pts, util::Buffer &buf );
	virtual ~CaptionData( void );

	BYTE tmd() const;
	BYTE group() const;
	QWORD pts() const;
	const util::Buffer &dataUnit() const;
	void show() const;

private:
	BYTE _group;
	QWORD _pts;
	util::Buffer _unit;
};

typedef struct {
	BYTE tag;
	Language name;
	BYTE flags;
} CaptionLanguage;
typedef std::vector<CaptionLanguage> CaptionLanguages;

class Management : public CaptionData {
public:
	Management( BYTE groupID, QWORD pts, util::Buffer &buf, const CaptionLanguages &langs );
	virtual ~Management();

	const CaptionLanguages &langs() const;
	void show() const;

private:
	CaptionLanguages _langs;
};

typedef CaptionData Statement;

}
}

