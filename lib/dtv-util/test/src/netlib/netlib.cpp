/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "../../../src/netlib/manager.h"
#include "../../../src/netlib/requestinfo.h"
#include "../../../src/netlib/reply.h"
#include "../../../src/task/dispatcherimpl.h"
#include "../../../src/log.h"
#include "../../../src/main.h"
#include "../../../src/registrator.h"
#include "../../../src/buffer.h"
#include "generated/config.h"
#include <gtest/gtest.h>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#if NETLIB_USE_CURL

//#define CHECK_INET

namespace fs = boost::filesystem;

// Content-Disposition test cases	http://greenbytes.de/tech/tc2231/

class NetlibTest : public testing::Test {
public:
	NetlibTest() {
		_mgr = NULL;
		_disp = new util::task::DispatcherImpl();
		_disp->onPost( boost::bind(&NetlibTest::runTasks,this) );
	}

	virtual ~NetlibTest() {
		delete _disp;
	}

	void reset() {
		_ready = 0;
	}

	void wait( int count=1 ) {
		bool found=false;
		while (!found) {
			{	//	Lock!
				boost::unique_lock<boost::mutex> lock( _mutex );
				if (_ready != count) {
					_cWakeup.wait( lock );
				}
				else {
					found=true;
				}
			}
			_disp->runAll();
		}
	}

	void notify() {
		_mutex.lock();
		_ready++;
		_cWakeup.notify_all();
		_mutex.unlock();
	}

	void runTasks() {
		_mutex.lock();
		_cWakeup.notify_all();
		_mutex.unlock();
	}

protected:
	virtual void SetUp() {
		util::main::init("util-netlib-test");
		util::log::setLevel("util", "curl", "all");
		util::log::setLevel("util", "netlib", "all");

		reset();
		_mgr = util::netlib::Manager::create();
		ASSERT_TRUE(_mgr);
		_mgr->dispatcher( _disp );
		ASSERT_TRUE(_mgr->initialize());
	}

	virtual void TearDown() {
		_mgr->finalize();
		delete _mgr;
		util::main::fin();
	}

	util::netlib::Manager *_mgr;
	util::task::DispatcherImpl *_disp;

private:
	boost::condition_variable _cWakeup;
	boost::mutex _mutex;
	int _ready;
};

TEST_F( NetlibTest, constructor ) {
}

static void downloadFinished( NetlibTest *test, bool result, int *ok ) {
	if (result) {
		(*ok)++;
	}
	test->notify();
}

TEST_F( NetlibTest, basic_file_fetch ) {
	int isOk = 0;
	fs::path dst = fs::temp_directory_path();
	dst /= "tvd.http";

	{
		util::netlib::RequestInfo *download = new util::netlib::RequestInfo();
		download->url( "http://www.tecnologia.lifia.info.unlp.edu.ar/" );
		download->verbose(true);
		download->onFinished( boost::bind(&downloadFinished,this,_2,&isOk) );

		util::id::Ident id = _mgr->start( download, dst.string() );
		ASSERT_TRUE( util::id::isValid(id) );
	}

	wait();
	ASSERT_EQ( isOk, 1 );

	fs::remove( dst );
}

TEST_F( NetlibTest, basic_mem_fetch ) {
	int isOk = 0;
	util::Buffer buf(128);

	{	//	Create download instance
		util::netlib::RequestInfo *req = new util::netlib::RequestInfo();
		req->onFinished( boost::bind(&downloadFinished,this,_2,&isOk) );
		req->verbose(true);
		req->url( "http://www.tecnologia.lifia.info.unlp.edu.ar/" );

		util::id::Ident id = _mgr->start( req, &buf );
		ASSERT_TRUE( util::id::isValid(id) );
	}

	wait();
	ASSERT_EQ( isOk, 1 );
	ASSERT_FALSE( buf.length() == 0 );
}

