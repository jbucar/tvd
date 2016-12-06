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

#include "generic.h"
#include "demuxers.h"
#include "../ts.h"
#include "../text.h"
#include <util/log.h>
#include <util/assert.h>

using namespace util;

namespace tuner {
namespace desc {
namespace demuxer {

//	0x05: Registration descriptor ISO 13818-1, 2.6.8
bool fnc_registration( FormatIdentifier &desc, BYTE *descPayload ) {
	size_t offset = DESC_HEADER;
	DTV_ASSERT( DESC_TAG(descPayload) == ident::registration );
	desc.format = RDW(descPayload,offset);
	desc.info.copy( (descPayload+offset), DESC_SIZE(descPayload)-offset );
	LTRACE("desc", "Registration: FormatIdentifier=%08x, info=%d", desc.format, desc.info.length() );
	return true;
}

//	0x09: Conditional access descriptor ETSI EN 300 468, 6.2.5
bool fnc_conditional_access( ca::Descriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::conditional_access );
	size_t offset = DESC_HEADER;
	ca::parse( desc, descPayload+offset, DESC_SIZE(descPayload)-offset );
	return true;
}

//	0x0A: Language descriptor ISO 13818-1, 2.6.18
bool fnc_language( LanguageDescriptor &desc, BYTE *descPayload ) {
	size_t offset = DESC_HEADER;
	DTV_ASSERT( DESC_TAG(descPayload) == ident::language );
	offset += parseLanguage(desc.language,descPayload+offset);
	desc.audioType = RB(descPayload,offset);
	LTRACE("desc", "Language: ISO 639 language=%s, audio type=%d", desc.language.code(), desc.audioType );
	return true;
}

//	0x10: Application storage descriptor ESTI TS 102 809, 5.3.10
bool fnc_application_storage( ApplicationStorageDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::application_storage );
	size_t offset = DESC_HEADER;
	desc.storageProperty = RB(descPayload,offset);
	desc.flags = RB(descPayload,offset);
	desc.flags =  desc.flags & 0xe0;
	desc.version = RDW(descPayload,offset);
	desc.priority = RB(descPayload,offset);
	LTRACE("desc", "Application Storage: storage_property=%d flags=%d version=%d priority=%d", desc.storageProperty, desc.flags, desc.version, desc.priority);
	return true;
}

//	0x13: Carousel Identifier descriptor ESTI TS 102 809, B.2.8.1
bool fnc_carousel_id( CarouselIdentifierDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::carousel_id );
	size_t offset = DESC_HEADER;
	desc.carouselID = RDW(descPayload,offset);
	desc.formatID = RB(descPayload,offset);
	size_t len = DESC_SIZE(descPayload);
	if (len > offset) {
		desc.data.copy( (descPayload+offset), len-offset );
	}
	LTRACE("desc", "Carousel Identifier: carouselID=%08x, formatID=%02x", desc.carouselID, desc.formatID );
	return true;
}

//	0x14: Association tag descriptor ETSI TR 101 202, 4.7.7.2	//	TODO: review!
bool fnc_association_tag( AssociationTagDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::association_tag );
    size_t offset = DESC_HEADER;
    desc.tag = RW(descPayload,offset);
    desc.use = RW(descPayload,offset);
	//BYTE selectorLen = RB(descPayload,offset);
	//      Documented in TR101202
	//      0x0000           DSI with IOR of SGW
	//      0x0100 - 0x1FFF  DVB reserved
	//      0x2000 - 0xFFFF  User private
	// switch(_use) {
	//      case 0:
	//              DWORD transactionID = RDW(data,offset);
	//              DWORD timeout = RDW(data,offset);
	//              break;
	//      case 1:
	//              break;
	//      case default:
	//              offset += selectorLen;
	//              break;
	// }
	LTRACE("desc", "Association Tag: tag=%02x, use=%02x", desc.tag, desc.use);
    return true;
}

