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

#include "generic.h"
#include "ca.h"
#include "../ts.h"
#include "../text.h"

using namespace util;

namespace tuner {
namespace desc {
namespace demuxer {

bool fnc_registration( FormatIdentifier &desc, BYTE *descPayload );
bool fnc_language( LanguageDescriptor &desc, BYTE *descPayload );
bool fnc_data_component( DataComponentDescriptor &desc, BYTE *descPayload );
bool fnc_service_list( ServiceListDescriptor &desc, BYTE *descPayload );
bool fnc_ts_information( TransportStreamInformationDescriptor &desc, BYTE *descPayload );
bool fnc_association_tag( AssociationTagDescriptor &desc, BYTE *descPayload );
bool fnc_stream_identifier( BYTE &tag, BYTE *descPayload );
bool fnc_service( ServiceDescriptor &desc, BYTE *descPayload );
bool fnc_local_time_offset( LocalTimeOffsetDescriptor &descs, BYTE *descPayload );
bool fnc_application_signalling( ApplicationSignallingDescriptor &descs, BYTE *descPayload );
bool fnc_network_name( std::string &descs, BYTE *descPayload );
bool fnc_system_management( SystemManagementDescriptor &descs, BYTE *descPayload );
bool fnc_carousel_id( CarouselIdentifierDescriptor &descs, BYTE *descPayload );
bool fnc_terrestrial_delivery_system( TerrestrialDeliverySystemDescriptor &descs, BYTE *descPayload );
bool fnc_parcial_reception( PartialReceptionDescriptor &descs, BYTE *descPayload );
bool fnc_short_event( ShortEventDescriptor &descs, BYTE *descPayload );
bool fnc_extended_event( ExtendedEventDescriptor &descs, BYTE *descPayload );
bool fnc_parental_rating( ParentalRatingDescriptor &descs, BYTE *descPayload );
bool fnc_video_decode( VideoDecodeDescriptor &descs, BYTE *descPayload );
bool fnc_conditional_access( ca::Descriptor &desc, BYTE *descPayload );
bool fnc_application_storage( ApplicationStorageDescriptor &desc, BYTE *descPayload );
bool fnc_linkage( LinkageDescriptors &desc, BYTE *descPayload );
bool fnc_component( ComponentDescriptor &desc, BYTE *descPayload );
bool fnc_content( ContentDescriptor &desc, BYTE *descPayload );
bool fnc_advanced_audio_coding( AdvancedAudioCodingDescriptor &desc, BYTE *descPayload );
bool fnc_digital_copy_control( DigitalCopyControl &desc, BYTE *descPayload );
bool fnc_audio_component( AudioComponentDescriptor &descVec, BYTE *descPayload );
bool fnc_data_content( DataContentDescriptor &desc, BYTE *descPayload );
bool fnc_logo_transmission( LogoTransmissionDescriptor &desc, BYTE *descPayload );
bool fnc_series_descriptor( SeriesDescriptor &desc, BYTE *descPayload );
bool fnc_event_group( EventGroupDescriptor &desc, BYTE *descPayload );
bool fnc_component_group( ComponentGroupDescriptor &desc, BYTE *descPayload );
bool fnc_content_availability( ContentAvailabilityDescriptor &desc, BYTE *descPayload );
bool fnc_partial_reception( PartialReceptionDescriptor &desc, BYTE *descPayload );
bool fnc_emergency_information( EmergencyInformationDescriptor &desc, BYTE *descPayload );

}
}
}

