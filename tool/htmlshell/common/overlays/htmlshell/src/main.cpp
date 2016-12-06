#include "base/logging.h"

#include "impl/app/maindelegate.h"
#include "content/public/app/content_main.h"

int main( int argc, const char **argv ) {
	tvd::MainDelegate delegate;
	content::ContentMainParams params(&delegate);
	params.argc = argc;
	params.argv = argv;
	return content::ContentMain(params);
}