//	0x40: Network name descriptor ETSI EN 300 468, 6.2.27
bool fnc_network_name( std::string &name, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::network_name );
	size_t offset = DESC_HEADER;
	name = std::string( (char *)(descPayload+offset), DESC_SIZE(descPayload)-offset );
	LTRACE("desc", "Network Name: network=%s", name.c_str());
	return true;
}

//	0x41: Service list descriptor
bool fnc_service_list( ServiceListDescriptor &services, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::service_list );
	size_t offset = DESC_HEADER;
	while (offset < DESC_SIZE(descPayload)) {
		WORD serviceID   = RW(descPayload,offset);
		BYTE serviceType = RB(descPayload,offset);
		services.push_back( std::make_pair(serviceID, serviceType ) );
		LTRACE("desc", "Service List: ID=%04x, type=%02x", serviceID, serviceType);
	}
	return true;
}

//	0x48: Service descriptor, ETSI EN 300 468, 6.2.33
bool fnc_service( ServiceDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::service );
	size_t offset = DESC_HEADER;
	desc.type = RB(descPayload,offset);
	offset += parseText( desc.provider, descPayload+offset );
	parseText( desc.name, descPayload+offset );
	LTRACE("desc", "Service: type=%02x, provider=%s, name=%s", desc.type, desc.provider.c_str(), desc.name.c_str());
	return true;
}

//	0x4A: Linkage descriptor ETSI EN 300 468 V1.11.1 (2010-04)
bool fnc_linkage( LinkageDescriptors &descs, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::linkage );
	LinkageDescriptor desc;
	size_t offset = DESC_HEADER;
	desc.tsID = RW(descPayload,offset);
	desc.nitID = RW(descPayload,offset);
	desc.srvID = RW(descPayload,offset);
	desc.type = RB(descPayload,offset);
	desc.data.copy( (descPayload+offset), DESC_SIZE(descPayload)-offset );
	descs.push_back( desc );
	LTRACE("desc", "Linkage: tsID=%04x, nitID=%04x, srvID=%04x, type=%02x, data=%d",
		desc.tsID, desc.nitID, desc.srvID, desc.type, desc.data.length() );
	return true;
}

//	0x4D: Short event descriptor ETSI EN 300 468, 6.2.37
bool fnc_short_event( ShortEventDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::short_event );
	size_t offset = DESC_HEADER;
	offset += parseLanguage(desc.language,descPayload+offset);
	offset += parseText( desc.event, descPayload+offset );
	parseText( desc.text, descPayload+offset );
	LTRACE("desc", "Short Event: language=%s, event=%s, text=%s",
		desc.language.code(), desc.event.c_str(), desc.text.c_str());
	return true;
}

//	0x4E: Extended event descriptor ETSI EN 300 468, 6.2.15
bool fnc_extended_event( ExtendedEventDescriptor &descs, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::extended_event );
	size_t offset = DESC_HEADER;
	ExtendedEvent desc;

	BYTE number = RB(descPayload,offset);
	BYTE desc_number = (number & 0xF0) >> 4;
	BYTE desc_lastNumber = (number & 0x0F);

	offset += parseLanguage( desc.language, descPayload+offset );

	BYTE lenItems = RB(descPayload,offset);
	size_t last = offset+lenItems;
	while (offset < last) {
		ExtendedEventItem item;
		offset += parseText( item.description, descPayload+offset );
		offset += parseText( item.content, descPayload+offset );
		desc.items.push_back(item);
	}
	parseText( desc.text, descPayload+offset );

	if (desc_number == 0) {
		descs.resize(desc_lastNumber+1);
		descs[0] = desc;
	} else {
		descs[desc_number] = desc;
	}

	LTRACE("desc", "Extended event: languaje=%s text=%s nitems=%d",
		desc.language.code(), desc.text.c_str(), desc.items.size());

	return true;
}

