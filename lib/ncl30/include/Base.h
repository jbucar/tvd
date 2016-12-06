/******************************************************************************
 Este arquivo eh parte da implementacao do ambiente declarativo do middleware
 Ginga (Ginga-NCL).

 Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

 Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
 os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
 Software Foundation.

 Este programa eh distribuido na expectativa de que seja util, porem, SEM
 NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
 ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
 GNU versao 2 para mais detalhes.

 Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
 com este programa; se nao, escreva para a Free Software Foundation, Inc., no
 endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

 Para maiores informacoes:
 ncl @ telemidia.puc-rio.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://www.telemidia.puc-rio.br
 ******************************************************************************
 This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

 Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.
 Copyright 2010 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata


 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
 details.

 You should have received a copy of the GNU General Public License version 2
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

 For further information contact:
 ncl @ telemidia.puc-rio.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://www.telemidia.puc-rio.br
 *******************************************************************************/

#ifndef BASE_H_
#define BASE_H_

#include <vector>
#include <map>
#include <string>
#include <set>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
  
  /**
   * Esta es la clase padre del resto de las bases. Se encarga de hacer el manejo de todos atributos que tienen en común.
   */ 
class Base
{
protected:
	std::string id;
	std::vector<Base*> baseSet;
	std::map<std::string, Base*>* baseAliases;
	std::map<std::string, Base*>* baseLocations;

	std::set<std::string> typeSet; //informacoes de tipo

public:
	explicit Base( const std::string &paramId );
	virtual ~Base();
	virtual bool addBase( Base* base, const std::string &alias, const std::string &location );

	virtual void clear();
	Base* getBase( const std::string &baseId );
	std::string getBaseAlias( Base* base );
	std::string getBaseLocation( Base* base );
	std::vector<Base*>* getBases();
	bool removeBase( Base* base );
	void setBaseAlias( Base* base, const std::string & alias );
	void setBaseLocation( Base* base, const std::string & location );
	const std::string & getId();
	void setId( const std::string & id );

	bool instanceOf( const std::string & s );
};
}
}
}
}

#endif /*BASE_H_*/