TEST_F( NetlibTest, get_headers ) {
	int isOk = 0;
	util::Buffer buf(128);
	util::Buffer headers(128);

	{	//	Create download instance
		util::netlib::RequestInfo *req = new util::netlib::RequestInfo();
		req->onFinished( boost::bind(&downloadFinished,this,_2,&isOk) );
		req->verbose(true);
		req->url( "http://www.tecnologia.lifia.info.unlp.edu.ar/" );

		util::id::Ident id = _mgr->start( req, &buf, &headers );
		ASSERT_TRUE( util::id::isValid(id) );
	}

	wait();
	ASSERT_EQ( isOk, 1 );
	ASSERT_FALSE( buf.length() == 0 );
	ASSERT_FALSE( headers.length() == 0 );
}

#ifdef CHECK_INET

TEST_F( NetlibTest, fetch_fail ) {
	int isOk = 0;

	fs::path dst = fs::temp_directory_path();
	dst /= "tvd.http";

	{	//	Create download instance
		util::netlib::RequestInfo *download = new util::netlib::RequestInfo();
		download->url( "ftp://tvd.lifia.info.unlp.edu.ar/sources/kuntur/docs/zamba/zamba_no_existe.pdf" );
		download->verbose(true);
		download->onFinished( boost::bind(&downloadFinished,this,_2,&isOk) );

		util::id::Ident id = _mgr->start( download, dst.string() );
		ASSERT_TRUE( util::id::isValid(id) );
	}

	wait();
	ASSERT_EQ( isOk, 0 );

	fs::remove( dst );
}

TEST_F( NetlibTest, multi_file_fetch ) {
	int downloaded = 0;
	std::vector<fs::path> files;


	{	//	Create download instance
		fs::path dst = fs::temp_directory_path();
		dst /= "dev.pdf";
		files.push_back( dst );

		util::netlib::RequestInfo *download = new util::netlib::RequestInfo();
		download->onFinished( boost::bind(&downloadFinished,this,_2,&downloaded) );
		download->url( "ftp://tvd.lifia.info.unlp.edu.ar/sources/kuntur/docs/ginga/dev.pdf" );
		download->verbose(true);

		util::id::Ident id = _mgr->start( download, dst.string() );
		ASSERT_TRUE( util::id::isValid(id) );
	}

	{	//	Create download instance
		fs::path dst = fs::temp_directory_path();
		dst /= "zamba.pdf";
		files.push_back( dst );

		util::netlib::RequestInfo *download = new util::netlib::RequestInfo();
		download->onFinished( boost::bind(&downloadFinished,this,_2,&downloaded) );
		download->url( "ftp://tvd.lifia.info.unlp.edu.ar/sources/kuntur/docs/zamba/zamba.pdf" );
		download->verbose(true);

		util::id::Ident id = _mgr->start( download, dst.string() );
		ASSERT_TRUE( util::id::isValid(id) );
	}

	wait(2);
	ASSERT_EQ( downloaded, 2 );

	BOOST_FOREACH( const fs::path &toRemove, files ) {
		fs::remove( toRemove );
	}
}

TEST_F( NetlibTest, stopRequest ) {
	util::id::Ident id;
	int downloaded = 0;

	//	Create download instance
	fs::path dst = fs::temp_directory_path();
	dst /= "dev.pdf";

	{
		util::netlib::RequestInfo *req = new util::netlib::RequestInfo();
		req->onFinished( boost::bind(&downloadFinished,this,_2,&downloaded) );
		req->url( "ftp://tvd.lifia.info.unlp.edu.ar/sources/kuntur/docs/ginga/dev.pdf" );
		req->verbose(true);

		id = _mgr->start( req, dst.string() );
		ASSERT_TRUE( util::id::isValid(id) );
	}

	_mgr->stop( id );

	ASSERT_EQ( downloaded, 0 );
}

