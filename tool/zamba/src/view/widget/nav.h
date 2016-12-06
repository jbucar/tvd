/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-luaz implementation.

    DTV-luaz is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-luaz is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-luaz.

    DTV-luaz es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-luaz se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

/*
 * Nav.h
 *
 *  Created on: May 4, 2011
 *      Author: gonzalo
 */

#ifndef NAV_H_
#define NAV_H_

#include <util/keydefs.h>
#include <boost/function.hpp>

#include <map>
#include <set>
#include <vector>

namespace wgt
{

class NavController
{
public:
	NavController();
	virtual ~NavController();

	void close();
	void open();

	void ignoreKey(util::key::type key, bool flag=true);
	void ignoreKeys(const std::vector<util::key::type>& keys);

	bool processKey(int key);
	virtual bool processKey(util::key::type key);

protected:

	virtual bool onMenu();
	virtual bool onOk();

	virtual bool onInfo();
	virtual bool onEPG();
	virtual bool onChannelDown();
	virtual bool onChannelUp();

	virtual bool onUpArrow();
	virtual bool onDownArrow();
	virtual bool onLeftArrow();
	virtual bool onRightArrow();

	virtual bool onVolumeUp();
	virtual bool onVolumeDown();

	virtual bool onDigitPressed(int d);
	virtual bool onRed();
	virtual bool onGreen();
	virtual bool onYellow();
	virtual bool onBlue();
	virtual bool onExit();
	virtual bool onMute();
	virtual bool onAudio();
	virtual bool onPrevious();
	virtual bool onFavorite();
	virtual bool onSubtitle();
	virtual bool onAspect();
	virtual bool onCh3_4();
	virtual bool onVideoMode();
	virtual bool onPageUp();
	virtual bool onPageDown();

private:
	std::set<util::key::type> _ignoredKeys;
};

}

#endif /* NAV_H_ */
