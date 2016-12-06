/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#include "../../util.h"
#include "../../../../src/service/ca/conditional.h"
#include "../../../../src/service/ca/system.h"
#include "../../../../src/service/ca/delegate.h"
#include "../../../../src/service/ca/decrypter.h"
#include "../../../../src/service/service.h"
#include "../../../../src/demuxer/psi/psidemuxer.h"
#include "../../../../src/demuxer/pes/pesdemuxer.h"
#include "../../../../src/demuxer/ts.h"
#include "../../../../src/provider/filter.h"
#include <boost/shared_ptr.hpp>
#include <gtest/gtest.h>

typedef std::map<void *,int> TestDlg;
class BasicDelegate : public tuner::ca::Delegate {
public:
	explicit BasicDelegate( tuner::ca::Conditional *sys, int &start, int &stop, TestDlg &srv )
		: tuner::ca::Delegate(sys), _start(start), _stop(stop), _srv(srv)
	{
	}

	int &_start, &_stop;
	TestDlg &_srv;

	virtual void start() {
		_start++;
	}
	virtual void stop() {
		_stop++;
	}

	//	Notifications
	virtual void onStart( tuner::Service *srv ) {
		std::map<void *,int>::const_iterator it=_srv.find(srv);
		if (it != _srv.end()) {
			_srv[srv] = 0;
		}
		else {
			_srv[srv]++;
		}
	}
	virtual void onStop( tuner::Service *srv ) {
		std::map<void *,int>::const_iterator it=_srv.find(srv);
		if (it != _srv.end()) {
			_srv[srv] = 0;
		}
		else {
			_srv[srv]++;
		}
	}

};

class TestFilterDelegate : public tuner::ca::FilterDelegate {
public:
	TestFilterDelegate() {}
	virtual ~TestFilterDelegate() {}

	virtual bool startFilter( tuner::PSIDemuxer *demux ) {
		_psi[demux->pid()] = demux;
		return true;
	}
	virtual bool startFilter( tuner::pes::FilterParams *params ) {
		_pes[params->pid] = params;
		return true;
	}

	template<typename T>
	void stopAux( tuner::ID pid, std::map<tuner::ID,T *> &vec, bool del ) {
		typename std::map<tuner::ID,T *>::iterator it = vec.find(pid);
		if (it != vec.end()) {
			T *f = (*it).second;
			vec.erase(it);
			if (del) {
				delete f;
			}
		}
	}

	virtual void stopFilter( tuner::ID pid ) {
		stopAux<tuner::PSIDemuxer>( pid, _psi, true );
		stopAux<tuner::pes::FilterParams>( pid, _pes, false );
	}

	void probe( tuner::ID pid, const std::string &file ) {
		tuner::PSIDemuxer *demux = _psi[pid];
		ASSERT_TRUE(demux != NULL);

		util::Buffer *sec = test::getSection( file.c_str() );
		ASSERT_TRUE(sec!=NULL);
		demux->process( sec );
		delete sec;
	}

	std::map<tuner::ID,tuner::PSIDemuxer *> _psi;
	std::map<tuner::ID,tuner::pes::FilterParams *> _pes;
};


TEST( ca, constructor ) {
	TestFilterDelegate *filter = new TestFilterDelegate();
	tuner::ca::Conditional ca(filter);
}

TEST( ca, start_stop_with_delegates ) {
	TestFilterDelegate *filter = new TestFilterDelegate();
	int start=0, stop=0;
	TestDlg srvTest;

	{
		tuner::ca::Conditional ca(filter);
		BasicDelegate *dlg = new BasicDelegate(&ca,start,stop,srvTest);
		ca.addDelegate(dlg);

		ca.start();
		ASSERT_EQ( start, 1 );
		ASSERT_EQ( stop, 0 );
		ASSERT_EQ( srvTest.size(), 0 );
		ca.start();
		ASSERT_EQ( start, 1 );
		ASSERT_EQ( stop, 0 );
		ASSERT_EQ( srvTest.size(), 0 );

		ca.stop();
		ASSERT_EQ( start, 1 );
		ASSERT_EQ( stop, 1 );
		ASSERT_EQ( srvTest.size(), 0 );
		ca.stop();
		ASSERT_EQ( start, 1 );
		ASSERT_EQ( stop, 1 );
		ASSERT_EQ( srvTest.size(), 0 );

		ca.start();
		ASSERT_EQ( start, 2 );
		ASSERT_EQ( stop, 1 );
		ASSERT_EQ( srvTest.size(), 0 );
	}

	ASSERT_EQ( start, 2 );
	ASSERT_EQ( stop, 2 );
	ASSERT_EQ( srvTest.size(), 0 );
}


