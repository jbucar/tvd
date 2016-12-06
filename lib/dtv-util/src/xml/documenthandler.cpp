/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "documenthandler.h"
#include "nodehandler.h"
#include "generated/config.h"
#if DOM_USE_XERCES
#	include "impl/xerces/documenthandler.h"
#endif
#if DOM_USE_TINYXML
#	include "impl/tinyxml/documenthandler.h"
#endif
#include "../log.h"
#include "../assert.h"
#include <boost/foreach.hpp>

namespace util {
namespace xml {
namespace dom {

namespace impl {
	static std::string docDeclaration = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n";
}

/**
 * Método estático utilizado para crear instancias de DocumentHandler.
 * @param useParam Indica la implementación interna a utilizar ( xerces | tinyxml ).
 * @return Una nueva instancia de DocumentHandler o NULL si hubo algun error en la creación.
 */
DocumentHandler *DocumentHandler::create( const std::string &useParam/*=""*/ ) {
	std::string use;

	if ( useParam.empty() ) {
#if DOM_USE_XERCES
		use = "xerces";
#elif DOM_USE_TINYXML
		use = "tinyxml";
#endif
	} else {
		use = useParam;
	}

	LTRACE("Util", "Using xml parser: %s", use.c_str());

#if DOM_USE_XERCES
	if ( use == "xerces" ) {
		return new xerces::DocumentHandler();
	}
#endif
#if DOM_USE_TINYXML
	if ( use == "tinyxml" ) {
		return new tinyxml::DocumentHandler();
	}
#endif
	LERROR("Util", "No xml handler specified");
	return NULL;
}

DocumentHandler::DocumentHandler() {
	_document = NULL;
	_initialized = false;
}

/**
 * Destructor base.
 */
DocumentHandler::~DocumentHandler() {
	DTV_ASSERT(!_document);
}

/**
 * Inicializa la instancia. Este método debe ser llamado antes de cualquier otro método.
 * @return true si la inicialización finalizó correctamente false en caso contrario.
 */
bool DocumentHandler::initialize() {
	LDEBUG("DocumentHandler", "initialize()");
	DTV_ASSERT(!_initialized);
	_initialized = init();
	return _initialized;
}

/**
 * Libera los recursos utilizados por el DocumentHandler. Debe ser llamado antes de destruir la instancia.
 */
void DocumentHandler::finalize() {
	LDEBUG("DocumentHandler", "finalize()");
	if (_document) {
		close();
	}
	if (_initialized) {
		fin();
		_initialized = false;
	}
}

/**
 * @return true si el DocumentHandler se encuentra inicializado, false en caso contrario.
 */
bool DocumentHandler::initialized() const {
	return _initialized;
}

/**
 * @return El nodo raiz del documento si algún documento fue creado o cargado, NULL en caso contrario.
 */
Node DocumentHandler::root() const {
	if (!_document) {
		LWARN("DocumentHandler", "Fail to get root element! Document does not exist");
		return NULL;
	}
	return rootImpl( _document );
}

/**
 * Carga un documento xml desde un string.
 * @param xml El xml a cargar.
 * @return true si pudo cargar el archivo correctamente, false en caso contrario.
 * @see close()
 */
bool DocumentHandler::loadXML( const std::string &xml ) {
	if (_document) {
		LWARN("DocumentHandler", "Fail to load document! Document already exists!");
		return false;
	}
	_document = loadXMLImpl( xml );
	return (_document != NULL);
}

/**
 * Carga un documento xml desde un string.
 * @param xml El xml a cargar.
 * @return true si pudo cargar el archivo correctamente, false en caso contrario.
 * @see close()
 */
bool DocumentHandler::loadXML( const Buffer *buf ) {
	if (_document) {
		LWARN("DocumentHandler", "Fail to load document! Document already exists!");
		return false;
	}
	_document = loadXMLImpl( buf );
	return (_document != NULL);
}

/**
 * Carga un documento desde un archivo xml.
 * @param file El path al archivo xml a cargar.
 * @return true si pudo cargar el archivo correctamente, false en caso contrario.
 * @see close()
 */
bool DocumentHandler::loadDocument( const std::string &file ) {
	if (_document) {
		LWARN("DocumentHandler", "Fail to load document! Document already exists!");
		return false;
	}
	_document = loadDocumentImpl( file );
	return (_document != NULL);
}

/**
 * Crea un nuevo documento.
 * @param rootElement El nombre del tag del nodo raiz.
 * @return true si pudo crear el documento correctamente, false en caso contrario.
 * @see close()
 */
bool DocumentHandler::createDocument( const std::string &rootElement ) {
	if (_document) {
		LWARN("DocumentHandler", "Fail to create document! Document already exists!");
		return false;
	}
	_document = createDocumentImpl( rootElement );
	return (_document != NULL);
}

/**
 * Guarda el documento a un archivo de texto.
 * @param file El path del archivo a crear.
 * @return true si pudo crear el archivo correctamente, false en caso contrario.
 */
bool DocumentHandler::save( const std::string &file ) {
	if (!_document) {
		LWARN("DocumentHandler", "Fail to save document! Document does not exist!");
		return false;
	}
	return saveImpl( _document, file );
}

bool DocumentHandler::save( util::Buffer *buf ) {
	if (!_document) {
		LWARN("DocumentHandler", "Fail to save document! Document does not exist!");
		return false;
	}
	return saveImpl( _document, buf );
}

/**
 * Cierra el documento actual. Debe ser llamado cuando ya no se precise un documento
 * creado o cargado.
 * @return true si pudo cerrar el documento correctamente, false si no se pudo efectuar
 * la operación o si no habia ningun documento abierto.
 */
bool DocumentHandler::close() {
	if (!_document) {
		LWARN("DocumentHandler", "Fail to close document! Document does not exist!");
		return false;
	}
	bool result = closeImpl( _document );
	_document = NULL;
	handler()->clearCache();
	return result;
}

/**
 * @return La declaración xml del documento.
 */
const std::string &DocumentHandler::declaration() const {
	return impl::docDeclaration;
}

/**
 * Crea un nodo de texto.
 * @param text El contenido del nodo.
 * @return El nodo creado o NULL si no se pudo crear el mismo.
 */
Node DocumentHandler::createTextNode( const std::string &text ) {
	if (!_document) {
		LWARN("DocumentHandler", "Fail to create text node! Document does not exist!");
		return NULL;
	}
	return createTextNodeImpl( _document, text );
}

/**
 * Crea un elemento.
 * @param tagname El nombre del tag xml del elemento.
 * @return El nodo creado o NULL si no se pudo crear el mismo.
 */
Node DocumentHandler::createElement( const std::string &tagname ) {
	if (!_document) {
		LWARN("DocumentHandler", "Fail to create element! Document does not exist!");
		return NULL;
	}
	return createElementImpl( _document, tagname );
}

bool DocumentHandler::init() {
	return true;
}

void DocumentHandler::fin() {
}

}
}
}
