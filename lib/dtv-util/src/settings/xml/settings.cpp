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
#include "../../mcr.h"
#include "../../log.h"
#include "../../assert.h"
#include "../../functions.h"
#include "../../xml/nodehandler.h"
#include "../../xml/documenthandler.h"
#include "../../xml/documentserializer.h"
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

namespace bfs = boost::filesystem;

namespace util {
namespace xml {

Settings::Settings( const std::string &name )
	: util::Settings( name )
{
	_doc = NULL;
	_needSync=false;
}

Settings::~Settings()
{
	DTV_ASSERT(!_doc);
}

//	Initialization/finalization
bool Settings::initialize() {
	LDEBUG("xml", "Initialize settings");

	_doc = dom::DocumentHandler::create();
	if (!_doc->initialize()) {
		DEL(_doc);
		return false;
	}

	std::string filename = name() + ".xml";
	if (bfs::exists(filename)) {
		_doc->loadDocument(filename);
	} else {
		bfs::path path(name());
		_doc->createDocument(path.filename().string());
		_needSync = true;
	}

	return true;
}

void Settings::finalize() {
	DTV_ASSERT(_doc);
	commit();
	_doc->finalize();
	DEL(_doc);
	LDEBUG("xml", "Finalize setting");
}

bool Settings::commit() {
	if (_needSync) {
		xml::dom::FileDocumentSerializer serializer( name()+".xml" );
		_doc->traverse( serializer );
	}
	_needSync=false;
	return true;
}

//	Clear all values
void Settings::clear() {
	_doc->close();
	_doc->createDocument(name());
	_needSync=true;
}

void Settings::clear( const std::string &key ) {
	dom::NodeHandler *hnd = _doc->handler();
	dom::Node node = _doc->root();

	std::vector<std::string> tokens;
	boost::split( tokens, key, boost::is_any_of(".") );

	BOOST_FOREACH( std::string nodeName, tokens ) {
		dom::Node tmp = hnd->child( node, nodeName );
		if (!tmp) {
			return;
		}
		node = tmp;
	}
	if (node!=_doc->root()) {
		hnd->remove( &node );
	}
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
	dom::NodeHandler *hnd = _doc->handler();
	dom::Node node = _doc->root();

	std::vector<std::string> tokens;
	boost::split( tokens, key, boost::is_any_of(".") );

	BOOST_FOREACH( std::string nodeName, tokens ) {
		dom::Node tmp = hnd->child( node, nodeName );
		if (!tmp) {
			LDEBUG("xml", "Setting doesn't exist. key: %s", key.c_str());
			return;
		}
		node = tmp;
	}
	const std::string res = hnd->textContent(node);
	buf.resize( (size_t)floor(double(res.size()) * 0.75) );
	size_t len = util::base64decode( res, buf.bytes(), buf.capacity() );
	buf.resize(len);
}

void Settings::put( const std::string &key, const Buffer &buf ) {
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
	hnd->textContent( node, util::base64encode( buf.bytes(), buf.length() ) );
	_needSync = true;
}

}
}
