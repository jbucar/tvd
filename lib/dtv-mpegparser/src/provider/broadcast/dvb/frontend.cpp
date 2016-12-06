/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-mpegparser implementation.

    DTV-mpegparser is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-mpegparser is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-mpegparser.

    DTV-mpegparser es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-mpegparser se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "frontend.h"
#include "provider.h"
#include <util/cfg/cfg.h>
#include <util/log.h>
#include <util/assert.h>
#include <util/mcr.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/dvb/dmx.h>
#include <linux/dvb/version.h>

#define DO_PARAM( p, i, c )	  \
	memset(&p[i],0,sizeof(struct dtv_property)); \
	p[i].cmd = c; \
	i++;

#define DO_PARAM1( p, i, c, v )	  \
	memset(&p[i],0,sizeof(struct dtv_property)); \
	p[i].cmd = c; \
	p[i].u.data = v; \
	i++;

namespace tuner {
namespace dvb {

struct lnb_range_type {
	unsigned int low;
	unsigned int hi;
	unsigned int lo;
	bool hiBand;
};
typedef struct lnb_range_type lnb_range;

static lnb_range bandaKu[] = {
	{ 10700000, 11700000, 9750000, false },
 	{ 11700000, 12750000, 10600000, true }
};

struct diseqc_cmd {
	struct dvb_diseqc_master_cmd cmd;
	uint32_t wait;
};

static inline void msleep(uint32_t msec) {
	struct timespec req = { msec / 1000, 1000000 * (msec % 1000) };
	while (nanosleep(&req, &req))
		;
}

Frontend::Frontend( const std::string &frontend )
	: _frontend(frontend)
{
	_frontendFD = -1;
	_caps = FE_IS_STUPID;

#if DVB_API_VERSION == 5 && DVB_API_VERSION_MINOR >= 3
	_useSet_v5 = util::cfg::getValue<bool>("tuner.provider.dvb.useSet_v5");
#else
	_useSet_v5 = false;
#endif
#if DVB_API_VERSION == 5 && DVB_API_VERSION_MINOR >= 10
	_useStats_v5 = util::cfg::getValue<bool>("tuner.provider.dvb.useStats_v5");
#else
	_useStats_v5 = false;
#endif
	_diseqcTimeout = util::cfg::getValue<int>("tuner.provider.dvb.diseqcTimeout");
}

Frontend::~Frontend()
{
	DTV_ASSERT(_frontendFD);
}

bool Frontend::initialize() {
	struct dvb_frontend_info fe_info;

	//	Open frontend
	if ((_frontendFD = open( _frontend.c_str(), O_RDWR )) < 0) {
		LERROR( "dvb", "Error, failed to open '%s': %d", _frontend.c_str(), errno );
		return false;
	}

	//	Determine FE type and caps
	if (ioctl(_frontendFD, FE_GET_INFO, &fe_info) == -1) {
		LERROR( "dvb", "Error, FE_GET_INFO failed: %d", errno );
		return false;
	}
	_caps = fe_info.caps;

	LDEBUG( "dvb", "Frontend info:" );
	LDEBUG( "dvb", "\t name=%s, type=%x", fe_info.name, fe_info.type );
	LDEBUG( "dvb", "\t frequency min=%d, max=%d, step=%d, tolerance=%d",
		fe_info.frequency_min, fe_info.frequency_max, fe_info.frequency_stepsize, fe_info.frequency_tolerance );
	LDEBUG( "dvb", "\t symbol rate min=%d, max=%d, tolerance=%d",
		fe_info.symbol_rate_min, fe_info.symbol_rate_max, fe_info.symbol_rate_tolerance );
	LDEBUG( "dvb", "\t notifier delay=%d", fe_info.notifier_delay );
	LDEBUG( "dvb", "\t caps=%08X", fe_info.caps );

	//	Load channels config and check
	if (!loadConfig()) {
		close( _frontendFD );
		return false;
	}

	return true;
}

void Frontend::finalize() {
	cleanConfig();
	close( _frontendFD );
}

bool Frontend::start( broadcast::IsdbtConfig *ch ) {
	unsigned int freq = ch->frequency();
	LDEBUG( "dvb", "Tune frequency: freq=%d", freq );
	return _useSet_v5 ? set_v5( freq ) : set_v3( freq );
}

bool Frontend::start( broadcast::SatelliteConfig *ch ) {
#if DVB_API_VERSION == 5 && DVB_API_VERSION_MINOR >= 3
	//	Setup LNB
	unsigned int freq = setupLNB( ch );
	if (!freq) {
		return false;
	}

	struct dtv_property params[7];
	int iParam=0;
	DO_PARAM( params,iParam,DTV_CLEAR);
	DO_PARAM1(params,iParam,DTV_SYMBOL_RATE,ch->symbolRate());
	DO_PARAM1(params,iParam,DTV_INNER_FEC,FEC_AUTO);
	DO_PARAM1(params,iParam,DTV_FREQUENCY,freq);
	DO_PARAM1(params,iParam,DTV_MODULATION,QAM_AUTO);
	DO_PARAM1(params,iParam,DTV_INVERSION, (_caps & FE_CAN_INVERSION_AUTO) ? INVERSION_AUTO : INVERSION_OFF);
	DO_PARAM(params,iParam,DTV_TUNE);

	//	Set
	DTV_ASSERT(iParam == sizeof(params)/sizeof(struct dtv_property));
	struct dtv_properties props = { 0, NULL };
	props.num = iParam;
	props.props = params;
	if (ioctl( _frontendFD, FE_SET_PROPERTY, &props ) != 0 ) {
		LERROR( "Frontend", "Cannot set property: err=%s", strerror(errno) );
		return false;
	}

	return true;

#else
	UNUSED(trans);
	LERROR( "Frontend", "API v5 not supported" );
	DTV_ASSERT(false);
	return false;
#endif
}

bool Frontend::set_v3( int freq ) {
	struct dvb_frontend_parameters param;

	memset( &param, 0, sizeof(dvb_frontend_parameters) );
	param.frequency                    = freq;
	param.inversion                    = (_caps & FE_CAN_INVERSION_AUTO) ? INVERSION_AUTO : INVERSION_OFF;
	param.u.ofdm.code_rate_HP          = FEC_3_4;
	param.u.ofdm.code_rate_LP          = FEC_AUTO;
	param.u.ofdm.constellation         = QAM_AUTO;
	param.u.ofdm.transmission_mode     = TRANSMISSION_MODE_AUTO;
	param.u.ofdm.guard_interval        = GUARD_INTERVAL_AUTO;
	param.u.ofdm.hierarchy_information = HIERARCHY_NONE;
	param.u.ofdm.bandwidth = BANDWIDTH_6_MHZ;

	//	Tune!
	if (ioctl( _frontendFD, FE_SET_FRONTEND, &param ) == -1) {
		LERROR("dvb", "Setting frontend parameters failed");
		return false;
	}

	return true;
}

bool Frontend::set_v5( int freq ) {
#if DVB_API_VERSION == 5 && DVB_API_VERSION_MINOR >= 3
	struct dtv_property params[6];
	int i=0;
	DO_PARAM( params, i, DTV_CLEAR);
	DO_PARAM1(params, i, DTV_DELIVERY_SYSTEM,SYS_DVBT);
	DO_PARAM1(params, i, DTV_FREQUENCY,freq * 1000);
	DO_PARAM1(params, i, DTV_MODULATION,QAM_AUTO);
	DO_PARAM1(params, i, DTV_INVERSION,INVERSION_AUTO);
	DO_PARAM( params, i, DTV_TUNE);

	DTV_ASSERT(i == sizeof(params)/sizeof(struct dtv_property));

	struct dtv_properties props = { 0, NULL };
	props.num = i;
	props.props = params;
	if (ioctl( _frontendFD, FE_SET_PROPERTY, &props ) != 0 ) {
		LERROR( "dvb", "Cannot set property" );
		return false;
	}

	return true;
#endif
	UNUSED(freq);
	LERROR( "dvb", "API v5 not supported" );
	DTV_ASSERT(false);
	return false;
}

//	Status/signal
util::BYTE Frontend::toPorcent( int value, int min, int max ) const {
	//	Stay within limits
	if (value < min) {
		value = min;
	}
	else if (value > max) {
		value = max;
	}

	value = ((value-min)*100)/(max-min);

	return (util::BYTE)value;
}

bool Frontend::getSignal_v5( status::Type &st ) const {
#if DVB_API_VERSION == 5 && DVB_API_VERSION_MINOR >= 10
	struct dtv_property params[2];
	int num=0;
	DO_PARAM( params, num, DTV_STAT_SIGNAL_STRENGTH );
	DO_PARAM( params, num, DTV_STAT_CNR );
	DTV_ASSERT(num == sizeof(params)/sizeof(struct dtv_property));

	struct dtv_properties props = { 0, NULL };
	props.num = num;
	props.props = params;
	if (!ioctl( _frontendFD, FE_GET_PROPERTY, &props )) {
		{	//	Get signal
			bool result=false;
			struct dtv_property *prop = &props.props[0];
			if (!prop->result && prop->u.st.len > 0) {
				LTRACE( "dvb", "Get signal: scale=%d, value=%ld", prop->u.st.stat[0].scale, prop->u.st.stat[0].svalue );
				if (prop->u.st.stat[0].scale == FE_SCALE_RELATIVE) {
					st.signal = util::BYTE((prop->u.st.stat[0].uvalue * 100) / 0xffff);
					result=true;
				}
				else if (prop->u.st.stat[0].scale == FE_SCALE_DECIBEL) {
					st.signal = toPorcent( (int)(prop->u.st.stat[0].svalue/1000), -90, -54 );
					result=true;
				}
			}

			if (!result) {
				LTRACE( "dvb", "Cannot get signal status using v5 API" );
				return false;
			}
		}

		{	//	Get snr
			bool result=false;
			struct dtv_property *prop = &props.props[1];
			if (!prop->result && prop->u.st.len > 0) {
				LTRACE( "dvb", "Get snr: scale=%d, value=%ld", prop->u.st.stat[0].scale, prop->u.st.stat[0].svalue );
				if (prop->u.st.stat[0].scale == FE_SCALE_RELATIVE) {
					st.snr = util::BYTE((prop->u.st.stat[0].uvalue * 100) / 0xffff);
					result=true;
				}
				else if (prop->u.st.stat[0].scale == FE_SCALE_DECIBEL) {
					st.snr = toPorcent( (int)(prop->u.st.stat[0].svalue)/1000,10,32);
					result=true;
				}
			}

			if (!result) {
				LTRACE( "dvb", "Cannot get snr using v5 API" );
				return false;
			}
		}

		return true;
	}

#endif
	UNUSED(st);
	LERROR( "dvb", "Stats using API v5 not supported" );
	return false;
}

bool Frontend::getSignal_v3( status::Type &st ) const {
	//	Get Signal-to-Noise ratio
	uint16_t snr=0;
	if (ioctl(_frontendFD, FE_READ_SNR, &snr) == -1) {
		LWARN( "dvb", "FE_READ_SNR failed" );
		return false;
	}
	st.snr = util::BYTE((snr*100)/0xFFFF);

	//	Get Signal Strength
	uint16_t ss=0;
	if (ioctl(_frontendFD, FE_READ_SIGNAL_STRENGTH, &ss) == -1) {
		LWARN( "dvb", "FE_READ_SIGNAL_STRENGTH failed" );
		return false;
	}
	st.signal = util::BYTE((ss*100)/0xFFFF);

	LTRACE( "dvb", "Get info: signal=%04x, snr=%04x", ss, snr );

	return true;
}

bool Frontend::getStatus( status::Type &st ) const {
	{	//	Get status
		fe_status_t fe_st;
		if (ioctl(_frontendFD, FE_READ_STATUS, &fe_st) == -1) {
			LERROR( "dvb", "FE_READ_STATUS failed" );
			return false;
		}
		st.isLocked = (fe_st & FE_HAS_LOCK && fe_st & FE_HAS_CARRIER) ? true : false;
	}

	if (st.isLocked) {
		bool result;

		if (_useStats_v5) {
			result=getSignal_v5( st );
		}
		else {
			result=getSignal_v3( st );
		}

		if (!result) {
			LWARN( "dvb", "Cannot get signal/snr information" );
			st.signal = 0;
			st.snr = 0;
		}
	}

	return true;
}

unsigned int Frontend::setupLNB( broadcast::SatelliteConfig *cfg ) {
	//	Find LNB range to use in banda Ku
	lnb_range *lnb=NULL;
	for (size_t i=0; i<sizeof(bandaKu)/sizeof(lnb_range); i++) {
		if (cfg->frequency() >= bandaKu[i].low && cfg->frequency() < bandaKu[i].hi) {
			lnb = &bandaKu[i];
			break;
		}
	}
	if (!lnb) {
		LWARN( "dvb", "Transponder is not in LNB frequency range: freq=%d", cfg->frequency() );
		return 0;
	}

	//	Setup tone
	fe_sec_tone_mode_t fe_tone = lnb->hiBand ? SEC_TONE_ON : SEC_TONE_OFF;

	//	Setup voltage
	fe_sec_voltage_t fe_voltage = (cfg->polarization() == broadcast::sat::polarization::horiz) ? SEC_VOLTAGE_18 : SEC_VOLTAGE_13;

	LDEBUG( "dvb", "Setup Diseqc: diseqc=%d, polarization=%s, band=%s",
		cfg->diseqc(), cfg->polarization() == broadcast::sat::polarization::horiz ? "H" : "V", lnb->hiBand ? "Hi" : "Lo" );

	//	Switch off continuos tone
	int err = ioctl( _frontendFD, FE_SET_TONE, SEC_TONE_OFF );
	if (err < 0) {
		LERROR( "dvb", "DiSEqC FE_SET_TONE error: err=%s", strerror(errno) );
		return 0;
	}

	//	Configure LNB voltage
	err = ioctl( _frontendFD, FE_SET_VOLTAGE, fe_voltage );
	if (err < 0) {
		LERROR( "dvb", "DiSEqC FE_SET_VOLTAGE error: err=%s", strerror(errno) );
		return 0;
	}

	//	Wait for at least 15 ms. Currently 100 ms because of broken drivers.
	msleep( _diseqcTimeout );

	//	Check lnb output
	if (cfg->diseqc() < 5) {
		struct dvb_diseqc_master_cmd cmd = { { 0xe0, 0x10, 0x38, 0xf0, 0x00, 0x00 }, 4 };
        cmd.msg[3] = 0xf0 | ((cfg->diseqc() - 1) << 2) | (fe_voltage == SEC_VOLTAGE_13 ? 0 : 2) | (fe_tone == SEC_TONE_ON ? 1 : 0);
		err = ioctl( _frontendFD, FE_DISEQC_SEND_MASTER_CMD, &cmd );
		if (err < 0) {
			LERROR( "dvb", "DiSEqC FE_DISEQC_SEND_MASTER_CMD error: err=%s", strerror(errno) );
			return 0;
		}
	}
	else if (cfg->diseqc() == 0xA || cfg->diseqc() == 0xB) {	//	A or B simple diseqc ("diseqc-compatible")
		fe_sec_mini_cmd_t burst = (cfg->diseqc() == 0xB) ? SEC_MINI_B : SEC_MINI_A;
		err = ioctl( _frontendFD, FE_DISEQC_SEND_BURST, burst );
		if (err < 0) {
			LERROR( "dvb", "DiSEqC FE_DISEQC_SEND_BURST error: err=%s", strerror(errno) );
			return 0;
		}
	}
	else {
		LERROR( "dvb", "Switch port invalid" );
		return 0;
	}

	//	Wait for at least 15 ms. Currently 100 ms because of broken drivers.
	msleep( _diseqcTimeout );

	err = ioctl( _frontendFD, FE_SET_TONE, fe_tone );
	if (err < 0) {
		LERROR( "dvb", "DiSEqC FE_SET_TONE error: err=%s", strerror(errno) );
		return 0;
	}

	//	Set IF output (950~2150)
	return abs(cfg->frequency() - lnb->lo);
}

}
}
