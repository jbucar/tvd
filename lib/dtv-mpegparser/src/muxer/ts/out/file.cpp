#include "file.h"
#include <util/buffer.h>
#include <util/log.h>
#ifdef _WIN32
#	include <io.h>
#	if ! defined(__MINGW32__)
#		define do_close ::_close
#		define do_write ::_write
#	else
#		define do_close ::close
#		define do_write ::write
#	endif
#else
#	define do_close ::close
#	define do_write ::write
#endif
#include <sys/stat.h>
#include <fcntl.h>


namespace tuner {
namespace muxer {
namespace ts {

FileOutput::FileOutput( const std::string &fileName )
	: _fileName(fileName)
{
	_fd = -1;
}

FileOutput::~FileOutput()
{
}

bool FileOutput::initialize() {
	LDEBUG( "FileOutput", "Initialize: url=%s", url().c_str() );

#ifdef _WIN32
	_fd = _open( url().c_str(), O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE );
#else
	_fd = open( url().c_str(), O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
#endif

	return _fd >= 0 ? true : false;
}

void FileOutput::finalize() {
	LDEBUG( "FileOutput", "Finalize" );
	close(_fd);
	_fd = -1;
}

const std::string FileOutput::url() const {
	return _fileName;
}

bool FileOutput::write( util::Buffer *buf ) {
	return do_write( _fd, buf->data(), buf->length() ) == (int)buf->length();
}

}
}
}

