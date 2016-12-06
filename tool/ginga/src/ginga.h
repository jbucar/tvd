#pragma once
#include <util/tool.h>

/**
 * Clase pincipal del middleware ginga.ar.
 * Permite la ejecución de aplicaciones ncl-lua acorde a la norma ABNT-15606.
 */
class Ginga : public util::Tool {
public:
	Ginga();
	virtual ~Ginga();

protected:
	virtual int run( util::cfg::cmd::CommandLine &cmd );

	//Configuration
	virtual void registerProperties( util::cfg::cmd::CommandLine &cmd );

	virtual void printVersion() const;
};
