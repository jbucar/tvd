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

#include "descriptor.h"

DemuxDescriptorTest::DemuxDescriptorTest() {}
DemuxDescriptorTest::~DemuxDescriptorTest() {}

void DemuxDescriptorTest::SetUp() {
	ASSERT_TRUE( desc.empty() );
}

void DemuxDescriptorTest::TearDown() {
}

void DemuxDescriptorTest::addData( util::BYTE *data, size_t len, const std::string &text, bool test_len/*=true*/ ) {
	desc.addDescriptor( data, len );
	if (test_len) {
		ASSERT_EQ( len, desc.length());
	}
	ASSERT_EQ( text, desc.show() );
}


//	Begin class descriptor tests
TEST( DescriptorTest, create ) {
	tuner::desc::Descriptors desc;
	ASSERT_EQ( desc.show(), "0" );
}

TEST( DescriptorTest, basic ) {
	util::BYTE data[] = { 0x00, 0x06, 0x0A, 0x04, 0x65, 0x6E, 0x67, 0x01 };
	size_t len = sizeof(data)/sizeof(util::BYTE);

	tuner::desc::Descriptors desc;
	ASSERT_TRUE( desc.empty() );

	ASSERT_EQ( desc.append( data, len ), len );
	ASSERT_EQ( desc.length(), len-2 );
	ASSERT_TRUE( !desc.empty() );
	{
		std::vector<util::BYTE> tags;
		ASSERT_EQ( desc.getTags( tags ), 1 );
		ASSERT_EQ( tags[0], 0x0A );
	}

	ASSERT_EQ( desc.show(), "1(0A)" );
	size_t ptr_off=0;
	util::BYTE *ptr = desc.get(0x0A,ptr_off);
	ASSERT_FALSE( ptr == NULL );
	ASSERT_TRUE( !memcmp(ptr, (data+2), desc.length() ) );
	ASSERT_TRUE( desc.find( 0x0A ) );
	ASSERT_FALSE( desc.find( 0x0F ) );
}

//	Descriptor test cases
TEST_F( DemuxDescriptorTest, parse_05 ) {
	util::BYTE data[] = { 0x05, 4, 0xAA, 0xBB, 0xCC, 0xDD };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(05)" );

	{	//	Parse descriptor
		tuner::desc::FormatIdentifier ident;
		ASSERT_TRUE( DESC_PARSE( desc, registration, ident ) );
		ASSERT_EQ( ident.format, 0xAABBCCDD );
		ASSERT_EQ( ident.info.length(), 0 );
	}
}

TEST_F( DemuxDescriptorTest, parse_05_2 ) {
	util::BYTE data[] = { 0x05, 8, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xEE, 0xEE, 0xEE };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(05)" );

	{	//	Parse descriptor
		tuner::desc::FormatIdentifier ident;
		ASSERT_TRUE( DESC_PARSE( desc, registration, ident ) );
		ASSERT_EQ( ident.format, 0xAABBCCDD );
		ASSERT_EQ( ident.info.length(), 4 );
	}
}

TEST_F( DemuxDescriptorTest, parse_0a ) {
	util::BYTE data[] = { 0x0A, 0x04, 0x65, 0x6E, 0x67, 0x01 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(0A)" );

	{	//	Parse descriptor
		tuner::desc::LanguageDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, language, d ) );
		ASSERT_EQ( d.language, "eng" );
		ASSERT_EQ( d.audioType, 0x01 );
	}
}

TEST_F( DemuxDescriptorTest, parse_0a_2 ) {
	util::BYTE data[] = { 0x0A, 0x04, 0x73, 0x70, 0x61, 0x03 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(0A)" );

	{	//	Parse descriptor
		tuner::desc::LanguageDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, language, d ) );
		ASSERT_EQ( d.language, "spa" );
		ASSERT_EQ( d.audioType, 0x03 );
	}
}

