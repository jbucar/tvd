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

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#pragma once

#include "window.h"
#include "image.h"

namespace wgt {

class View;

class ImageViewer : public Window
{
public:
	ImageViewer(int x, int y, int w, int h, const colorMap_t &colorMap, const std::vector<std::string>& imgs);
	virtual ~ImageViewer();
	void draw(View* view);

	//	setters
	void images(std::vector<std::string> images);
	void current(int current);

	//	getters
	int current();
	ImagePtr currentImage();

protected:
	bool onLeftArrow();
	bool onRightArrow();

	bool onMenu();
	bool onOk();
	bool onInfo();
	bool onEPG();
	bool onChannelDown();
	bool onChannelUp();
	bool onUpArrow();
	bool onDownArrow();
	bool onVolumeUp();
	bool onVolumeDown();
	bool onDigitPressed(int d);
	bool onRed();
	bool onGreen();
	bool onYellow();
	bool onBlue();
	bool onMute();
	bool onAudio();
	bool onPrevious();
	bool onFavorite();
	bool onSubtitle();
	bool onAspect();
	bool onCh3_4();
	bool onVideoMode();

private:
	int _current;
	std::vector<std::string> _images;
	ImagePtr _image;
	std::string _exitCbk;
};

DEF_SHARED(ImageViewer);

}

#endif // IMAGEVIEWER_H