TEST_F( NetlibTest, stopAll ) {
	int downloaded = 0;
	std::vector<fs::path> files;

	{	//	Create download instance
		fs::path dst = fs::temp_directory_path();
		dst /= "dev.pdf";
		files.push_back( dst );

		util::netlib::RequestInfo *download = new util::netlib::RequestInfo();
		download->onFinished( boost::bind(&downloadFinished,this,_2,&downloaded) );
		download->url( "ftp://tvd.lifia.info.unlp.edu.ar/sources/kuntur/docs/ginga/dev.pdf" );
		download->verbose(true);

		util::id::Ident id = _mgr->start( download, dst.string() );
		ASSERT_TRUE( util::id::isValid(id) );
	}

	{	//	Create download instance
		fs::path dst = fs::temp_directory_path();
		dst /= "zamba.pdf";
		files.push_back( dst );

		util::netlib::RequestInfo *download = new util::netlib::RequestInfo();
		download->onFinished( boost::bind(&downloadFinished,this,_2,&downloaded) );
		download->url( "ftp://tvd.lifia.info.unlp.edu.ar/sources/kuntur/docs/zamba/zamba.pdf" );
		download->verbose(true);

		util::id::Ident id = _mgr->start( download, dst.string() );
		ASSERT_TRUE( util::id::isValid(id) );
	}

	_mgr->stopAll();
	ASSERT_EQ( downloaded, 0 );
}

TEST_F( NetlibTest, cda_api ) {
	int isOk = 0;
	util::Buffer buf;

	{	//	Create download instance
		util::netlib::RequestInfo *download = new util::netlib::RequestInfo();
		download->onFinished( boost::bind(&downloadFinished,this,_2,&isOk) );
		download->verbose(true);

		{
			std::vector<std::string> headers;
			headers.push_back( "User-Agent: curl/7.34.0" );
			headers.push_back( "apikey: 3b215471706c796964217d6172705d" );
			download->headers( headers );
		}

		download->method( "POST" );
		download->postFields( "call=highlights" );
		download->url( "http://test.cda.admin.dcarsat.com.ar/servicios/api/" );

		util::id::Ident id = _mgr->start( download, &buf );
		ASSERT_TRUE( util::id::isValid(id) );
	}

	wait();
	ASSERT_EQ( isOk, 1 );
	ASSERT_FALSE( buf.length() == 0 );
}

struct TestCases {
	const char *url;
	const char *filename;
};

TEST_F( NetlibTest, parse_names ) {
	static const TestCases tests[] = {
		{ "http://greenbytes.de/tech/tc2231/inlonly.asis", "inlonly.asis" },
		{ "http://greenbytes.de/tech/tc2231/inlwithasciifilename.asis", "foo.html" },
		{ "http://greenbytes.de/tech/tc2231/inlwithfnattach.asis", "Not an attachment!" },
		{ "http://greenbytes.de/tech/tc2231/attonly.asis", "attonly.asis" },
		{ "http://greenbytes.de/tech/tc2231/attwithasciifnescapedchar.asis", "foo.html" },
		{ "http://greenbytes.de/tech/tc2231/attwithasciifnescapedquote.asis", "\"quoting\" tested.html" },
		{ "http://greenbytes.de/tech/tc2231/attwithquotedsemicolon.asis", "Here's a semicolon;.html" },
		{ NULL, NULL }
	};

	int downloaded;
	int i=0;
	while (tests[i].url) {
		downloaded = 0;
		std::string filename = util::netlib::temporaryName();
		util::Buffer headers;

		{	//	Make request
			util::netlib::RequestInfo *download = new util::netlib::RequestInfo();
			download->onFinished( boost::bind(&downloadFinished,this,_2,&downloaded) );
			download->url( tests[i].url );
			download->verbose(true);

			util::id::Ident id = _mgr->start( download, filename, &headers );
			ASSERT_TRUE( util::id::isValid(id) );
		}

		wait();
		ASSERT_EQ( downloaded, 1 );
		ASSERT_FALSE( headers.length() == 0 );

		{
			std::string dstName = util::netlib::resolveName( tests[i].url, &headers );
			ASSERT_EQ( tests[i].filename, dstName );
		}

		fs::remove( filename );

		reset();
		++i;
	}
}

#endif    //	CHECK_INET
#endif    //	NETLIB_USE_CURL
