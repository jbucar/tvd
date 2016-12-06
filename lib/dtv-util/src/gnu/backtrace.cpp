#include "../assert.h"
#include "../log.h"
#include "generated/config.h"
#include <execinfo.h>
#include <cxxabi.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

// stacktrace.h (c) 2008, Timo Bingmann from http://idlebox.net/
// published under the WTFPL v2.0

namespace util {
namespace impl {

static FILE *out = NULL;

static void sighandler(int sig) {
	LERROR( "Tool", "Got signal %d", sig );
	util::abort();
}

}	//	namespace impl

#define BACKTRACE_MAX_FRAMES (20)
void dumpBacktrace() {
	fprintf(impl::out, "[util] Begin backtrace\n");

	// storage array for stack trace address data
	void* addrlist[BACKTRACE_MAX_FRAMES+1];

	// retrieve current stack addresses
	int addrlen = ::backtrace(addrlist, (int) (sizeof(addrlist) / sizeof(void*)));
	if (!addrlen) {
		fprintf(impl::out, "\t<empty, possibly corrupt>\n");
		return;
	}

	// resolve addresses into strings containing "filename(function+address)",
	// this array must be free()-ed
	char** symbollist = backtrace_symbols(addrlist, addrlen);
	if (!symbollist) {
		fprintf(impl::out, "\t<out of memory>\n");
		return;
	}

	// allocate string which will be filled with the demangled function name
	size_t funcnamesize = 256;
	char* funcname = (char*)malloc(funcnamesize);
	if (!funcname) {
		fprintf(impl::out, "\t<out of memory>\n");
		return;
	}

	// iterate over the returned symbol lines. skip the first, it is the
	// address of this function.
	for (int i = 1; i < addrlen; i++) {
		char *begin_name = NULL;
		char *begin_offset = NULL;
		char *end_offset = NULL;
		char *begin_addr = NULL;
		char *end_addr = NULL;

		// find parentheses and +address offset surrounding the mangled name:
		// ./module(function+0x15c) [0x8048a6d]
		for (char *p = symbollist[i]; *p; ++p) {
			if (*p == '(')
				begin_name = p;
			else if (*p == '+')
				begin_offset = p;
			else if (*p == ')' && begin_offset)
				end_offset = p;
			else if (*p == '[')
				begin_addr = p;
			else if (*p == ']' && begin_addr)
				end_addr = p;
		}

		if (begin_name && begin_offset && end_offset
			&& begin_name < begin_offset
			&& begin_addr < end_addr)
		{
			*begin_name++ = '\0';
			*begin_offset++ = '\0';
			*end_offset = '\0';
			*begin_addr++ = '\0';
			*end_addr = '\0';

			// mangled name is now in [begin_name, begin_offset) and caller
			// offset in [begin_offset, end_offset). now apply __cxa_demangle():
			int status;
			char* ret = abi::__cxa_demangle(begin_name, funcname, &funcnamesize, &status);
			if (status == 0) {
				funcname = ret; // use possibly realloc()-ed string
				fprintf(impl::out, "\t%s in %s+%s at %s\n", begin_addr, funcname, begin_offset, symbollist[i] );
			}
			else {
				// demangling failed. Print function name as a C function with no arguments.
				fprintf(impl::out, "\t%s : %s()+%s\n", symbollist[i], begin_name, begin_offset);
			}
		}
		else {
			// couldn't parse the line? print the whole line.
			fprintf(impl::out, "\t%s\n", symbollist[i]);
		}
	}

	free(funcname);
	free(symbollist);
	fprintf(impl::out, "[util] End backtrace\n");
}

void setupUnhandledException() {
	std::string fileOutput(GNU_BACKTRACE_OUTPUT);
	if (fileOutput.empty()) {
		impl::out = stderr;
	}
	else {
		impl::out = fopen( fileOutput.c_str(), "w" );
		if (!impl::out) {
			impl::out = stderr;
		}
	}
	struct sigaction sa;
	sa.sa_handler = impl::sighandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGSEGV, &sa, NULL);
	sigaction(SIGBUS, &sa, NULL);
	sigaction(SIGILL, &sa, NULL);

	//	Setup custom debugging
	std::set_terminate( &util::abort );
}

}

