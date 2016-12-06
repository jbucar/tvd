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
 * pindialog.h
 *
 *  Created on: Jun 6, 2011
 *      Author: gonzalo
 */

#ifndef PINDIALOG_H_
#define PINDIALOG_H_

#include "dialog.h"

namespace wgt {

class PINDialog : public Dialog {
public:
	typedef boost::function<void ()> Callback;

	PINDialog(int x, int y, int w, int h, const colorMap_t &aMap, int borderWidth, unsigned int pinSize);
	virtual ~PINDialog();

	virtual void open();

	void prompt(const std::string& p);
	void callback(boost::function<void ()> f);
	void callbacks(const std::string& fOk, const std::string& fCancelled);
	void onWrongPassword( Callback callback );

	void pinCorrect();
	void pinIncorrect();
	void pinClose();

protected:
	virtual bool onDigitPressed(int d);
	virtual bool onExit();

private:
	std::string _typedPIN;
	unsigned int _pinSize;
	std::string _prompt;
	Callback _callback;
	Callback _onWrongPass;
	std::string _lcallback;
	std::string _lcancelled;
	bool _wrongPIN;
	std::string _prevTitle;
	std::string _prevIcon;

	void fillText();
};

DEF_SHARED(PINDialog);

}

#endif /* PINDIALOG_H_ */
