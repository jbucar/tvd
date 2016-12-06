/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "../player.h"
#include <canvas/types.h>

namespace canvas {
	class System;
	class Surface;
}

namespace player {

namespace fit {
namespace mode {

enum type  {
	fill,
	hidden,
	meet,
	meetBest,
	slice
};

}	//	namespace mode
}	//	namespace fit

namespace property {
	class FocusHandler;
}

class GraphicProperties;
class FocusProperties;

class GraphicPlayer : public player::Player {
public:
	explicit GraphicPlayer( Device *dev );
	virtual ~GraphicPlayer();

protected:
	//	Initialization
	virtual bool startPlay();
	virtual void stopPlay();
	virtual void registerProperties();
	virtual void beginRefresh();
	virtual void endRefresh();
	virtual void onPropertyChanged();

	void redraw();

	// Properties callbacks
	void applyZIndex();

	//	Events
	virtual void onBoundsChanged( const canvas::Rect &rect );
	virtual void onSizeChanged( const canvas::Size &size );
	virtual void onPositionChanged( const canvas::Point &point );

	//	Getters
	canvas::Canvas *canvas() const;
	canvas::Surface* surface() const;
	virtual bool isVisible() const;
	virtual bool isGraphic() const;
	fit::mode::type getFitMode() const;

	void applyVisible();
	virtual canvas::composition::mode flushCompositionMode() const;

private:
	std::string _fitMode;
	GraphicProperties *_graphic;
	FocusProperties *_focus;
	canvas::Surface *_layer;
	int _zIndex;
	bool _visible;
};

}
