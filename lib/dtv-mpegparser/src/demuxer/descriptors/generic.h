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
#pragma once

#include "../descriptors.h"
#include "../../service/types.h"
#include "../../language.h"
#include <util/buffer.h>
#include <util/any.h>
#include <vector>

#define USER_PRIVATE_DESCRIPTOR  64
#define DESC_HEADER              2

#define DESC_TAG(ptr)            ((ptr)[0])
#define DESC_SIZE(ptr)           size_t(((ptr)[1])+DESC_HEADER)


namespace tuner {
namespace desc {

//	Service name descriptor
typedef std::string ServiceName;

//	Registration descriptor
struct FormatIdentifierStruct {
	util::DWORD format;
	util::Buffer info;
};
typedef struct FormatIdentifierStruct FormatIdentifier;

//	Service list descriptor
typedef std::pair<ID,service::Type> Service;
typedef std::vector<Service> ServiceListDescriptor;

//	Service descriptor
struct ServiceInfo {
	service::Type type;
	std::string provider;
	std::string name;
};
typedef struct ServiceInfo ServiceDescriptor;
void show( const ServiceDescriptor &desc );

//	Language descriptor (0x0a)
struct LanguageDescriptorStruct {
	Language language;
	util::BYTE audioType;
};
typedef LanguageDescriptorStruct LanguageDescriptor;

//	Parental rating descriptor
struct ParentalRating {
	Language language;
	util::BYTE rating;
};
typedef std::vector<struct ParentalRating> ParentalRatingDescriptor;
void show( const ParentalRatingDescriptor &desc );

//	Component Descriptor (0x10)
struct ApplicationStorageStruct {
	util::BYTE storageProperty;
	util::BYTE flags;
	util::DWORD version;
	util::BYTE priority;
};
typedef struct ApplicationStorageStruct ApplicationStorageDescriptor;
void show( const ApplicationStorageDescriptor &desc );

//	Short event descriptor
struct ShortEvent {
	Language language;
	std::string event;
	std::string text;
};
typedef struct ShortEvent ShortEventDescriptor;
void show( const ShortEventDescriptor &desc );

//	Extended event descriptor
struct ExtendedEventItemStruct {
	std::string description;
	std::string content;
};
typedef struct ExtendedEventItemStruct ExtendedEventItem;

struct ExtendedEventStruct {
	Language language;
	std::string text;
	std::vector<ExtendedEventItem> items;
};
typedef  struct ExtendedEventStruct ExtendedEvent;

typedef std::vector<ExtendedEvent> ExtendedEventDescriptor;
void show( const ExtendedEvent &desc );

//	Content descriptor
typedef std::vector<util::WORD> ContentDescriptor;
void show( const ContentDescriptor &desc );

//	Data Content Descriptor (0xC7)
struct DataContentStruct {
	util::WORD dataComponentId;
	util::BYTE entryComponent;
	util::Buffer selector;
	std::vector<util::BYTE> componentRefs;
	Language lan;
	std::string text;
};
typedef struct DataContentStruct DataContentDescriptor;

//	Video Decode Descriptor (0xC8)
struct VideoDecodeStruct {
	bool stillPictureFlag;
	bool sequenceEndCodeFlag;
	util::BYTE videoEncodeFormat;
};
typedef struct VideoDecodeStruct VideoDecodeDescriptor;
void show( const VideoDecodeDescriptor &desc );

//	Advanced Audio Coding Descriptor
typedef struct {
	util::BYTE profileAndLevel;
} AdvancedAudioCodingDescriptor;
void show( const AdvancedAudioCodingDescriptor &desc );

//	Digital Copy Control Descriptor (0xC1) - Component Control Data
struct ComponentControlStruct {
	util::BYTE ct;
	util::BYTE drcd;
	bool mbFlag;
	util::BYTE cct;
	util::BYTE apscd;
	util::BYTE mb;
};
typedef struct ComponentControlStruct ComponentControl;

//	Digital Copy Control Descriptor (0xC1)
struct DigitalCopyControl {
	util::BYTE drcd;
	bool mbFlag;
	bool ccFlag;
	util::BYTE cct;
	util::BYTE apscd;
	util::BYTE mb;
	std::vector<ComponentControl> componentControls;
};
typedef struct DigitalCopyControl DigitalCopyControlDescriptor;

//	Audio Component Descriptor
typedef struct {
	util::BYTE streamContent;
	util::BYTE componentType;
	util::BYTE componentTag;
	util::BYTE streamType;
	util::BYTE simulcastGroupTag;
	bool multiLanguageFlag;
	bool mainComponentFlag;
	util::BYTE qualityIndicator;
	util::BYTE samplingRate;
	std::vector<Language> languages;
	std::string text;
} AudioComponentStruct;
typedef AudioComponentStruct AudioComponent;
typedef std::vector<AudioComponent> AudioComponentDescriptor;
void show( const AudioComponent &desc );

//	Application Signalling Descriptor
struct ApplicationSignallingElementStruct {
	util::WORD type;
	util::BYTE version;
};
typedef struct ApplicationSignallingElementStruct ApplicationSignallingElementInfo;
typedef std::vector<ApplicationSignallingElementInfo> ApplicationSignallingDescriptor;
void show( const ApplicationSignallingDescriptor &desc );

//	Terrestrial Delivery System Descriptor (0xFA)
struct TerrestrialDeliverySystemStruct {
	util::WORD area;
	util::BYTE interval;
	util::BYTE mode;
	std::vector<util::WORD> frequencies;
};
typedef struct TerrestrialDeliverySystemStruct TerrestrialDeliverySystemDescriptor;

//	Partial Reception Descriptor (0xFB)
typedef std::vector<util::WORD> PartialReceptionDescriptor;

//	System Management Descriptor (0xFE)
struct SystemManagementStruct {
	util::WORD systemID;
	util::Buffer info;
};
typedef struct SystemManagementStruct SystemManagementDescriptor;

//	Association Tag Descriptor
struct AssociationTagStruct {
	util::WORD tag;
	util::WORD use;
};
typedef AssociationTagStruct AssociationTagDescriptor;
void show( const AssociationTagDescriptor &desc );


//	Data Component Descriptor
struct DataComponentStruct {
	util::WORD codingMethodID;
	Buffer info;
};
typedef DataComponentStruct DataComponentDescriptor;
void show( const DataComponentDescriptor &desc );

//	Transport Stream Information Descriptor
struct TransmissionTypeStruct {
	util::BYTE type;
	std::vector<util::WORD> services;
};
typedef struct TransmissionTypeStruct TransmissionTypeInfo;

struct TransportStreamInformationStruct {
	util::BYTE remoteControlKeyID;
	std::string name;
	std::vector<TransmissionTypeInfo> transmissions;
};
typedef TransportStreamInformationStruct TransportStreamInformationDescriptor;
void show( const TransportStreamInformationDescriptor &desc );

//	Linkage descriptor (0x4a)
struct LinkageStruct {
	util::WORD tsID;
	util::WORD nitID;
	util::WORD srvID;
	util::BYTE type;
	util::Buffer data;
};
typedef struct LinkageStruct LinkageDescriptor;
typedef std::vector<LinkageDescriptor> LinkageDescriptors;

//	Series Descriptor (0xD5)
struct SeriesStruct {
	util::WORD id;
	util::BYTE repeatLabel;
	util::BYTE programPattern;
	bool expireDateFlag;
	util::WORD expireDate;
	util::WORD episodeNumber;
	util::WORD lastEpisodeNumber;
	std::string name;
};
typedef struct SeriesStruct SeriesDescriptor;

//	Event Group Descriptor (0x0d6)
struct EventStruct {
	util::WORD serviceID;
	util::WORD eventID;
};
typedef struct EventStruct Event;

struct GroupStruct {
	util::WORD originalNetworkID;
	util::WORD transportStreamID;
	util::WORD serviceID;
	util::WORD eventID;
};
typedef struct GroupStruct Group;

union DataUnion {
	Group groupData;
	util::WORD privateData;
};
typedef union DataUnion Data;

struct EventGroupStruct {
	util::BYTE groupType;
	std::vector<Event> events;
	std::vector<Data> data;
};
typedef struct EventGroupStruct EventGroupDescriptor;

//	Component Group Descriptor (0xD9)
struct CAUnitStruct {
	util::BYTE id;
	util::Buffer componentsTag;
};
typedef struct CAUnitStruct CAUnit;

struct ComponentGroupMemberStruct {
	util::BYTE id;
	std::vector<CAUnit> units;
	util::BYTE totalBitrate;
	std::string text;
};
typedef struct ComponentGroupMemberStruct ComponentGroupMember;

struct ComponentGroupStruct {
	util::BYTE type;
	bool totalBitrateFlag;
	std::vector<ComponentGroupMember> groups;
};
typedef struct ComponentGroupStruct ComponentGroupDescriptor;

// Content Availability Descriptor (0xDE)
struct ContentAvailability {
	bool imageConstraintToken;
	bool retentionMode;
	util::BYTE retentionState;
	bool encriptionMode;
};
typedef struct ContentAvailability ContentAvailabilityDescriptor;

//	Logo Transmission descriptor
#define LOGO_TYPE1 0x01
#define LOGO_TYPE2 0x02
#define LOGO_TYPE3 0x03
struct LogoTransmissionStruct {
	util::BYTE type;
	Any data;
};
struct LogoTransmissionType1Struct {
	util::WORD id;
	util::WORD version;
	util::WORD downloadID;
};
typedef struct LogoTransmissionType1Struct LogoTransmissionType1;
typedef util::WORD                         LogoTransmissionType2;
typedef std::string                        LogoTransmissionType3;
typedef struct LogoTransmissionStruct      LogoTransmissionDescriptor;

void show( const LogoTransmissionDescriptor &desc );

//	Local Time Offset Descriptor
struct LocalTimeOffsetStruct {
	Language countryCode;
	util::BYTE countryRegion;
	bool localTimeOffsetPolarity;
	util::WORD localTimeOffset;
	util::WORD changingDate;
	util::DWORD timeOfChange;
	util::WORD nextTimeOffset;
};
typedef struct LocalTimeOffsetStruct LocalTimeOffset;
typedef std::vector<LocalTimeOffset> LocalTimeOffsetDescriptor;
void show( const LocalTimeOffsetDescriptor &desc );

//	Component Descriptor (0x50)
struct ComponentStruct {
	util::BYTE streamContent;
	util::BYTE componentType;
	util::BYTE componentTag;
	Language language;
	std::string text;
};
typedef struct ComponentStruct ComponentDescriptor;
void show( const ComponentDescriptor &desc );

//	Carousel identifier descriptor
struct CarouselIdentifierStruct {
	util::DWORD carouselID;
	util::BYTE  formatID;
	util::Buffer data;
};

//	if formatID == 0x01
struct CarouselEnhancedBootStruct {
	util::BYTE  _moduleVersion;
	util::WORD  _moduleID;
	util::WORD  _blockSize;
	util::DWORD _moduleSize;
	util::BYTE  _compressedMethod;
	util::DWORD _originalSize;
	util::BYTE  _timeout;
	//    biop::ObjectKeyType _key;	//	TODO: Review becouse ETSI TS 102 809 this field is present!
	util::Buffer _privateData;
};
typedef CarouselEnhancedBootStruct CarouselEnhancedBoot;
typedef struct CarouselIdentifierStruct CarouselIdentifierDescriptor;

//	Generic methods
util::BYTE getStreamIdentifier( const Descriptors &desc );
bool getVideoDecode( const Descriptors &desc, int &width, int &height );

//	Emergency Information Descriptor (0xFC)
struct ServiceEmergencyInformationStruct {
	util::WORD serviceID;
	bool startEndFlag;
	bool signalLevel;
	std::vector<util::WORD> areaCodes;
};
typedef struct ServiceEmergencyInformationStruct ServiceEmergencyInformation;
typedef std::vector<ServiceEmergencyInformation> EmergencyInformationDescriptor;

}
}

