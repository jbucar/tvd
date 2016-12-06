#pragma once

#ifdef _WIN32
#	ifdef LIFIA_EXPORTS
#		define LIFIA_API __declspec(dllexport)
#	else
#		define LIFIA_API __declspec(dllimport)
#	endif
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <windows.h>
#else
#	define LIFIA_API
#endif
#include <string>
#include <stdint.h>

namespace tvd {

#define MAKE_API_VERSION(major, minor) (((major & 0x0000FFFFu) << 16) | (minor & 0x0000FFFFu))
#define GET_MAJOR_VERSION(version) ((version & 0xFFFF0000u) >> 16)
#define GET_MINOR_VERSION(version) (version & 0x0000FFFFu)

/******************************************************************************
 *************************** HtmlShell exposed api ****************************
 ******************************************************************************/
class HtmlShellWrapper {
public:
	virtual bool hasApi( const std::string &/*name*/, uint32_t /*major*/, uint32_t /*minor*/ ) { return false; }

	template<typename T>
	T *getApi( const std::string &name, uint32_t major, uint32_t minor ) {
		return static_cast<T*>(getApiImpl(name, major, minor));
	}

protected:
	virtual void *getApiImpl( const std::string &/*name*/, uint32_t /*major*/, uint32_t /*minor*/ ) { return NULL; }
};

/******************************************************************************
 ***************************** Library interface ******************************
 ******************************************************************************/
class LibWrapper {
public:
	LibWrapper( const std::string &name, const std::string &version )
		: _name(name), _version(version) {}

	const std::string &name() const { return _name; }
	const std::string &version() const { return _version; }

private:
	std::string _name;
	std::string _version;
};

typedef LibWrapper* (*InitFn)(HtmlShellWrapper *wrapper);
typedef void(*FinFn)();

extern "C" {
LIFIA_API LibWrapper *init( HtmlShellWrapper *wrapper );
LIFIA_API void fin();
}

}