//	0x50: Component descriptor ETSI EN 300 468, 6.2.8
bool fnc_component( ComponentDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::component );
	size_t offset = DESC_HEADER;
	desc.streamContent  = RB(descPayload,offset);
	desc.streamContent &= 0x0F;
	desc.componentType = RB(descPayload,offset);
	desc.componentTag  = RB(descPayload,offset);
	offset += parseLanguage( desc.language, descPayload+offset );
	parseText( desc.text, descPayload+offset, DESC_SIZE(descPayload)-offset );

	LTRACE("desc", "Component Descriptor: stream=%02x, type=%02x, tag=%02x, lang=%s, text=%s",
		desc.streamContent, desc.componentType, desc.componentTag, desc.language.code(), desc.text.c_str());

	return true;
}

//	0x52: Stream identifier ETSI EN 300 468, 6.2.39
bool fnc_stream_identifier( BYTE &tag, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::stream_identifier );
	size_t offset = DESC_HEADER;
	tag = RB(descPayload,offset);
	LTRACE("desc", "Stream Identifier: tag=%02x", tag);
	return true;
}

//	0x54: Content descriptor ETSI EN 300 468, 6.2.9
bool fnc_content( ContentDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::content );
	size_t offset = DESC_HEADER;
	size_t len = DESC_SIZE(descPayload);
	util::WORD con;

    while (offset < len && (len-offset) >= 2) {
        con = RW(descPayload,offset);
        desc.push_back( con );
    }

    LTRACE("desc", "Content");
    BOOST_FOREACH( util::WORD elem, desc ) {
	    LTRACE("desc", "\tcontent=%04x", elem);
    }
	return true;
}

//	0x55: Parental rating descriptor ETSI EN 300 468, 6.2.28
bool fnc_parental_rating( ParentalRatingDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::parental_rating );

	size_t offset = DESC_HEADER;
	while (offset < DESC_SIZE(descPayload)) {
		ParentalRating elem;
		offset += parseLanguage(elem.language,descPayload+offset);
		elem.rating = RB(descPayload,offset);
		desc.push_back( elem );
	}

	LTRACE("desc", "Parental Rating");
	BOOST_FOREACH( const ParentalRating &elem, desc ) {
		LTRACE("desc", "\tlanguage=%s, rating=%02x", elem.language.code(), elem.rating);
	}
	return true;
}

//	0x58: Local time offset ETSI EN 300 468, 6.2.20
bool fnc_local_time_offset( LocalTimeOffsetDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::local_time_offset );
	size_t offset = DESC_HEADER;
	while (offset < DESC_SIZE(descPayload)) {
		LocalTimeOffsetStruct off;

		offset += parseLanguage(off.countryCode,descPayload+offset);

		{
			BYTE tmp = RB(descPayload,offset);
			off.countryRegion = util::BYTE(tmp >> 2);
			off.localTimeOffsetPolarity = (tmp & 0x00000001) ? true : false;
		}

		off.localTimeOffset = RW(descPayload,offset);
		off.changingDate = RW(descPayload,offset);

		{
			DWORD tmp1 = RDW(descPayload,offset);
			BYTE tmp2 = RB(descPayload,offset);

			off.timeOfChange = (tmp1 & 0xFFFFFF00) >> 8;
			off.nextTimeOffset = util::WORD(((tmp1 & 0x000000FF) << 8) | tmp2);
		}

		desc.push_back( off );
	}

	BOOST_FOREACH( const LocalTimeOffset &lt, desc ) {
		LTRACE("desc", "Country=%s, Region=%02x, Polarity=%d, Offset=%04x, chgDate=%04x, chgTime=%06x, Next=%04x",
			lt.countryCode.code(), lt.countryRegion, lt.localTimeOffsetPolarity, lt.localTimeOffset,
			lt.changingDate, lt.timeOfChange, lt.nextTimeOffset);
	}

	return true;
}

//	0x6F: Application signaling descriptor ESTI TS 102 809, 5.3.5.1
bool fnc_application_signalling( ApplicationSignallingDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::application_signalling );
	size_t offset = DESC_HEADER;
	ApplicationSignallingElementInfo app;

	while (offset < DESC_SIZE(descPayload)) {
		//	15 bits of type
		app.type = RW(descPayload,offset);
		app.type &= 0xEFFF;

		//	5 bits of vesrion
		app.version = RB(descPayload,offset);
		app.version &= 0x1F;

		//	Add application type
		desc.push_back( app );
	}

	BOOST_FOREACH( const ApplicationSignallingElementInfo elem, desc ) {
		LTRACE("desc", "Application Signalling: type=%02x, version=%02x", elem.type, elem.type);
	}

    return true;
}

