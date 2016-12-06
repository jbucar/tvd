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

#include "nodehandler.h"
#include "../log.h"
#include <boost/foreach.hpp>

namespace util {
namespace xml {
namespace dom {

/**
 * Constructor base.
 */
NodeHandler::NodeHandler()
{
}

/**
 * Destructor base.
 */
NodeHandler::~NodeHandler()
{
}

/**
 * Obtiene el nodo hijo con el tag indicado.
 * @param node El nodo a consultar.
 * @param tagname El nombre del tag del nodo hijo que se desea acceder.
 * @return El nodo hijo con el tag indicado o NULL si no existe un hijo con ese tag.
 */
Node NodeHandler::child( Node node, const std::string &tagname ) {
	std::vector<Node> childrenList;
	children( node, childrenList );

	BOOST_FOREACH( Node childNode, childrenList ) {
		if (tagName(childNode) == tagname) {
			return childNode;
		}
	}
	LDEBUG("NodeHandler", "Node %s hasn't got any child named %s", tagName(node).c_str(), tagname.c_str() );
	return NULL;
}

/**
 * Obtiene el nodo hijo en la posición indicada.
 * @param node El nodo a consultar.
 * @param pos La posición del hijo al que se desea acceder.
 * @return El nodo hijo en la posición indicada o NULL si la posición es invalida.
 */
Node NodeHandler::child( Node node, size_t pos ) {
	std::vector<Node> childrenList;
	children( node, childrenList );

	if (pos >= childrenList.size()) {
		LERROR("NodeHandler", "position %d is invalid", pos );
		return NULL;
	}
	return childrenList.at( pos );
}

/**
 * @param node El nodo a consultar.
 * @return La cantidad de nodos hijos que posee el nodo indicado
 */
size_t NodeHandler::childrenCount( Node node ) {
	std::vector<Node> childrenList;
	children( node, childrenList );
	return childrenList.size();
}

/**
 * @param node El nodo a consultar
 * @return true si el nodo indicado posee nodos hijos, false en caso contrario.
 */
bool NodeHandler::hasChildren( Node node ) {
	std::vector<Node> childrenList;
	children( node, childrenList );
	return !childrenList.empty();
}

}
}
}

