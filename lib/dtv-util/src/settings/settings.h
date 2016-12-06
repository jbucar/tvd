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
#pragma once

#include "../buffer.h"
#include <string>

namespace util {

class Settings {
public:
	explicit Settings( const std::string &name );
	virtual ~Settings();

	//	Initialization/finalization
	virtual bool initialize();
	virtual void finalize();

	//	Commit transaction
	virtual bool commit();

	//	Clear values
	virtual void clear()=0;
	virtual void clear( const std::string &key )=0;

	//	Get/Put values
	virtual void get( const std::string &key, bool &value ) const=0;
	virtual void put( const std::string &key, bool value )=0;

	virtual void get( const std::string &key, int &value ) const =0;
	virtual void put( const std::string &key, int value )=0;

	virtual void get( const std::string &key, unsigned int &value ) const =0;
	virtual void put( const std::string &key, unsigned int value )=0;

	virtual void get( const std::string &key, long int &value ) const =0;
	virtual void put( const std::string &key, long int value )=0;

	virtual void get( const std::string &key, unsigned long int &value ) const =0;
	virtual void put( const std::string &key, unsigned long int value )=0;

	virtual void get( const std::string &key, long long int &value ) const =0;
	virtual void put( const std::string &key, long long int value )=0;	

	virtual void get( const std::string &key, unsigned long long int &value ) const =0;
	virtual void put( const std::string &key, unsigned long long int value )=0;	

	virtual void get( const std::string &key, float &value ) const =0;
	virtual void put( const std::string &key, float value )=0;

	virtual void get( const std::string &key, double &value ) const=0;
	virtual void put( const std::string &key, double value )=0;

	virtual void get( const std::string &key, std::string &value ) const =0;
	virtual void put( const std::string &key, const std::string &value )=0;
	void put( const std::string &key, const char *value );

	virtual void get( const std::string &key, Buffer &buf ) const=0;
	virtual void put( const std::string &key, const Buffer &buf )=0;

	//	Getters
	const std::string &name() const;

	//	Instance creation
	static Settings *create( const std::string &name, const std::string &useParam="" );

private:
	std::string _name;
};

}