TEST_F( DemuxDescriptorTest, parse_14 ) {
	util::BYTE data[] = { 0x14, 0x0D, 0x00, 0x01, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(14)" );

	{	//	Parse descriptor
		tuner::desc::AssociationTagDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, association_tag, d ) );
		ASSERT_EQ( d.tag, 0x01 );
		ASSERT_EQ( d.use, 0x00 );
	}
}

TEST_F( DemuxDescriptorTest, parse_40 ) {
	util::BYTE data[] = { 0x40, 0x08, 0x4C, 0x49, 0x46, 0x49, 0x41, 0x54, 0x56, 0x44 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(40)" );

	{	//	Parse descriptor
		std::string name;
		ASSERT_TRUE( DESC_PARSE( desc, network_name, name ) );
		ASSERT_EQ( name, "LIFIATVD" );
	}
}

TEST_F( DemuxDescriptorTest, parse_41 ) {
	util::BYTE data[] = { 0x41, 0x0F, 0xF1, 0xC0, 0x01, 0xF1, 0xC1, 0x01, 0xF1, 0xC2, 0x01, 0xF1, 0xC3, 0x01, 0xF1, 0xD8, 0xC0 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(41)" );

	{	//	Parse descriptor
		tuner::desc::ServiceListDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, service_list, d ) );
		ASSERT_EQ( d.size(), 5 );
		tuner::ID srvID = 0xf1c0;
		for (size_t i=0; i<4; i++) {
			ASSERT_EQ( d[i].first, srvID++ );
			ASSERT_EQ( d[i].second, 0x01 );
		}
		ASSERT_EQ( d[4].first, 0xf1d8 );
		ASSERT_EQ( d[4].second, 0xc0 );
	}
}

TEST_F( DemuxDescriptorTest, parse_48 ) {
	util::BYTE data[] = { 0x48, 0x11, 0x01, 0x00, 0x0E, 0x4D, 0x6F, 0x64, 0x34, 0x20, 0x45, 0x6E, 0x63, 0x75, 0x65, 0x6E, 0x74, 0x72, 0x6F };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(48)" );

	{	//	Parse descriptor
		tuner::desc::ServiceDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, service, d ) );
		ASSERT_EQ( d.type, 0x01 );
		ASSERT_EQ( d.provider, "" );
		ASSERT_EQ( d.name, "Mod4 Encuentro" );
	}
}

TEST_F( DemuxDescriptorTest, parse_4D ) {
	util::BYTE data[] = { 0x4D, 0x22, 0x73, 0x70, 0x61, 0x08, 0x54, 0x56, 0x55, 0x20, 0x55, 0x4E, 0x4C, 0x50, 0x15, 0x54, 0x72, 0x61, 0x6E, 0x73, 0x6D, 0x69, 0x73, 0x69, 0xF3, 0x6E, 0x20, 0x64, 0x65, 0x20, 0x70, 0x72, 0x75, 0x65, 0x62, 0x61 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(4D)" );

	{	//	Parse descriptor
		tuner::desc::ShortEventDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, short_event, d ) );
		std::string tmp = d.language.code();
		ASSERT_EQ( tmp, "spa" );
		ASSERT_EQ( d.event, "TVU UNLP" );
		ASSERT_EQ( d.text, "Transmisi\xC3\xB3n de prueba" );
	}
}

TEST_F( DemuxDescriptorTest, parse_4E ) {
	util::BYTE data[] = { 0x4E, 0x1D, 0x00, 0x73, 0x70, 0x61, 0x00, 0x15, 0x54, 0x72, 0x61, 0x6E, 0x73, 0x6D, 0x69, 0x73, 0x69, 0xF3, 0x6E, 0x20, 0x64, 0x65, 0x20, 0x70, 0x72, 0x75, 0x65, 0x62, 0x61};
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(4E)" );

	{	//	Parse descriptor
		tuner::desc::ExtendedEventDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, extended_event, d ) );
		ASSERT_EQ(d.size(), 1);
		std::string tmp = d[0].language.code();
		ASSERT_EQ( tmp, "spa" );
		ASSERT_EQ( d[0].items.size(), 0);
		ASSERT_EQ( d[0].text, "Transmisi\xC3\xB3n de prueba" );
	}
}

