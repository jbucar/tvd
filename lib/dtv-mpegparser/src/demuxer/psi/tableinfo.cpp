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
#include "tableinfo.h"
#include "psi.h"
#include <util/log.h>

namespace tuner {

TableInfo::TableInfo()
{
	_sections = 0;
	_current = _lastExpired = PSI_INVALID_VERSION;
}

TableInfo::~TableInfo()
{
}

bool TableInfo::checkSection( BYTE *section ) {
	bool parse=true;

	//	Get current and last section number
	BYTE curSection	 = PSI_NUMBER(section);
	size_t lastSection = PSI_LAST(section)+1;

	//	First section to parse?
	if (!_sections && lastSection) {
		_secsParsed.resize( lastSection, false );
	}
	else if (_secsParsed.size() != lastSection ||
		curSection > lastSection ||
		_secsParsed[curSection])
	{
		parse=false;
	}

	if (parse) {
		LTRACE( "TableInfo", "Parse section: sections=%04x, cur=%04x, last=%04x, table=%p",
			_sections, curSection, lastSection, this );

		//	Mark section parsed ...
		markSection( curSection );
	}

	return parse;
}

size_t TableInfo::sections() const {
	return _secsParsed.size();
}

void TableInfo::markSection( int sec ) {
	if (!_secsParsed[sec]) {
		_secsParsed[sec] = true;
		_sections++;
	}
}

void TableInfo::markSection( int first, int last ) {
	// LDEBUG( "TableInfo", "Mark sections: first=%06x, last=%06x, cant=%d",
	// 	first, last, _secsParsed.size() );
	if (last <= static_cast<int>(_secsParsed.size())) {
		while (first < last) {
			markSection( first );
			first++;
		}
	}
	else {
		LWARN( "TableInfo", "Mark of parsed sections invalid: first=%02x, last=%02x, size=%x",
			first, last, _secsParsed.size() );
	}
}

bool TableInfo::wasParsed( BYTE num ) const {
	bool result=false;
	if (num < _secsParsed.size()) {
		result=_secsParsed[num];
	}
	return result;
}

bool TableInfo::isComplete() const {
	//LDEBUG( "TableInfo", "count=%d, parsed=%d", _secsParsed.size(), _sections );
	return _secsParsed.size() == (size_t)_sections;
}

bool TableInfo::versionChanged( Version newVersion ) const {
	return _current != newVersion;
}

void TableInfo::update( Version ver ) {
	_sections = 0;
	_secsParsed.clear();
	_current = ver;
	reset();
}

void TableInfo::reset() {
}

bool TableInfo::expire( ID pid ) {
	if (_current != _lastExpired) {
		LDEBUG( "TableInfo", "Expire: pid=%04x, current=%x, last=%x", pid, _current, _lastExpired );
		_lastExpired = _current;
		return true;
	}
	return false;
}

Version TableInfo::current() const {
	return _current;
}

}

