
#pragma once
#include <util/tool.h>

/**
 * Clase pincipal de ZaMBA.
 * Desde aquí se inicia la ejecución de la tool.
 */
class LZapper : public util::Tool {
public:
	LZapper();
	virtual ~LZapper();

protected:
	virtual void registerProperties( util::cfg::cmd::CommandLine &cmd );

	int run( util::cfg::cmd::CommandLine &cmd );
};