TEST_F( DemuxDescriptorTest, parse_4E_2 ) {
	util::BYTE data[] = { 0x4E, 0x22, 0x00, 0x73, 0x70, 0x61, 0x07, 0x04, 0x49, 0x74, 0x65, 0x6d, 0x01, 0x49, 0x15, 0x54, 0x72, 0x61, 0x6E, 0x73, 0x6D, 0x69, 0x73, 0x69, 0xF3, 0x6E, 0x20, 0x64, 0x65, 0x20, 0x70, 0x72, 0x75, 0x65, 0x62, 0x61};
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(4E)" );

	{	//	Parse descriptor
		tuner::desc::ExtendedEventDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, extended_event, d ) );
		ASSERT_EQ(d.size(), 1);
		std::string tmp = d[0].language.code();
		ASSERT_EQ( tmp, "spa" );
		ASSERT_EQ( d[0].items.size(), 1);
		ASSERT_EQ( d[0].items[0].description, "Item");
		ASSERT_EQ( d[0].items[0].content, "I");
		ASSERT_EQ( d[0].text, "Transmisi\xC3\xB3n de prueba" );
	}
}

TEST_F( DemuxDescriptorTest, parse_4E_3 ) {
	util::BYTE data[] = { 0x4E, 0x29, 0x00, 0x73, 0x70, 0x61, 0x0E, 0x04, 0x49, 0x74, 0x65, 0x6d, 0x01, 0x49, 0x04, 0x4A, 0x74, 0x65, 0x6d, 0x01, 0x4A, 0x15, 0x54, 0x72, 0x61, 0x6E, 0x73, 0x6D, 0x69, 0x73, 0x69, 0xF3, 0x6E, 0x20, 0x64, 0x65, 0x20, 0x70, 0x72, 0x75, 0x65, 0x62, 0x61};
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(4E)" );

	{	//	Parse descriptor
		tuner::desc::ExtendedEventDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, extended_event, d ) );
		ASSERT_EQ(d.size(), 1);
		std::string tmp = d[0].language.code();
		ASSERT_EQ( tmp, "spa" );
		ASSERT_EQ( d[0].items.size(), 2);
		ASSERT_EQ( d[0].items[0].description, "Item");
		ASSERT_EQ( d[0].items[0].content, "I");
		ASSERT_EQ( d[0].items[1].description, "Jtem");
		ASSERT_EQ( d[0].items[1].content, "J");
		ASSERT_EQ( d[0].text, "Transmisi\xC3\xB3n de prueba" );
	}
}

TEST_F( DemuxDescriptorTest, parse_4E_4 ) {
	util::BYTE data[] = { 0x4E, 0x29, 0x01, 0x73, 0x70, 0x61, 0x0E, 0x04, 0x49, 0x74, 0x65, 0x6d, 0x01, 0x49, 0x04, 0x4A, 0x74, 0x65, 0x6d, 0x01, 0x4A, 0x15, 0x54, 0x72, 0x61, 0x6E, 0x73, 0x6D, 0x69, 0x73, 0x69, 0xF3, 0x6E, 0x20, 0x64, 0x65, 0x20, 0x70, 0x72, 0x75, 0x65, 0x62, 0x61};
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(4E)");

	util::BYTE data2[] = { 0x4E, 0x29, 0x10, 0x73, 0x70, 0x61, 0x0E, 0x04, 0x49, 0x74, 0x65, 0x6d, 0x01, 0x49, 0x04, 0x4A, 0x74, 0x65, 0x6d, 0x01, 0x4A, 0x15, 0x54, 0x72, 0x61, 0x6E, 0x73, 0x6D, 0x69, 0x73, 0x69, 0xF3, 0x6E, 0x20, 0x64, 0x65, 0x20, 0x70, 0x72, 0x75, 0x65, 0x62, 0x61};
	len = sizeof(data2)/sizeof(util::BYTE);
	addData( data2, len, "2(4E,4E)", false);

	{	//	Parse descriptor
		tuner::desc::ExtendedEventDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, extended_event, d ) );
		ASSERT_EQ(d.size(), 2);
		std::string tmp = d[0].language.code();
		ASSERT_EQ( tmp, "spa" );
		ASSERT_EQ( d[0].items.size(), 2);
		ASSERT_EQ( d[0].items[0].description, "Item");
		ASSERT_EQ( d[0].items[0].content, "I");
		ASSERT_EQ( d[0].items[1].description, "Jtem");
		ASSERT_EQ( d[0].items[1].content, "J");
		ASSERT_EQ( d[0].text, "Transmisi\xC3\xB3n de prueba" );

		tmp = d[1].language.code();
		ASSERT_EQ( tmp, "spa" );
		ASSERT_EQ( d[1].items.size(), 2);
		ASSERT_EQ( d[1].items[0].description, "Item");
		ASSERT_EQ( d[1].items[0].content, "I");
		ASSERT_EQ( d[1].items[1].description, "Jtem");
		ASSERT_EQ( d[1].items[1].content, "J");
		ASSERT_EQ( d[1].text, "Transmisi\xC3\xB3n de prueba" );
	}
}


