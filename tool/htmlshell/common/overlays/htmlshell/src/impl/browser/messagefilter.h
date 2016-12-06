#pragma once

#include "ipc/ipc_channel_proxy.h"
#include "ipc/message_filter.h"
#include <string>

namespace content {
class RenderProcessHost;
}

namespace tvd {

class BrowserClient;

class MessageFilter : public IPC::MessageFilter {
public:
	MessageFilter( BrowserClient *client, content::RenderProcessHost *host );
	~MessageFilter() override;

	// IPC::ChannelProxy::MessageFilter implementation.
	void OnFilterAdded( IPC::Sender *sender ) override;
	void OnFilterRemoved() override;
	bool OnMessageReceived( const IPC::Message &message ) override;

	bool Send( IPC::Message *message );

protected:
	// IPC message handlers:
	void onGetSiteInfo( int viewRoutingId, int frameRoutingId, int *siteId, std::string *js );

private:
	BrowserClient *_client;
	content::RenderProcessHost *_host;
	IPC::Sender *_sender;

	DISALLOW_COPY_AND_ASSIGN(MessageFilter);
};

}
