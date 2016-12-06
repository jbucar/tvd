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
#include <string>
#include <vector>

namespace util {
namespace xml {
namespace dom {

/**
 * Estructura que representa un atributo de un nodo XML.
 * Posee dos campos: name y value, los cuales hacen referencia al nombre y al valor del atributo respectivamente
 */
struct Attribute {
	Attribute( std::string &n, std::string &v ) : name(n), value(v) {}
	Attribute( const Attribute &other ) : name(other.name), value(other.value) {}

	Attribute& operator=( const Attribute &other ) {
		this->name = other.name;
		this->value = other.value;
		return *this;
	}

	std::string &name;
	std::string &value;

private:
	Attribute();
};

/**
 * La clase NodeHandler provee la funcionalidad necesaria para la edición de un nodo DOM y sus atributos.
 */
class NodeHandler {
public:
	NodeHandler();
	virtual ~NodeHandler();

	Node child( Node node, const std::string &tagname );
	Node child( Node node, size_t pos );
	size_t childrenCount( Node node );
	bool hasChildren( Node node );

	/**
	 * Elimina un nodo del documento xml.
	 * @param node El nodo a eliminar
	 */
	 virtual void remove( Node *node )=0;

	/**
	 * Obtiene la lista de nodos hijos del nodo indicado.
	 * @param node El nodo a consultar.
	 * @param[out] childrens La lista de nodos hijos.
	 */
	virtual void children( Node node, std::vector<Node> &childrens )=0;

	/**
	 * Agrega un nodo como hijo del nodo indicado.
	 * @param node El nodo padre.
	 * @param child El nodo hijo.
	 */
	virtual void appendChild( Node node, Node child )=0;

	/**
	 * @param node El nodo a consultar.
	 * @return true si el nodo indicado posee un padre, false si es el nodo raiz del documento.
	 */
	virtual bool hasParent( Node node )=0;

	/**
	 * @param node El nodo a consultar.
	 * @return EL nodo padre del nodo indicado o NULL si es el nodo raiz del documento.
	 */
	virtual Node parent( Node node )=0;


	/**
	 * @param node El nodo a consultar.
	 * @return El nombre del tag del nodo indicado.
	 */
	virtual const std::string &tagName( Node node )=0;

	/**
	 * @param node El nodo a consultar.
	 * @return El contendio del nodo indicado.
	 */
	virtual const std::string &textContent( Node node )=0;

	/**
	 * Setea el contenido del nodo indicado.
	 * @param node El nodo a modificar.
	 * @param text El contenido a setear en el nodo indicado.
	 */
	virtual void textContent( Node node, const std::string &text )=0;

	/**
	 * @param node El nodo a consultar.
	 * @return true si el nodo indicado posee al menos un atributo, false en caso contrario.
	 */
	virtual bool hasAttributes( Node node )=0;

	/**
	 * Obtiene la lista de atributos del nodo indicado.
	 * @param node El nodo a consultar.
	 * @param[out] attribs La lista de atributos del nodo indicado.
	 */
	virtual void attributes( Node node, std::vector<Attribute> &attribs )=0;

	/**
	 * @param node El nodo a consultar.
	 * @param attribute El nombre de un atributo.
	 * @return true si el nodo indicado posee un atributo con dicho nombre, false en caso contrario.
	 */
	virtual bool hasAttribute( Node node, const std::string &attribute)=0;

	/**
	 * @param node El nodo a consultar.
	 * @param name El nombre del atributo a obtener.
	 * @return El valor del atributo en el nodo indicado o un string vacio si no existe dicho atributo.
	 */
	virtual const std::string &attribute( Node node, const std::string &name )=0;

	/**
	 * Setea un atributo.
	 * @param node El nodo a modificar.
	 * @param name El nombre del atributo a setear.
	 * @param value El valor del atributo a setear.
	 */
	virtual void setAttribute( Node node, const std::string &name, const std::string &value )=0;

	/**
	 * Elimina un atributo de un nodo.
	 * @param node El nodo a modificar.
	 * @param name El nombre del atributo a eliminar.
	 */
	virtual void removeAttribute( Node node, const std::string &name )=0;

	// Implementation
	virtual void clearCache()=0;
};

}
}
}