//	0x7C: Advanced audio coding descriptor ABNT NBR 15608-3:2011
bool fnc_advanced_audio_coding( AdvancedAudioCodingDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::advanced_audio_coding );
	size_t offset = DESC_HEADER;
	desc.profileAndLevel = RB(descPayload,offset);
	LTRACE("desc", "Advanced Audio Coding: profileAndLevel=0x%02x", desc.profileAndLevel);
	return true;
}

//	0xC1: Digital copy control Arib STD-B10 Part2, 6.2.23
bool fnc_digital_copy_control( DigitalCopyControl &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::digital_copy_control );
	size_t offset = DESC_HEADER;
	util::BYTE tmp = RB(descPayload,offset);
	desc.drcd = (tmp & 0xC0) >> 6;
	desc.mbFlag = (tmp & 0x20) == 0x20;
	desc.ccFlag = (tmp & 0x10) == 0x10;
	desc.cct = util::BYTE((tmp & 0x0C) >> 2);
	if (desc.cct != 0x00) {
		desc.apscd = (tmp & 0x03);
	} else {
		desc.apscd = 0x00;
	}
	if (desc.mbFlag) {
		desc.mb = RB(descPayload,offset);
	}
	if (desc.ccFlag) {
		util::BYTE len = RB(descPayload,offset);
		util::BYTE pos = 0x00;
		while (pos<len) {
			ComponentControl cc;
			cc.ct = RB(descPayload,offset);
			pos ++;
			tmp = RB(descPayload,offset);
			pos ++;
			cc.drcd = (tmp & 0xC0) >> 6;
			cc.mbFlag = (tmp & 0x20) == 0x20;
			cc.cct = util::BYTE((tmp & 0x0C) >> 2);
			if (cc.cct != 0x00) {
				cc.apscd = (tmp & 0x03);
			} else {
				cc.apscd = 0x00;
			}
			if (cc.mbFlag) {
				cc.mb = RB(descPayload,offset);
				pos++;
			}
			desc.componentControls.push_back(cc);
		}
	}
	LTRACE("desc", "Digital Copy Control: data=0x%.2x type=0x%.2x", desc.drcd, desc.cct);
	return true;
}

//	0xC4: Audio component descriptor Arib STD-B10 Part2, 6.2.26
bool fnc_audio_component( AudioComponentDescriptor &descVec, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::audio_component );
	size_t offset = DESC_HEADER;
	AudioComponent desc;
	desc.streamContent = RB(descPayload,offset);
	desc.streamContent &= 0x0F;
	desc.componentType = RB(descPayload,offset);
	desc.componentTag  = RB(descPayload,offset);
	desc.streamType    = RB(descPayload,offset);
	desc.simulcastGroupTag = RB(descPayload,offset);

	BYTE data = RB(descPayload,offset);
	desc.multiLanguageFlag = (data & 0x80) ? true : false;
	desc.mainComponentFlag = (data & 0x40) ? true : false;
	desc.qualityIndicator  = util::BYTE((data & 0x30) >> 4);
	desc.samplingRate      = util::BYTE((data & 0x0E) >> 1);

	Language lang;
	offset += parseLanguage( lang, descPayload+offset );
	desc.languages.push_back( lang );

	if (desc.multiLanguageFlag) {
		offset += parseLanguage( lang, descPayload+offset );
		desc.languages.push_back( lang );
	}
	size_t len = DESC_SIZE(descPayload);
	if (len > offset) {
		desc.text.assign( (char *)(descPayload+offset), len-offset );
		//offset += (len-offset);
	}

	descVec.push_back( desc );

	LTRACE("desc", "Audio component: content=%02x, type=%01x, tag=%01x, multilingual=%d",
		desc.streamContent, desc.componentType, desc.componentTag, desc.multiLanguageFlag);
	BOOST_FOREACH(Language lang, desc.languages) {
		LTRACE("desc", "\t Language: %s", lang.code());
	}

	return true;
}

