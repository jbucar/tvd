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

#include "../../../types.h"
#include "../../../language.h"
#include "../dsmcc/dsmcc.h"
#include <vector>

//	ARIB STD - B21
#define SPECIFIER_TYPE(s)   util::BYTE(((s) & 0xFF000000) >> 24)
#define SPECIFIER_DATA(s)   ((s) & 0x00FFFFFF)
#define MAKER_ID(model)     (((model) & 0xFF00) >> 8)
#define MODEL_ID(model)     ((model) & 0x00FF)
#define GROUP_ID(version)   util::BYTE(((version) & 0xF000) >> 12)
#define VERSION_ID(version) util::WORD((version) & 0x0FFF)
#define DOWNLOAD_SPECIFIER  0xFF819282

namespace tuner {

class Sdtt {
public:
	typedef struct {
		QWORD startTime;
		DWORD duration;
	} ScheduleElementaryInfo;

	typedef struct {
		bool reboot;
		bool addOn;
		DWORD componentSize;
		DWORD downloadID;
		DWORD timeoutDII;
		DWORD leakRate;
		BYTE  componentTag;
		bool hasCompatibility;
		bool hasModuleInfo;
		dsmcc::compatibility::Descriptors compatibilities;
		dsmcc::module::Modules modules;
		Language lang;
		std::string text;
	} DownloadContentDescriptor;

    typedef struct {
    	BYTE group;
		WORD targetVersion;
		WORD newVersion;
		BYTE downloadLevel;
		BYTE versionIndicator;
    	std::vector<ScheduleElementaryInfo> schedules;
		BYTE scheduleTimeShiftInfo;
		std::vector<DownloadContentDescriptor> contents;
    } ElementaryInfo;

    Sdtt( Version version, ID modelID, ID tsId, ID netID, ID serviceId, std::vector<ElementaryInfo> elements );
    virtual ~Sdtt( void );

    //  Getters
    Version version() const;
	ID modelID() const;
	ID tsID() const;
    ID nitID() const;
    ID serviceID() const;
    const std::vector<ElementaryInfo> &elements() const;
	void show() const;

private:
    Version _version;
	ID _modelID;
    ID _tsID;
    ID _nitID;
    ID _serviceID;
    std::vector<ElementaryInfo> _elements;
};

}
