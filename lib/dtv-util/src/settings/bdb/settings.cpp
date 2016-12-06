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
#include "../../string.h"
#include "../../assert.h"
#include <string.h>
#include <boost/algorithm/string/predicate.hpp>
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4510)
#pragma warning(disable:4512)
#pragma warning(disable:4610)
#endif
#include <db.h>
#ifdef _WIN32
#pragma warning(pop)
#endif

namespace util {
namespace bdb {

class SettingsError : public std::exception {
public:
	SettingsError( const std::string reason ) : _reason( reason ) {}
	virtual ~SettingsError() throw() {}

	virtual const char* what() const throw() {
		return _reason.c_str();
	}

	std::string _reason;
};

Settings::Settings( const std::string &name )
	: util::Settings( name )
{
}

Settings::~Settings()
{
	DTV_ASSERT(!_dbp);
}

//	Initialization/finalization
bool Settings::initialize() {
	u_int32_t flags;
	int ret;

	LDEBUG("bdb", "Initialize settings");

	//	Initialize db structure
	ret = db_create(&_dbp, NULL, 0);
	if (ret) {
		LERROR( "bdb", "Cannot create bdb databse" );
		return false;
	}

	//	Database open flags
	flags = DB_CREATE;	//	If the database does not exist, create it.

	//	Set name
	std::string dbName = name();
	if (!boost::algorithm::iends_with(dbName, ".bdb")) {
		dbName += ".bdb";
	}

	//	Try open database ...
	ret = _dbp->open( _dbp, NULL, dbName.c_str(), NULL, DB_BTREE, flags, 0 );
	if (ret) {
		LWARN( "bdb", "Cannot open database, try remove it" );

		//	Remove database
		_dbp->remove( _dbp, dbName.c_str(), NULL, 0 );

		//	Invalid? truncate and try open again!
		flags = DB_TRUNCATE;
		ret = _dbp->open(_dbp, NULL, dbName.c_str(), NULL, DB_BTREE, (DBTYPE)flags, 0 );
		if (ret) {
			LERROR( "bdb", "Unable to open bdb database: file=%s, error=%s",
				dbName.c_str(), db_strerror(ret) );
			finalize();
			return false;
		}
	}

	return true;
}

void Settings::finalize() {
	LDEBUG("bdb", "Finalize settings");
	if (_dbp) {
		_dbp->close(_dbp, 0);
		_dbp = NULL;
	}
}

bool Settings::commit() {
	return _dbp->sync( _dbp, 0 ) ? false : true;
}

//	Clear all values
void Settings::clear() {
	u_int32_t cant=0;
	if (_dbp->truncate(_dbp, NULL, &cant, 0)) {
		LWARN("bdb", "Fail to clear database");
	} else {
		LTRACE("bdb", "%u records deleted from database", cant);
	}
}

void Settings::clear( const std::string &key ) {
	DBT dKey;
	memset(&dKey, 0, sizeof(DBT));
	dKey.data = (void*)key.c_str();
	dKey.size = key.length()+1;
	_dbp->del( _dbp, NULL, &dKey, 0 );
}

//	Get/Put values
void Settings::get( const std::string &key, bool &value ) const {
	loadImpl( key, value );
}

void Settings::put( const std::string &key, bool value ) {
	saveImpl( key, value );
}

void Settings::get( const std::string &key, int &value ) const {
	loadImpl( key, value );
}

void Settings::put( const std::string &key, int value ) {
	saveImpl( key, value );
}

void Settings::get( const std::string &key, unsigned int &value ) const {
	loadImpl( key, value );
}

void Settings::put( const std::string &key, unsigned int value ) {
	saveImpl( key, value );
}

void Settings::get( const std::string &key, unsigned long int &value ) const {
	loadImpl( key, value );
}

void Settings::put( const std::string &key, unsigned long int value ) {
	saveImpl( key, value );
}

void Settings::get( const std::string &key, long int &value ) const {
	loadImpl( key, value );
}

void Settings::put( const std::string &key, long int value ) {
	saveImpl( key, value );
}

void Settings::get( const std::string &key, long long int &value ) const {
	loadImpl( key, value );
}

void Settings::put( const std::string &key, long long int value ) {
	saveImpl( key, value );
}

void Settings::get( const std::string &key, unsigned long long int &value ) const {
	loadImpl( key, value );
}

void Settings::put( const std::string &key, unsigned long long int value ) {
	saveImpl( key, value );
}

void Settings::get( const std::string &key, float &value ) const {
	loadImpl( key, value );
}

void Settings::put( const std::string &key, float value ) {
	saveImpl( key, value );
}

void Settings::get( const std::string &key, double &value ) const {
	loadImpl( key, value );
}

void Settings::put( const std::string &key, double value ) {
	saveImpl( key, value );
}

void Settings::get( const std::string &key, std::string &value ) const {
	DBT ret;
	if (load( key, ret )) {
		value.assign( (const char *)ret.data, ret.size );
	}
}

void Settings::put( const std::string &key, const std::string &value ) {
	save( key, (void *)value.c_str(), value.length() );
}

void Settings::get( const std::string &key, Buffer &buf ) const {
	DBT ret;
	if (load( key, ret )) {
		buf.copy( (const char *)ret.data, ret.size );
	}
}

void Settings::put( const std::string &key, const Buffer &buf ) {
	save( key, buf.data(), buf.length() );
}

void Settings::save( const std::string &key, const void *data, size_t len ) {
	DBT dKey, dData;

	//	Zero out the DBTs before using them.
	memset(&dKey, 0, sizeof(DBT));
	memset(&dData, 0, sizeof(DBT));

	//	Assign key
	dKey.data = (void *)key.c_str();
	dKey.size = key.length()+1;

	dData.data = (void *)data;
	dData.size = len;

	//	Store in db
	int ret = _dbp->put( _dbp, NULL, &dKey, &dData, 0 );
	if (ret) {
		throw SettingsError( util::format( "Unable to save key %s into db: %s", key.c_str(), db_strerror(ret) ) );
	}
}

bool Settings::load( const std::string &key, DBT &dData ) const {
	DBT dKey;

	//	Zero out the DBTs before using them.
	memset(&dKey, 0, sizeof(DBT));
	memset(&dData, 0, sizeof(DBT));

	//	Assign key
	dKey.data = (void *)key.c_str();
	dKey.size = key.length()+1;

	//	Retrieve data from db
	int ret = _dbp->get( _dbp, NULL, &dKey, &dData, 0 );
	if (ret == DB_NOTFOUND) {
		return false;
	} else if (ret) {
		throw SettingsError( util::format( "Unable to load key %s from db: %s", key.c_str(), db_strerror(ret)) );
	}
	return true;
}

void Settings::load( const std::string &key, void *value, size_t len ) const {
	DBT data;
	if (load( key, data )) {
		if (data.size != len) {
			throw SettingsError( util::format( "Value length of key %s differ from expected: %d", key.c_str(), len) );
		}
		memcpy( value, data.data, len );
	}
}

}
}

