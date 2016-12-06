#pragma once

#include <util/tool.h>

/**
* Clase perteneciente a la documentación "build_cmake".
* Ejemplo de utilización de dtv-sample y herencia de util::Tool
*/
class SampleTool : public util::Tool {
public:
	SampleTool();
	virtual ~SampleTool();

protected:
	int run( util::cfg::cmd::CommandLine &cmd );
        virtual void registerProperties( util::cfg::cmd::CommandLine &cmd );
};

