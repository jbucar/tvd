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

#ifndef _LAYOUTREGION_H_
#define _LAYOUTREGION_H_

#include "../Entity.h"
#include "types.h"

#include <map>
#include <string>
#include <vector>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace layout {
  
class LayoutRegion;

typedef std::map<std::string, LayoutRegion*> LayoutRegions;

/**
 * Clase que representa a una Region de @c NCL.
 */
class LayoutRegion: public Entity
{

public:
	explicit LayoutRegion( const std::string &id );
	virtual ~LayoutRegion();
	virtual void setDevice( Device &device );
	virtual Device &getDevice();

	virtual void addRegion( LayoutRegion* region );
	virtual void removeRegion( LayoutRegion* region );
	virtual LayoutRegion* cloneRegion();
	virtual LayoutRegion* getRegionRecursively( const std::string &id );

	virtual void setParent( LayoutRegion* parent );

	bool setProperty( const std::string &name, const std::string &value );

	int getLeft();
	int getRight();
	int getBottom();
	int getTop();
	int getWidth();
	int getHeight();
	int getZIndex();
	const std::string &getTitle();

protected:

	//	 Properties
	void setLeft( const std::string &value );
	void setTop( const std::string &value );
	void setWidth( const std::string &value );
	void setHeight( const std::string &value);
	void setRight( const std::string &value);
	void setBottom( const std::string &value);
	void setZIndex( int value );
	void setTitle( const std::string &value );

private:
	Device _device;
	std::string _width;
	std::string _left;
	std::string _right;
	std::string _height;
	std::string _top;
	std::string _bottom;
	int _zIndex;
	std::string _title;
	LayoutRegion* _parent;
	LayoutRegions _childs;
	bool _haveParent;

};
}
}
}
}
}

#endif //_LAYOUTREGION_H_
