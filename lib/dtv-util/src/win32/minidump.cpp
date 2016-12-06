#include "../log.h"
#include <boost/filesystem.hpp>
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib,"Dbghelp.lib")

namespace util {
namespace win32 {

namespace bfs = boost::filesystem;

//	Global variable
static LPTOP_LEVEL_EXCEPTION_FILTER _oldHandler = NULL;

#define APIERROR( s, e )	\
	LERROR( "win32", s, (e), lastErrorDesc(e).c_str() );

static std::string lastErrorDesc( DWORD errorCode ) {
	LPVOID lpMsgBuf;
	DWORD length;
	std::string buf;

	lpMsgBuf = NULL;
	length = ::FormatMessage(
					FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					errorCode,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					(LPTSTR) &lpMsgBuf,
					0,
					NULL
					);

	if (!length) {
		buf = "Not error description";
	}
	else {
		//	Strip last character (^M)
		((LPSTR)lpMsgBuf)[length-2] = '\0';
		buf = (LPSTR)lpMsgBuf;
	}

	// Free the buffer.
	::LocalFree( lpMsgBuf );
	return buf;
}

//	Aux function callback
static LONG dumpToFile( const std::string &fileName, DWORD dwProcID, struct _EXCEPTION_POINTERS *pExceptionInfo ) {
	LONG retval = EXCEPTION_CONTINUE_SEARCH;
	HANDLE hFile;

	hFile = ::CreateFile( fileName.c_str(),
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL );
	if (hFile != INVALID_HANDLE_VALUE) {
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = 0;

		MINIDUMP_TYPE mType=MINIDUMP_TYPE(
			MiniDumpWithDataSegs |
			MiniDumpWithFullMemory |
			MiniDumpWithHandleData |
			MiniDumpWithUnloadedModules |
			MiniDumpWithIndirectlyReferencedMemory |
			MiniDumpWithProcessThreadData |
			MiniDumpWithPrivateReadWriteMemory
		);

		// write the dump
		LDEBUG( "win32", "Call into MiniDumpWriteDump to file=%s.", fileName.c_str() );
		BOOL bOk = ::MiniDumpWriteDump(
			::GetCurrentProcess(),
			dwProcID,
			hFile,
			mType,
			&ExInfo,
			NULL,
			NULL );

		if (bOk) {
			retval = EXCEPTION_EXECUTE_HANDLER;
			LINFO( "win32", "Dump to file success." );
		}
		else {
			APIERROR( "Dump error %d: %s", ::GetLastError() );
		}
		::CloseHandle(hFile);
	}
	else {
		APIERROR( "Cannot open dump file: %d: %s", ::GetLastError() );
	}

	return retval;
}

static LONG WINAPI topLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo ) {
	LONG retval;
	DWORD dwProcID=::GetCurrentProcessId();

	util::log::fin();

#ifdef _DEBUG
	bfs::path tmp = bfs::temp_directory_path();
	tmp /= "dump_%%%%%%%%";
	bfs::path name = bfs::unique_path( tmp );
	retval=dumpToFile( name.string(), dwProcID, pExceptionInfo );
#else
	LERROR( "win32", "Unhandled exception: eip=%08lx.", pExceptionInfo->ExceptionRecord->ExceptionAddress );
	retval = EXCEPTION_EXECUTE_HANDLER;
#endif

	return retval;
}

}	//	namespace win32

void setupUnhandledException() {
	win32::_oldHandler=::SetUnhandledExceptionFilter( topLevelFilter );
	std::set_terminate( &util::abort );
}

void dumpBacktrace() {
}

}

