#pragma once

namespace aml {
namespace h264 {

bool updateHeader( Packet *info, Packet *pkt );
bool updateFrame( Packet *pkt );

}
}


