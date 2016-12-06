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

#include "imageviewer.h"

#include "../view.h"
#include "../modules/wgt.h"

namespace wgt{

ImageViewer::ImageViewer(int x, int y, int w, int h, const colorMap_t &colorMap, const std::vector<std::string>& imgs)
	: Window(x, y, w, h, colorMap)
	, _current(-1)
	, _exitCbk("")
{
	_image = ImagePtr(new Image(0, 0, w, h, colorMap, ""));
	addChild(_image);
	fixOffsets();
	images(imgs);
}

ImageViewer::~ImageViewer()
{
}

void ImageViewer::draw(View* view)
{
	view->draw(this);
}

//	getters
int ImageViewer::current()
{
	return _current;
}

ImagePtr ImageViewer::currentImage()
{
	return _image;
}

//	setters
void ImageViewer::images(std::vector<std::string> images)
{
	_images = images;
	if (_images.size()) {
		_current = 0;
		_image->path(images[_current]);
	}
}

void ImageViewer::current(int current)
{
	_current = current;
	_image->path(_images[_current]);
}

//	process keys
bool ImageViewer::onLeftArrow()
{
	if (current() > 0) {
		current(_current-1);
	} else {
		current(_images.size()-1);
	}
	needsRepaint(true);
	return true;
}

bool ImageViewer::onRightArrow()
{
	if (current() < (int) _images.size()-1) {
		current(_current+1);
	} else {
		current(0);
	}
	needsRepaint(true);
	return true;
}

bool ImageViewer::onMenu() {
	return true;
}
bool ImageViewer::onOk() {
	return true;
}
bool ImageViewer::onInfo()
{
	return true;
}
bool ImageViewer::onEPG()
{
	return true;
}
bool ImageViewer::onChannelDown()
{
	return true;
}
bool ImageViewer::onChannelUp()
{
	return true;
}
bool ImageViewer::onUpArrow()
{
	return true;
}
bool ImageViewer::onDownArrow()
{
	return true;
}
bool ImageViewer::onVolumeUp()
{
	return true;
}
bool ImageViewer::onVolumeDown()
{
	return true;
}
bool ImageViewer::onDigitPressed(int)
{
	return true;
}
bool ImageViewer::onGreen()
{
	return true;
}
bool ImageViewer::onYellow()
{
	return true;
}
bool ImageViewer::onAudio()
{
	return true;
}
bool ImageViewer::onMute()
{
	return true;
}
bool ImageViewer::onBlue()
{
	return true;
}
bool ImageViewer::onPrevious()
{
	return true;
}
bool ImageViewer::onRed()
{
	return true;
}
bool ImageViewer::onFavorite()
{
	return true;
}
bool ImageViewer::onSubtitle()
{
	return true;
}
bool ImageViewer::onAspect()
{
	return true;
}
bool ImageViewer::onCh3_4()
{
	return true;
}
bool ImageViewer::onVideoMode()
{
	return true;
}

}

