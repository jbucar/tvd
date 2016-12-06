#pragma once

#include "base/memory/scoped_ptr.h"
#include "ui/aura/client/window_tree_client.h"

namespace aura {
class Window;
class WindowTreeHost;
namespace client {
class DefaultCaptureClient;
class FocusClient;
}
}

namespace gfx {
class Size;
class Rect;
}

namespace tvd {

class PlatformDataAura : public aura::client::WindowTreeClient {
public:
	explicit PlatformDataAura( const gfx::Rect &bounds );
	~PlatformDataAura();

	// Overridden from aura::client::WindowTreeClient:
	aura::Window *GetDefaultParent( aura::Window *context, aura::Window *window, const gfx::Rect &bounds ) override;

private:
	scoped_ptr<aura::WindowTreeHost> _host;
	scoped_ptr<aura::client::FocusClient> _focusClient;
	scoped_ptr<aura::client::DefaultCaptureClient> _captureClient;

	DISALLOW_COPY_AND_ASSIGN(PlatformDataAura);
};

}
