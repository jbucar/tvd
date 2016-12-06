/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include <stdlib.h>
#include <boost/function.hpp>

struct AAsset;
struct ALooper;
struct android_app;
struct AInputEvent;
struct ANativeWindow;
struct _JNIEnv;
typedef _JNIEnv JNIEnv;
class _jobject;
typedef _jobject* jobject;
class _jstring;
typedef _jstring* jstring;

namespace util {
namespace android {

class Glue {
public:
	explicit Glue();
	~Glue();

	//	Setup
	typedef boost::function<int32_t (AInputEvent *)> InputCallback;
	void setInputCallback( const InputCallback &callback );

	//	Getters
	struct android_app *app() const;
	struct ALooper *looper() const;
	struct ANativeWindow *window() const;
	const char *internalDataPath() const;
	const char *externalDataPath() const;
	std::string getIntentFilename();

	//	Run/exit
	void run( struct android_app *state );	
	void exit();
	bool needExit() const;
	bool haveGUI() const;

	//	JNI
	jobject obj() const;
	JNIEnv *attachCurrentThread();
	void detachCurrentThread( JNIEnv **jni );

	//	Process glue data
	bool process( int ident, void *out );

	//	Implementation
	int32_t onInput( AInputEvent *event );
	void onCmd( int32_t cmd );

protected:
	bool waitGUI();

private:
	void extract_assets();
	int tar_extract(const char *dirname, char *fname, int l, char *b, AAsset* asset);
	std::string convert(JNIEnv* env, jstring s);

	struct android_app *_app;
	InputCallback _inputCallback;
	bool _waitInput;
	bool _guiActive;
	bool _exitApp;
};

Glue *glue();

}
}
	