TEST( ca, start_stop_cat ) {
	TestFilterDelegate *filter = new TestFilterDelegate();
	int start=0, stop=0;
	TestDlg srvTest;

	{
		tuner::ca::Conditional ca(filter);
		BasicDelegate *dlg = new BasicDelegate(&ca,start,stop,srvTest);
		ca.addDelegate(dlg);

		ca.start();
		ASSERT_EQ( filter->_psi.size(), 1 );

		ca.stop();
		ASSERT_EQ( filter->_psi.size(), 0 );
	}
}

TEST( ca, process_cat ) {
	TestFilterDelegate *filter = new TestFilterDelegate();
	int start=0, stop=0;
	TestDlg srvTest;

	{
		tuner::ca::Conditional ca(filter);
		BasicDelegate *dlg = new BasicDelegate(&ca,start,stop,srvTest);
		ca.addDelegate(dlg);

		ca.start();
		ASSERT_EQ( filter->_psi.size(), 1 );

		filter->probe( 0x1, "cat.sec" );
		{
			std::vector<tuner::ID> caSystems;
			ca.getAvailables( caSystems );
			ASSERT_EQ( caSystems.size(), 8 );
			ASSERT_EQ( caSystems[0], 0x1860 );
			tuner::ca::System *sys = ca.getSystem( 0x1860 );
			ASSERT_TRUE( sys != NULL );
		}

		ca.stop();
		ASSERT_EQ( filter->_psi.size(), 0 );

		{
			std::vector<tuner::ID> caSystems;
			ca.getAvailables( caSystems );
			ASSERT_EQ( caSystems.size(), 0 );
			tuner::ca::System *sys = ca.getSystem( 0x1860 );
			ASSERT_TRUE( sys == NULL );
		}
	}
}

static void onRaw( const tuner::ca::System::RawData & /*raw*/ ) {
}

TEST( ca, start_emm ) {
	TestFilterDelegate *filter = new TestFilterDelegate();
	int start=0, stop=0;
	TestDlg srvTest;

	{
		tuner::ca::Conditional ca(filter);
		BasicDelegate *dlg = new BasicDelegate(&ca,start,stop,srvTest);
		ca.addDelegate(dlg);

		ca.start();
		ASSERT_EQ( filter->_psi.size(), 1 );

		filter->probe( 0x1, "cat.sec" );
		std::vector<tuner::ID> caSystems;
		{
			std::vector<tuner::ID> caSystems;
			ca.getAvailables( caSystems );
			ASSERT_EQ( caSystems.size(), 8 );
			ASSERT_EQ( caSystems[0], 0x1860 );
		}

		tuner::ca::System *sys = ca.getSystem( 0x1860 );

		//	Start EMM
		ASSERT_TRUE( sys != NULL );
		ASSERT_EQ( sys->startEMM( &onRaw ), true );
		ASSERT_EQ( sys->startEMM( &onRaw ), false );

		ASSERT_EQ( sys->startECM( 0x1000, &onRaw ), false );

		ca.stop();
		ASSERT_EQ( filter->_psi.size(), 0 );

		ca.getAvailables( caSystems );
		ASSERT_EQ( caSystems.size(), 0 );
	}
}

TEST( ca, start_ecm ) {
	TestFilterDelegate *filter = new TestFilterDelegate();
	int start=0, stop=0;
	TestDlg srvTest;

	{
		tuner::ca::Conditional ca(filter);
		BasicDelegate *dlg = new BasicDelegate(&ca,start,stop,srvTest);
		ca.addDelegate(dlg);

		ca.start();
		ASSERT_EQ( filter->_psi.size(), 1 );

		filter->probe( 0x1, "cat.sec" );
		std::vector<tuner::ID> caSystems;
		{
			std::vector<tuner::ID> caSystems;
			ca.getAvailables( caSystems );
			ASSERT_EQ( caSystems.size(), 8 );
			ASSERT_EQ( caSystems[0], 0x1860 );
		}

		tuner::ca::System *sys = ca.getSystem( 0x1860 );
		ASSERT_TRUE( sys != NULL );

		tuner::Service *srv;

		{	//	Start a service
			tuner::ID srvID = 0x1000;
			tuner::ID srvPID = 0x100;
			tuner::ID pcrPID = 0x101;

			ASSERT_FALSE( sys->isAvailable( srvID ) );

			std::vector<tuner::ElementaryInfo> elems;
			tuner::desc::Descriptors descs;
			{
				tuner::ElementaryInfo elem;
				elem.streamType = 0x1b;
				elem.pid = 0x102;
				elems.push_back( elem );
			}
			{
				util::BYTE desc[] = { 0x09, 0x04, 0x18, 0x60, 0xe1, 0x25 };
				descs.addDescriptor( desc, 6 );
			}
			tuner::Pmt *pmt = new tuner::Pmt( srvPID, 1, srvID, pcrPID, descs, elems );
			srv = test::createService( srvID, srvPID, pmt );
		}

		ca.onStart( srv );

		//	Check system is available on this service
		ASSERT_TRUE( sys->isAvailable( srv->id() ) );
		ASSERT_FALSE( sys->isAvailable( 0x1001 ) );

		//	Start ECM
		ASSERT_EQ( sys->startECM( srv->id(), &onRaw ), true );

		ca.onStop( srv );

		ca.stop();
		ASSERT_EQ( filter->_psi.size(), 0 );

		delete srv;

		ca.getAvailables( caSystems );
		ASSERT_EQ( caSystems.size(), 0 );
	}
}

