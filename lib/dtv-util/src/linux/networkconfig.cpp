/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "networkconfig.h"
#include "../net/ipv4/sockaddr.h"
#include "../net/ipv6/sockaddr.h"
#include "../log.h"
#include "../assert.h"
#include <algorithm>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <ifaddrs.h>
#include <resolv.h>
#include <string.h>

namespace util {
namespace net {

NetworkConfig::NetworkConfig()
{
}

NetworkConfig::~NetworkConfig()
{
}

bool NetworkConfig::getDNS( dns::Config &info ) {
	DTV_ASSERT( info.servers.empty() );

	if (res_init()) {
		LERROR( "NetworkInformation", "Cannot get name servers" );
		return false;
	}

	info.domain = _res.defdname;
	for (int i = 0; i < _res.nscount; i ++) {
		std::string ns = inet_ntoa(((struct sockaddr_in *)&_res.nsaddr_list[i])->sin_addr);
		info.servers.push_back(ns);
	}

	return true;
}

struct AdapterFinder {
	AdapterFinder( const char *name ) : _name(name) {}

	bool operator()( const Adapter &adapter ) const {
		return adapter.name == _name;
	}

	const char *_name;
};

static inline SockAddr get( sockaddr *sa ) {
	size_t len = (sa->sa_family == AF_INET) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6);
	return SockAddr( sa, len );
}

bool NetworkConfig::getAdapters( Adapters &adapters ) {
	DTV_ASSERT( adapters.empty() );

	struct ifaddrs *ifaddr, *ifa;
	if (getifaddrs(&ifaddr) == -1) {
		LERROR( "NetworkInformation", "Cannot list adapters" );
		return false;
	}

	for (ifa=ifaddr; ifa != NULL; ifa=ifa->ifa_next) {
		if (!ifa->ifa_addr) {
			continue;
		}

		if (ifa->ifa_addr->sa_family != AF_INET && ifa->ifa_addr->sa_family != AF_INET6) {
			continue;
		}

		Adapters::iterator it=std::find_if(
			adapters.begin(),
			adapters.end(),
			AdapterFinder(ifa->ifa_name)
			);
		if (it == adapters.end()) {
			Adapter adapter;
			adapter.name = ifa->ifa_name;
			it = adapters.insert( adapters.end(), adapter );
		}

		AddressInfo info;
		info.addr = get(ifa->ifa_addr);
		info.netmask = get(ifa->ifa_netmask);
		it->addresses.push_back( info );
	}

	freeifaddrs(ifaddr);

	return true;
}

#define BUFSIZE 8192
static void parseAddr( SockAddr &addr, int family, void *data ) {
	if (family == AF_INET) {
		struct sockaddr_in saddr_in;
		saddr_in.sin_family = AF_INET;
		saddr_in.sin_addr.s_addr = *(u_int *)data;
		saddr_in.sin_port = 0;
		addr = SockAddr( (struct sockaddr *)&saddr_in, sizeof(saddr_in) );
	}
	else {
		DTV_ASSERT(false);
	}
}

bool NetworkConfig::getRoutes( Routes &routes ) {
    struct nlmsghdr *nlMsg;
    int sock, len;
	char msgBuf[BUFSIZE];

	//	Create NETLINK socket
	if ((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0) {
		LERROR( "NetworkInformation", "Cannot create netlink socked" );
		return false;
	}

	memset(msgBuf, 0, BUFSIZE);

	//	point the header and the msg structure pointers into the buffer
	nlMsg = (struct nlmsghdr *)msgBuf;
	nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));  // Length of message.
	nlMsg->nlmsg_type = RTM_GETROUTE;   // Get the routes from kernel routing table .
	nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;    // The message is a request for dump.
	nlMsg->nlmsg_seq = 0;    // Sequence of the message packet.
	nlMsg->nlmsg_pid = getpid();    // PID of process sending the request.

	//	send the request
	if (send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0) {
		LERROR( "NetworkInformation", "Write to socket failed" );
		return false;
	}

	//	read the response
	if ((len = readNLSocket(sock, msgBuf)) < 0) {
		return false;
	}

	// Parse the response
	for (; NLMSG_OK(nlMsg, (size_t)len); nlMsg = NLMSG_NEXT(nlMsg, len)) {
		struct rtmsg *rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);

		//	Checks
		if ((rtMsg->rtm_family != AF_INET && rtMsg->rtm_family != AF_INET6) || (rtMsg->rtm_table != RT_TABLE_MAIN))
			continue;

		RouteInfo info;

		if (rtMsg->rtm_family == AF_INET) {
			ipv4::SockAddr def;
			info.gateway = def.addr();
			info.src = def.addr();
			info.dst = def.addr();
		}

		//	Get the rtattr field
		struct rtattr *rtAttr = (struct rtattr *) RTM_RTA(rtMsg);
		int rtLen = RTM_PAYLOAD(nlMsg);
		for (; RTA_OK(rtAttr, rtLen); rtAttr = RTA_NEXT(rtAttr, rtLen)) {
			switch (rtAttr->rta_type) {
				case RTA_OIF: {
					char ifName[IF_NAMESIZE];
					if_indextoname(*(int *) RTA_DATA(rtAttr), ifName);
					info.adapter = ifName;
					break;
				}
				case RTA_GATEWAY: {
					parseAddr( info.gateway, rtMsg->rtm_family, RTA_DATA(rtAttr) );
					break;
				}
				case RTA_PREFSRC: {
					parseAddr( info.src, rtMsg->rtm_family, RTA_DATA(rtAttr) );
					break;
				}
				case RTA_DST: {
					parseAddr( info.dst, rtMsg->rtm_family, RTA_DATA(rtAttr) );
					break;
				}
			}
		}

		routes.push_back( info );
	}

	close(sock);

	return true;
}

int NetworkConfig::readNLSocket(int sockFd, char *bufPtr) {
	struct nlmsghdr *nlHdr;
	int readLen = 0, msgLen = 0;

	do {
		//	Recieve response from the kernel
		if ((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0) {
			LERROR( "NetworkInformation", "Read from socket failed" );
			return -1;
		}

		nlHdr = (struct nlmsghdr *) bufPtr;

		//	Check if the header is valid
		if ((NLMSG_OK(nlHdr, (size_t)readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR)) {
			LERROR( "NetworkInformation", "Error in recieved packet" );
			return -1;
		}

		//	Check if the its the last message
		if (nlHdr->nlmsg_type == NLMSG_DONE) {
			break;
		} else {
			//	Else move the pointer to buffer appropriately
			bufPtr += readLen;
			msgLen += readLen;
		}

		//	Check if its a multi part message
		if ((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0) {
			break;
		}
	} while ((nlHdr->nlmsg_seq != 0) || (nlHdr->nlmsg_pid != (size_t)getpid()));

	return msgLen;
}

}
}

