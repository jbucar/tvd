/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "../types.h"
#include <util/types.h>
#include <boost/signals2.hpp>

#define CANVAS_LAYER_MIN_OPACITY      0
#define CANVAS_LAYER_MAX_OPACITY     10
#define CANVAS_LAYER_DEFAULT_OPACITY  8

namespace canvas {

class Layer {
public:
	explicit Layer( int id, int zIndex=0, bool enabled=true );
	virtual ~Layer();

	//	Initialization
	virtual bool initialize();
	virtual void finalize();

	//	Layer operations
	typedef boost::signals2::signal<void (bool state)> EnabledChanged;
	EnabledChanged &onEnableChanged();
	void enable( bool state );
	virtual void clear();
	virtual void flush();

	//	Resolution
	virtual mode::type defaultMode() const;
	virtual const std::vector<mode::type> supportedModes() const;
	virtual void mode( mode::type mode );

	//	Color key
	virtual bool colorKey( util::BYTE r, util::BYTE g, util::BYTE b );

	//	Opacity
	typedef boost::signals2::signal<void (int)> OpacityChanged;
	bool opacity( int val );
	int opacity() const;
	OpacityChanged &onOpacityChanged();

	//	Getters
	int id() const;
	virtual Surface *getSurface();
	int zIndex() const;

protected:
	virtual void setEnable( bool state );
	virtual bool setAlpha( util::BYTE val );

private:
	int _id;
	int _zIndex;
	int _opacity;
	bool _enable;
	EnabledChanged _enableChanged;
	OpacityChanged _onOpacityChanged;
};

}

