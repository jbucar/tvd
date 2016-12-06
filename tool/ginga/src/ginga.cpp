#include "ginga.h"
#include "generated/config.h"
#include <ncl30-presenter/impl/presentationenginemanager.h>
#include <gingaplayer/system.h>
#include <gingaplayer/device.h>
#include <gingaplayer/remote/device.h>
#include <connector/connector.h>
#include <connector/gingaversion.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/fs.h>
#include <util/cfg/cfg.h>
#include <util/cfg/cmd/commandline.h>
#ifdef ANDROID
#include <util/android/android.h>
#endif
#include <boost/filesystem/operations.hpp>
#include <string.h>
#include <stdio.h>

namespace fs = boost::filesystem;
namespace bptgn = br::pucrio::telemidia::ginga::ncl;

CREATE_TOOL(Ginga,GINGA)

Ginga::Ginga()
{
}

Ginga::~Ginga()
{
}

//Configuration
void Ginga::registerProperties( util::cfg::cmd::CommandLine &cmd ) {
	Tool::registerProperties( cmd );

	registerProp("ncl","Start ncl file", std::string(""), cmd);
	registerProp("base_id", 'i', "Set the base ID name", std::string("-1"), cmd);
	registerProp("connector", 'c', "Start connector using arg as pipe.", std::string(DEFAULT_CONNECTOR_FILE), cmd);
}

int Ginga::run( util::cfg::cmd::CommandLine &cmd ) {
	printf("\n   --------------------------| Ginga.ar %s |--------------------------\n", version().c_str());

	//	Set windows title
	util::cfg::setValue("gui.window.title", "Ginga");
	//	Setup icon
	util::cfg::setValue(
		"gui.window.icon",
		util::fs::make( util::fs::installDataDir(), "res", "ginga.ico" )
	);

	fs::path url( getCfg<std::string> ("ncl") );

	if (!url.is_absolute()) {
		url = fs::current_path() / url;
	}

	if (exists(url) && is_regular_file(url)) {
		player::System *sys = new player::System();
		player::Device *dev = NULL;

		//	Use connector?
		if (cmd.isSet("connector")) {
			connector::Connector *conn=new connector::Connector( getCfg<std::string>("connector"), false );
			dev = new player::remote::Device( sys, conn );
		}
		else {
			dev = new player::Device( sys );
		}

		DTV_ASSERT(dev);

		//	Initialize device and add to system
		if (dev->initialize()) {
			sys->addDevice( dev );

			//	Create presentation engine manager
			bptgn::PresentationEngineManagerImpl *pem = new bptgn::PresentationEngineManagerImpl(sys);

			pem->setBaseId( getCfg<std::string> ("base_id") );
			if (pem->startDocument( url.string() )) {
				pem->run();
			}
			delete pem;
		} else {
			LERROR( "Ginga", "Cannot initialize device" );
		}

		sys->destroy();
		delete sys;
	} else {
		LERROR( "Ginga", "invalid ncl file path: url=%s", url.string().c_str());
	}

	LINFO("Ginga", "Process finished.");
	return 0;
}

void Ginga::printVersion() const {
	printf("Ginga.ar %s by LIFIA\n\
Based on Ginga 0.11.2\n\
Copyright 2002-2010 The TeleMidia PUC-Rio Team.\n\
Copyright 2010 LIFIA - Facultad de Informática - Univ. Nacional de La Plata\n", version().c_str());
}
