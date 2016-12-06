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

#include "android.h"
#include "android_native_app_glue.h"
#include "../assert.h"
#include "../log.h"
#include "../fs.h"
#include "../main.h"
#include <stdlib.h>

#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/param.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#define ASSET_FILENAME "assets.tar"
#define ASSET_FOLDER "/share"

namespace util {
namespace android {

namespace impl {

static Glue _glue;

static void on_handle_cmd( struct android_app* app, int32_t cmd ) {
	DTV_ASSERT(app);
	DTV_ASSERT(app->userData);
	Glue *glue = (Glue *)app->userData;
	glue->onCmd( cmd );
}

static int32_t on_handle_input( struct android_app* app, AInputEvent* event ) {
	DTV_ASSERT(app);
	DTV_ASSERT(app->userData);
	Glue *glue = (Glue *)app->userData;
	return glue->onInput( event );
}

}

#define min(a,b) (((a)<(b))?(a):(b))
enum Header {
	NAME=0, MODE = 100, UID = 108, GID = 116, SIZE = 124, MTIME = 136, CHK=148,
	TYPE = 156, LINK = 157, MAGIC=257, VERS=263, UNAME=265, GNAME=297, MAJ = 329,
	MIN = 337, END = 512
};

enum Type {
	REG = '0', HARDLINK = '1', SYMLINK = '2', CHARDEV='3', BLOCKDEV='4',
	DIRECTORY='5', FIFO='6'
};

Glue *glue() {
	return &impl::_glue;
}

Glue::Glue()
{
	_app = NULL;
	_guiActive = false;
	_exitApp = false;
}

Glue::~Glue()
{
}

struct android_app *Glue::app() const {
	return _app;
}

struct ALooper *Glue::looper() const {
	return _app->looper;
}

struct ANativeWindow *Glue::window() const {
	return _app->window;
}

const char *Glue::internalDataPath() const {
	DTV_ASSERT(_app);
	DTV_ASSERT(_app->activity);
	return _app->activity->internalDataPath;
}

const char *Glue::externalDataPath() const {
	DTV_ASSERT(_app);
	DTV_ASSERT(_app->activity);
	return _app->activity->externalDataPath;
}

std::string Glue::getIntentFilename() {
	jobject intent;
	std::string ret;

	JNIEnv *_jni = attachCurrentThread();
	if(!_jni) {
		LERROR( "android", "Cannot get JNI." );
		return ret;
	}
	jobject me = obj();
	{
		jclass acl = _jni->GetObjectClass(me); //class pointer of NativeActivity
		jmethodID giid = _jni->GetMethodID(acl, "getIntent", "()Landroid/content/Intent;");
		intent = _jni->CallObjectMethod(me, giid); //Got our intent
		if (!intent) {
			LERROR( "android", "Cannot get intent" );
			detachCurrentThread( &_jni );
			return ret;
		}
	}
	LINFO( "android", "Got the intent.");

	jobject data;
	{
		jclass acl = _jni->GetObjectClass(intent); //class pointer of Intent
		jmethodID gdid = _jni->GetMethodID(acl, "getData","()Landroid/net/Uri;");
		data = _jni->CallObjectMethod(intent, gdid); //Got our data
	}
	if (!data) {
		LERROR( "android", "cannot get intent data");
		detachCurrentThread( &_jni );
		return ret;
	}
	std::string scheme;
	{
		jclass acl = _jni->GetObjectClass(data); //class pointer of Uri
		jmethodID gdid = _jni->GetMethodID(acl, "getScheme", "()Ljava/lang/String;");
		scheme = convert(_jni, (jstring) _jni->CallObjectMethod(data, gdid)); //Got our scheme
	}

	LINFO( "android", "Got the scheme: %s", scheme.c_str());
	boost::algorithm::to_lower(scheme);
	if(scheme == "file") {
		jclass acl = _jni->GetObjectClass(data); //class pointer of Uri
		jmethodID gdid = _jni->GetMethodID(acl, "getPath", "()Ljava/lang/String;");
		ret = convert(_jni, (jstring) _jni->CallObjectMethod(data, gdid)); //Got our path
	} else if (scheme != "content"){
		jclass acl = _jni->GetObjectClass(data); //class pointer of Uri
		jmethodID gdid = _jni->GetMethodID(acl, "toString", "()Ljava/lang/String;");
		ret = convert(_jni, (jstring) _jni->CallObjectMethod(data, gdid)); //Got our string
	}
	LINFO( "android", "Filename From Intent: %s", ret.c_str());
	detachCurrentThread( &_jni );
	return ret;
}

void Glue::setInputCallback( const InputCallback &callback ) {
	_inputCallback = callback;
}

void Glue::exit() {
	_exitApp = true;
}

bool Glue::needExit() const {
	return _app->destroyRequested || _exitApp;
}

bool Glue::haveGUI() const {
	return _guiActive;
}

bool Glue::process( int ident, void *out ) {
	bool result=false;
	if (ident == LOOPER_ID_MAIN || ident == LOOPER_ID_INPUT) {
		if (out) {
			struct android_poll_source *source = (struct android_poll_source *)out;
			source->process(_app, source);
		}
		result=true;
	}
	return result;
}

//	JNI
jobject Glue::obj() const {
	return _app->activity->clazz;
}

JNIEnv *Glue::attachCurrentThread() {
	JNIEnv *jni;

	//	Get JNI env
	DTV_ASSERT(_app->activity);
	DTV_ASSERT(_app->activity->vm);
	int error = _app->activity->vm->AttachCurrentThread(&jni, NULL);
	if (error || !jni) {
		LERROR( "android", "Cannot get JNI envirioment" );
		return NULL;
	}
	return jni;
}

void Glue::detachCurrentThread( JNIEnv **jni ) {
	_app->activity->vm->DetachCurrentThread();
	*jni = NULL;
}

//	Implementation
int32_t Glue::onInput( AInputEvent* event ) {
	_waitInput = true;

	if (!_inputCallback.empty()) {
		return _inputCallback(event);
	}
	return 0;
}

void Glue::onCmd( int32_t cmd ) {
	if (cmd == APP_CMD_INIT_WINDOW) {
		_guiActive = _app->window ? true : false;
	}
	else if (cmd == APP_CMD_TERM_WINDOW) {
		_guiActive = false;
	}
}

bool Glue::waitGUI() {
	bool exit=false;

	_guiActive = false;
	_exitApp = false;

	//	loop waiting for stuff to do.
	while (!exit && !_guiActive) {
		int ident;
		int events;
		void *out;

		//	We will block forever waiting for events.
		if ((ident=ALooper_pollAll( -1, NULL, &events, (void**)&out )) >= 0) {
			process( ident, out );
		}

		if (needExit()) {
			exit=true;
		}
	}

	LDEBUG("android", "Glue::waitGUI: guiActive=%d, exitApp=%d, int_path=%s, ext_path=%s",
		_guiActive,
		_exitApp,
		internalDataPath(),
		externalDataPath()
	);

	return exit;
}


int Glue::tar_extract(const char *dirname, char *fname, int l, char *b, AAsset* asset) {
	static char lname[101] = {0};
	FILE *f = NULL;

	memcpy(lname, b+LINK, 100);
	unlink(fname);

	boost::filesystem::path full_path(dirname);
        full_path /= fname;
	const char* fp = full_path.string().c_str();

	switch(b[TYPE]) {
	case REG:
		f = fopen(fp, "w");
		if(! f) {
			LWARN("android", "error [%d] creating file %s", errno, fp);
		}
		break;
	case HARDLINK:
		link(lname,fname);
		break;
	case SYMLINK:
		symlink(lname,fname);
		break;
	case DIRECTORY:
		if(! boost::filesystem::create_directories(fp)) {
			LWARN("android", "error creating directory %s", fp);
		}
		break;
	case CHARDEV:
	case BLOCKDEV:
		mknod(fp, (b[TYPE] == '3' ?
				S_IFCHR : S_IFBLK) | strtoul(b + MODE,0,8),
					makedev(strtoul(b + MAJ,0,8),
						strtoul(b + MIN,0,8)));
		break;
	case FIFO:
		mknod(fp, S_IFIFO | strtoul(b + MODE,0,8), 0);
		break;
	default:
		fprintf(stderr,"%s: not supported filetype %c\n", fp, b[TYPE]);
	}

	for(;l>0; l-=END){
		AAsset_read(asset, b, END);
		if(f) fwrite(b, min(l, 512), 1, f);
	}
	LINFO("android", "extracted %s", fp);
	if(f) fclose(f);
	return 0;
}

void Glue::extract_assets() {

	struct stat st;
	char buf[END];
	int nb_read, l;

	char fname[101];
	fname[100] = '\0';

	LINFO("android", "preparing to extract assets");

	//	Use internal API becouse the path does not exist and the API check if exists
	const char* app_dir = externalDataPath();

	std::string dataPath( app_dir );
	dataPath += "/share"; //Patch for android > 4.4

	// check if extraction is needed
	if (stat(dataPath.c_str(), &st) != -1) {
		// Path already exists
		LINFO("android", "skipped asset extraction");
		return;
	}

	LINFO("android", "creating %s directory", app_dir);
	boost::filesystem::create_directories(app_dir);

	AAssetManager* mgr = app()->activity->assetManager;

	AAsset* asset = AAssetManager_open(mgr, ASSET_FILENAME, AASSET_MODE_UNKNOWN);
	if(! asset) {
		LERROR( "android", "failed to open asset %s", ASSET_FILENAME);
		return;
	}

	while ((nb_read = AAsset_read(asset, buf, END)) > 0) {
		if(*buf == '\0')
			break;
		memcpy(fname, buf, 100);
		l = strtol(buf+SIZE, 0, 8);
		tar_extract(app_dir, fname, l, buf, asset);
	}
	AAsset_close(asset);
}

std::string Glue::convert(JNIEnv* env, jstring s) {
	if(s == NULL || env == NULL) {
		return std::string("");
	}
	const char* str = env->GetStringUTFChars(s, 0);
	std::string ret = str;
	env->ReleaseStringUTFChars(s, str);

	return ret;
}

void Glue::run( struct android_app *state ) {
	int argc = 1;
	char *argv[] = { (char *)"android", NULL, NULL, NULL };

	//	Setup glue
	_app = state;
	_app->userData = this;
	_app->onAppCmd = impl::on_handle_cmd;
	_app->onInputEvent = impl::on_handle_input;

	LINFO("android", "Glue::run" );

	std::string intent = getIntentFilename();
	if( intent.size() ) {
		const char ncl_parameter[] = "--ncl";
		argv[1] = (char*) malloc(strlen(ncl_parameter)+1);
		strcpy(argv[1], ncl_parameter);

		argv[2] = (char*) malloc(intent.size()+1);
		strcpy(argv[2], intent.c_str());

		argc += 2;
	}
	char **pargv = (char **)argv;

	extract_assets();

	bool exit = false;
	while (!exit) {
		//	Wait for window or exit
		exit = waitGUI();
		if (!exit) {
			//	Run
			util::main::run_tvd_tool(argc,pargv);
		}
	}

	// Free ncl parameters
	free(argv[1]);
	free(argv[2]);
}

}	//	namespace android
}	//	namespace util

//	Android entry point
void android_main( struct android_app* state ) {
    //	Make sure glue isn't stripped.
	app_dummy();

	//	Run
	util::android::impl::_glue.run(state);
}