//	0xC7: Data content descriptor: Arib STD-B10 Part2, 6.2.28
bool fnc_data_content( DataContentDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::data_content );
	size_t offset = DESC_HEADER;
	desc.dataComponentId = RW(descPayload,offset);
	desc.entryComponent = RB(descPayload,offset);
	util::BYTE cant = RB(descPayload,offset);
	desc.selector.copy( (descPayload+offset), cant );
	descPayload += cant;
	cant = RB(descPayload,offset);
	for (int i=0; i<cant; i++) {
		util::BYTE tmp = RB(descPayload,offset);
		desc.componentRefs.push_back(tmp);
	}
	offset += parseLanguage(desc.lan, descPayload+offset);
	parseText(desc.text, descPayload+offset);

	LTRACE("desc", "Data Contents: dataComponentid=%d, entryComponent=%d selector=%s lan=%s text=%s",
	       desc.dataComponentId, desc.entryComponent, desc.selector.asHexa().c_str(), desc.lan.code(), desc.text.c_str());
	BOOST_FOREACH( util::BYTE compRef, desc.componentRefs ) {
		LTRACE("desc", "\tcomponentRef=%d", compRef);
	}
	return true;
}

//	0xC8: Video decode control descriptor Arib STD-B10 Part2, 6.2.30
bool fnc_video_decode( VideoDecodeDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::video_decode );
	BYTE data = GET_BYTE(descPayload);
	desc.stillPictureFlag    = (data & 0x80) ? true : false;
	desc.sequenceEndCodeFlag = (data & 0x40) ? true : false;
	desc.videoEncodeFormat   = util::BYTE((data & 0x3C) >> 2);
	LTRACE("desc", "Video decode: stillPicture=%d, sequence=%d, format=%d",
		desc.stillPictureFlag, desc.sequenceEndCodeFlag, desc.videoEncodeFormat);
	return true;
}

//	0xCD: Information of TS Arib STD-B10 Part2, 6.2.42
bool fnc_ts_information( TransportStreamInformationDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::ts_information );
	size_t offset = DESC_HEADER;
	BYTE data;

	//	Remote control key id
	desc.remoteControlKeyID = RB(descPayload,offset);

	data = RB(descPayload,offset);

	//	TS name
	BYTE tsNameLen = BYTE(data >> 2);
	desc.name = std::string( (char *)(descPayload+offset), tsNameLen );
	offset += tsNameLen;

	//	Transmision type count
	BYTE typeCount = (data & 0x3);
	for (int type=0; type<typeCount; type++) {
		TransmissionTypeStruct tType;

		tType.type = RB(descPayload,offset);
		data       = RB(descPayload,offset);
		for (BYTE srv=0; srv<data; srv++) {
			WORD serviceID = RW(descPayload,offset);
			tType.services.push_back( serviceID );
		}
		desc.transmissions.push_back( tType );
	}

	LTRACE("desc", "Transport Stream information: name=%s, remote control=%02x", desc.name.c_str(), desc.remoteControlKeyID);
	BOOST_FOREACH( const TransmissionTypeInfo &tType, desc.transmissions ) {
		LTRACE("desc", "\ttransmisionType=%02x", tType.type);
		BOOST_FOREACH( WORD serviceID, tType.services ) {
			LTRACE("desc", "\t\tserviceID=%04x", serviceID);
		}
	}

	return true;
}

