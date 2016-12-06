/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-pvr implementation.

    DTV-pvr is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-pvr is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-pvr.

    DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "../../../src/dvb/control.h"
#include <gtest/gtest.h>

TEST( DVBControl, constructor_with_none_content_and_none_age ) {
	pvr::dvb::Control *control = new pvr::dvb::Control(tuner::desc::parental::content::none, tuner::desc::parental::age::none);
	ASSERT_FALSE( control->hasSex() );
	ASSERT_FALSE( control->hasViolence() );
	ASSERT_FALSE( control->hasDrugs() );
	ASSERT_EQ( 0, control->age() );
}

TEST( DVBControl, constructor_with_sex_content_and_none_age ) {
	pvr::dvb::Control *control = new pvr::dvb::Control(tuner::desc::parental::content::sex, tuner::desc::parental::age::none);
	ASSERT_TRUE( control->hasSex() );
	ASSERT_FALSE( control->hasViolence() );
	ASSERT_FALSE( control->hasDrugs() );
}

TEST( DVBControl, constructor_with_violence_content_and_none_age ) {
	pvr::dvb::Control *control = new pvr::dvb::Control(tuner::desc::parental::content::violence, tuner::desc::parental::age::none);
	ASSERT_FALSE( control->hasSex() );
	ASSERT_TRUE( control->hasViolence() );
	ASSERT_FALSE( control->hasDrugs() );
}

TEST( DVBControl, constructor_with_drugs_content_and_none_age ) {
	pvr::dvb::Control *control = new pvr::dvb::Control(tuner::desc::parental::content::drugs, tuner::desc::parental::age::none);
	ASSERT_FALSE( control->hasSex() );
	ASSERT_FALSE( control->hasViolence() );
	ASSERT_TRUE( control->hasDrugs() );
}

TEST( DVBControl, constructor_with_none_content_and_age_year10 ) {
	pvr::dvb::Control *control = new pvr::dvb::Control(tuner::desc::parental::content::none, tuner::desc::parental::age::year10);
	ASSERT_EQ( 10, control->age() );
}

TEST( DVBControl, constructor_with_none_content_and_age_year12 ) {
	pvr::dvb::Control *control = new pvr::dvb::Control(tuner::desc::parental::content::none, tuner::desc::parental::age::year12);
	ASSERT_EQ( 12, control->age() );
}

TEST( DVBControl, constructor_with_none_content_and_age_year14 ) {
	pvr::dvb::Control *control = new pvr::dvb::Control(tuner::desc::parental::content::none, tuner::desc::parental::age::year14);
	ASSERT_EQ( 14, control->age() );
}

TEST( DVBControl, constructor_with_none_content_and_age_year16 ) {
	pvr::dvb::Control *control = new pvr::dvb::Control(tuner::desc::parental::content::none, tuner::desc::parental::age::year16);
	ASSERT_EQ( 16, control->age() );
}

TEST( DVBControl, constructor_with_none_content_and_age_year18 ) {
	pvr::dvb::Control *control = new pvr::dvb::Control(tuner::desc::parental::content::none, tuner::desc::parental::age::year18);
	ASSERT_EQ( 18, control->age() );
}

TEST( DVBControl, constructor_with_all_content_and_age ) {
	util::BYTE content = tuner::desc::parental::content::violence | tuner::desc::parental::content::sex | tuner::desc::parental::content::drugs;
	pvr::dvb::Control *control = new pvr::dvb::Control(content, tuner::desc::parental::age::year10);
	ASSERT_TRUE( control->hasSex() );
	ASSERT_TRUE( control->hasViolence() );
	ASSERT_TRUE( control->hasDrugs() );
	ASSERT_EQ( 10, control->age() );
}

static tuner::desc::Descriptors descs( util::BYTE rating ) {
	util::BYTE data[] = { 0x55, 4, 'A', 'R', 'G', rating};
	size_t len = sizeof(data)/sizeof(util::BYTE);
	tuner::desc::Descriptors descs;
	descs.addDescriptor( data, len );
	return descs;
}

TEST( DVBControl, parse_with_age_and_no_content ) {
	pvr::dvb::Control *control = pvr::dvb::Control::parse(descs(0x05)); // 16 years

	ASSERT_TRUE( control != NULL );
	ASSERT_FALSE( control->hasSex() );
	ASSERT_FALSE( control->hasViolence() );
	ASSERT_FALSE( control->hasDrugs() );
	ASSERT_EQ( 16, control->age() );
}

TEST( DVBControl, parse_with_age_and_content ) {
	pvr::dvb::Control *control = pvr::dvb::Control::parse(descs(0x75)); // 16 years with scenes of sex, violence and drugs

	ASSERT_TRUE( control != NULL );
	ASSERT_TRUE( control->hasSex() );
	ASSERT_TRUE( control->hasViolence() );
	ASSERT_TRUE( control->hasDrugs() );
	ASSERT_EQ( 16, control->age() );
}
TEST( DVBControl, parse_with_no_content_and_no_age ) {
	tuner::desc::Descriptors d = descs(0x01); // ATP
	ASSERT_TRUE( pvr::dvb::Control::parse(d) == NULL );
}
