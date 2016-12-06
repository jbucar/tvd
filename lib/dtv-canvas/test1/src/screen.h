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

#pragma once

#include "../../src/screen.h"
#include "../../src/types.h"
#include <gtest/gtest.h>


class ScreenOk : public canvas::Screen {
public:
	virtual canvas::connector::type defaultConnector() const { return canvas::connector::vga; }
	virtual std::vector<canvas::connector::type> supportedConnectors() const {
		std::vector<canvas::connector::type> tmp;
		for (int i=0; i<canvas::connector::LAST_CONNECTOR; i++) {
			tmp.push_back( (canvas::connector::type) i );
		}
		return tmp;
	}

	virtual canvas::aspect::type defaultAspect() const { return canvas::aspect::a16_9; }
	virtual std::vector<canvas::aspect::type> supportedAspects() const {
		std::vector<canvas::aspect::type> tmp;
		for (int i=0; i<canvas::aspect::LAST_ASPECT; i++) {
			tmp.push_back( (canvas::aspect::type) i );
		}
		return tmp;
	}

	virtual canvas::mode::type defaultMode( canvas::connector::type /*conn*/ ) const { return canvas::mode::m576i_50; }
	virtual std::vector<canvas::mode::type> supportedModes( canvas::connector::type /*conn*/ ) const {
		std::vector<canvas::mode::type> tmp;
		for (int i=0; i<canvas::mode::LAST_MODE; i++) {
			tmp.push_back( (canvas::mode::type) i );
		}
		return tmp;
	}

protected:
	virtual bool init() { return true; }
	virtual void fin() {}

	virtual bool turnOnImpl() { return true; }
	virtual bool turnOffImpl() { return true; }

	virtual bool aspectImpl( canvas::aspect::type /*vAspect*/ ) { return true; }
	virtual bool modeImpl( canvas::connector::type /*conn*/, canvas::mode::type /*vMode*/ ) { return true; }
};

class ScreenUnsuported : public ScreenOk {
	virtual canvas::connector::type defaultConnector() const { return canvas::connector::vga; }
	virtual std::vector<canvas::connector::type> supportedConnectors() const {
		std::vector<canvas::connector::type> tmp;
		tmp.push_back( canvas::connector::vga );
		return tmp;
	}

	virtual canvas::aspect::type defaultAspect() const { return canvas::aspect::a16_9; }
	virtual std::vector<canvas::aspect::type> supportedAspects() const {
		std::vector<canvas::aspect::type> tmp;
		tmp.push_back( canvas::aspect::a16_9 );
		return tmp;
	}

	virtual canvas::mode::type defaultMode( canvas::connector::type /*conn*/ ) const { return canvas::mode::m576i_50; }
	virtual std::vector<canvas::mode::type> supportedModes( canvas::connector::type /*conn*/ ) const {
		std::vector<canvas::mode::type> tmp;
		tmp.push_back( canvas::mode::m576i_50 );
		return tmp;
	}
};

class ScreenFailSet : public ScreenOk {
	virtual std::vector<canvas::aspect::type> supportedAspects() const {
		std::vector<canvas::aspect::type> tmp;
		tmp.push_back( canvas::aspect::a16_9 );
		tmp.push_back( canvas::aspect::a4_3 );
		return tmp;
	}

	virtual std::vector<canvas::mode::type> supportedModes( canvas::connector::type /*conn*/ ) const {
		std::vector<canvas::mode::type> tmp;
		tmp.push_back( canvas::mode::m576i_50 );
		tmp.push_back( canvas::mode::m1080i_60 );
		return tmp;
	}

protected:
	virtual bool aspectImpl( canvas::aspect::type /*vAspect*/ ) { return false; }
	virtual bool modeImpl( canvas::connector::type /*conn*/, canvas::mode::type /*vMode*/ ) { return false; }
};

class ScreenFailInit : public ScreenOk {
protected:
	virtual bool init() { return false; }
};

class ScreenFailTurnOn : public ScreenOk {
protected:
	virtual bool turnOnImpl() { return false; }
};

class ScreenFailTurnOff : public ScreenOk {
protected:
	virtual bool turnOffImpl() { return false; }
};

class ScreenTest : public testing::Test {
public:
	ScreenTest();
	virtual ~ScreenTest();

protected:
	bool init();
	void fin();

	virtual void SetUp();
	virtual void TearDown();

	virtual canvas::Screen *createScreen();

	canvas::Screen *_scr;
};

class ScreenFailInitializationTest : public ScreenTest {
protected:
	virtual void SetUp() { init(); }
	virtual void TearDown() { fin(); };

	virtual canvas::Screen *createScreen() { return new ScreenFailInit(); }
};

class ScreenFailTurnOnTest : public ScreenFailInitializationTest {
protected:
	virtual canvas::Screen *createScreen() { return new ScreenFailTurnOn(); }
};

class ScreenFailTurnOffTest : public ScreenFailInitializationTest {
protected:
	virtual canvas::Screen *createScreen() { return new ScreenFailTurnOff(); }
};

class ScreenOkTest : public ScreenTest {
public:
	void onModeChanged( canvas::mode::type *newMode, canvas::mode::type set ) {
		*newMode = set;
	}

	void onAspectChanged( canvas::aspect::type *newAspect, canvas::aspect::type set ) {
		*newAspect = set;
	}

protected:
	virtual canvas::Screen *createScreen() { return new ScreenOk(); }
};

class ScreenUnsuportedTest : public ScreenOkTest {
protected:
	virtual canvas::Screen *createScreen() { return new ScreenUnsuported(); }
};

class ScreenFailSetTest : public ScreenOkTest {
protected:
	virtual canvas::Screen *createScreen() { return new ScreenFailSet(); }
};
