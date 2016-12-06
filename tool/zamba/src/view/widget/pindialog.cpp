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
 * pindialog.cpp
 *
 *  Created on: Jun 6, 2011
 *      Author: gonzalo
 */

#include "pindialog.h"
#include "../view.h"
#include "../luacallback.h"
#include "../modules/wgt.h"
#include <util/mcr.h>
#include <sstream>
#include <iostream>

namespace wgt {

PINDialog::PINDialog(int x, int y, int w, int h, const colorMap_t &colorMap, int borderWidth, unsigned int pinSize)
	: Dialog(x,y, w,h, colorMap, borderWidth)
	, _typedPIN("")
	, _pinSize(pinSize)
	, _prompt("")
	, _callback(NULL)
	, _onWrongPass(NULL)
	, _lcallback("")
	, _lcancelled("")
	, _wrongPIN(false)
{
	text()->lines(5);
	text()->fontSize(14);
	text()->leading(10);
	fixOffsets();
}

PINDialog::~PINDialog() {}

void PINDialog::prompt(const std::string& p)
{
	_prompt = p;
}

void PINDialog::pinClose() {
	onExit();
}

void PINDialog::fillText()
{
	std::string maskedPIN = " ";
	for (size_t i=0; i<_typedPIN.size(); ++i) {
		maskedPIN += "*    ";
	}
	for (size_t i=_typedPIN.size(); i< _pinSize; ++i) {
		maskedPIN += "_    ";
	}

	text()->text(_prompt + "\n" + maskedPIN + "\n\n" + help());
}

void PINDialog::callback( Callback callback )
{
	_callback = callback;
}

void PINDialog::callbacks(const std::string& fOk, const std::string& fCancelled)
{
	_lcallback  = fOk;
	_lcancelled = fCancelled;
}

void PINDialog::onWrongPassword( Callback callback ) {
	_onWrongPass = callback;
}

void PINDialog::open()
{
	_typedPIN = "";
	_wrongPIN = false;
	fillText();
	Dialog::open();
}

bool PINDialog::onDigitPressed(int d)
{
	if (!_wrongPIN) {
		std::stringstream ssKey;
		ssKey << _typedPIN << d;
		_typedPIN = ssKey.str();
		fillText();
	}
	needsRepaint(true);
	return true;
}

void PINDialog::pinCorrect()
{
	if (_callback) {
		_callback();
	}
	if (_lcallback.size()) {
		Wgt::call(_lcallback);
	}
}

void PINDialog::pinIncorrect()
{
	if (_onWrongPass) {
		_onWrongPass();
	}
	_wrongPIN = true;
}

bool PINDialog::onExit()
{
	if (_lcancelled.size()) {
		Wgt::call(_lcancelled);
	}
	return true;
}


}
