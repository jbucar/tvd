#include "lzapper.h"
#include "view/view.h"
#include "view/modules/canvas.h"
#include "view/modules/fs.h"
#include "view/modules/application_helper.h"
#include "generated/config.h"
#include <luaz/lua.h>
#include <luaz/ui.h>
#include <luaz/modules/module.h>
#include <zapper/zapper.h>
#include <util/cfg/cfg.h>
#include <util/cfg/cmd/commandline.h>

CREATE_TOOL(LZapper,ZAMBA)

namespace impl {

namespace view {
	void attach( std::vector<std::string> & /*deps*/ ) {}
	void initialize( zapper::Zapper * /*mgr*/ ) {}
	void finalize( void ) {}
	void start( lua_State *st ) { wgt::View::init(st); }
	void stop( void ) { wgt::View::fin(); }
}

DO_MODULE(view);

namespace canvas {
	void attach( std::vector<std::string> & /*deps*/ ) {}
	void initialize( zapper::Zapper * zapper ) {
		::canvas::init(zapper);
	}
	void finalize( void ) {
		::canvas::fin();
	}
	void start( lua_State *st ) {
		::canvas::start(st);
	}
	void stop( void ) {}
}

DO_MODULE(canvas);
DO_MODULE(fs);
DO_MODULE(application_helper);

}	// End impl namespace

LZapper::LZapper() {
}

LZapper::~LZapper() {
}

//Configuration
void LZapper::registerProperties( util::cfg::cmd::CommandLine &cmd ) {
	Tool::registerProperties( cmd );
}

int LZapper::run( util::cfg::cmd::CommandLine & /*cmd*/ ) {
	//	Set windows title
	luaz::UI ui( "zamba" );

	{	//	Add modules
		luaz::module::addModule( &impl::mod_canvas );
		luaz::module::addModule( &impl::mod_view );
		luaz::module::addModule( &impl::mod_fs );
		luaz::module::addModule( &impl::mod_application_helper );
	}

	{	//	Setup zapper browser enabled variable
		ui.setEnvironmentVar("videoEnabled", ZAMBA_VIDEO_ENABLED);
		ui.setEnvironmentVar("musicEnabled", ZAMBA_MUSIC_ENABLED);
		ui.setEnvironmentVar("appsEnabled", (bool) ZAMBA_APPS_ENABLED);
	}

	return ui.run( name() + ".lua" );
}
