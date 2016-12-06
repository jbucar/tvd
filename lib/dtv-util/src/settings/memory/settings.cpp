/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

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

#include "settings.h"
#include "../../log.h"

namespace util {
namespace memory {

Settings::Settings( const std::string &name )
	: util::Settings( name )
{
}

Settings::~Settings()
{
}

//	Initialization/finalization
bool Settings::initialize() {
	LDEBUG("memory", "Initialize settings");
	return true;
}

void Settings::finalize() {
	LDEBUG("memory", "Finalize setting");
}

bool Settings::commit() {
	return true;
}

//	Clear all values
void Settings::clear() {
	_database.clear();
}

void Settings::clear( const std::string &key ) {
	_database.erase( key );
}

//	Get/Put values
void Settings::get( const std::string &key, bool &value ) const {
	getImpl( key, value );
}

void Settings::put( const std::string &key, bool value ) {
	putImpl( key, value );
}

void Settings::get( const std::string &key, int &value ) const {
	getImpl( key, value );
}

void Settings::put( const std::string &key, int value ) {
	putImpl( key, value );
}

void Settings::get( const std::string &key, unsigned int &value ) const {
	getImpl( key, value );
}

void Settings::put( const std::string &key, unsigned int value ) {
	putImpl( key, value );
}

void Settings::get( const std::string &key, unsigned long int &value ) const {
	getImpl( key, value );
}

void Settings::put( const std::string &key, unsigned long int value ) {
	putImpl( key, value );
}

void Settings::get( const std::string &key, long int &value ) const {
	getImpl( key, value );
}

void Settings::put( const std::string &key, long int value ) {
	putImpl( key, value );
}

void Settings::get( const std::string &key, long long int &value ) const {
	getImpl( key, value );
}

void Settings::put( const std::string &key, long long int value ) {
	putImpl( key, value );
}

void Settings::get( const std::string &key, unsigned long long int &value ) const {
	getImpl( key, value );
}

void Settings::put( const std::string &key, unsigned long long int value ) {
	putImpl( key, value );
}

void Settings::get( const std::string &key, float &value ) const {
	getImpl( key, value );
}

void Settings::put( const std::string &key, float value ) {
	putImpl( key, value );
}

void Settings::get( const std::string &key, double &value ) const {
	getImpl( key, value );
}

void Settings::put( const std::string &key, double value ) {
	putImpl( key, value );
}

void Settings::get( const std::string &key, std::string &value ) const {
	getImpl( key, value );
}

void Settings::put( const std::string &key, const std::string &value ) {
	putImpl( key, value );
}

void Settings::get( const std::string &key, Buffer &buf ) const {
	getImpl(key, buf);
}

void Settings::put( const std::string &key, const Buffer &buf ) {
	putImpl( key, buf );
}

void Settings::insert( const std::string &key, Any value ) {
	std::pair<Database::iterator,bool> res = _database.insert( Database::value_type(key, value) );
	if (!res.second) {
		res.first->second = value;
	}
}

}
}