//	0xCF: Logo transmission descriptor
bool fnc_logo_transmission( LogoTransmissionDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::logo_transmission );
	size_t offset = DESC_HEADER;
	desc.type = RB(descPayload,offset);
	switch (desc.type) {
		case LOGO_TYPE1: {
			LogoTransmissionType1 logo;
			logo.id   = RW(descPayload,offset);
			logo.id  &= 0x01FF;
			logo.version  = RW(descPayload,offset);
			logo.version &= 0x0FFF;
			logo.downloadID = RW(descPayload,offset);
			desc.data = logo;
			break;
		}
		case LOGO_TYPE2: {
			LogoTransmissionType2 logo;
			logo  = RW(descPayload,offset);
			logo &= 0x01FF;
			desc.data = logo;
			break;
		}
		case LOGO_TYPE3: {
			LogoTransmissionType3 logo;
			parseText( logo, descPayload+offset, DESC_SIZE(descPayload)-offset );
			desc.data = logo;
			break;
		}
		default:
			LWARN("desc", "invalid type of logo transmission descriptor type=%02x", desc.type);
			break;
	};

	switch (desc.type) {
		case LOGO_TYPE1: {
			const LogoTransmissionType1 &logo = desc.data.get<LogoTransmissionType1>();
			LTRACE("desc", "Logo transmission descriptor: type=01, id=%04x, ver=%04x, downloadID=%04x",
				logo.id, logo.version, logo.downloadID);
			break;
		}
		case LOGO_TYPE2: {
			const LogoTransmissionType2 &logo = desc.data.get<LogoTransmissionType2>();
			LTRACE("desc", "Logo transmission descriptor: type=02, id=%04x", logo);
			break;
		}
		case LOGO_TYPE3: {
			const LogoTransmissionType3 &logo = desc.data.get<LogoTransmissionType3>();
			LTRACE("desc", "Logo transmission descriptor: type=03, str=%s", logo.c_str());
			break;
		}
	};

	return true;
}

//	0xD5: Series descriptor Arib STD-B10 Part2, 6.2.33
bool fnc_series_descriptor( SeriesDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::series_descriptor );
	size_t offset = DESC_HEADER;

	desc.id = RW(descPayload,offset);
	util::BYTE tmp = RB(descPayload,offset);
	desc.repeatLabel = (tmp & 0xF0) >> 4;
	desc.programPattern = util::BYTE((tmp & 0x0E) >> 1);
	desc.expireDateFlag = (tmp & 0x01) == 0x01;
	desc.expireDate = RW(descPayload,offset);
	util::DWORD tmp2 = RDW(descPayload,offset);
	desc.episodeNumber = util::WORD((tmp2 & 0xFFF00000) >> 20);
	desc.lastEpisodeNumber = util::WORD((tmp2 & 0x000FFF00) >> 8);
	offset--;
	size_t len = DESC_SIZE(descPayload);
	if (offset<len) {
		parseText( desc.name, descPayload+offset, len-offset  );
	}
	LTRACE("desc", "Series descriptor: id=%d name=\"%s\" episode=%d lastEpisode=%d", desc.id, desc.name.c_str(), desc.episodeNumber, desc.lastEpisodeNumber);
	return true;
}

//	0xD6: Event group descriptor Arib STD-B10 Part2, 6.2.34
bool fnc_event_group( EventGroupDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::event_group );
	size_t offset = DESC_HEADER;

	util::BYTE tmp = RB(descPayload, offset);
	desc.groupType = (tmp & 0xF0) >> 4;
	int count = tmp & 0x0F;

	for(int i=0; i < count; i++ ) {
		Event ev;
		ev.serviceID = RW( descPayload, offset );
		ev.eventID = RW( descPayload, offset );
		desc.events.push_back(ev);
	}

	size_t len = DESC_SIZE(descPayload);
	if (offset < len) {
		if ( desc.groupType == 4 || desc.groupType == 5) {
			while (offset < len) {
				Data data;
				data.groupData.originalNetworkID = RW(descPayload, offset);
				data.groupData.transportStreamID = RW(descPayload, offset);
				data.groupData.serviceID = RW(descPayload, offset);
				data.groupData.eventID = RW(descPayload, offset);
				desc.data.push_back(data);
			}
		} else {
			while (offset < len) {
				Data data;
				data.privateData = RW(descPayload, offset);
				desc.data.push_back(data);
			}
		}
	}

	LTRACE("desc", "Event Group: type=0x%X, event count=%d, data count=%d", desc.groupType, desc.events.size(), desc.data.size());
	BOOST_FOREACH( Event event, desc.events ) {
		LTRACE("desc", "\t serviceID=%d, eventID=%d", event.serviceID, event.eventID);
	}
	if (desc.groupType == 4 || desc.groupType == 5) {
		BOOST_FOREACH( Data data, desc.data ) {
			LTRACE("desc", "\t originalNetworkID=%d, transportStreamID=%d, serviceID=%d, eventID=%d",
				data.groupData.originalNetworkID,
				data.groupData.transportStreamID,
				data.groupData.serviceID,
				data.groupData.eventID
      			);
		}
	} else {
		std::stringstream stream;
		BOOST_FOREACH( Data data, desc.data ) {
			stream << data.privateData << ',';
		}
		std::string str( stream.str() );
		LTRACE("desc", "\t privateData=[%s]", str.substr(0,str.length()-1).c_str());
	}
	return true;
}