TEST_F( DemuxDescriptorTest, parse_52 ) {
	util::BYTE data[] = { 0x52, 0x1, 0xAC };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(52)" );

	{	//	Parse descriptor
		util::BYTE tag;
		ASSERT_TRUE( DESC_PARSE( desc, stream_identifier, tag ) );
		ASSERT_EQ( tag, 0xAC );
	}
}

TEST_F( DemuxDescriptorTest, parse_54 ) {
	util::BYTE data[] = { 0x54, 2, 0xAC, 0xCA };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(54)" );

	{	//	Parse descriptor
		tuner::desc::ContentDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, content, d ) );
		ASSERT_EQ( d.size(), 1 );
		ASSERT_EQ( d[0], 0xACCA );
	}
}

TEST_F( DemuxDescriptorTest, parse_54_2 ) {
	util::BYTE data[] = { 0x54, 4, 0xAC, 0xCA, 0xBB, 0xAA };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(54)" );

	{	//	Parse descriptor
		tuner::desc::ContentDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, content, d ) );
		ASSERT_EQ( d.size(), 2 );
		ASSERT_EQ( d[0], 0xACCA );
		ASSERT_EQ( d[1], 0xBBAA );
	}
}

TEST_F( DemuxDescriptorTest, parse_55 ) {
	util::BYTE data[] = { 0x55, 4, 'A', 'R', 'G', 0x01 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(55)" );

	{	//	Parse descriptor
		tuner::desc::ParentalRatingDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, parental_rating, d ) );
		ASSERT_EQ( d.size(), 1 );
		ASSERT_EQ( d[0].language, "ARG" );
		ASSERT_EQ( d[0].rating, 0x01 );
	}
}

TEST_F( DemuxDescriptorTest, parse_55_2 ) {
	util::BYTE data[] = { 0x55, 8, 'A', 'R', 'G', 0x01, 'B', 'R', 'A', 0x02 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(55)" );

	{	//	Parse descriptor
		tuner::desc::ParentalRatingDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, parental_rating, d ) );
		ASSERT_EQ( d.size(), 2 );
		ASSERT_EQ( d[0].language, "ARG" );
		ASSERT_EQ( d[0].rating, 0x01 );
		ASSERT_EQ( d[1].language, "BRA" );
		ASSERT_EQ( d[1].rating, 0x02 );
	}
}

TEST_F( DemuxDescriptorTest, parse_58 ) {
	util::BYTE data[] = { 0x58, 0x0D, 0x41, 0x52, 0x47, 0x02, 0x00, 0x00, 0xF9, 0xDB, 0x00, 0x00, 0x00, 0x00, 0x00 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(58)" );

	{	//	Parse descriptor
		tuner::desc::LocalTimeOffsetDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, local_time_offset, d ) );
		ASSERT_EQ( d.size(), 1 );
		ASSERT_EQ( d[0].countryCode, "ARG" );
		ASSERT_EQ( d[0].countryRegion, 0x00 );
		ASSERT_EQ( d[0].localTimeOffsetPolarity, false );
		ASSERT_EQ( d[0].localTimeOffset, 0x0000 );
		ASSERT_EQ( d[0].changingDate, 0x00F9DB );
		ASSERT_EQ( d[0].timeOfChange, 0x0000 );
		ASSERT_EQ( d[0].nextTimeOffset, 0x0000 );
	}
}

