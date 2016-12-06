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

#include "../../nodehandler.h"
#include <string>
#include <vector>
#include <set>

namespace util {
namespace xml {
namespace dom {
namespace tinyxml {
	
class NodeHandler : public dom::NodeHandler {
public:
	NodeHandler();
	virtual ~NodeHandler();

	// Children methods
	virtual void children( Node node, std::vector<Node> &childrens );
	virtual void appendChild( Node node, Node child );
	virtual void remove( Node *node );

	// Parent methods
	virtual bool hasParent( Node node );
	virtual Node parent( Node node );

	// Content methods
	virtual const std::string &tagName( Node node );
	virtual const std::string &textContent( Node node );
	virtual void textContent( Node node, const std::string &text );

	// Attribute methods
	virtual bool hasAttributes( Node node );
	virtual void attributes( Node node, std::vector<Attribute> &attribs );
	virtual bool hasAttribute( Node node, const std::string &attribute);
	virtual const std::string &attribute( Node node, const std::string &name );
	virtual void setAttribute( Node node, const std::string &name, const std::string &value );
	virtual void removeAttribute( Node node, const std::string &name );

	// Implementation
	virtual void clearCache();

protected:
	const std::string &stdString( const char *key );

private:
	// HACK: find a better way to implement StrCache
	typedef std::set<std::string> StrCache;
	StrCache _strCache;
};

}
}
}
}