//	0xD6: Component group descriptor Arib STD-B10 Part2, 6.2.37
bool fnc_component_group( ComponentGroupDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::component_group );

	size_t offset = DESC_HEADER;
	util::BYTE tmp = RB(descPayload,offset);
	desc.type = (tmp & 0xE0) >> 5;
	desc.totalBitrateFlag = (tmp & 0x10) ? true : false;
	int cantGroups = tmp & 0x0F;
	for (int i=0; i<cantGroups; i++) {
		ComponentGroupMember cgm;

		tmp = RB(descPayload,offset);
		cgm.id = (tmp & 0xF0) >> 4;
		int cantUnits = tmp & 0x0F;
		for (int j=0; j<cantUnits; j++) {
			CAUnit cau;

			tmp = RB(descPayload,offset);
			cau.id = (tmp & 0xF0) >> 4;
			int cantComponents = tmp & 0x0F;
			for (int k=0; k<cantComponents; k++) {
				util::BYTE componentTag = RB(descPayload,offset);
				cau.componentsTag.append((char*)&componentTag, 1);
			}

			cgm.units.push_back(cau);
		}
		if (desc.totalBitrateFlag) {
			cgm.totalBitrate = RB(descPayload,offset);
		} else {
			cgm.totalBitrate = 0;
		}
		offset += parseText( cgm.text, descPayload+offset );

		desc.groups.push_back(cgm);
	}

	LTRACE("desc", "Component Group: type=0x%X, totalBitrateFlag=%d, group count=%d", desc.type, desc.totalBitrateFlag, cantGroups);
	BOOST_FOREACH( ComponentGroupMember cgm, desc.groups ) {
		LTRACE("desc", "\tGroup: id=%d, totalBitrate=%d, text=%s, CAUnit count=%d", cgm.id, cgm.totalBitrate, cgm.text.c_str(), cgm.units.size() );
		BOOST_FOREACH( CAUnit cau, cgm.units) {
			LTRACE("desc", "\t\tCAUnit: id=%d, components tags=%s", cau.id, cau.componentsTag.asHexa().c_str());
		}
	}
	return true;
}

//	0xDE: Content availability descriptor Arib STD-B10 Part2, 6.2.37
bool fnc_content_availability( ContentAvailabilityDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::content_availability );
	size_t offset = DESC_HEADER;
	util::BYTE tmp = RB(descPayload,offset);
	desc.imageConstraintToken = (tmp & 0x20) ? false : true;
	desc.retentionMode = (tmp & 0x10) ? true : false;
	desc.retentionState = util::BYTE((tmp & 0x0E) >> 1);
	desc.encriptionMode = (tmp & 0x01) ? false : true;

	LTRACE("desc", "Content Availability: imageConstraintToken=%d retentionMode=%d retentionState=0x%.2x encriptionMode=%d",
		desc.imageConstraintToken, desc.retentionMode, desc.retentionState,	desc.encriptionMode);

	return true;
}

