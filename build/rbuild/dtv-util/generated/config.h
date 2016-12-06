#pragma once

#define UTIL_NAME "util"
#define UTIL_DESCRIPTION "This is a misc. util library."
#define UTIL_VENDOR "LIFIA"
#define UTIL_VERSION "0.0.3141"
#define UTIL_REPO_VERSION "1.0-3141-hsdfdhajsdhfa"

// Option base(android)
#define BASE_USE_ANDROID 0

// Option base(linux)
#define BASE_USE_LINUX 1

// Option io(ev)
#define IO_USE_EV 1

// Option io(libevent)
#define IO_USE_LIBEVENT 0

// Option io(uv)
#define IO_USE_UV 0

// Option log(android)
#define LOG_USE_ANDROID 0

// Option log(log)
#define LOG_USE_LOG 1

// Option settings(bdb)
#define SETTINGS_USE_BDB 1

// Option settings(memory)
#define SETTINGS_USE_MEMORY 1

// Option settings(xml)
#define SETTINGS_USE_XML 1

// Option backtrace(breakpad)
#define BACKTRACE_USE_BREAKPAD 0

// Option backtrace(gnu)
#define BACKTRACE_USE_GNU 1

// Option backtrace(win32)
#define BACKTRACE_USE_WIN32 0

// Option utf8_converter(iconv)
#define UTF8_CONVERTER_USE_ICONV 1

// Option utf8_converter(embed)
#define UTF8_CONVERTER_USE_EMBED 1

// Option dom(tinyxml)
#define DOM_USE_TINYXML 1

// Option dom(xerces)
#define DOM_USE_XERCES 0

// Option netlib(curl)
#define NETLIB_USE_CURL 1

// Parameter linux_install_prefix
#define LINUX_INSTALL_PREFIX "/usr"
// Parameter gnu_backtrace_output
#define GNU_BACKTRACE_OUTPUT ""

