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

#include "frontend.h"
#include <util/log.h>

extern "C" {
#include "stapp_main.h"
}

namespace st {

#ifdef ST_7108
static bool hack() {
	bool result=true;
#define I2C_BUS_WR_TIMEOUT 1000 /* Write access timeout in ms units */
	//	I2c_write 0 34 0F 0 (port=0,addr=34,token=0F
	ST_ErrorCode_t     ErrCode=ST_NO_ERROR;
	S32                SlaveAddress=0x34,I2CPort=0,NumToWrite;
	U8                 BufferWrite[64];
	U32                NumWritten;
	STI2C_OpenParams_t I2C_OpenParams;
	STI2C_Handle_t     I2C_Handle;

	NumToWrite = 0;
	BufferWrite[NumToWrite++] = 0x0F;
	BufferWrite[NumToWrite++] = 0x00;

	//	Now perform the write transaction on
	I2C_OpenParams.BusAccessTimeOut = 100000;
	I2C_OpenParams.AddressType      = STI2C_ADDRESS_7_BITS;
	I2C_OpenParams.I2cAddress       = (U16)SlaveAddress;
	I2C_OpenParams.BaudRate         = STI2C_RATE_NORMAL;
	ErrCode=STI2C_Open(I2C_DeviceName[I2CPort],&I2C_OpenParams,&I2C_Handle);
	if (ErrCode!=ST_NO_ERROR) {
		LERROR( "st", "Unable to open i2c handle %d",I2CPort);
		return false;
	}

	ErrCode=STI2C_Write(I2C_Handle,BufferWrite,NumToWrite,I2C_BUS_WR_TIMEOUT,&NumWritten);
	if (ErrCode!=ST_NO_ERROR || (S32)NumWritten != NumToWrite) {
		LERROR( "st", "Unable to write on i2c!" );
		result=false;
	}

	ErrCode=STI2C_Close(I2C_Handle);
	if (ErrCode!=ST_NO_ERROR) {
		LERROR( "st", "Unable to close i2c handle!" );
		result=false;
	}
	return result;
}
#endif

static S32 findFrontendID( void ) {
	S32 id=0;

	while (id < TUNER_MAX_NUMBER) {
		if (TUNER_Handle[id] != 0) {
			return id;
		}
		id++;
	}
	throw;
	return id;
}

Frontend::Frontend()
{
	_tunerID = -1;
}

Frontend::~Frontend()
{
}

//	Initialization
bool Frontend::initialize() {
	_tunerID = findFrontendID();
	LINFO("st", "Using tunerID=%d", _tunerID);

#ifdef ST_7108
	if (!hack()) {
		return false;
	}
#endif

	return _tunerID >= 0;
}

//	Status/signal
bool Frontend::getStatus( tuner::status::Type &st ) const {
	//	Get tuner status
	TUNER_Status_t TUNER_Status;
	ST_ErrorCode_t ErrCode=TUNER_GetStatus(_tunerID,&TUNER_Status);
	if (ErrCode!=ST_NO_ERROR) {
		LWARN("st", "Unable to get tuner status");
		return false;
	}

	LTRACE("st", "status: frequency=%d, modulation=%d, fecRate=%d, state=%d, signal=%d",
		TUNER_Status.TUNER_FrequencyKhz, TUNER_Status.TUNER_Modulation, TUNER_Status.TUNER_FecRate, TUNER_Status.TUNER_State, TUNER_Status.TUNER_SignalQuality );

	st.isLocked = (TUNER_Status.TUNER_State == TUNER_STATE_LOCKED);
	if (st.isLocked) {
		st.signal = (TUNER_Status.TUNER_SignalQuality*100)/0xFFFFFFFF;
		st.snr =  (TUNER_Status.TUNER_BitErrorRate*100)/0xFFFFFFFF;
	}

	return true;
}

bool Frontend::start( tuner::broadcast::IsdbtConfig *ch ) {
	ST_ErrorCode_t ErrCode=ST_NO_ERROR;
	U32            Polarization,SymbolRate,FECRates,FECType,PilotOn,RollOff,PLSIndex,Modulation;
#ifndef ST_7109
	U32            RFSource_Id=0;
#endif

	//	Setup
    SymbolRate   = 0;
    Modulation   = TUNER_MOD_64QAM;
    FECRates     = TUNER_FEC_3_4;
    Polarization = TUNER_PLR_HORIZONTAL;
    FECType      = TUNER_FEC_TYPE_AUTO_SEARCH;
    PilotOn      = 0;
    RollOff      = 0;
    PLSIndex     = 0;

	//	Call Set Frequency
#ifdef ST_7109
    ErrCode=TUNER_SetFrequency(_tunerID,ch->frequency(),SymbolRate,Modulation,FECRates,Polarization,FECType,PilotOn,RollOff,PLSIndex);
#else
    ErrCode=TUNER_SetFrequency(_tunerID,ch->frequency(),SymbolRate,Modulation,FECRates,Polarization,FECType,PilotOn,RollOff,PLSIndex,RFSource_Id);
#endif
	if (ErrCode!=ST_NO_ERROR) {
		LWARN("st", "Lock failed: err=%08x", ErrCode );
		return false;
	}

	return true;
}

}