//	0xFA: Terrestrial delivery system descriptor Arib STD-B10 Part2, 6.2.31
bool fnc_terrestrial_delivery_system( TerrestrialDeliverySystemDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::terrestrial_delivery_system );
	size_t offset = DESC_HEADER;
	util::WORD data = RW(descPayload,offset);
	desc.area     = (data & 0xFFF0) >> 4;
	desc.interval = util::BYTE((data & 0x000C) >> 2);
	desc.mode     = data & 0x0003;

	while (offset < DESC_SIZE(descPayload)) {
		data = RW(descPayload,offset);
		desc.frequencies.push_back( data );
	}

	LTRACE("desc", "Terrestrial Delivery: area=0x%.4x, interval=0x%.2x, mode=0x%.2x", desc.area, desc.interval, desc.mode);
	BOOST_FOREACH( util::WORD freq, desc.frequencies ) {
		LTRACE("desc", "\tfrequency=%d", freq);
	}
	return true;
}

//	0xFB: Partial reception descriptor Arib STD-B10 Part2, 6.2.32
bool fnc_partial_reception( PartialReceptionDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::partial_reception );
	size_t offset = DESC_HEADER;
	util::WORD data;
	while (offset < DESC_SIZE(descPayload)) {
		data = RW(descPayload,offset);
		desc.push_back( data );
	}
	LTRACE("desc", "Partial Reception: Services-Ids=%d", desc.size());
	BOOST_FOREACH( WORD serviceID, desc ) {
		LTRACE("desc", "\t%d", serviceID);
	}
	return true;
}

//	0xFC: Emergency Information Descriptor ISDB-T HARMONIZATION DOCUMENT; PART 3: Emergency Warning Broadcast System EWBS (5/2013)
bool fnc_emergency_information( EmergencyInformationDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::emergency_information );
	size_t offset = DESC_HEADER;
	while (offset < DESC_SIZE(descPayload)) {
		ServiceEmergencyInformation info;
		info.serviceID = RW(descPayload,offset);
		BYTE data = RB(descPayload,offset);
		info.startEndFlag = (data & 0x80) ? true : false;
		info.signalLevel = (data & 0x40) ? true : false;
		BYTE lenItems = RB(descPayload,offset);
		size_t last = offset+lenItems;
		while (offset < last) {
			util::WORD tmp = RW(descPayload,offset);
			info.areaCodes.push_back( ((tmp & 0xFFF0) >> 4) );
		}

		desc.push_back( info );
	}

	BOOST_FOREACH( const ServiceEmergencyInformation &srvEID, desc ) {
		LTRACE("desc", "Emergency Information: serviceID=%04x, startEndFlag=%d, signalLevel=%d, areas=%d",
			srvEID.serviceID, srvEID.startEndFlag, srvEID.signalLevel, srvEID.areaCodes.size()
		);
	}

	return true;
}

//	0xFD: Data component descriptor Arib STD-B10 Part2, 6.2.20
bool fnc_data_component( DataComponentDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::data_component );
	size_t offset = DESC_HEADER;
	desc.codingMethodID = RW(descPayload,offset);
	desc.info.copy( (descPayload+offset), DESC_SIZE(descPayload)-offset );
	LTRACE("desc", "Data component: methodID=%04x, additional info=%s",
		desc.codingMethodID, desc.info.asHexa().c_str());
    return true;
}

//	0xFE: System management descriptor Arib STD-B10 Part2, 6.2.21
bool fnc_system_management( SystemManagementDescriptor &desc, BYTE *descPayload ) {
	DTV_ASSERT( DESC_TAG(descPayload) == ident::system_management );
	size_t offset = DESC_HEADER;
	desc.systemID  = RW(descPayload,offset);
	size_t len = DESC_SIZE(descPayload);
	if (offset < len) {
		desc.info.copy( (descPayload+offset), DESC_SIZE(descPayload)-offset );
	}
	LTRACE("desc", "System Management: systemID=0x%.4x, additional info=\"%d\"", desc.systemID, desc.info.length());
	return true;
}

}	//	namespace demuxer
}
}

