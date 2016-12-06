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

#include "../settings.h"
#include "../../log.h"
#include "../../xml/types.h"
#include "../../xml/documenthandler.h"
#include "../../xml/nodehandler.h"
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

namespace util {
namespace xml {

class Settings : public util::Settings {
public:
	explicit Settings( const std::string &name );
	virtual ~Settings();

	//	Initialization/finalization
	virtual bool initialize();
	virtual void finalize();

	virtual bool commit();

	//	Clear values
	virtual void clear();
	virtual void clear( const std::string &key );

	//	Get/Put values
	virtual void get( const std::string &key, bool &value ) const;
	virtual void put( const std::string &key, bool value );

	virtual void get( const std::string &key, int &value ) const;
	virtual void put( const std::string &key, int value );

	virtual void get( const std::string &key, unsigned int &value ) const;
	virtual void put( const std::string &key, unsigned int value );

	virtual void get( const std::string &key, long int &value ) const;
	virtual void put( const std::string &key, long int value );

	virtual void get( const std::string &key, unsigned long int &value ) const;
	virtual void put( const std::string &key, unsigned long int value );

	virtual void get( const std::string &key, long long int &value ) const;
	virtual void put( const std::string &key, long long int value );

	virtual void get( const std::string &key, unsigned long long int &value ) const;
	virtual void put( const std::string &key, unsigned long long int value );

	virtual void get( const std::string &key, float &value ) const;
	virtual void put( const std::string &key, float value );

	virtual void get( const std::string &key, double &value ) const;
	virtual void put( const std::string &key, double value );

	virtual void get( const std::string &key, std::string &value ) const;
	virtual void put( const std::string &key, const std::string &value );

	virtual void get( const std::string &key, Buffer &buf ) const;
	virtual void put( const std::string &key, const Buffer &buf );

protected:
	template<typename T>
	inline void putImpl( const std::string &key, T value );

	template<typename T>
	inline void getImpl( const std::string &key, T &value ) const;

private:
	dom::DocumentHandler *_doc;
	bool _needSync;
};

template<typename T>
inline void Settings::putImpl( const std::string &key, T value ) {
	dom::NodeHandler *hnd = _doc->handler();
	dom::Node node = _doc->root();

	std::vector<std::string> tokens;
	boost::split( tokens, key, boost::is_any_of(".") );

	BOOST_FOREACH( std::string nodeName, tokens ) {
		dom::Node tmp = hnd->child( node, nodeName );
		if (!tmp) {
			tmp = _doc->createElement(nodeName);
			hnd->appendChild(node, tmp);
		}
		node = tmp;
	}
	hnd->textContent( node, boost::lexical_cast<std::string>(value) );
	_needSync = true;
}

template<typename T>
inline void Settings::getImpl( const std::string &key, T &value ) const {
	dom::NodeHandler *hnd = _doc->handler();
	dom::Node node = _doc->root();

	std::vector<std::string> tokens;
	boost::split( tokens, key, boost::is_any_of(".") );

	BOOST_FOREACH( std::string nodeName, tokens ) {
		dom::Node tmp = hnd->child( node, nodeName );
		if (!tmp) {
			LOG_PUBLISH(DEBUG, "util", "xml", "Setting doesn't exist. key: %s", key.c_str());
			return;
		}
		node = tmp;
	}
	value = boost::lexical_cast<T>(hnd->textContent(node));
}

}
}