class ConstDelegate : public tuner::ca::Delegate {
public:
	ConstDelegate( tuner::ca::Conditional *sys ) : tuner::ca::Delegate(sys) {}
	virtual int canDecrypt( tuner::ID /*srvID*/ ) const { return 10; }
};

TEST( ca, decrypt_const_even_cw ) {
	TestFilterDelegate *filter = new TestFilterDelegate();
	TestDlg srvTest;

	{
		tuner::ca::Conditional ca(filter);
		ConstDelegate *dlg = new ConstDelegate(&ca);
		ca.addDelegate(dlg);

		ca.start();
		ASSERT_EQ( filter->_psi.size(), 1 );

		//	Create decrypter for service 0x100
		tuner::ca::Decrypter *csa = ca.allocDecrypter( 0x100 );
		ASSERT_TRUE( csa != NULL );

		//	Get buffer for ts
		util::Buffer *buf = test::getSection( "csa_even.ts" );
		ASSERT_TRUE(buf!=NULL);

		//	Update keys
		tuner::ca::Keys keys;
		memcpy(keys.even,"1122334455667788",CW_KEY_SIZE);
		memcpy(keys.odd,"1122334455667788",CW_KEY_SIZE);
		csa->updateKeys( 0x100, keys );

		ASSERT_EQ( csa->decrypt( buf ), true );

#if 0
		//	Test decrypt
		util::BYTE *ptr = buf->bytes();
		size_t payloadOffset = TS_HEAD_SIZE + TSA_LEN(ptr) + 1;
		ASSERT_EQ( ptr[payloadOffset], 0x3A );
#endif

		delete buf;
		ca.freeDecrypter( csa );

		ca.stop();
		ASSERT_EQ( filter->_psi.size(), 0 );
	}
}

class UpdateKeysDelegate : public tuner::ca::Delegate {
public:
	UpdateKeysDelegate( tuner::ca::Conditional *sys ) : tuner::ca::Delegate(sys) {}
	virtual int canDecrypt( tuner::ID /*srvID*/ ) const { return 10; }

	void sendKeys( tuner::ID srvID ) {
		//	Update keys
		tuner::ca::Keys keys;
		memcpy(keys.even,"1122334455667788",CW_KEY_SIZE);
		memcpy(keys.odd,"1122334455667788",CW_KEY_SIZE);
		updateKeys( srvID, keys );
	}
};

TEST( ca, update_keys ) {
	TestFilterDelegate *filter = new TestFilterDelegate();
	TestDlg srvTest;

	{
		tuner::ca::Conditional ca(filter);
		UpdateKeysDelegate *dlg = new UpdateKeysDelegate(&ca);
		ca.addDelegate(dlg);

		ca.start();
		ASSERT_EQ( filter->_psi.size(), 1 );

		//	Create decrypter for service 0x100
		tuner::ca::Decrypter *csa = ca.allocDecrypter( 0x100 );
		ASSERT_TRUE( csa != NULL );

		{	//	Test with invalid keys
			util::Buffer *buf = test::getSection( "csa_even.ts" );
			ASSERT_TRUE(buf!=NULL);

			dlg->sendKeys( 0x101 );
			ASSERT_EQ( csa->decrypt( buf ), true );
#if 0
			util::BYTE *ptr = buf->bytes();
			size_t payloadOffset = TS_HEAD_SIZE + TSA_LEN(ptr) + 1;
			ASSERT_EQ( ptr[payloadOffset], 0x72 );
#endif

			delete buf;
		}

		{	//	Test with valid keys
			util::Buffer *buf = test::getSection( "csa_even.ts" );
			ASSERT_TRUE(buf!=NULL);

			dlg->sendKeys( 0x100 );
			ASSERT_EQ( csa->decrypt( buf ), true );
#if 0
			util::BYTE *ptr = buf->bytes();
			size_t payloadOffset = TS_HEAD_SIZE + TSA_LEN(ptr) + 1;
			ASSERT_EQ( ptr[payloadOffset], 0x3A );
#endif

			delete buf;
		}

		ca.freeDecrypter( csa );

		ca.stop();
		ASSERT_EQ( filter->_psi.size(), 0 );
	}
}