TEST_F( DemuxDescriptorTest, parse_cf_type_03 ) {
	util::BYTE data[] = { 0xCF, 0x11, 0x03, 0x0E, 0x45, 0x6E, 0x63, 0x75, 0x65, 0x6E, 0x74, 0x72, 0x6F, 0x20, 0x4D, 0x6F, 0x76, 0x69, 0x6C };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(CF)" );

	{	//	Parse descriptor
		tuner::desc::LogoTransmissionDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, logo_transmission, d ) );
		ASSERT_EQ( d.type, 0x03 );
		const tuner::desc::LogoTransmissionType3 &dLogo = d.data.get<tuner::desc::LogoTransmissionType3>();
		ASSERT_EQ( dLogo, "Encuentro Movil" );
	}
}

TEST_F( DemuxDescriptorTest, parse_cf_type_01 ) {
	util::BYTE data[] = { 0xCF, 0x07, 0x01, 0xFE, 0x01, 0xF0, 0x01, 0x00, 0x01 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(CF)" );

	{	//	Parse descriptor
		tuner::desc::LogoTransmissionDescriptor d;
		ASSERT_TRUE( DESC_PARSE( desc, logo_transmission, d ) );
		ASSERT_EQ( d.type, 0x01 );
		const tuner::desc::LogoTransmissionType1 &dLogo = d.data.get<tuner::desc::LogoTransmissionType1>();
		ASSERT_EQ( dLogo.id, (0xFE01 & 0x01FF) );
		ASSERT_EQ( dLogo.version, (0xF001 & 0x0FFF) );
		ASSERT_EQ( dLogo.downloadID, 0x0001 );
	}
}

TEST_F( DemuxDescriptorTest, parse_fa ) {
	util::BYTE data[] = { 0xFA, 0x04, 0x53, 0xD6, 0x0E, 0x94 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(FA)" );

	{	//	Parse FA descriptor
		tuner::desc::TerrestrialDeliverySystemDescriptor terrestrial_descriptor;
		ASSERT_TRUE( DESC_PARSE( desc, terrestrial_delivery_system, terrestrial_descriptor ) );

		ASSERT_TRUE( terrestrial_descriptor.area == 1341 );
		ASSERT_TRUE( terrestrial_descriptor.interval == 0x01 );
		ASSERT_TRUE( terrestrial_descriptor.mode == 0x02 );

		ASSERT_TRUE( terrestrial_descriptor.frequencies.size() == 1 );
		ASSERT_TRUE( terrestrial_descriptor.frequencies[0] == 0xe94 );
	}
}

TEST_F( DemuxDescriptorTest, parse_fc_00 ) {
	util::BYTE data[] = { 0xFC, 0x04, 0xF1, 0xC0, 0xBF, 0x00 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(FC)" );

	{	//	Parse FC descriptor
		tuner::desc::EmergencyInformationDescriptor emergency_information_desc;
		ASSERT_TRUE( DESC_PARSE( desc, emergency_information, emergency_information_desc ) );
		ASSERT_EQ( emergency_information_desc.size(), 1 );
		const tuner::desc::ServiceEmergencyInformation &info = emergency_information_desc[0];
		ASSERT_EQ( info.serviceID, 0xF1C0 );
		ASSERT_EQ( info.startEndFlag, 1 );
		ASSERT_EQ( info.signalLevel, 0 );
		ASSERT_EQ( info.areaCodes.size(), 0 );
	}
}

TEST_F( DemuxDescriptorTest, parse_fc_multiple ) {
	util::BYTE data[] = { 0xFC, 0x08, 0xF1, 0xC0, 0xBF, 0x00, 0xF1, 0xC0, 0xBF, 0x00 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(FC)" );

	{	//	Parse FC descriptor
		tuner::desc::EmergencyInformationDescriptor emergency_information_desc;
		ASSERT_TRUE( DESC_PARSE( desc, emergency_information, emergency_information_desc ) );
		ASSERT_EQ( emergency_information_desc.size(), 2 );
		{
			const tuner::desc::ServiceEmergencyInformation &info = emergency_information_desc[0];
			ASSERT_EQ( info.serviceID, 0xF1C0 );
			ASSERT_EQ( info.startEndFlag, 1 );
			ASSERT_EQ( info.signalLevel, 0 );
			ASSERT_EQ( info.areaCodes.size(), 0 );
		}
		{
			const tuner::desc::ServiceEmergencyInformation &info = emergency_information_desc[1];
			ASSERT_EQ( info.serviceID, 0xF1C0 );
			ASSERT_EQ( info.startEndFlag, 1 );
			ASSERT_EQ( info.signalLevel, 0 );
			ASSERT_EQ( info.areaCodes.size(), 0 );
		}
	}
}

TEST_F( DemuxDescriptorTest, parse_fc_01 ) {
	util::BYTE data[] = { 0xFC, 0x06, 0xF1, 0xC0, 0xFF, 0x02, 0x0E, 0x7F };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(FC)" );

	{	//	Parse FC descriptor
		tuner::desc::EmergencyInformationDescriptor emergency_information_desc;
		ASSERT_TRUE( DESC_PARSE( desc, emergency_information, emergency_information_desc ) );
		ASSERT_EQ( emergency_information_desc.size(), 1 );
		const tuner::desc::ServiceEmergencyInformation &info = emergency_information_desc[0];
		ASSERT_EQ( info.serviceID, 0xF1C0 );
		ASSERT_EQ( info.startEndFlag, 1 );
		ASSERT_EQ( info.signalLevel, 1 );
		ASSERT_EQ( info.areaCodes.size(), 1 );
		ASSERT_EQ( info.areaCodes[0], 231 );
	}
}

TEST_F( DemuxDescriptorTest, parse_fc_02 ) {
	util::BYTE data[] = { 0xFC, 0x08, 0xF1, 0xC0, 0x7F, 0x04, 0x30, 0x9F, 0x0E, 0x7F };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(FC)" );

	{	//	Parse FC descriptor
		tuner::desc::EmergencyInformationDescriptor emergency_information_desc;
		ASSERT_TRUE( DESC_PARSE( desc, emergency_information, emergency_information_desc ) );
		ASSERT_EQ( emergency_information_desc.size(), 1 );
		const tuner::desc::ServiceEmergencyInformation &info = emergency_information_desc[0];
		ASSERT_EQ( info.serviceID, 0xF1C0 );
		ASSERT_EQ( info.startEndFlag, 0 );
		ASSERT_EQ( info.signalLevel, 1 );
		ASSERT_EQ( info.areaCodes.size(), 2 );
		ASSERT_EQ( info.areaCodes[0], 777 );
		ASSERT_EQ( info.areaCodes[1], 231 );
	}
}

TEST_F( DemuxDescriptorTest, parse_fd ) {
	util::BYTE data[] = { 0xFD, 0x05, 0x00, 0xA3, 0x00, 0x09, 0xE0 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(FD)" );

	{	//	Parse FD descriptor
		tuner::desc::DataComponentDescriptor sys_desc;
		ASSERT_TRUE( DESC_PARSE( desc, data_component, sys_desc ) );
		ASSERT_TRUE( sys_desc.codingMethodID == 0xA3 );
		ASSERT_TRUE( sys_desc.info.length() == 3 );
	}
}

TEST_F( DemuxDescriptorTest, parse_fe ) {
	util::BYTE data[] = { 0xFE, 0x02, 0x03, 0x01 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(FE)" );

	{	//	Parse FE descriptor
		tuner::desc::SystemManagementDescriptor sys_desc;
		ASSERT_TRUE( DESC_PARSE( desc, system_management, sys_desc ) );
		ASSERT_TRUE( sys_desc.systemID == 0x301 );
		ASSERT_TRUE( sys_desc.info.length() == 0 );
	}
}

TEST_F( DemuxDescriptorTest, parse_fe_with_info ) {
	util::BYTE data[] = { 0xFE, 0x04, 0x03, 0x01, 0x2, 0x2 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(FE)" );

	{	//	Parse FE descriptor
		tuner::desc::SystemManagementDescriptor sys_desc;
		ASSERT_TRUE( DESC_PARSE( desc, system_management, sys_desc ) );
		ASSERT_TRUE( sys_desc.systemID == 0x301 );
		ASSERT_TRUE( sys_desc.info.length() == 2 );
	}
}

TEST_F( DemuxDescriptorTest, parse_cd ) {
	util::BYTE data[] = { 0xCD, 0x0E, 0x11, 0x02, 0xAF, 0x01, 0xF1, 0xD8, 0x0F, 0x03, 0xF1, 0xC0, 0xF1, 0xC1, 0xF1, 0xC2 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(CD)" );

	{	//	Parse FD descriptor
		tuner::desc::TransportStreamInformationDescriptor sys_desc;
		ASSERT_TRUE( DESC_PARSE( desc, ts_information, sys_desc ) );
		ASSERT_EQ( sys_desc.remoteControlKeyID, 0x11 );
		ASSERT_EQ( sys_desc.name, "" );
		ASSERT_EQ( sys_desc.transmissions.size(), 2 );
		ASSERT_EQ( sys_desc.transmissions[0].type, 0xAF );
		ASSERT_EQ( sys_desc.transmissions[0].services.size(), 1 );
		ASSERT_EQ( sys_desc.transmissions[0].services[0], 0xF1D8 );

		ASSERT_EQ( sys_desc.transmissions[1].type, 0x0F );
		ASSERT_EQ( sys_desc.transmissions[1].services.size(), 3 );
		ASSERT_EQ( sys_desc.transmissions[1].services[0], 0xF1C0 );
		ASSERT_EQ( sys_desc.transmissions[1].services[1], 0xF1C1 );
		ASSERT_EQ( sys_desc.transmissions[1].services[2], 0xF1C2 );
	}
}

TEST_F( DemuxDescriptorTest, parse_c4 ) {
	util::BYTE data[] = { 0xC4, 0x36, 0xF6, 0x21, 0x23, 0x21, 0xFF, 0xD7, 0x73, 0x70, 0x61, 0x70, 0x6F, 0x72, 0x44, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x63, 0x69, 0xF3, 0x6E, 0x20, 0x64, 0x65, 0x6C, 0x20, 0x61, 0x75, 0x64, 0x69, 0x6F, 0x20, 0x64, 0x65, 0x20, 0x4D, 0x65, 0x6E, 0x74, 0x65, 0x73, 0x20, 0x63, 0x72, 0x69, 0x6D, 0x69, 0x6E, 0x61, 0x6C, 0x65, 0x73 };
	size_t len = sizeof(data)/sizeof(util::BYTE);
	addData( data, len, "1(C4)" );

	{	//	Parse FE descriptor
		tuner::desc::AudioComponentDescriptor sys_desc;
		ASSERT_TRUE( DESC_PARSE( desc, audio_component, sys_desc ) );
		ASSERT_EQ( sys_desc.size(), 1 );

		const tuner::desc::AudioComponent &ac = sys_desc[0];
		ASSERT_EQ( ac.streamContent, 0x6 );
		ASSERT_EQ( ac.componentType, 0x21 );
		ASSERT_EQ( ac.componentTag, 0x23 );
		ASSERT_EQ( ac.streamType, 0x21 );
		ASSERT_EQ( ac.simulcastGroupTag, 0xFF );
		ASSERT_EQ( ac.languages.size(), 2 );
		ASSERT_EQ( ac.text, "Descripci\xF3n del audio de Mentes criminales" );
	}
}

