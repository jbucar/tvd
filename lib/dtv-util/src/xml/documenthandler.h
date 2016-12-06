/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

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

#include "types.h"
#include "nodehandler.h"
#include "../log.h"
#include <string>
#include <boost/foreach.hpp>

namespace util {

class Buffer;

namespace xml {
namespace dom {

enum Order {
	preorder,
	postorder,
	inorder,
};

/**
 * La clase DocumentHandler permite manipular documentos XML de tipo DOM proveyendo funcionalidad
 * para crearlos o cargarlos desde un archivo asi tambien como para guardarlos a un archivo de texto.
 */
class DocumentHandler {
public:
	virtual ~DocumentHandler();

	bool initialize();
	void finalize();
	bool initialized() const;

	// Document methods
	bool loadXML( const std::string &xml );
	bool loadXML( const Buffer *buf );
	bool loadDocument( const std::string &file );
	bool createDocument( const std::string &rootElement );
	bool save( const std::string &file );
	bool save( util::Buffer *buf );
	bool close();

	const std::string &declaration() const;

	// Node methods
	Node root() const;
	Node createTextNode( const std::string &text );
	Node createElement( const std::string &tagname );

	/**
	 * @return El NodeHandler para manipular los nodos del documento.
	 */
	virtual NodeHandler *handler()=0;

	/**
	 * Serializa un documento xml.
	 * @param serializer Objeto o estructura al que serán enviado los nodos durante el recorrido.
	 * Debe tener definido los métodos:
	 * @code{cpp}void processDeclaration( const std::string &docDecl );@endcode
	 * - docDecl es la declaración xml del documento.
	 * .
	 * @code{cpp}void processNode( Node node, NodeHandler *handler, int level, bool going );@endcode
	 * - node es el nodo actual.
	 * - handler es el manejador a utilizar para obtener los datos del nodo.
	 * - level es el nivel del nodo en el documento (teniendo el nodo raiz nivel 0, sus hijos nivel 1, etc.).
	 * - going indica si el recorrido se encuentra en su face de ida (true) o de vuelta (false).
	 * @param order Enumerativo que indica el orden en que se recorrerá el documento (preorder, postorder, inorder).
	 */
	template<typename T>
	inline void traverse( T &serializer, Order order=preorder );

	static DocumentHandler *create( const std::string &use="" );

protected:
	DocumentHandler();

	virtual bool init();
	virtual void fin();

	// Document methods impl
	virtual Document loadXMLImpl( const std::string &xml )=0;
	virtual Document loadXMLImpl( const Buffer *buf )=0;
	virtual Document loadDocumentImpl( const std::string &file )=0;
	virtual Document createDocumentImpl( const std::string &name )=0;
	virtual bool saveImpl( Document doc, const std::string &file )=0;
	virtual bool saveImpl(  Document doc, util::Buffer *buf )=0;
	virtual bool closeImpl( Document doc )=0;

	// Node methods impl
	virtual Node rootImpl( Document doc ) const=0;
	virtual Node createTextNodeImpl( Document doc, const std::string &text )=0;
	virtual Node createElementImpl( Document doc, const std::string &tagname )=0;

	template<typename T>
	inline void preOrder( Node node, NodeHandler *handler, int level, T &serializer );

	template<typename T>
	inline void postOrder( Node node, NodeHandler *handler, int level, T &serializer );

	template<typename T>
	inline void inOrder( Node node, NodeHandler *handler, int level, T &serializer );

private:
	Document _document;
	bool _initialized;
};

template<typename T>
inline void DocumentHandler::traverse( T &serializer, Order order/*=preorder*/ ) {
	serializer.processDeclaration( declaration() );
	switch(order) {
		case preorder: preOrder( root(), handler(), 0, serializer ); break;
		case postorder: postOrder( root(), handler(), 0, serializer ); break;
		case inorder: inOrder( root(), handler(), 0, serializer ); break;
		default:
			LOG_PUBLISH( WARN, "util", "DocumentHandler", "Unknown order=%d for serialization", order);
	}
}

template<typename T>
inline void DocumentHandler::preOrder( Node node, NodeHandler *handler, int level, T &serializer ) {
	serializer.processNode( node, handler, level, true );

	std::vector<Node> childrenList;
	handler->children( node, childrenList );

	BOOST_FOREACH( Node child, childrenList ) {
		preOrder( child, handler, level+1, serializer );
	}

	serializer.processNode( node, handler, level, false );
}

template<typename T>
inline void DocumentHandler::postOrder( Node node, NodeHandler *handler, int level, T &serializer ) {
	std::vector<Node> childrenList;
	handler->children( node, childrenList );

	BOOST_FOREACH( Node child, childrenList ) {
		postOrder( child, handler, level+1, serializer );
	}

	serializer.processNode( node, handler, level, true );
	serializer.processNode( node, handler, level, false );
}

template<typename T>
inline void DocumentHandler::inOrder( Node node, NodeHandler *handler, int level, T &serializer ) {
	std::vector<Node> childrenList;
	handler->children( node, childrenList );
	size_t cant = childrenList.size();

	if (cant>0) {
		inOrder( childrenList[0], handler, level+1, serializer );
	}
	serializer.processNode( node, handler, level, true );
	for( size_t pos=1; pos<cant; pos++ ) {
		inOrder( childrenList[pos], handler, level+1, serializer );
	}
	serializer.processNode( node, handler, level, false );
}

}
}
}
