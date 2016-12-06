/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "delegate.h"
#include "viewer.h"
#include "caption.h"
#include "../streaminfo.h"
#include "../../../../stream/basicpipe.h"
#include "../../../../demuxer/text.h"
#include "../../../../demuxer/pes/caption/captiondemuxer.h"
#include <util/task/dispatcher.h>
#include <util/string.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <util/functions.h>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <stdio.h>

#define MIN_WAIT_INTER_CAPTION  100
#define MAX_WAIT_INTER_CAPTION  1500
#ifdef DEBUG_CAPTION
#	define DEBUG_CMD(i) _debugData.push_back(i);
#else
#	define DEBUG_CMD(i)
#endif

namespace tuner {
namespace player {
namespace es {
namespace cc {

Delegate::Delegate( Viewer *view )
	: _view( view )
{
	_pipe = new tuner::stream::BasicPipe( "cc", 100, 1024 );
	_exit=false;
	_running=false;
	_superImposeStream = view->isCC() ? NULL : player::aribSuperImpose();
}

Delegate::~Delegate()
{
	delete _superImposeStream;
	delete _pipe;
	delete _view;
}

//	Stream player methods
bool Delegate::init() {
	LDEBUG( "cc::Delegate", "Initialize cc delegate" );
	_exit=false;
	_running=false;
	_thread = boost::thread( boost::bind(&Delegate::playCaptions,this) );
	return true;
}

void Delegate::fin() {
	LDEBUG( "cc::Delegate", "Finalize cc delegate" );
	_mutex.lock();
	_exit=true;
	_running=false;
	_mutex.unlock();

	_pipe->exit();
	_thread.join();

	CLEAN_ALL( tuner::arib::Statement *, _sts );
}

pes::mode::type Delegate::mode() const {
	return pes::mode::pes;
}

stream::Pipe *Delegate::pipe( pes::type::type /*esType*/ ) const {
	return _pipe;
}

StreamInfo *Delegate::canPlay( const ElementaryInfo &info, ID tag ) const {
	if (_superImposeStream && _superImposeStream->check(info,tag)) {
		return _superImposeStream;
	}
	return NULL;
}

void Delegate::start() {
	LDEBUG( "cc::Delegate", "Start stream" );

	_view->start();
	_view->dispatcher()->registerTarget( this, "pvr::dvb::CaptionViewer" );

	//	Enable pipe
	_pipe->enable(true);

	//	Run thread
	_mutex.lock();
	_running=true;
	_mutex.unlock();
}

void Delegate::stop() {
	LDEBUG( "cc::Delegate", "Stop stream" );

	_view->dispatcher()->unregisterTarget( this );

	//	Disable pipe
	_pipe->enable(false);

	//	Cleanup statements
	_mutex.lock();
	_running=false;
	CLEAN_ALL( tuner::arib::Statement *, _sts );
	_mutex.unlock();

	//	Finalize view
	_view->stop();
}

//	Aux Demuxer
void Delegate::onShow( const std::string &data ) {
	_view->show( data );	//	If empty, show to clean the text!
}

void Delegate::onStatement( tuner::arib::Statement *st ) { // Service thread
	_mutex.lock();
	if (_running) {
		_sts.push_back( st );
	}
	else {
		delete st;
	}
	_mutex.unlock();
}

void Delegate::onManagement( tuner::arib::CaptionDemuxer *caption, tuner::arib::Management *mgt ) { // Service thread
	if (mgt->langs().size() > 0) {
		//	Clear management callback
		tuner::arib::CaptionDemuxer::ManagementCallback callback;
		caption->onManagement( callback );

		//	Set filter on statement
		caption->onStatement( boost::bind(&Delegate::onStatement,this,_1) );

		//	Setup group to filter: TODO: if group == 0x20 ? have 2 captions!
		util::BYTE group= (mgt->group() == 0) ? 1 : 0x21;
		caption->filterGroup( group );
	}

	mgt->show();
	delete mgt;
}

void Delegate::playCaptions() {
	//	Create PES demuxer
	tuner::arib::CaptionDemuxer *demux=new tuner::arib::CaptionDemuxer(_view->isCC());
	demux->onManagement( boost::bind(&Delegate::onManagement,this,demux,_1) );

	LINFO( "cc::Delegate", "Begin thread" );
	while (!_exit) {
		//	Get and process pes packets
		util::Buffer *pesPkt = _pipe->popBuffer();
		if (pesPkt) {
			demux->parse( pesPkt->bytes(), pesPkt->length() );
			_pipe->free( pesPkt );
		}

		//	Process statements
		while (!_exit && !_sts.empty()) {
			//	Get statement
			tuner::arib::Statement *st = _sts.front();
			_sts.pop_front();

			//	Process statement
			const util::Buffer &buf=st->dataUnit();
			if (buf.length() > 0) {
				//	Parse statement
				processUnit( &buf );

				//	Concatenate all lines
				std::string showText;
				for (size_t i=0; i<_lines.size(); i++) {
					if (i) {
						showText += "\n";
					}
					showText += _lines[i];
				}

				//	Pass to UTF8
				util::toUTF8( charsetName( 0 ), showText );

				_view->dispatcher()->post( this, boost::bind(&Delegate::onShow,this,showText) );
			}

			delete st;
		}
	}

	delete demux;
	LINFO( "cc::Delegate", "End thread" );
}

void Delegate::processUnit( const util::Buffer *buf ) {
	util::BYTE *data=buf->bytes();
	size_t len = buf->length();
	int skip;

#ifdef DEBUG_CAPTION
	_debugData.clear();
	LDEBUG( "cc::Delegate", "Process Unit: len=%d, data=%s", len, buf->asHexa().c_str() );
#endif

	_lines.clear();
	_text.clear();

	while (len > 0) {
		skip=0;
		switch ((*data >> 5) & 0x3) {
			case 0x3:
			case 0x2:
			case 0x1: {
				skip = processChar( data, len );
				break;
			}
			case 0x0: { // C0/C1
				skip = processControlCode(data, len);
				break;
			}
		}

		len -= skip;
		data += skip;
	}

	endLine();

#ifdef DEBUG_CAPTION
	std::string debug;
	BOOST_FOREACH( const std::string &str, _debugData ) {
		debug += "\t";
		debug += str;
		debug += "\n";
	}
	LDEBUG( "cc::Delegate", "Process Unit end:\n%s", debug.c_str() );
#endif
}

void Delegate::endLine() {
	if (!_text.empty()) {
		boost::trim(_text);
		DEBUG_CMD( util::format( "<Text(%s)>", _text.c_str() ) );
		_lines.push_back( _text );
		_text.clear();
	}
}

int Delegate::processControlCode( util::BYTE *data, int len ) {
	int pos;
	switch (data[0]) {
		case CTRL_CODE_APS: {   //	Set active position
			if (data[1] >= 0x40 && data[1] <= 0x7F &&
				data[2] >= 0x40 && data[2] <= 0x7F)
			{
				setActivePosition( data[1] & 0x3F, data[2] & 0x3F );
			}
			pos=3;
			break;
		}
		case CTRL_CODE_COL: {  //	Color
			setColour( data[1] );
			pos=2;
			break;
		}
		case CTRL_CODE_SSZ: { //	Specifies the character size is small
			pos=1;
			setCharacterSmallSize();
			break;
		}
		case CTRL_CODE_MSZ: {  //	Specifies the character size is middle
			pos=1;
			setCharacterMiddleSize();
			break;
		}
		case CTRL_CODE_NSZ: {  //	Specifies the character size is normal
			pos=1;
			setCharacterNormalSize();
			break;
		}
		case CTRL_CODE_WHF: {  //	Foreground color: White
			setForegroundColour( 0xFF, 0xFF, 0xFF );
			pos=1;
			break;
		}
		case CTRL_CODE_CSI:
			pos=processCSI( data, len );
			break;
		case CTRL_CODE_CS: {   //	Clear screen
			pos=1;
			_lines.clear();
			_text.clear();
			DEBUG_CMD( "<CS>" );
			break;
		}
		case CTRL_CODE_APF: {   //	Active position forward
			setActivePositionForward();
			pos=1;
			break;
		}
		case CTRL_CODE_APB: {   //	Active position backward
			setActivePositionBackward();
			pos=1;
			break;
		}
		case CTRL_CODE_APD: {	//	Active position down
			setActivePositionDown();
			pos=1;
			break;
		}
		case CTRL_CODE_APU: {	//	Active position up
			setActivePositionUp();
			pos=1;
			break;
		}
		case CTRL_CODE_NULL:   //	Null
		case CTRL_CODE_BEL:    //	Bell
		case CTRL_CODE_APR:
		case CTRL_CODE_LS1:
		case CTRL_CODE_LS0:
		case CTRL_CODE_PAPF:
		case CTRL_CODE_CAN:
		case CTRL_CODE_SS2:
		case CTRL_CODE_ESC:
		case CTRL_CODE_SS3:
		case CTRL_CODE_RS:
		case CTRL_CODE_US:
		case CTRL_CODE_BKF:
		case CTRL_CODE_RDF:
		case CTRL_CODE_GRF:
		case CTRL_CODE_YLF:
		case CTRL_CODE_BLF:
		case CTRL_CODE_MGF:
		case CTRL_CODE_CNF:
		case CTRL_CODE_SZX:
		case CTRL_CODE_FLC:
		case CTRL_CODE_CDC:
		case CTRL_CODE_POL:
		case CTRL_CODE_WMM:
		case CTRL_CODE_MACRO:
		case CTRL_CODE_HLC:
		case CTRL_CODE_RPC:
		case CTRL_CODE_SPL:
		case CTRL_CODE_STL:
		case CTRL_CODE_TIME:
		default: {
			pos=len;
			LWARN( "cc::Delegate", "Unhanded ctrl code: code=%02x", data[0] );
			break;
		}
	};

	return pos;
}

#define CHECK_PARAMS_EXACT(count)	\
	if (current_params != count) { \
		LWARN( "cc::Delegate", "Invalid parameters: max=%d, availables=%d", count, current_params ); \
		break; \
	}

#define CHECK_PARAMS_RANGE(min,max)	\
	if (current_params <= min || current_params >= max) { \
		LWARN( "cc::Delegate", "Invalid parameters: min=%d, max=%d, availables=%d, count=%d", min, max, current_params ); \
		break; \
	}

int Delegate::processCSI( util::BYTE *data, int len ) {
	int params[CSI_MAX_PARAMS];

	//	CSI Command sequence: CSI P1 I1 P2 I1 P3 ... I1 PN I2 F
	//	Examples:
	//		9B:32:20:58
	//		9B:36:30:38:3B:33:39:30:20:56

	int pos=1;	//	Skip CSI code
	int current_params=0;
	while (data[pos] != CSI_I2 && pos < len) {
		params[current_params] = 0;
		while (data[pos] != CSI_I1 && data[pos] != CSI_I2 && pos < len) {
			params[current_params] *= 10;
			params[current_params] += data[pos] & 0x0F;
			pos++;
		}
		if (data[pos] == CSI_I1) {	//	Skip I1
			pos++;
		}
		current_params++;
		DTV_ASSERT(current_params <= CSI_MAX_PARAMS);
	}
	pos++;	//	Skip I2

	//	Check len
	if (pos > len) {
		return len;
	}

	switch (data[pos]) {
		case CSI_SWF: {
			CHECK_PARAMS_RANGE(0,4);
			setWritingFormat( params, current_params );
			break;
		}
		case CSI_SDP:
			CHECK_PARAMS_EXACT(2);
			setDisplayPosition( params[0], params[1] );
			break;
		case CSI_SDF:
			CHECK_PARAMS_EXACT(2);
			setDisplayFormat( params[0], params[1] );
			break;
		case CSI_SSM:
			CHECK_PARAMS_EXACT(2);
			setCharacterComposition( params[0], params[1] );
			break;
		case CSI_SHS:
			CHECK_PARAMS_EXACT(1);
			setHorizontalSpacing( params[0] );
			break;
		case CSI_SVS:
			CHECK_PARAMS_EXACT(1);
			setVerticalSpacing( params[0] );
			break;
		case CSI_RCS:
			CHECK_PARAMS_EXACT(1);
			setRasterColour( params[0] );
			break;
		default:
			LWARN( "cc::Delegate", "Unhandled CSI extension: code=%02x, params=%d",
				data[pos], current_params );
			break;
	}

	return pos+1;	//	Skip command
}

int Delegate::processChar( util::BYTE *data, int /*len*/ ) {
	if ((*data & 0x7F) == CTRL_CODE_SP) {
		_text += data[0];
	}
	else if ((*data & 0x7F) == CTRL_CODE_DEL) {
		if (!_text.empty()) {
			_text.resize( _text.size()-1 );
		}
	}
	else {
		_text += data[0];
	}
	return 1;
}

//	Commands
void Delegate::setActivePosition( int p1, int p2 ) {
	DEBUG_CMD( util::format( "<APS(%d,%d)>", p1, p2 ) );
	UNUSED(p1);
	UNUSED(p2);
	endLine();
}

void Delegate::setActivePositionForward() {
	DEBUG_CMD("<APF>");
}

void Delegate::setActivePositionBackward() {
	DEBUG_CMD("<APB>");
}

void Delegate::setActivePositionDown() {
	DEBUG_CMD("<APD>");
}

void Delegate::setActivePositionUp() {
	DEBUG_CMD("<APU>");
}

void Delegate::setForegroundColour( util::BYTE r, util::BYTE g, util::BYTE b, util::BYTE alpha/*=255*/ ) {
	DEBUG_CMD( util::format( "<fg color: rgba=%02x,%02x,%02x,%02x>", r, g, b, alpha ) );
	UNUSED(r);
	UNUSED(g);
	UNUSED(b);
	UNUSED(alpha);
}

void Delegate::setBackgroundColour( util::BYTE r, util::BYTE g, util::BYTE b, util::BYTE alpha/*=255*/ ) {
	DEBUG_CMD( util::format( "<bg color: rgba=%02x,%02x,%02x,%02x>", r, g, b, alpha ) );
	UNUSED(r);
	UNUSED(g);
	UNUSED(b);
	UNUSED(alpha);
}

void Delegate::setColour( int p1 ) {
	switch (p1) {
		case 0x50:
		case 0x70:	//	Black
			setBackgroundColour( 0, 0, 0 );
			break;
		case 0x51:
		case 0x71:	//	Red
			setBackgroundColour( 0xFF, 0, 0 );
			break;
		case 0x52:
		case 0x72:	//	Green
			setBackgroundColour( 0, 0xFF, 0 );
			break;
		case 0x53:
		case 0x73:	//	Yellow
			setBackgroundColour( 0xFF, 0xFF, 0 );
			break;
		case 0x54:
		case 0x74:	//	Blue
			setBackgroundColour( 0, 0xFF, 0xFF );
			break;
		case 0x55:
		case 0x75:	//	Magenta
			setBackgroundColour( 0xFF, 0, 0xFF );
			break;
		case 0x56:
		case 0x76:	//	Cyan
			setBackgroundColour( 0, 0xFF, 0xFF );
			break;
		case 0x57:
		case 0x77:	//	White
			setBackgroundColour( 0xFF, 0xFF, 0xFF );
			break;
		case 0x58:
		case 0x78:	//	Transparent
			setBackgroundColour( 0, 0, 0, 0 );
			break;
		case 0x59:
		case 0x79:	//	Half red
			setBackgroundColour( 0x88, 0, 0 );
			break;
		case 0x5A:
		case 0x7A:	//	Half green
			setBackgroundColour( 0, 0x88, 0 );
			break;
		case 0x5B:
		case 0x7B:	//	Half yellow
			setBackgroundColour( 0x88, 0x88, 0 );
			break;
		case 0x5C:
		case 0x7C:	//	Half blue
			setBackgroundColour( 0, 0, 0x88 );
			break;
		case 0x5D:
		case 0x7D:	//	Half magenta
			setBackgroundColour( 0x88, 0, 0x88 );
			break;
		case 0x5E:
		case 0x7E:	//	Half cyan
			setBackgroundColour( 0, 0x88, 0x88 );
			break;
		case 0x5F:
		case 0x7F:	//	Half white
			setBackgroundColour( 0x88, 0x88, 0x88 );
			break;
		case 0x60:	//	Black
			setForegroundColour( 0, 0, 0 );
			break;
		case 0x61:	//	Red
			setForegroundColour( 0xFF, 0, 0 );
			break;
		case 0x62:	//	Green
			setForegroundColour( 0, 0xFF, 0 );
			break;
		case 0x63:	//	Yellow
			setForegroundColour( 0xFF, 0xFF, 0 );
			break;
		case 0x64:	//	Blue
			setForegroundColour( 0, 0, 0xFF );
			break;
		case 0x65:	//	Magenta
			setForegroundColour( 0xFF, 0, 0xFF );
			break;
		case 0x66:	//	Cyan
			setForegroundColour( 0, 0xFF, 0xFF );
			break;
		case 0x67:	//	White
			setForegroundColour( 0xFF, 0xFF, 0xFF );
			break;
		case 0x48:
		case 0x68:	//	Transparent
			setForegroundColour( 0, 0, 0, 0 );
			break;
		case 0x49:
		case 0x69:	//	Half red
			setForegroundColour( 0x88, 0, 0 );
			break;
		case 0x4A:
		case 0x6A:	//	Half green
			setForegroundColour( 0, 0x88, 0 );
			break;
		case 0x4B:
		case 0x6B:	//	Half Yellow
			setForegroundColour( 0x88, 0x88, 0 );
			break;
		case 0x4C:
		case 0x6C:	//	Half Blue
			setForegroundColour( 0, 0, 0x88 );
			break;
		case 0x4D:
		case 0x6D:	//	Half Magenta
			setForegroundColour( 0x88, 0, 0x88 );
			break;
		case 0x4E:
		case 0x6E:	//	Half Cyan
			setForegroundColour( 0, 0x88, 0x88 );
			break;
		case 0x4F:
		case 0x6F:	//	Half White
			setForegroundColour( 0x88, 0x88, 0x88 );
			break;
		default:
			LWARN("cc::Delegate", "Unknown color: p==%02x", p1 );
			break;
	};
}

void Delegate::setCharacterSmallSize() {
	DEBUG_CMD("<SSZ>");
}

void Delegate::setCharacterMiddleSize() {
	DEBUG_CMD("<MSZ>");
}

void Delegate::setCharacterNormalSize() {
	DEBUG_CMD("<NSZ>");
}

//	CSI commands
void Delegate::setWritingFormat( int *params, int count ) {
	//	Check writing form
	if (params[0] >= 0 && params[0] <= 12) {
		setWritingForm( params[0] );
	}

	//	Check size
	if (count > 1) {
		switch (params[1]) {
			case 0x30:
				setCharacterSmallSize();
				break;
			case 0x31:
				setCharacterMiddleSize();
				break;
			case 0x33:
				setCharacterNormalSize();
				break;
		};
	}
}

void Delegate::setWritingForm( int value ) {
	//	Value can be:
	//		0: horizontal writing form in standard density
	//		1: vertical writing form in standard density
	//		2: horizontal writing form in high density
	//		3: vertical writing form in high density
	//		4: horizontal writing form in Western language
	//		5: horizontal writing form in 1920x1080
	//		6: vertical writing form in 1920x1080
	//		7: horizontal writing form in 960x540
	//		8: vertical writing form in 950x540
	//		9: horizontal writing form in 720x480
	//	   10: vertical writing form in 720x480
	//	   11: horizontal writing form in 1280x720
	//	   12: vertical writing form in 1280x720

	DEBUG_CMD( util::format("<CSI_SWF(%d)>", value ) );
	UNUSED(value);
}

void Delegate::setDisplayPosition( int horizontal, int vertical ) {
	DEBUG_CMD( util::format( "<SDP(%d,%d)>", horizontal, vertical ) );
	UNUSED(horizontal);
	UNUSED(vertical);
}

void Delegate::setDisplayFormat( int horizontal, int vertical ) {
	DEBUG_CMD( util::format("<SDF(%d,%d)>", horizontal, vertical ));
	UNUSED(horizontal);
	UNUSED(vertical);
}

void Delegate::setCharacterComposition( int horizontal, int vertical ) {
	DEBUG_CMD( util::format("<SSM(%d,%d)>", horizontal, vertical ) );
	UNUSED(horizontal);
	UNUSED(vertical);
}

void Delegate::setHorizontalSpacing( int value ) {
	DEBUG_CMD( util::format("<SHS(%d)>", value ));
	UNUSED(value);
}

void Delegate::setVerticalSpacing( int value ) {
	DEBUG_CMD( util::format("<SVS(%d)>", value ));
	UNUSED(value);
}

void Delegate::setRasterColour( int value ) {
	DEBUG_CMD( util::format("<RCS(%d)>", value ));
	UNUSED(value);
}

}
}
}
}

