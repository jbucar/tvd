#include "messagefilter.h"
#include "browserclient.h"
#include "siteinstancehost.h"
#include "../common/messages.h"
#include "base/compiler_specific.h"
#include "base/bind.h"
#include "content/common/view_messages.h"
#include "content/public/browser/render_process_host.h"

namespace tvd {

MessageFilter::MessageFilter( BrowserClient *client, content::RenderProcessHost *host )
	: _client(client), _host(host), _sender(NULL)
{
}

MessageFilter::~MessageFilter()
{
}

void MessageFilter::OnFilterAdded( IPC::Sender *sender ) {
	VLOG(1) << "Filter added";
	_sender = sender;
}

void MessageFilter::OnFilterRemoved() {
	VLOG(1) << "Filter removed";
}

bool MessageFilter::OnMessageReceived( const IPC::Message &message ) {
	bool handled = true;
	IPC_BEGIN_MESSAGE_MAP(MessageFilter, message)
		IPC_MESSAGE_HANDLER(Tvd_GetNewSiteInfo, onGetSiteInfo)
		IPC_MESSAGE_UNHANDLED(handled = false)
	IPC_END_MESSAGE_MAP()
	return handled;
}

bool MessageFilter::Send( IPC::Message *message ) {
  return _host->Send(message);
}

void MessageFilter::onGetSiteInfo( int viewRoutingId, int frameRoutingId, int *siteId, std::string *js ) {
	CHECK_GT(viewRoutingId, 0);
	CHECK_GT(frameRoutingId, 0);

	LOG(INFO) << "Got render site info! view_routing_id=" << viewRoutingId << ", frame_routing_id=" << frameRoutingId;
	SiteInstanceHost *site = _client->updateSite(_host->GetID(), viewRoutingId, _host->GetID(), frameRoutingId);
	if (site) {
		site->signalLaunched();
		*siteId = site->id();
		*js = site->initScript();
	} else {
		LOG(WARNING) << "Fail to get site info! Routing-ids:(" << viewRoutingId << "," << frameRoutingId << ")";
		*siteId = -1;
		*js = "";
	}
}

}
